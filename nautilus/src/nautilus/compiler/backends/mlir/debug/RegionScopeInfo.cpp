
#include "nautilus/compiler/backends/mlir/debug/RegionScopeInfo.hpp"
#include <llvm/Support/Path.h>
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

	llvm::StringRef regionFileName = fileLineCol.getFilename().getValue();
	if (regionFileName.empty() || regionFileName == functionFile.getName().getValue()) {
		return ::mlir::LLVM::DILexicalBlockAttr::get(ctx, parentScope, functionFile, fileLineCol.getLine(),
		                                             fileLineCol.getColumn());
	}
	auto regionFile = ::mlir::LLVM::DIFileAttr::get(ctx, llvm::sys::path::filename(regionFileName),
	                                                llvm::sys::path::parent_path(regionFileName));
	return ::mlir::LLVM::DILexicalBlockFileAttr::get(ctx, parentScope, regionFile, /*discriminator=*/0);
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
