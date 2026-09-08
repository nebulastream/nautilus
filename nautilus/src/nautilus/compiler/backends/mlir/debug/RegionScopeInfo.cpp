
#include "nautilus/compiler/backends/mlir/debug/RegionScopeInfo.hpp"
#include <mlir/IR/BuiltinAttributes.h>

namespace nautilus::compiler::mlir {

namespace {

// Tags the top-level FusedLoc produced by attachRegionScope() so
// findRegionScopeChain() can find it unambiguously among any other fused
// location metadata a Location tree may carry (e.g. the DISubprogramAttr
// that DIScopeForLLVMFuncOpPass later attaches to the function's own
// location, or the scope FusedLocs wrapOpForRegionInlining() itself builds).
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

// One region chain node, split into its leaf NameLoc (name + FileLineColLoc)
// and, when nested, the parent's own chain node. `leaf` is null when `chain`
// isn't shaped like something buildRegionScopeChain() would produce.
struct ChainNode {
	::mlir::NameLoc leaf;
	::mlir::LocationAttr parent;
};

ChainNode decomposeChain(::mlir::LocationAttr chain) {
	if (auto nameLoc = llvm::dyn_cast<::mlir::NameLoc>(chain)) {
		return {nameLoc, nullptr};
	}
	if (auto fused = llvm::dyn_cast<::mlir::FusedLoc>(chain); fused && fused.getLocations().size() == 2) {
		if (auto nameLoc = llvm::dyn_cast<::mlir::NameLoc>(fused.getLocations()[0])) {
			return {nameLoc, fused.getLocations()[1]};
		}
	}
	return {};
}

} // namespace

::mlir::LLVM::DISubprogramAttr resolveInnermostRegionSubprogram(::mlir::MLIRContext* ctx,
                                                                ::mlir::LocationAttr regionChain,
                                                                ::mlir::LLVM::DISubprogramAttr subprogram,
                                                                ::mlir::LLVM::DIFileAttr functionFile,
                                                                RegionSubprogramCache& cache) {
	if (!regionChain) {
		return subprogram;
	}
	if (auto it = cache.find(regionChain); it != cache.end()) {
		return it->second;
	}
	auto node = decomposeChain(regionChain);
	if (!node.leaf) {
		return subprogram;
	}
	llvm::StringRef name = node.leaf.getName().strref();
	auto displayName = ::mlir::StringAttr::get(ctx, name.empty() ? llvm::StringRef("<region>") : name);
	auto subroutineType = ::mlir::LLVM::DISubroutineTypeAttr::get(ctx, /*types=*/ {});
	auto regionSubprogram = ::mlir::LLVM::DISubprogramAttr::get(
	    ctx, ::mlir::DistinctAttr::create(::mlir::UnitAttr::get(ctx)), subprogram.getCompileUnit(),
	    /*scope=*/functionFile, displayName, /*linkageName=*/displayName, functionFile, subprogram.getLine(),
	    subprogram.getLine(), ::mlir::LLVM::DISubprogramFlags::Definition, subroutineType,
	    /*retainedNodes=*/ {}, /*annotations=*/ {});
	cache[regionChain] = regionSubprogram;
	return regionSubprogram;
}

namespace {

// Builds the synthetic "call site" location for one ancestor region level.
// There is no real per-transition call line to recover -- region() bodies
// are traced inline, not called -- so every level reuses `subprogram`'s own
// safe entry line, always filed under `functionFile` (see
// resolveInnermostRegionSubprogram's doc comment for why).
::mlir::Location buildCallerChain(::mlir::MLIRContext* ctx, ::mlir::LocationAttr regionChain,
                                  ::mlir::LLVM::DISubprogramAttr subprogram, ::mlir::LLVM::DIFileAttr functionFile,
                                  RegionSubprogramCache& cache) {
	auto entryLoc = ::mlir::FileLineColLoc::get(functionFile.getName(), subprogram.getLine(), 1);
	if (!regionChain) {
		return ::mlir::FusedLoc::get({::mlir::Location(entryLoc)}, subprogram, ctx);
	}
	auto node = decomposeChain(regionChain);
	if (!node.leaf) {
		return ::mlir::FusedLoc::get({::mlir::Location(entryLoc)}, subprogram, ctx);
	}
	auto regionSubprogram = resolveInnermostRegionSubprogram(ctx, regionChain, subprogram, functionFile, cache);
	auto callerLoc = buildCallerChain(ctx, node.parent, subprogram, functionFile, cache);
	return ::mlir::CallSiteLoc::get(::mlir::FusedLoc::get({::mlir::Location(entryLoc)}, regionSubprogram, ctx),
	                                callerLoc);
}

} // namespace

::mlir::Location wrapOpForRegionInlining(::mlir::MLIRContext* ctx, ::mlir::Location opLoc,
                                         ::mlir::LocationAttr regionChain, ::mlir::LLVM::DISubprogramAttr subprogram,
                                         ::mlir::LLVM::DIFileAttr functionFile, RegionSubprogramCache& cache) {
	if (!regionChain) {
		return ::mlir::FusedLoc::get({opLoc}, subprogram, ctx);
	}
	auto node = decomposeChain(regionChain);
	if (!node.leaf) {
		return ::mlir::FusedLoc::get({opLoc}, subprogram, ctx);
	}
	auto regionSubprogram = resolveInnermostRegionSubprogram(ctx, regionChain, subprogram, functionFile, cache);
	auto callerLoc = buildCallerChain(ctx, node.parent, subprogram, functionFile, cache);
	return ::mlir::CallSiteLoc::get(::mlir::FusedLoc::get({opLoc}, regionSubprogram, ctx), callerLoc);
}

} // namespace nautilus::compiler::mlir
