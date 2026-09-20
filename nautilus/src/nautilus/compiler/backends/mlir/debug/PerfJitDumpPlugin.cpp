#include "nautilus/compiler/backends/mlir/debug/PerfJitDumpPlugin.hpp"
#include "nautilus/compiler/backends/mlir/debug/JitCodeRanges.hpp"

#if defined(__linux__)

#include <llvm/ADT/StringSet.h>
#include <llvm/DebugInfo/DWARF/DWARFContext.h>
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

// The jitdump writer takes each record's `TotalSize` as given rather than
// recomputing it, and a zero makes `perf inject` stop at the first record, so
// these mirror the on-disk layout exactly (jitdump-specification.txt):
// a 16-byte record header (id, total_size, timestamp) plus the body.
constexpr uint32_t kRecordHeaderSize = 16;

PerfJITCodeLoadRecord makeCodeLoadRecord(const JitCodeRange& range, uint64_t codeIndex) {
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

PerfJITDebugInfoRecord makeDebugInfoRecord(const JitCodeRange& range) {
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
	if (auto transportError = epc.callSPSWrapper<llvm::orc::shared::SPSError(llvm::orc::shared::SPSPerfJITRecordBatch)>(
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
		const llvm::StringSet<> functionNames = collectFunctionNames(graph);

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
