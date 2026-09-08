#pragma once

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
// MLIRLoweringProvider / MLIR-pass boundary -- the chain is self-contained and
// resolveRegionScope() turns it into DWARF scopes without consulting anything
// but the Location tree itself.

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

// Resolves a region chain (as produced by buildRegionScopeChain) into a DWARF
// scope: one DILexicalBlockAttr per nesting level, always filed under
// `functionFile` -- deliberately never DILexicalBlockFileAttr (which looks
// like the obvious fit for a scope whose file differs from its parent's, but
// which LLVM's LexicalScopes builder unwraps unconditionally via
// DILocalScope::getNonLexicalBlockFileScope() before constructing the DWARF
// scope tree, so it can never surface as its own DW_TAG_lexical_block), and
// deliberately never the region's own real source file either: a DILocation
// has no file of its own, so every op nested under this scope would have its
// dump-relative line silently reinterpreted as a line in that unrelated real
// file. The outermost level is parented on `subprogram`. Returns `subprogram`
// itself for a null chain, so a block or op with no enclosing region keeps
// today's behaviour unchanged.
::mlir::LLVM::DIScopeAttr resolveRegionScope(::mlir::MLIRContext* ctx, ::mlir::LocationAttr regionChain,
                                             ::mlir::LLVM::DISubprogramAttr subprogram,
                                             ::mlir::LLVM::DIFileAttr functionFile);

} // namespace nautilus::compiler::mlir
