#include "nautilus/compiler/backends/mlir/PropagatePersonalityPass.hpp"
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/Pass/Pass.h>

namespace nautilus::compiler::mlir {

namespace {

constexpr llvm::StringLiteral PERSONALITY_ATTR = "personality";

struct PropagatePersonalityPass
    : public ::mlir::PassWrapper<PropagatePersonalityPass, ::mlir::OperationPass<::mlir::ModuleOp>> {
	MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(PropagatePersonalityPass)

	llvm::StringRef getArgument() const final {
		return "nautilus-propagate-personality";
	}
	llvm::StringRef getDescription() const final {
		return "Set the personality attribute on functions that received a landing pad through inlining";
	}

	void runOnOperation() override {
		auto module = getOperation();

		// Every landing pad MLIRLoweringProvider emits uses the same personality,
		// so reuse whichever one is still attached to a function. The inliner may
		// already have erased the callee that carried it; the lowering provider
		// always declares the Itanium C++ personality alongside the first landing
		// pad, so fall back to that.
		::mlir::Attribute personality;
		module.walk([&](::mlir::func::FuncOp func) {
			if (auto attr = func->getDiscardableAttr(PERSONALITY_ATTR)) {
				personality = attr;
				return ::mlir::WalkResult::interrupt();
			}
			return ::mlir::WalkResult::advance();
		});
		if (!personality) {
			personality = ::mlir::FlatSymbolRefAttr::get(module.getContext(), "__gxx_personality_v0");
		}

		module.walk([&](::mlir::func::FuncOp func) {
			if (func->getDiscardableAttr(PERSONALITY_ATTR)) {
				return;
			}
			const bool hasLandingPad =
			    func.walk([](::mlir::LLVM::LandingpadOp) { return ::mlir::WalkResult::interrupt(); }).wasInterrupted();
			if (hasLandingPad) {
				func->setDiscardableAttr(PERSONALITY_ATTR, personality);
			}
		});
	}
};

} // namespace

std::unique_ptr<::mlir::Pass> createPropagatePersonalityPass() {
	return std::make_unique<PropagatePersonalityPass>();
}

} // namespace nautilus::compiler::mlir
