#include "nautilus/compiler/ir/passes/LoopInvariantCodeMotionPass.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/OperationProperties.hpp"
#include "nautilus/compiler/ir/passes/Dominators.hpp"
#include "nautilus/compiler/ir/passes/LoopInfo.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nautilus::compiler::ir {

namespace {

using OpType = Operation::OperationType;

/// An operand is safe to reference from the preheader iff its definition lies
/// *outside* the loop body (already-hoisted operations have their defining
/// block updated to the preheader, which is itself outside the body). Every
/// such definition dominates the preheader in a natural loop with a dedicated
/// preheader, so moving a user there can never break SSA dominance.
///
/// Note this is deliberately stricter than `LoopInfo::isLoopInvariant`: a
/// pass-through loop argument carries a loop-invariant *value*, but the SSA
/// object is a header/body block argument defined *inside* the loop, so an
/// operation reading it cannot be hoisted without also substituting the
/// preheader's incoming value -- out of scope for v1.
bool operandAvailableInPreheader(Operation* operand, const std::unordered_map<Operation*, BasicBlock*>& definingBlock,
                                 const std::unordered_set<BasicBlock*>& loopBody) {
	auto it = definingBlock.find(operand);
	if (it == definingBlock.end()) {
		return true;
	}
	return !loopBody.contains(it->second);
}

/// A constant, non-zero divisor makes a division safe to speculate into the
/// preheader (it can no longer trap). Everything else keeps `DivOp`/`ModOp`
/// pinned inside the loop, even though they are otherwise pure.
bool hasNonZeroConstantDivisor(Operation* op) {
	Operation* divisor = nullptr;
	if (auto* div = dyn_cast<DivOperation>(op)) {
		divisor = div->getRightInput();
	} else if (auto* mod = dyn_cast<ModOperation>(op)) {
		divisor = mod->getRightInput();
	} else {
		return false;
	}
	if (auto* c = dyn_cast<ConstIntOperation>(divisor)) {
		return c->getValue() != 0;
	}
	if (auto* c = dyn_cast<ConstFloatOperation>(divisor)) {
		return c->getValue() != 0.0;
	}
	return false;
}

bool isHoistable(Operation* op, const std::unordered_map<Operation*, BasicBlock*>& definingBlock,
                 const std::unordered_set<BasicBlock*>& loopBody) {
	const auto type = op->getOperationType();
	if (isTerminatorOp(type) || !isPureOp(type)) {
		return false; // loads/stores/calls/terminators are never moved.
	}
	if ((type == OpType::DivOp || type == OpType::ModOp) && !hasNonZeroConstantDivisor(op)) {
		return false; // do not speculate a possibly-trapping division.
	}
	for (auto* operand : op->getInputs()) {
		if (!operandAvailableInPreheader(operand, definingBlock, loopBody)) {
			return false;
		}
	}
	return true;
}

/// True if @p loop's body contains a *nested* loop: a back edge whose target is
/// some body block other than the outer header. A back edge is `pred -> block`
/// where `block` dominates `pred`; the outer loop's own back edge targets the
/// header (excluded), so any other one marks an inner loop header.
///
/// v1 hoists only out of loops with no nested sub-loop: hoisting a value whose
/// live range would then span an inner loop's back edge miscompiles on the
/// direct-lowering interpreter backends, whose simple allocators only keep a
/// value live across the *single* loop it was hoisted out of (values crossing a
/// loop are otherwise always block arguments in trace IR). Single-loop hoists
/// are backend-correct; nested ones are conservatively skipped.
bool containsNestedLoop(const NaturalLoop& loop, const Dominators& dom) {
	for (auto* block : loop.body) {
		if (block == loop.header) {
			continue;
		}
		for (auto* pred : block->getPredecessors()) {
			if (loop.body.contains(pred) && dom.dominates(block, pred)) {
				return true; // back edge pred -> block, targeting a non-outer-header body block.
			}
		}
	}
	return false;
}

bool hoistLoop(const NaturalLoop& loop, std::unordered_map<Operation*, BasicBlock*>& definingBlock) {
	BasicBlock* preheader = loop.preheader;
	Operation* preheaderTerminator = preheader->getTerminatorOp();
	bool changed = false;

	// Fixed point: hoisting an operation makes it loop-external, which can in
	// turn make a dependent operation hoistable on a later round. Each hoist
	// strictly shrinks the body's operation count, so this terminates.
	bool roundChanged = true;
	while (roundChanged) {
		roundChanged = false;
		for (auto* block : loop.body) {
			// Snapshot: hoisting mutates the block's operation list.
			const auto ops = block->getOperations();
			for (auto* op : ops) {
				if (!isHoistable(op, definingBlock, loop.body)) {
					continue;
				}
				block->removeOperation(op);
				preheader->addOperationBefore(preheaderTerminator, op);
				definingBlock[op] = preheader;
				roundChanged = true;
				changed = true;
			}
		}
	}
	return changed;
}

bool applyToFunction(FunctionOperation& fn) {
	auto loops = findNaturalLoops(fn);
	if (loops.empty()) {
		return false;
	}
	// One shared, incrementally-updated definition map across every loop in
	// the function: hoisting an inner-loop operation into an inner preheader
	// that lies within an outer loop's body lets the outer loop hoist it
	// further on its own turn.
	auto definingBlock = computeDefiningBlocks(fn);
	const Dominators dom(fn);
	bool changed = false;
	for (const auto& loop : loops) {
		if (containsNestedLoop(loop, dom)) {
			continue; // v1: only loops without a nested sub-loop are optimized.
		}
		changed |= hoistLoop(loop, definingBlock);
	}
	return changed;
}

} // namespace

bool LoopInvariantCodeMotionPass::apply(IRGraph& ir) {
	bool changed = false;
	for (auto* fn : ir.getFunctionOperations()) {
		if (fn != nullptr) {
			changed |= applyToFunction(*fn);
		}
	}
	return changed;
}

} // namespace nautilus::compiler::ir
