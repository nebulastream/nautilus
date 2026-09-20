#include "nautilus/compiler/backends/mlir/debug/JitSymbolRegistrationPlugin.hpp"

#if defined(__linux__)

#include "nautilus/compiler/JitSymbolRegistry.hpp"
#include "nautilus/compiler/backends/mlir/debug/JitCodeRanges.hpp"
#include <algorithm>
#include <llvm/DebugInfo/DWARF/DWARFContext.h>
#include <llvm/ExecutionEngine/Orc/Debugging/DebugInfoSupport.h>
#include <utility>
#include <vector>

namespace nautilus::compiler::mlir {

JitSymbolRegistrationPlugin::JitSymbolRegistrationPlugin(bool emitRegionSymbols, ModuleIndex moduleIndex)
    : emitRegionSymbols_(emitRegionSymbols), moduleIndex_(moduleIndex) {
}

void JitSymbolRegistrationPlugin::modifyPassConfig(llvm::orc::MaterializationResponsibility&, llvm::jitlink::LinkGraph&,
                                                   llvm::jitlink::PassConfiguration& config) {
	config.PostFixupPasses.push_back([this](llvm::jitlink::LinkGraph& graph) -> llvm::Error {
		// PostFixup, like the jitdump writer: addresses are final by then, and
		// the .debug_* sections are still around because
		// DebugInfoPreservationPlugin (installed alongside) keeps JITLink from
		// pruning them.
		std::unique_ptr<llvm::DWARFContext> dwarf;
		llvm::StringMap<std::unique_ptr<llvm::MemoryBuffer>> debugSections;
		auto context = llvm::orc::createDWARFContext(graph);
		if (!context) {
			// No DWARF is not fatal: without it every symbol yields a single
			// unqualified range, which still names JIT frames in a profile --
			// just without resolving region() scopes inside them.
			llvm::consumeError(context.takeError());
		} else {
			std::tie(dwarf, debugSections) = std::move(*context);
		}

		const llvm::StringSet<> functionNames = collectFunctionNames(graph);

		std::vector<JitSymbol> published;
		// The line table the ranges already carry, kept so a profiler can
		// attribute a sample to the Nautilus-IR operation it came from and not
		// just to the enclosing region. One row per IR operation, so it is only
		// worth carrying because the DWARF was parsed for the names anyway.
		std::vector<JitLine> publishedLines;
		std::string sourceFile;

		for (auto* symbol : graph.defined_symbols()) {
			if (!symbol->hasName() || !symbol->isCallable() || symbol->getSize() == 0) {
				continue;
			}
			for (auto& range : computeCodeRanges(*symbol, dwarf.get(), emitRegionSymbols_, functionNames)) {
				if (range.end <= range.start) {
					continue;
				}
				for (const auto& line : range.lines) {
					publishedLines.push_back(
					    JitLine {static_cast<uintptr_t>(line.Addr), static_cast<uint32_t>(line.Lineno), moduleIndex_});
					if (sourceFile.empty()) {
						// A nautilus compile points every line at one IR dump, so
						// the first row's file names the whole module's.
						sourceFile = line.Name;
					}
				}
				published.push_back(JitSymbol {static_cast<uintptr_t>(range.start), static_cast<uintptr_t>(range.end),
				                               std::move(range.name), moduleIndex_});
			}
		}

		auto& registry = JitSymbolRegistry::instance();
		registry.addAll(std::move(published));
		// After the ranges: resolveLine() bounds a line lookup by the range that
		// covers the address, so the ranges have to be there first.
		std::sort(publishedLines.begin(), publishedLines.end(),
		          [](const JitLine& left, const JitLine& right) { return left.address < right.address; });
		registry.addLines(std::move(publishedLines), std::move(sourceFile), moduleIndex_);
		return llvm::Error::success();
	});
}

} // namespace nautilus::compiler::mlir

#endif // __linux__
