
#include "nautilus/compiler/ir/passes/DeadCodeEliminationPass.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"
#include "nautilus/compiler/ir/passes/RewriterCache.hpp"

namespace nautilus::compiler::ir {

namespace {

bool applyToFunction(const IRGraph& ir, FunctionOperation& fn, FunctionRewriter& rewriter) {
	size_t erased = 0;
	for (auto* block : fn.getBasicBlocks()) {
		// Snapshot: `eraseIfDead` mutates the block's operation list as it
		// cascades, including possibly erasing later entries in this same
		// snapshot before we reach them -- `eraseIfDead` is idempotent on an
		// already-erased operation (it checks the rewriter's tracking table
		// first), so re-visiting one here is harmless.
		auto ops = block->getOperations();
		for (auto* op : ops) {
			erased += rewriter.eraseIfDead(op, &ir);
		}
	}
	return erased != 0;
}

} // namespace

bool DeadCodeEliminationPass::apply(IRGraph& ir) {
	RewriterCache rewriters(ir);
	return apply(ir, rewriters);
}

bool DeadCodeEliminationPass::apply(IRGraph& ir, RewriterCache& rewriters) {
	bool changed = false;
	for (auto* fn : ir.getFunctionOperations()) {
		if (fn != nullptr) {
			changed |= applyToFunction(ir, *fn, rewriters.rewriterFor(*fn));
		}
	}
	return changed;
}

} // namespace nautilus::compiler::ir
