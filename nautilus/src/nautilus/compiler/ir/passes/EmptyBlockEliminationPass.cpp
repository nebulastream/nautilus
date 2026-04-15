
#include "nautilus/compiler/ir/passes/EmptyBlockEliminationPass.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"

namespace nautilus::compiler::ir {

namespace {

/// Returns true iff @p block contains exactly one op, an unconditional
/// branch to a target distinct from @p block itself, @p block is not the
/// entry block of @p fn, and every predecessor's terminator is itself an
/// unconditional branch (not an `IfOp`).
///
/// The `IfOp` predecessor restriction is a pragmatic concession to the
/// bytecode backend, which lowers an `IfOp` by emitting the
/// argument-passing moves for *both* arms before the conditional jump.
/// That scheme assumes the two successor blocks' argument registers do
/// not alias, an invariant the trace→IR translator happens to preserve
/// because intermediate empty blocks decouple the register layout. Once
/// this pass short-circuits those intermediaries, the two arms can end
/// up writing to overlapping registers and clobber each other.
///
/// Skipping elimination in that case keeps the pass correct on every
/// backend without requiring changes to BC lowering; the common case of
/// chains of unconditional-branch empty blocks is still fully handled.
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
	if (target == nullptr || target == &block) {
		return false;
	}
	for (auto* pred : block.getPredecessors()) {
		if (pred == nullptr) {
			continue;
		}
		auto* predTerm = pred->getTerminatorOp();
		if (predTerm == nullptr) {
			continue;
		}
		if (predTerm->getOperationType() != Operation::OperationType::BranchOp) {
			// Conservative: any non-unconditional-branch predecessor
			// (IfOp today, potentially switch-like ops later) may rely
			// on the empty block as a register-allocation decoupling
			// point. Leave those cases alone.
			return false;
		}
	}
	return true;
}

/// Rebuilds @p predInv (an invocation in `pred`'s terminator that was
/// targeting `victim`) so that it carries the argument list for the edge
/// `pred -> victim's successor`.
///
/// Let N = |victimOutgoing.args|, V = victim's block-arg count. For each
/// i in [0, N):
///   - if victimOutgoing.args[i] is victim's block argument at index j,
///     the new i-th arg is the value pred was supplying at index j
///     (i.e. predInv's original j-th arg);
///   - otherwise the original value passes through (constant / external).
///
/// This is an arity change in general (N may differ from V), so we clear
/// and re-append instead of using replaceArgument.
void rebuildInvocationForSkippedBlock(common::Arena& arena, BasicBlockInvocation& predInv, BasicBlock& victim,
                                      const BasicBlockInvocation& victimOutgoing) {
	const auto victimOutArgs = victimOutgoing.getArguments();
	// Snapshot pred's current argument list before clearing; we use it to
	// resolve substitutions in the loop below.
	std::vector<Operation*> predSuppliedArgs(predInv.getArguments().begin(), predInv.getArguments().end());
	const auto& victimParams = victim.getArguments();

	predInv.clearArguments();

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
		predInv.addArgument(arena, replacement);
	}
}

void applyToFunction(common::Arena& arena, FunctionOperation& fn) {
	bool changed = true;
	// Generous fixed-point bound: each round either removes a block or
	// none at all (we break on change). With at most B blocks, at most B
	// iterations are productive; round up for safety.
	const size_t iterationBound = fn.getBasicBlocks().size() * 4u + 8u;
	size_t iterations = 0;
	while (changed && iterations++ < iterationBound) {
		changed = false;
		// Snapshot the block list because detachBasicBlock mutates the
		// underlying vector.
		auto blocks = fn.getBasicBlocks();
		for (auto* block : blocks) {
			if (!isRemovableEmptyBlock(*block, fn)) {
				continue;
			}
			auto* terminator = block->getTerminatorOp();
			auto* branch = as<BranchOperation>(terminator);
			auto& outgoing = branch->getNextBlockInvocation();
			auto* succ = const_cast<BasicBlock*>(outgoing.getBlock());

			// Snapshot predecessors before rewiring (replaceSuccessor
			// mutates the predecessor list of `block`).
			auto preds = block->getPredecessors();
			for (auto* pred : preds) {
				if (pred == nullptr) {
					continue;
				}
				auto* predTerminator = pred->getTerminatorOp();
				if (predTerminator == nullptr) {
					continue;
				}
				// Rebuild every invocation in pred's terminator that was
				// targeting `block` so it passes the right argument list
				// to `succ`.
				for (auto* predInv : getSuccessorInvocations(*predTerminator)) {
					if (predInv->getBlock() == block) {
						rebuildInvocationForSkippedBlock(arena, *predInv, *block, outgoing);
					}
				}
				pred->replaceSuccessor(block, succ);
			}

			// All predecessor edges redirected; detach the empty block.
			fn.detachBasicBlock(block);
			changed = true;
			break;
		}
	}
}

} // namespace

void EmptyBlockEliminationPass::apply(IRGraph& ir) {
	common::Arena& arena = ir.getArena();
	for (auto* fn : ir.getFunctionOperations()) {
		if (fn != nullptr) {
			applyToFunction(arena, *fn);
		}
	}
}

} // namespace nautilus::compiler::ir
