
#include "nautilus/compiler/ir/passes/BlockArgumentPruningPass.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/passes/Dominators.hpp"
#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <unordered_set>
#include <vector>

namespace nautilus::compiler::ir {

namespace {

/// Every invocation targeting @p block, scanning each predecessor's
/// terminator once (the predecessor list is a multiset -- a shared-target
/// if lists its block twice, but both invocations are found on the first
/// scan of that terminator).
std::vector<BasicBlockInvocation*> invocationsTargeting(BasicBlock* block) {
	std::vector<BasicBlockInvocation*> result;
	std::unordered_set<BasicBlock*> visitedPreds;
	for (auto* pred : block->getPredecessors()) {
		if (pred == nullptr || !visitedPreds.insert(pred).second) {
			continue;
		}
		auto* terminator = pred->getTerminatorOp();
		if (terminator == nullptr) {
			continue;
		}
		for (auto* inv : getSuccessorInvocations(*terminator)) {
			if (inv->getBlock() == block) {
				result.push_back(inv);
			}
		}
	}
	return result;
}

/// The value every incoming edge agrees on for argument slot @p index of
/// @p arg's block, or nullptr. Edges passing the argument itself back (a
/// loop latch threading the header's own argument through unchanged) are
/// ignored, per the standard SSA phi-elimination rule; at least one
/// non-self edge must exist.
Operation* agreedPassThroughValue(const BasicBlockArgument* arg, size_t index,
                                  const std::vector<BasicBlockInvocation*>& invocations) {
	Operation* agreed = nullptr;
	for (const auto* inv : invocations) {
		const auto values = inv->getArguments();
		if (index >= values.size()) {
			return nullptr; // arity drift: leave it to the verifier.
		}
		Operation* value = values[index];
		if (value == arg) {
			continue;
		}
		if (value == nullptr || (agreed != nullptr && agreed != value)) {
			return nullptr;
		}
		agreed = value;
	}
	return agreed;
}

bool applyToFunction(FunctionOperation& fn, common::Arena& arena) {
	FunctionRewriter rewriter(fn, arena);
	// This pass never adds or removes CFG edges, so dominators stay valid
	// across every mutation it makes.
	const Dominators dominators(fn);

	// Reverse post-order first, so a value threaded through a chain of blocks
	// is resolved at the head of the chain before the blocks downstream ask
	// what their edges agree on -- each pass-through slot then folds straight
	// to the dominating definition instead of to the previous block's argument,
	// which would cost another round (and another move of every use) per hop.
	// Blocks the entry cannot reach follow in block-list order: nothing
	// dominates them, so only their unused slots can go, but those still can.
	std::vector<BasicBlock*> order;
	order.reserve(fn.getBasicBlocks().size());
	for (const auto* block : dominators.reversePostOrder()) {
		order.push_back(const_cast<BasicBlock*>(block));
	}
	for (auto* block : fn.getBasicBlocks()) {
		if (!dominators.isReachable(block)) {
			order.push_back(block);
		}
	}

	size_t totalArguments = 0;
	for (auto* block : fn.getBasicBlocks()) {
		totalArguments += block->getArguments().size();
	}

	bool anyChanged = false;
	bool changed = true;
	// Every productive round removes at least one argument slot.
	const size_t iterationBound = totalArguments + 8u;
	size_t iterations = 0;
	std::vector<FunctionRewriter::BlockArgumentSlot> prunable;
	while (changed && iterations++ < iterationBound) {
		changed = false;
		prunable.clear();
		// Decide first, remove afterwards. Rewiring a pass-through slot's uses
		// happens on the spot (it is what lets the next block down the chain
		// see the final value), but the slots themselves come out in one batch
		// per round: removing them one at a time re-derives every targeting
		// invocation's use edges per slot, and for a value passed through the
		// whole function that scan is as long as the function.
		for (auto* block : order) {
			if (block == fn.getEntryBlock()) {
				continue; // entry arguments are the function ABI.
			}
			auto invocations = invocationsTargeting(block);
			const auto& args = block->getArguments();
			for (size_t i = 0; i < args.size(); ++i) {
				BasicBlockArgument* arg = args[i];
				if (rewriter.useCount(arg) == 0) {
					prunable.push_back({block, i});
					continue;
				}
				Operation* agreed = agreedPassThroughValue(arg, i, invocations);
				if (agreed == nullptr) {
					continue;
				}
				BasicBlock* definingBlock = rewriter.definingBlock(agreed);
				// `dominates` is false whenever `block` is unreachable, so
				// detached regions are conservatively skipped.
				if (definingBlock == nullptr || !dominators.dominates(definingBlock, block)) {
					continue;
				}
				rewriter.replaceAllUses(arg, agreed);
				prunable.push_back({block, i});
			}
		}
		if (!prunable.empty()) {
			rewriter.removeBlockArguments(prunable);
			changed = true;
			anyChanged = true;
		}
	}
	return anyChanged;
}

} // namespace

bool BlockArgumentPruningPass::apply(IRGraph& ir) {
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
