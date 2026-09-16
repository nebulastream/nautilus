#include "nautilus/compiler/backends/mlir/debug/PerfJitDumpPlugin.hpp"

#if defined(__linux__)

#include <llvm/DebugInfo/DWARF/DWARFContext.h>
#include <llvm/ADT/StringSet.h>
#include <llvm/ExecutionEngine/JITLink/JITLink.h>
#include <llvm/ExecutionEngine/Orc/Debugging/DebugInfoSupport.h>
#include <llvm/ExecutionEngine/Orc/Shared/PerfSharedStructs.h>
#include <llvm/Support/Process.h>
#include <llvm/Support/Threading.h>
#include <llvm/Support/raw_ostream.h>

#include <string>
#include <utility>
#include <vector>

namespace nautilus::compiler::mlir {

namespace {

using llvm::orc::PerfJITCodeLoadRecord;
using llvm::orc::PerfJITCodeUnwindingInfoRecord;
using llvm::orc::PerfJITDebugEntry;
using llvm::orc::PerfJITDebugInfoRecord;
using llvm::orc::PerfJITRecordBatch;
using llvm::orc::PerfJITRecordType;

// One contiguous span of a function's code that shares an inline (region)
// stack, and so becomes one JIT_CODE_LOAD record.
struct CodeRange {
	uint64_t start = 0;
	uint64_t end = 0;
	std::string name;
	// Line-table rows falling in [start, end), for this range's debug record.
	std::vector<PerfJITDebugEntry> lines;
};

// `execute::outer::hot` for an address inside region "hot" nested in "outer",
// or just the symbol's own name where no region() encloses it.
//
// DWARF inline frames come back leaf-to-root, with the last frame being the
// function itself; the region levels are everything below it, re-ordered
// outermost-first so the qualified name reads the way the source nests. The
// symbol's own name is used as the base rather than the DWARF function name:
// the two agree for `execute`, but the symbol name is what perf would have
// shown anyway, so a range with no regions keeps exactly today's name.
std::string qualifiedName(llvm::StringRef symbolName, const llvm::DIInliningInfo& inlineStack,
                          const llvm::StringSet<>& functionNames) {
	std::string name = symbolName.str();
	const uint32_t frames = inlineStack.getNumberOfFrames();
	if (frames < 2) {
		return name;
	}
	// Outermost region first, so the name reads the way the source nests. The
	// root frame is the function itself and is already covered by symbolName.
	//
	// Only region frames are appended. An inline stack also contains genuinely
	// inlined *function* calls -- at -O3 a Nautilus-to-Nautilus call inside a
	// region is one -- and naming those too would split the region's code into
	// alternating `...::hot` / `...::hot::callee` ranges, shattering one region
	// into a dozen separately-named symbols that perf does not recombine (it
	// keys a symbol by its address, not its name). Attributing a callee's
	// inlined body to the region it was inlined into is also the more useful
	// reading for a profile: that time really is the region's.
	for (uint32_t i = frames - 1; i-- > 0;) {
		llvm::StringRef frameName = inlineStack.getFrame(i).FunctionName;
		if (frameName.empty() || frameName == llvm::DILineInfo::BadString) {
			continue;
		}
		// Regions are told apart from genuinely inlined function calls by
		// elimination: every real function in this object has a symbol in the
		// link graph, and a region never does. (DWARF offers nothing more
		// direct here -- a region's synthetic subprogram is shaped exactly
		// like any other, and a marker in DW_AT_linkage_name does not survive:
		// LLVM does not emit linkage names for abstract subprograms, so a
		// reader just gets DW_AT_name back.) A region named identically to a
		// function in the same module is therefore attributed to its parent
		// rather than named -- a cosmetic loss, and the conservative way to be
		// wrong.
		if (functionNames.contains(frameName)) {
			continue;
		}
		name += "::";
		name += frameName.str();
	}
	return name;
}

// Splits one symbol into the code ranges that become its jitdump records.
//
// Without region symbols (or without debug info to derive them from) this is
// always a single range covering the whole symbol -- exactly what
// PerfSupportPlugin would have emitted, minus the ordering defect.
//
// With them, the line table supplies the address grid: DWARF emits a row
// wherever the location changes, inlinedAt included, so consecutive rows
// sharing an inline stack delimit a region's extent. Ranges partition the
// symbol and so stay disjoint, which the jitdump format requires -- a region
// whose code the optimizer has interleaved with its siblings simply yields
// several ranges that share a name rather than one.
std::vector<CodeRange> computeCodeRanges(const llvm::jitlink::Symbol& symbol, llvm::DWARFContext* dwarf,
                                         bool emitRegionSymbols, const llvm::StringSet<>& functionNames) {
	const uint64_t address = symbol.getAddress().getValue();
	const uint64_t size = symbol.getSize();
	const auto name = symbol.getName() ? llvm::StringRef(*symbol.getName()) : llvm::StringRef();

	std::vector<CodeRange> ranges;
	if (dwarf == nullptr) {
		ranges.push_back({address, address + size, name.str(), {}});
		return ranges;
	}

	const auto sectionIndex = symbol.getBlock().getSection().getOrdinal();
	const auto sectioned = llvm::object::SectionedAddress {address, sectionIndex};
	auto table = dwarf->getLineInfoForAddressRange(sectioned, size,
	                                              llvm::DILineInfoSpecifier::FileLineInfoKind::AbsoluteFilePath);

	if (table.empty()) {
		ranges.push_back({address, address + size, name.str(), {}});
		return ranges;
	}

	for (const auto& [rowAddress, lineInfo] : table) {
		std::string rangeName = name.str();
		if (emitRegionSymbols) {
			// FunctionNameKind must be requested explicitly -- it defaults to
			// None, which leaves every frame's FunctionName as "<invalid>"
			// and so silently yields no region names at all.
			auto stack = dwarf->getInliningInfoForAddress(
			    llvm::object::SectionedAddress {rowAddress, sectionIndex},
			    llvm::DILineInfoSpecifier {llvm::DILineInfoSpecifier::FileLineInfoKind::AbsoluteFilePath,
			                               llvm::DINameKind::ShortName});
			rangeName = qualifiedName(name, stack, functionNames);
		}
		if (ranges.empty() || ranges.back().name != rangeName) {
			if (!ranges.empty()) {
				ranges.back().end = rowAddress;
			}
			ranges.push_back({rowAddress, address + size, std::move(rangeName), {}});
		}
		ranges.back().lines.push_back(
		    PerfJITDebugEntry {rowAddress, lineInfo.Line, lineInfo.Column, lineInfo.FileName});
	}

	// The line table's first row can sit past the symbol's entry point (the
	// prologue carries no location of its own). Pull the first range back so
	// the ranges cover the symbol from its start; otherwise samples in the
	// prologue land in no record at all and perf reports them as unknown.
	ranges.front().start = address;
	ranges.back().end = address + size;
	return ranges;
}

// The jitdump writer takes each record's `TotalSize` as given rather than
// recomputing it, and a zero makes `perf inject` stop at the first record, so
// these mirror the on-disk layout exactly (jitdump-specification.txt):
// a 16-byte record header (id, total_size, timestamp) plus the body.
constexpr uint32_t kRecordHeaderSize = 16;

PerfJITCodeLoadRecord makeCodeLoadRecord(const CodeRange& range, uint64_t codeIndex) {
	PerfJITCodeLoadRecord record;
	record.Prefix.Id = PerfJITRecordType::JIT_CODE_LOAD;
	record.Prefix.TotalSize = 0; // filled in by the jitdump writer
	record.Pid = static_cast<uint32_t>(llvm::sys::Process::getProcessId());
	record.Tid = static_cast<uint32_t>(llvm::get_threadid());
	record.Vma = range.start;
	record.CodeAddr = range.start;
	record.CodeSize = range.end - range.start;
	record.CodeIndex = codeIndex;
	record.Name = range.name;
	// body: pid, tid (4+4) + vma, code_addr, code_size, code_index (4*8),
	// then the NUL-terminated name, then the machine code itself.
	record.Prefix.TotalSize = static_cast<uint32_t>(kRecordHeaderSize + 40 + record.Name.size() + 1 + record.CodeSize);
	return record;
}

PerfJITDebugInfoRecord makeDebugInfoRecord(const CodeRange& range) {
	PerfJITDebugInfoRecord record;
	record.Prefix.Id = PerfJITRecordType::JIT_CODE_DEBUG_INFO;
	record.Prefix.TotalSize = 0;
	record.CodeAddr = range.start;
	record.Entries = range.lines;
	// body: code_addr, nr_entry (2*8), then per entry addr, line, discrim
	// (8+4+4) and the NUL-terminated file name.
	uint64_t total = kRecordHeaderSize + 16;
	for (const auto& entry : record.Entries) {
		total += 16 + entry.Name.size() + 1;
	}
	record.Prefix.TotalSize = static_cast<uint32_t>(total);
	return record;
}

// .eh_frame, so `perf record --call-graph dwarf` can walk out of a JIT frame.
// Mirrors PerfSupportPlugin's own unwinding record; emitted once per object
// rather than per range, since it describes the whole linked object.
PerfJITCodeUnwindingInfoRecord makeUnwindingRecord(llvm::jitlink::LinkGraph& graph) {
	PerfJITCodeUnwindingInfoRecord record {};
	record.Prefix.Id = PerfJITRecordType::JIT_CODE_UNWINDING_INFO;
	record.Prefix.TotalSize = 0;

	auto* ehFrame = graph.findSectionByName(".eh_frame");
	if (ehFrame == nullptr) {
		return record;
	}
	const auto ehFrameRange = llvm::jitlink::SectionRange(*ehFrame);
	record.EHFrameAddr = ehFrameRange.getStart().getValue();
	record.UnwindDataSize = ehFrameRange.getSize();
	// Zero means "not mapped": perf reads the data out of the record rather
	// than from a mapping of the JIT'd object.
	record.MappedSize = 0;

	auto* ehFrameHdr = graph.findSectionByName(".eh_frame_hdr");
	if (ehFrameHdr == nullptr) {
		record.EHFrameHdrAddr = 0;
		record.EHFrameHdrSize = 0;
		record.Prefix.TotalSize = static_cast<uint32_t>(kRecordHeaderSize + 24 + record.UnwindDataSize);
		return record;
	}
	const auto ehFrameHdrRange = llvm::jitlink::SectionRange(*ehFrameHdr);
	record.EHFrameHdrAddr = ehFrameHdrRange.getStart().getValue();
	record.EHFrameHdrSize = ehFrameHdrRange.getSize();
	record.UnwindDataSize += ehFrameHdrRange.getSize();
	// body: unwind_data_size, eh_frame_hdr_size, mapped_size (3*8), then the
	// unwind payload itself.
	record.Prefix.TotalSize = static_cast<uint32_t>(kRecordHeaderSize + 24 + record.UnwindDataSize);
	return record;
}

// PerfJITRecordBatch's unwinding record is a POD with no default member
// initializers, so a default-constructed batch carries an uninitialized one
// that gets serialized along with everything else -- the jitdump writer then
// reads garbage sizes and walks off the end. Every batch must start from a
// zeroed record; only the batch that actually carries unwind info fills it in.
PerfJITRecordBatch makeBatch() {
	PerfJITRecordBatch batch;
	batch.UnwindingRecord = PerfJITCodeUnwindingInfoRecord {};
	batch.UnwindingRecord.Prefix.Id = PerfJITRecordType::JIT_CODE_UNWINDING_INFO;
	batch.UnwindingRecord.Prefix.TotalSize = 0;
	return batch;
}

// A wrapper call has two error channels: the transport itself can fail, and
// the wrapper can return an Error of its own (SPS deserializes that into the
// result reference, which must be consumed either way). Fold both into one.
llvm::Error callVoidWrapper(llvm::orc::ExecutorProcessControl& epc, llvm::orc::ExecutorAddr address) {
	llvm::Error wrapperError = llvm::Error::success();
	if (auto transportError = epc.callSPSWrapper<llvm::orc::shared::SPSError()>(address, wrapperError)) {
		llvm::consumeError(std::move(wrapperError));
		return transportError;
	}
	return wrapperError;
}

llvm::Error callBatchWrapper(llvm::orc::ExecutorProcessControl& epc, llvm::orc::ExecutorAddr address,
                             const PerfJITRecordBatch& batch) {
	llvm::Error wrapperError = llvm::Error::success();
	if (auto transportError =
	        epc.callSPSWrapper<llvm::orc::shared::SPSError(llvm::orc::shared::SPSPerfJITRecordBatch)>(
	            address, wrapperError, batch)) {
		llvm::consumeError(std::move(wrapperError));
		return transportError;
	}
	return wrapperError;
}

} // namespace

PerfJitDumpPlugin::PerfJitDumpPlugin(llvm::orc::ExecutorProcessControl& epc, llvm::orc::ExecutorAddr registerStart,
                                     llvm::orc::ExecutorAddr registerEnd, llvm::orc::ExecutorAddr registerImpl,
                                     bool emitDebugInfo, bool emitUnwindInfo, bool emitRegionSymbols)
    : epc_(epc), registerEnd_(registerEnd), registerImpl_(registerImpl), emitDebugInfo_(emitDebugInfo),
      emitUnwindInfo_(emitUnwindInfo), emitRegionSymbols_(emitRegionSymbols) {
	// Opens the jitdump file and its marker mapping. perf discovers the file
	// from the mmap event this produces, so it has to happen before any
	// record is written.
	if (auto error = callVoidWrapper(epc_, registerStart)) {
		llvm::errs() << "nautilus: could not start the perf jitdump writer: " << llvm::toString(std::move(error))
		             << "\n";
	}
}

PerfJitDumpPlugin::~PerfJitDumpPlugin() {
	// Appends the JIT_CODE_CLOSE record `perf inject` uses to treat the dump
	// as complete.
	if (auto error = callVoidWrapper(epc_, registerEnd_)) {
		llvm::consumeError(std::move(error));
	}
}

void PerfJitDumpPlugin::modifyPassConfig(llvm::orc::MaterializationResponsibility&, llvm::jitlink::LinkGraph&,
                                         llvm::jitlink::PassConfiguration& config) {
	config.PostFixupPasses.push_back([this](llvm::jitlink::LinkGraph& graph) -> llvm::Error {
		std::unique_ptr<llvm::DWARFContext> dwarf;
		llvm::StringMap<std::unique_ptr<llvm::MemoryBuffer>> debugSections;
		if (emitDebugInfo_) {
			auto context = llvm::orc::createDWARFContext(graph);
			if (!context) {
				// No DWARF is not fatal: fall back to symbols without line
				// tables rather than failing the link.
				llvm::consumeError(context.takeError());
			} else {
				std::tie(dwarf, debugSections) = std::move(*context);
			}
		}

		// One batch per code range. JITLoaderPerf writes a batch's debug
		// records before its code records, so a batch holding exactly one of
		// each is what puts them adjacent in the file -- which is the whole
		// point (see the header).
		llvm::StringSet<> functionNames;
		for (auto* symbol : graph.defined_symbols()) {
			if (symbol->hasName()) {
				functionNames.insert(llvm::StringRef(*symbol->getName()));
			}
		}

		for (auto* symbol : graph.defined_symbols()) {
			if (!symbol->hasName() || !symbol->isCallable() || symbol->getSize() == 0) {
				continue;
			}
			for (const auto& range : computeCodeRanges(*symbol, dwarf.get(), emitRegionSymbols_, functionNames)) {
				if (range.end <= range.start) {
					continue;
				}
				auto batch = makeBatch();
				if (emitDebugInfo_ && !range.lines.empty()) {
					batch.DebugInfoRecords.push_back(makeDebugInfoRecord(range));
				}
				batch.CodeLoadRecords.push_back(makeCodeLoadRecord(range, codeIndex_++));
				if (auto error = callBatchWrapper(epc_, registerImpl_, batch)) {
					return error;
				}
			}
		}

		if (emitUnwindInfo_) {
			auto batch = makeBatch();
			batch.UnwindingRecord = makeUnwindingRecord(graph);
			if (batch.UnwindingRecord.UnwindDataSize > 0) {
				if (auto error = callBatchWrapper(epc_, registerImpl_, batch)) {
					return error;
				}
			}
		}
		return llvm::Error::success();
	});
}

} // namespace nautilus::compiler::mlir

#endif // __linux__
