
#include "nautilus/compiler/backends/mlir/debug/NormalizeInlineLocationsPass.hpp"
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/Location.h>
#include <mlir/IR/Operation.h>
#include <mlir/Pass/Pass.h>

namespace nautilus::compiler::mlir {

namespace {

// Mirrors `extractFileLoc()` in MLIR's own
// mlir/lib/Dialect/LLVMIR/Transforms/DIScopeForLLVMFuncOp.cpp. It has to match
// that function exactly, because the point of this pass is to decide in advance
// which locations it will hand back a null attribute for -- the ones it then
// unconditionally dereferences. Note in particular that a `CallSiteLoc`
// resolves to its *caller*: a call frame is filed under the file the call was
// written in, not the file of the code inlined into it.
::mlir::FileLineColLoc extractFileLoc(::mlir::Location loc) {
	if (auto fileLoc = llvm::dyn_cast<::mlir::FileLineColLoc>(loc)) {
		return fileLoc;
	}
	if (auto nameLoc = llvm::dyn_cast<::mlir::NameLoc>(loc)) {
		return extractFileLoc(nameLoc.getChildLoc());
	}
	if (auto opaqueLoc = llvm::dyn_cast<::mlir::OpaqueLoc>(loc)) {
		return extractFileLoc(opaqueLoc.getFallbackLocation());
	}
	if (auto fusedLoc = llvm::dyn_cast<::mlir::FusedLoc>(loc)) {
		for (auto child : fusedLoc.getLocations()) {
			if (auto fileLoc = extractFileLoc(child)) {
				return fileLoc;
			}
		}
	}
	if (auto callSiteLoc = llvm::dyn_cast<::mlir::CallSiteLoc>(loc)) {
		return extractFileLoc(callSiteLoc.getCaller());
	}
	return {};
}

// Rewrites @p loc so that every level of its inlined-call nesting has a file
// location `extractFileLoc()` can find, dropping the levels that do not.
//
// The result satisfies the invariant `DIScopeForLLVMFuncOpPass` assumes but
// never checks: for a `CallSiteLoc`, both `extractFileLoc(callee)` and
// `extractFileLoc(caller)` are non-null, and the same holds recursively for a
// callee that is itself a `CallSiteLoc`. That is exactly the spine
// `getNestedLoc()` walks, so the invariant holds however deep the nesting goes
// -- an entry function calling a helper calling a helper, a `region()` inside
// an inlined callee, or any combination of the two.
//
// Only `CallSiteLoc` nodes are rewritten. Anything else is returned untouched,
// which keeps the `region()` markers (`RegionScopeInfo.hpp`) that
// `EmitDbgValuePass` resolves later intact, and leaves an operation that simply
// has no location alone -- the pass handles that case correctly on its own.
::mlir::Location normalizeInlineLoc(::mlir::Location loc) {
	auto callSite = llvm::dyn_cast<::mlir::CallSiteLoc>(loc);
	if (!callSite) {
		return loc;
	}
	::mlir::Location callee = normalizeInlineLoc(callSite.getCallee());
	::mlir::Location caller = normalizeInlineLoc(callSite.getCaller());
	// A frame with no source position of its own cannot become a DWARF scope:
	// dropping it attributes the operation to the call site instead, which is
	// the closest position actually known. Dropping the callee rather than
	// substituting a placeholder line keeps the emitted line table honest --
	// a made-up line would send `perf annotate` to unrelated source.
	if (!extractFileLoc(callee)) {
		return caller;
	}
	// Symmetrically, a call site with no position cannot anchor the frame it
	// introduces, so the inlined code is reported where it was written.
	if (!extractFileLoc(caller)) {
		return callee;
	}
	if (callee == callSite.getCallee() && caller == callSite.getCaller()) {
		return loc;
	}
	return ::mlir::CallSiteLoc::get(callee, caller);
}

struct NormalizeInlineLocationsPass
    : public ::mlir::PassWrapper<NormalizeInlineLocationsPass, ::mlir::OperationPass<::mlir::ModuleOp>> {
	MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(NormalizeInlineLocationsPass)

	llvm::StringRef getArgument() const final {
		return "nautilus-normalize-inline-locations";
	}
	llvm::StringRef getDescription() const final {
		return "Drop inlined-call location frames that carry no source position";
	}

	void runOnOperation() override {
		getOperation().walk([](::mlir::Operation* op) {
			op->setLoc(normalizeInlineLoc(op->getLoc()));
			// Block arguments become phis, which keep the location set on the
			// argument rather than on any operation, so they need the same
			// treatment to stay consistent with the block they belong to.
			for (::mlir::Region& region : op->getRegions()) {
				for (::mlir::Block& block : region) {
					for (::mlir::BlockArgument arg : block.getArguments()) {
						arg.setLoc(normalizeInlineLoc(arg.getLoc()));
					}
				}
			}
		});
	}
};

} // namespace

std::unique_ptr<::mlir::Pass> createNormalizeInlineLocationsPass() {
	return std::make_unique<NormalizeInlineLocationsPass>();
}

} // namespace nautilus::compiler::mlir
