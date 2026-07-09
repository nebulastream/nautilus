
#include "nautilus/compiler/ir/passes/ConstantBranchFoldingPass.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"
#include <unordered_set>
#include <vector>

namespace nautilus::compiler::ir {

namespace {

/// Builds a standalone `BranchOperation` (not yet inserted into any block)
/// adopting @p taken's target and argument list.
BranchOperation* buildBranchAdopting(common::Arena& arena, const BasicBlockInvocation& taken) {
	auto* branch = arena.create<BranchOperation>();
	auto& inv = branch->getNextBlockInvocation();
	inv.setBlock(const_cast<BasicBlock*>(taken.getBlock()));
	for (auto* arg : taken.getArguments()) {
		inv.addArgument(arena, arg);
	}
	return branch;
}

bool sameTargetSameArgs(const BasicBlockInvocation& a, const BasicBlockInvocation& b) {
	if (a.getBlock() == nullptr || a.getBlock() != b.getBlock()) {
		return false;
	}
	const auto aArgs = a.getArguments();
	const auto bArgs = b.getArguments();
	if (aArgs.size() != bArgs.size()) {
		return false;
	}
	for (size_t i = 0; i < aArgs.size(); ++i) {
		if (aArgs[i] != bArgs[i]) {
			return false;
		}
	}
	return true;
}

/// Folds @p ifOp (the terminator of @p block) into a `BranchOperation` when
/// either its condition is a compile-time constant or both arms already
/// agree on target and arguments. Returns whether a fold happened.
bool tryFoldIf(FunctionRewriter& rewriter, common::Arena& arena, BasicBlock& block, IfOperation& ifOp) {
	const BasicBlockInvocation* taken = nullptr;
	if (auto* cond = dyn_cast<ConstBooleanOperation>(ifOp.getBooleanValue())) {
		taken = cond->getValue() ? &ifOp.getTrueBlockInvocation() : &ifOp.getFalseBlockInvocation();
	} else if (sameTargetSameArgs(ifOp.getTrueBlockInvocation(), ifOp.getFalseBlockInvocation())) {
		taken = &ifOp.getTrueBlockInvocation();
	}
	if (taken == nullptr || taken->getBlock() == nullptr) {
		return false; // Defensive: a well-formed If always has both arms wired.
	}
	auto* branch = buildBranchAdopting(arena, *taken);
	rewriter.replaceTerminator(&block, branch);
	return true;
}

/// Every block reachable from @p fn's entry block via successor edges,
/// found by a plain worklist DFS (function CFGs here are small).
std::unordered_set<BasicBlock*> computeReachable(FunctionOperation& fn) {
	std::unordered_set<BasicBlock*> visited;
	auto* entry = fn.getEntryBlock();
	if (entry == nullptr) {
		return visited;
	}
	std::vector<BasicBlock*> worklist {entry};
	while (!worklist.empty()) {
		auto* block = worklist.back();
		worklist.pop_back();
		if (block == nullptr || !visited.insert(block).second) {
			continue;
		}
		for (auto* succ : block->getSuccessors()) {
			worklist.push_back(succ);
		}
	}
	return visited;
}

/// Detaches every block not reachable from the entry block. Every
/// predecessor of an unreachable block is itself unreachable (a reachable
/// predecessor would make the block reachable too), so it is safe to clear
/// every unreachable block's predecessor list up front -- this is what lets
/// an unreachable *cycle* (no block in it individually has zero
/// predecessors) satisfy `FunctionRewriter::eraseBlock`'s precondition.
bool sweepUnreachableBlocks(FunctionRewriter& rewriter, FunctionOperation& fn) {
	const auto reachable = computeReachable(fn);
	std::vector<BasicBlock*> unreachable;
	for (auto* block : fn.getBasicBlocks()) {
		if (!reachable.contains(block)) {
			unreachable.push_back(block);
		}
	}
	if (unreachable.empty()) {
		return false;
	}
	for (auto* block : unreachable) {
		block->clearPredecessors();
	}
	for (auto* block : unreachable) {
		rewriter.eraseBlock(block);
	}
	return true;
}

bool applyToFunction(FunctionOperation& fn, common::Arena& arena) {
	FunctionRewriter rewriter(fn, arena);
	bool changed = false;

	bool foldedThisRound = true;
	while (foldedThisRound) {
		foldedThisRound = false;
		for (auto* block : fn.getBasicBlocks()) {
			auto* ifOp = dyn_cast<IfOperation>(block->getTerminatorOp());
			if (ifOp == nullptr) {
				continue;
			}
			if (tryFoldIf(rewriter, arena, *block, *ifOp)) {
				changed = true;
				foldedThisRound = true;
			}
		}
	}

	changed |= sweepUnreachableBlocks(rewriter, fn);
	return changed;
}

} // namespace

bool ConstantBranchFoldingPass::apply(IRGraph& ir) {
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
