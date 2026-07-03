
#include "nautilus/compiler/ir/passes/EmptyBlockEliminationPass.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <unordered_set>

namespace nautilus::compiler::ir {

namespace {

/// Returns true iff @p block contains exactly one op, an unconditional
/// branch to a target distinct from @p block itself, and @p block is not
/// the entry block of @p fn.
bool isRemovableEmptyBlock(BasicBlock& block, const FunctionOperation& fn) {
	if (&block == fn.getEntryBlock()) {
		return false;
	}
	const auto& ops = block.getOperations();
	if (ops.size() != 1) {
		return false;
	}
	auto* only = ops.front();
	if (only->getOperationType() != Operation::OperationType::BranchOp) {
		return false;
	}
	const auto* branch = as<BranchOperation>(only);
	const auto* target = branch->getNextBlockInvocation().getBlock();
	return target != nullptr && target != &block;
}

/// Computes the argument list @p predInv (currently targeting `victim`)
/// must carry once retargeted straight to `victim`'s successor.
///
/// Let N = |victimOutgoing.args|, V = victim's block-arg count. For each
/// i in [0, N):
///   - if victimOutgoing.args[i] is victim's block argument at index j,
///     the new i-th arg is the value pred was supplying at index j
///     (i.e. predInv's original j-th arg);
///   - otherwise the original value passes through (constant / external).
///
/// A pure query -- it does not mutate @p predInv; the caller applies the
/// result via `FunctionRewriter::setInvocationArguments`.
std::vector<Operation*> substitutedArguments(const BasicBlockInvocation& predInv, const BasicBlock& victim,
                                             const BasicBlockInvocation& victimOutgoing) {
	const auto victimOutArgs = victimOutgoing.getArguments();
	const auto predSuppliedArgs = predInv.getArguments();
	const auto& victimParams = victim.getArguments();

	std::vector<Operation*> result;
	result.reserve(victimOutArgs.size());
	for (auto* victimOutArg : victimOutArgs) {
		Operation* replacement = victimOutArg;
		// If this outgoing arg references one of victim's block arguments,
		// substitute the corresponding value that pred was supplying.
		for (size_t j = 0; j < victimParams.size(); ++j) {
			if (victimParams[j] == victimOutArg) {
				if (j < predSuppliedArgs.size()) {
					replacement = predSuppliedArgs[j];
				}
				break;
			}
		}
		result.push_back(replacement);
	}
	return result;
}

bool applyToFunction(FunctionOperation& fn, common::Arena& arena) {
	FunctionRewriter rewriter(fn, arena);

	bool anyChanged = false;
	bool changed = true;
	// Generous fixed-point bound: each round either removes a block or
	// none at all (we break on change). With at most B blocks, at most B
	// iterations are productive; round up for safety.
	const size_t iterationBound = fn.getBasicBlocks().size() * 4u + 8u;
	size_t iterations = 0;
	while (changed && iterations++ < iterationBound) {
		changed = false;
		// Snapshot the block list because eraseBlock mutates the underlying
		// vector.
		auto blocks = fn.getBasicBlocks();
		for (auto* block : blocks) {
			if (!isRemovableEmptyBlock(*block, fn)) {
				continue;
			}
			auto* terminator = block->getTerminatorOp();
			auto* branch = as<BranchOperation>(terminator);
			auto& outgoing = branch->getNextBlockInvocation();
			auto* succ = const_cast<BasicBlock*>(outgoing.getBlock());

			// Snapshot predecessors before rewiring (setInvocationTarget
			// mutates `block`'s predecessor list); dedup by predecessor
			// block since one predecessor's terminator may hold more than
			// one invocation targeting `block` (both arms of an `if`).
			auto preds = block->getPredecessors();
			std::unordered_set<BasicBlock*> visitedPreds;
			for (auto* pred : preds) {
				if (pred == nullptr || !visitedPreds.insert(pred).second) {
					continue;
				}
				auto* predTerminator = pred->getTerminatorOp();
				if (predTerminator == nullptr) {
					continue;
				}
				for (auto* predInv : getSuccessorInvocations(*predTerminator)) {
					if (predInv->getBlock() != block) {
						continue;
					}
					auto newArgs = substitutedArguments(*predInv, *block, outgoing);
					rewriter.setInvocationArguments(*predInv, newArgs);
					rewriter.setInvocationTarget(*predInv, succ);
				}
			}

			// Every predecessor edge now bypasses `block` directly to
			// `succ`; `block` has zero predecessors and can be erased.
			// `eraseBlock` also drops `block`'s own outgoing edge's
			// predecessor entry on `succ` (the edge that predates all of
			// the rewiring above and is never touched by it).
			rewriter.eraseBlock(block);
			changed = true;
			anyChanged = true;
			break;
		}
	}
	return anyChanged;
}

} // namespace

bool EmptyBlockEliminationPass::apply(IRGraph& ir) {
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
