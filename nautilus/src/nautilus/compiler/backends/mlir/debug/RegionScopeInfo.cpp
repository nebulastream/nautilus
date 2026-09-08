
#include "nautilus/compiler/backends/mlir/debug/RegionScopeInfo.hpp"
#include <mlir/IR/BuiltinAttributes.h>

namespace nautilus::compiler::mlir {

namespace {

// Tags the top-level FusedLoc produced by attachRegionScope() so
// findRegionScopeChain() can find it unambiguously among any other fused
// location metadata a Location tree may carry (e.g. the DISubprogramAttr
// that DIScopeForLLVMFuncOpPass later attaches to the function's own
// location, or the per-block DILexicalBlockAttr EmitDbgValuePass fuses onto
// every op once block scoping has been resolved).
constexpr llvm::StringLiteral kRegionScopeMarker = "nautilus.region.scope";

} // namespace

::mlir::LocationAttr buildRegionScopeChain(::mlir::MLIRContext* ctx, llvm::StringRef name, llvm::StringRef file,
                                           unsigned line, unsigned column, ::mlir::LocationAttr parentChain) {
	auto fileAttr = ::mlir::StringAttr::get(ctx, file);
	auto fileLineCol = ::mlir::FileLineColLoc::get(fileAttr, line, column);
	auto nameLoc = ::mlir::NameLoc::get(::mlir::StringAttr::get(ctx, name), fileLineCol);
	if (!parentChain) {
		return nameLoc;
	}
	return ::mlir::FusedLoc::get({::mlir::Location(nameLoc), ::mlir::Location(parentChain)}, ::mlir::Attribute(), ctx);
}

::mlir::Location attachRegionScope(::mlir::MLIRContext* ctx, ::mlir::Location base, ::mlir::LocationAttr regionChain) {
	if (!regionChain) {
		return base;
	}
	return ::mlir::FusedLoc::get({base, ::mlir::Location(regionChain)},
	                             ::mlir::StringAttr::get(ctx, kRegionScopeMarker), ctx);
}

::mlir::LocationAttr findRegionScopeChain(::mlir::Location loc) {
	::mlir::LocationAttr result;
	loc->walk([&](::mlir::Location inner) {
		if (auto fused = llvm::dyn_cast<::mlir::FusedLoc>(inner)) {
			if (auto marker = llvm::dyn_cast_or_null<::mlir::StringAttr>(fused.getMetadata())) {
				if (marker.getValue() == kRegionScopeMarker && fused.getLocations().size() > 1) {
					result = fused.getLocations()[1];
					return ::mlir::WalkResult::interrupt();
				}
			}
		}
		return ::mlir::WalkResult::advance();
	});
	return result;
}

namespace {

// Resolves one region chain node -- either the bare leaf NameLoc a region
// with no parent builds, or the FusedLoc(leaf, parentChain) a nested region
// builds -- into its DWARF scope, recursing into the parent chain first so
// outer regions are always constructed before the scopes that nest in them.
::mlir::LLVM::DIScopeAttr resolveChainNode(::mlir::MLIRContext* ctx, ::mlir::LocationAttr chain,
                                           ::mlir::LLVM::DISubprogramAttr subprogram,
                                           ::mlir::LLVM::DIFileAttr functionFile) {
	::mlir::LocationAttr parentChain;
	auto nameLoc = llvm::dyn_cast<::mlir::NameLoc>(chain);
	if (!nameLoc) {
		auto fused = llvm::dyn_cast<::mlir::FusedLoc>(chain);
		if (!fused || fused.getLocations().size() != 2) {
			return subprogram;
		}
		nameLoc = llvm::dyn_cast<::mlir::NameLoc>(fused.getLocations()[0]);
		parentChain = fused.getLocations()[1];
	}
	auto fileLineCol =
	    nameLoc ? llvm::dyn_cast<::mlir::FileLineColLoc>(nameLoc.getChildLoc()) : ::mlir::FileLineColLoc();
	if (!fileLineCol) {
		return subprogram;
	}
	::mlir::LLVM::DIScopeAttr parentScope =
	    parentChain ? resolveChainNode(ctx, parentChain, subprogram, functionFile) : subprogram;

	// Deliberately always DILexicalBlockAttr parented with `functionFile` --
	// never DILexicalBlockFileAttr, and never the region's own real source
	// file either, even though that file is exactly what a user would expect
	// to see on this scope.
	//
	// DILexicalBlockFileAttr looks like the obvious fit for "this scope's
	// file differs from its enclosing one", but LLVM's LexicalScopes builder
	// unwraps it unconditionally via DILocalScope::getNonLexicalBlockFileScope()
	// before constructing the DWARF scope tree -- whether it is used as an
	// op's own scope or only as another scope's parent -- so it can never
	// surface as its own DW_TAG_lexical_block.
	//
	// A plain DILexicalBlockAttr does materialize, but a DILocation has no
	// file of its own: every op nested under this scope resolves its file by
	// walking up to here, and those ops' line numbers are always relative to
	// the function's own file (the Nautilus IR dump or MLIR snapshot -- see
	// DebugInfoOptions.hpp), never to the region's real source file. Giving
	// this scope the region's real file would silently reinterpret every
	// nested op's dump-relative line as a line in that unrelated file instead
	// (verified: GDB then "steps" through arbitrary lines of the user's C++
	// source that have nothing to do with the region). The region's real
	// file/line is still readable in a plain MLIR dump via the chain's own
	// NameLoc (see attachRegionScope) -- it just cannot safely become this
	// scope's DWARF file.
	return ::mlir::LLVM::DILexicalBlockAttr::get(ctx, parentScope, functionFile, /*line=*/0, /*column=*/0);
}

} // namespace

::mlir::LLVM::DIScopeAttr resolveRegionScope(::mlir::MLIRContext* ctx, ::mlir::LocationAttr regionChain,
                                             ::mlir::LLVM::DISubprogramAttr subprogram,
                                             ::mlir::LLVM::DIFileAttr functionFile) {
	if (!regionChain) {
		return subprogram;
	}
	return resolveChainNode(ctx, regionChain, subprogram, functionFile);
}

} // namespace nautilus::compiler::mlir
