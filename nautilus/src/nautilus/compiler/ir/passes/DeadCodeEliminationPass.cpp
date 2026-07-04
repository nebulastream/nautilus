
#include "nautilus/compiler/ir/passes/DeadCodeEliminationPass.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"

namespace nautilus::compiler::ir {

namespace {

bool applyToFunction(FunctionOperation& fn, common::Arena& arena) {
	FunctionRewriter rewriter(fn, arena);
	size_t erased = 0;
	for (auto* block : fn.getBasicBlocks()) {
		// Snapshot: `eraseIfDead` mutates the block's operation list as it
		// cascades, including possibly erasing later entries in this same
		// snapshot before we reach them -- `eraseIfDead` is idempotent on an
		// already-erased operation (it checks the rewriter's tracking table
		// first), so re-visiting one here is harmless.
		auto ops = block->getOperations();
		for (auto* op : ops) {
			erased += rewriter.eraseIfDead(op);
		}
	}
	return erased != 0;
}

} // namespace

bool DeadCodeEliminationPass::apply(IRGraph& ir) {
	common::Arena& arena = ir.getArena();
	bool changed = false;
	for (auto* fn : ir.getFunctionOperations()) {
		if (fn != nullptr) {
			changed |= applyToFunction(*fn, arena);
		}
	}
	return changed;
}

} // namespace nautilus::compiler::ir
