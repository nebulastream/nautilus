#pragma once

#if defined(__linux__)

#include <cstdint>
#include <llvm/ADT/StringSet.h>
#include <llvm/DebugInfo/DWARF/DWARFContext.h>
#include <llvm/ExecutionEngine/JITLink/JITLink.h>
#include <llvm/ExecutionEngine/Orc/Shared/PerfSharedStructs.h>
#include <string>
#include <vector>

namespace nautilus::compiler::mlir {

// One contiguous span of a JIT-linked function's code whose every instruction
// shares an inline (region) stack, together with the name it should appear
// under in a profile.
//
// This is the unit both consumers of the backend's DWARF need: the jitdump
// writer turns one into a JIT_CODE_LOAD record for an out-of-process `perf
// record`, and the JIT symbol registry publishes one so an in-process profiler
// can resolve a sampled instruction pointer without perf's tooling at all.
struct JitCodeRange {
	uint64_t start = 0;
	/// Exclusive.
	uint64_t end = 0;
	/// Region-qualified, e.g. `execute::query::aggregate::mix`.
	std::string name;
	/// Line-table rows falling in [start, end). Empty when the caller asked for
	/// no debug info, or when the object carried no usable DWARF.
	std::vector<llvm::orc::PerfJITDebugEntry> lines;
};

// `execute::outer::hot` for an address inside region "hot" nested in "outer",
// or just the symbol's own name where no region() encloses it.
//
// DWARF inline frames come back leaf-to-root, with the last frame being the
// function itself; the region levels are everything below it, re-ordered
// outermost-first so the qualified name reads the way the source nests. The
// symbol's own name is used as the base rather than the DWARF function name:
// the two agree for `execute`, but the symbol name is what perf would have
// shown anyway, so a range with no regions keeps exactly the name it always had.
//
// @param functionNames every defined symbol in the link graph, used to tell a
//        region frame from a genuinely inlined function call; see the
//        implementation for why DWARF offers nothing more direct.
std::string qualifiedName(llvm::StringRef symbolName, const llvm::DIInliningInfo& inlineStack,
                          const llvm::StringSet<>& functionNames);

// Splits one symbol into the code ranges it should be reported as.
//
// Without region symbols (or without debug info to derive them from) this is
// always a single range covering the whole symbol.
//
// With them, the line table supplies the address grid: DWARF emits a row
// wherever the location changes, inlinedAt included, so consecutive rows
// sharing an inline stack delimit a region's extent. Ranges partition the
// symbol and so stay disjoint, which the jitdump format requires -- a region
// whose code the optimizer has interleaved with its siblings simply yields
// several ranges that share a name rather than one.
std::vector<JitCodeRange> computeCodeRanges(const llvm::jitlink::Symbol& symbol, llvm::DWARFContext* dwarf,
                                            bool emitRegionSymbols, const llvm::StringSet<>& functionNames);

// The names of every defined symbol in @p graph, as `computeCodeRanges` wants
// them.
llvm::StringSet<> collectFunctionNames(llvm::jitlink::LinkGraph& graph);

} // namespace nautilus::compiler::mlir

#endif // __linux__
