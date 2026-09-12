#pragma once

#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/StringRef.h>
#include <mlir/Dialect/LLVMIR/LLVMAttrs.h>
#include <mlir/IR/Location.h>

namespace nautilus::compiler::mlir {

// Encodes the nesting of Nautilus region() scopes (docs/region.md) into plain
// MLIR Locations at MLIR-generation time, so the information survives dialect
// conversion (every conversion pattern preserves an op's location, which is
// exactly how the rest of the debug-info pipeline already gets away with
// tagging ops before DISubprograms exist) and can be recovered afterwards, once
// DIScopeForLLVMFuncOpPass has materialized a DISubprogram for the function.
//
// A region chain is a LocationAttr built by buildRegionScopeChain(): the
// innermost region is a NameLoc(name, FileLineColLoc(file, line, column)),
// optionally fused with its parent's chain (built the same way) when the
// region is nested. It is a LocationAttr rather than a Location because,
// unlike Location, LocationAttr is nullable -- exactly what "no enclosing
// region" needs. No Nautilus IR object needs to be threaded across the
// MLIRLoweringProvider / MLIR-pass boundary -- the chain is self-contained.
//
// A region is lowered to a DWARF *inlined subroutine*, not a plain lexical
// block: a synthetic, function-shaped DISubprogramAttr stands in for the
// region, and wrapOpForRegionInlining() wraps each op's location in a
// CallSiteLoc chain -- MLIR's builtin representation of an inlined call
// stack, which LLVM's DWARF backend turns into the DILocation/`inlinedAt`
// chain that makes gdb show an inlined frame. This is deliberate: a plain
// DILexicalBlock scope, as an earlier version of this file used, never shows
// up as its own entry in `bt` -- lexical blocks aren't call boundaries, and
// gdb only prints one line per frame (a real call, or a DW_TAG_inlined_
// subroutine masquerading as one). Modeling region() as "as if inlined"
// reuses the exact mechanism gdb already has for showing an -O2-inlined C++
// function as its own backtrace line, giving `region("accumulate", ...)` a
// real `#0 accumulate () at ... / #1 execute () at ...` frame pair.

// Builds (or extends) the chain for one region level. `parentChain` is the
// enclosing region's own chain (built by a previous call to this function),
// or a null LocationAttr for a region opened directly in the function body --
// i.e. one whose RegionSpec::parent is NO_REGION.
::mlir::LocationAttr buildRegionScopeChain(::mlir::MLIRContext* ctx, llvm::StringRef name, llvm::StringRef file,
                                           unsigned line, unsigned column, ::mlir::LocationAttr parentChain);

// Fuses `regionChain` onto `base` so the region nesting travels with the op's
// location. Returns `base` unchanged when `regionChain` is null (NO_REGION),
// so callers can use the result unconditionally.
::mlir::Location attachRegionScope(::mlir::MLIRContext* ctx, ::mlir::Location base, ::mlir::LocationAttr regionChain);

// Recovers the region chain fused onto `loc` by attachRegionScope(), or a
// null LocationAttr if none is present (the op was not traced inside a
// region()).
::mlir::LocationAttr findRegionScopeChain(::mlir::Location loc);

// Per-function cache of the synthetic DISubprogramAttr built for each region
// level (see resolveInnermostRegionSubprogram / wrapOpForRegionInlining),
// keyed by that level's own chain node. Repeated resolution of the same
// region must reuse one DISubprogram instance rather than minting a fresh
// `distinct` one each time -- gdb's inlined-frame handling keys off DISub-
// program identity, so a fresh one per call would make every reference look
// like a different "function".
using RegionSubprogramCache = llvm::DenseMap<::mlir::Attribute, ::mlir::LLVM::DISubprogramAttr>;

// Resolves (and memoizes in `cache`) the synthetic DISubprogramAttr standing
// in for the innermost region in `regionChain`. Always filed under
// `functionFile` and given `subprogram`'s own (already-verified-non-zero)
// line: a DILocation has no file of its own, so anything scoped under this
// subprogram -- including the region's own local variables -- would have its
// dump-relative line silently reinterpreted against a different file if this
// used the region's real source file/line instead (see
// wrapOpForRegionInlining). Returns `subprogram` itself for a null chain.
::mlir::LLVM::DISubprogramAttr resolveInnermostRegionSubprogram(::mlir::MLIRContext* ctx,
                                                                ::mlir::LocationAttr regionChain,
                                                                ::mlir::LLVM::DISubprogramAttr subprogram,
                                                                ::mlir::LLVM::DIFileAttr functionFile,
                                                                RegionSubprogramCache& cache);

// Wraps `opLoc` (an op's real, dump-relative location, untouched) so it reads
// as having been inlined from region() scope(s) all the way out to
// `subprogram` itself: one MLIR CallSiteLoc per nesting level, innermost
// first, translating to the DILocation/`inlinedAt` chain LLVM's DWARF backend
// needs to synthesize a DW_TAG_inlined_subroutine per level. Every level but
// the innermost has no real "call site" to point at -- region() bodies are
// traced inline, never actually called -- so ancestor levels reuse
// `subprogram`'s own safe entry line as a placeholder, always filed under
// `functionFile` for the same file-inheritance reason as
// resolveInnermostRegionSubprogram. Returns `opLoc` fused directly onto
// `subprogram` unchanged for a null chain, so a block with no enclosing
// region keeps today's behaviour.
::mlir::Location wrapOpForRegionInlining(::mlir::MLIRContext* ctx, ::mlir::Location opLoc,
                                         ::mlir::LocationAttr regionChain, ::mlir::LLVM::DISubprogramAttr subprogram,
                                         ::mlir::LLVM::DIFileAttr functionFile, RegionSubprogramCache& cache);

// Removes the region-scope marker attachRegionScope() added, returning the
// op's plain underlying location.
//
// The marker FusedLoc holds two children: the op's real location and the
// region chain. MLIR's debug translation cannot pick a line from a multi-child
// FusedLoc and emits `line: 0`, so a still-marked location costs every op in
// the region its line number. Strip first, wrap second.
::mlir::Location stripRegionScope(::mlir::Location loc);

} // namespace nautilus::compiler::mlir
