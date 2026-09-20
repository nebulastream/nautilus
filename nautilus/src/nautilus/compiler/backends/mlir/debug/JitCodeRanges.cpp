#include "nautilus/compiler/backends/mlir/debug/JitCodeRanges.hpp"

#if defined(__linux__)

#include <utility>

namespace nautilus::compiler::mlir {

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

std::vector<JitCodeRange> computeCodeRanges(const llvm::jitlink::Symbol& symbol, llvm::DWARFContext* dwarf,
                                            bool emitRegionSymbols, const llvm::StringSet<>& functionNames) {
	const uint64_t address = symbol.getAddress().getValue();
	const uint64_t size = symbol.getSize();
	const auto name = symbol.getName() ? llvm::StringRef(*symbol.getName()) : llvm::StringRef();

	std::vector<JitCodeRange> ranges;
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
		    llvm::orc::PerfJITDebugEntry {rowAddress, lineInfo.Line, lineInfo.Column, lineInfo.FileName});
	}

	// The line table's first row can sit past the symbol's entry point (the
	// prologue carries no location of its own). Pull the first range back so
	// the ranges cover the symbol from its start; otherwise samples in the
	// prologue land in no record at all and perf reports them as unknown.
	ranges.front().start = address;
	ranges.back().end = address + size;
	return ranges;
}

llvm::StringSet<> collectFunctionNames(llvm::jitlink::LinkGraph& graph) {
	llvm::StringSet<> functionNames;
	for (const auto* symbol : graph.defined_symbols()) {
		if (symbol->hasName()) {
			functionNames.insert(llvm::StringRef(*symbol->getName()));
		}
	}
	return functionNames;
}

} // namespace nautilus::compiler::mlir

#endif // __linux__
