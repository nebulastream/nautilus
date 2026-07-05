
#include "nautilus/compiler/ir/passes/BlockMergingPass.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"

namespace nautilus::compiler::ir {

namespace {

/// Returns the merge candidate reached from @p block, or nullptr: the target
/// of @p block's unconditional branch, provided that target's only
/// predecessor edge is this branch and it is neither @p block itself nor
/// the entry block.
BasicBlock* mergeableSuccessor(BasicBlock& block, const FunctionRewriter& rewriter, const FunctionOperation& fn) {
	auto* terminator = block.getTerminatorOp();
	if (terminator == nullptr) {
		return nullptr;
	}
	const auto* branch = dyn_cast<BranchOperation>(terminator);
	if (branch == nullptr) {
		return nullptr;
	}
	const auto& invocation = branch->getNextBlockInvocation();
	auto* succ = const_cast<BasicBlock*>(invocation.getBlock());
	if (succ == nullptr || succ == &block || succ == fn.getEntryBlock()) {
		return nullptr;
	}
	// Exactly one predecessor *edge*: the predecessor list is a multiset, so
	// a shared-target if (one predecessor, two edges) is correctly rejected.
	if (succ->getPredecessors().size() != 1) {
		return nullptr;
	}
	// An invocation argument defined in `succ` itself only occurs on an
	// unreachable cycle (a reachable use would violate SSA dominance);
	// merging would dangle the substituted uses, so leave the shape to the
	// unreachable-block sweep.
	for (auto* value : invocation.getArguments()) {
		if (value != nullptr && rewriter.definingBlock(value) == succ) {
			return nullptr;
		}
	}
	return succ;
}

bool applyToFunction(FunctionOperation& fn, common::Arena& arena) {
	FunctionRewriter rewriter(fn, arena);

	bool anyChanged = false;
	bool changed = true;
	// Generous fixed-point bound: every productive round removes one block,
	// so with B blocks at most B rounds are productive.
	const size_t iterationBound = fn.getBasicBlocks().size() * 4u + 8u;
	size_t iterations = 0;
	while (changed && iterations++ < iterationBound) {
		changed = false;
		// Snapshot the block list: the merge detaches a block from the
		// underlying vector.
		auto blocks = fn.getBasicBlocks();
		for (auto* block : blocks) {
			auto* succ = mergeableSuccessor(*block, rewriter, fn);
			if (succ == nullptr) {
				continue;
			}
			rewriter.mergeIntoPredecessor(block, succ);
			changed = true;
			anyChanged = true;
			break;
		}
	}
	return anyChanged;
}

} // namespace

bool BlockMergingPass::apply(IRGraph& ir) {
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
