
#include "nautilus/compiler/ir/passes/AlgebraicSimplificationPass.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/OperationProperties.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"
#include <deque>
#include <unordered_set>
#include <vector>

namespace nautilus::compiler::ir {

namespace {

using OpType = Operation::OperationType;

bool isConstOp(const Operation* op) {
	return op != nullptr && isConstantOp(op->getOperationType());
}

bool isIntConstWithValue(const Operation* op, int64_t v) {
	const auto* c = dyn_cast<ConstIntOperation>(op);
	return c != nullptr && c->getValue() == v;
}

bool isFloatConstWithValue(const Operation* op, double v) {
	const auto* c = dyn_cast<ConstFloatOperation>(op);
	return c != nullptr && c->getValue() == v;
}

bool isBoolConstWithValue(const Operation* op, bool v) {
	const auto* c = dyn_cast<ConstBooleanOperation>(op);
	return c != nullptr && c->getValue() == v;
}

/// Zero-constant matching either an integer or a boolean stamp -- BinaryComp
/// (BAND/BOR/XOR) folds over both, mirroring
/// `ConstantFoldingAndCopyPropagationPass::tryFold`'s BinaryComp handling.
bool isIntOrBoolZero(const Operation* op) {
	return isIntConstWithValue(op, 0) || isBoolConstWithValue(op, false);
}

Operation* makeIntConst(FunctionRewriter& rewriter, Operation* anchor, int64_t value, Type stamp) {
	return rewriter.createBefore<ConstIntOperation>(anchor, anchor->getIdentifier(), value, stamp);
}

Operation* makeBoolConst(FunctionRewriter& rewriter, Operation* anchor, bool value) {
	return rewriter.createBefore<ConstBooleanOperation>(anchor, anchor->getIdentifier(), value);
}

/// `stamp` may be an integer stamp (BinaryComp over ints) or `Type::b`
/// (BinaryComp over bools); picks the matching zero-representation.
Operation* makeZeroLike(FunctionRewriter& rewriter, Operation* anchor, Type stamp) {
	if (stamp == Type::b) {
		return makeBoolConst(rewriter, anchor, false);
	}
	return makeIntConst(rewriter, anchor, 0, stamp);
}

CompareOperation::Comparator invertComparator(CompareOperation::Comparator cmp) {
	switch (cmp) {
	case CompareOperation::EQ:
		return CompareOperation::NE;
	case CompareOperation::NE:
		return CompareOperation::EQ;
	case CompareOperation::LT:
		return CompareOperation::GE;
	case CompareOperation::LE:
		return CompareOperation::GT;
	case CompareOperation::GT:
		return CompareOperation::LE;
	case CompareOperation::GE:
		return CompareOperation::LT;
	}
	return cmp;
}

/// Operand-order mirror (`a cmp b` == `b reverseComparator(cmp) a`) -- used
/// by canonicalization, distinct from `invertComparator`'s logical negation.
CompareOperation::Comparator reverseComparator(CompareOperation::Comparator cmp) {
	switch (cmp) {
	case CompareOperation::LT:
		return CompareOperation::GT;
	case CompareOperation::LE:
		return CompareOperation::GE;
	case CompareOperation::GT:
		return CompareOperation::LT;
	case CompareOperation::GE:
		return CompareOperation::LE;
	case CompareOperation::EQ:
	case CompareOperation::NE:
		return cmp;
	}
	return cmp;
}

Operation* canonicalizeAdd(FunctionRewriter& rewriter, AddOperation* op) {
	if (isConstOp(op->getLeftInput()) && !isConstOp(op->getRightInput())) {
		return rewriter.createBefore<AddOperation>(op, op->getIdentifier(), op->getRightInput(), op->getLeftInput());
	}
	return nullptr;
}

Operation* canonicalizeMul(FunctionRewriter& rewriter, MulOperation* op) {
	if (isConstOp(op->getLeftInput()) && !isConstOp(op->getRightInput())) {
		return rewriter.createBefore<MulOperation>(op, op->getIdentifier(), op->getRightInput(), op->getLeftInput());
	}
	return nullptr;
}

Operation* canonicalizeAnd(FunctionRewriter& rewriter, AndOperation* op) {
	if (isConstOp(op->getLeftInput()) && !isConstOp(op->getRightInput())) {
		return rewriter.createBefore<AndOperation>(op, op->getIdentifier(), op->getRightInput(), op->getLeftInput());
	}
	return nullptr;
}

Operation* canonicalizeOr(FunctionRewriter& rewriter, OrOperation* op) {
	if (isConstOp(op->getLeftInput()) && !isConstOp(op->getRightInput())) {
		return rewriter.createBefore<OrOperation>(op, op->getIdentifier(), op->getRightInput(), op->getLeftInput());
	}
	return nullptr;
}

Operation* canonicalizeBinaryComp(FunctionRewriter& rewriter, BinaryCompOperation* op) {
	if (isConstOp(op->getLeftInput()) && !isConstOp(op->getRightInput())) {
		return rewriter.createBefore<BinaryCompOperation>(op, op->getIdentifier(), op->getRightInput(),
		                                                  op->getLeftInput(), op->getType());
	}
	return nullptr;
}

Operation* canonicalizeCompare(FunctionRewriter& rewriter, CompareOperation* op) {
	if (isConstOp(op->getLeftInput()) && !isConstOp(op->getRightInput())) {
		return rewriter.createBefore<CompareOperation>(op, op->getIdentifier(), op->getRightInput(), op->getLeftInput(),
		                                               reverseComparator(op->getComparator()));
	}
	return nullptr;
}

Operation* simplifyAdd(FunctionRewriter& rewriter, AddOperation* op) {
	Operation* l = op->getLeftInput();
	Operation* r = op->getRightInput();
	const auto stamp = op->getStamp();
	// `x+0`/`0+x` -> x: integers and the `ptr+0` strength-reduction-residue
	// case share the same "right/left operand is an integer 0" test; floats
	// are excluded entirely (not IEEE-exact for -0.0).
	if (isInteger(stamp) || stamp == Type::ptr) {
		if (isIntConstWithValue(r, 0)) {
			return l;
		}
		if (isIntConstWithValue(l, 0)) {
			return r;
		}
	}
	return canonicalizeAdd(rewriter, op);
}

Operation* simplifySub(FunctionRewriter& rewriter, SubOperation* op) {
	Operation* l = op->getLeftInput();
	Operation* r = op->getRightInput();
	const auto stamp = op->getStamp();
	if (isInteger(stamp)) {
		if (isIntConstWithValue(r, 0)) {
			return l;
		}
		if (l == r) {
			return makeIntConst(rewriter, op, 0, stamp);
		}
	}
	return nullptr;
}

Operation* simplifyMul(FunctionRewriter& rewriter, MulOperation* op) {
	Operation* l = op->getLeftInput();
	Operation* r = op->getRightInput();
	const auto stamp = op->getStamp();
	// `x*1`/`1*x` -> x is IEEE-exact for floats too, unlike `x*0`.
	if (isIntConstWithValue(r, 1) || isFloatConstWithValue(r, 1.0)) {
		return l;
	}
	if (isIntConstWithValue(l, 1) || isFloatConstWithValue(l, 1.0)) {
		return r;
	}
	if (isInteger(stamp) && (isIntConstWithValue(r, 0) || isIntConstWithValue(l, 0))) {
		return makeIntConst(rewriter, op, 0, stamp);
	}
	return canonicalizeMul(rewriter, op);
}

Operation* simplifyDiv(Operation* op) {
	auto* div = as<DivOperation>(op);
	Operation* r = div->getRightInput();
	if (isIntConstWithValue(r, 1) || isFloatConstWithValue(r, 1.0)) {
		return div->getLeftInput();
	}
	return nullptr;
}

Operation* simplifyAnd(FunctionRewriter& rewriter, AndOperation* op) {
	Operation* l = op->getLeftInput();
	Operation* r = op->getRightInput();
	if (isBoolConstWithValue(r, true)) {
		return l;
	}
	if (isBoolConstWithValue(l, true)) {
		return r;
	}
	if (isBoolConstWithValue(r, false) || isBoolConstWithValue(l, false)) {
		return makeBoolConst(rewriter, op, false);
	}
	return canonicalizeAnd(rewriter, op);
}

Operation* simplifyOr(FunctionRewriter& rewriter, OrOperation* op) {
	Operation* l = op->getLeftInput();
	Operation* r = op->getRightInput();
	if (isBoolConstWithValue(r, false)) {
		return l;
	}
	if (isBoolConstWithValue(l, false)) {
		return r;
	}
	if (isBoolConstWithValue(r, true) || isBoolConstWithValue(l, true)) {
		return makeBoolConst(rewriter, op, true);
	}
	return canonicalizeOr(rewriter, op);
}

Operation* simplifyNot(FunctionRewriter& rewriter, NotOperation* op) {
	Operation* input = op->getInput();
	if (auto* innerNot = dyn_cast<NotOperation>(input)) {
		return innerNot->getInput();
	}
	if (auto* cmp = dyn_cast<CompareOperation>(input)) {
		return rewriter.createBefore<CompareOperation>(op, op->getIdentifier(), cmp->getLeftInput(),
		                                               cmp->getRightInput(), invertComparator(cmp->getComparator()));
	}
	return nullptr;
}

Operation* simplifyBinaryComp(FunctionRewriter& rewriter, BinaryCompOperation* op) {
	Operation* l = op->getLeftInput();
	Operation* r = op->getRightInput();
	const auto stamp = op->getStamp();
	switch (op->getType()) {
	case BinaryCompOperation::BAND:
		if (l == r) {
			return l;
		}
		if (isIntOrBoolZero(l) || isIntOrBoolZero(r)) {
			return makeZeroLike(rewriter, op, stamp);
		}
		break;
	case BinaryCompOperation::BOR:
		if (l == r) {
			return l;
		}
		if (isIntOrBoolZero(r)) {
			return l;
		}
		if (isIntOrBoolZero(l)) {
			return r;
		}
		break;
	case BinaryCompOperation::XOR:
		if (l == r) {
			return makeZeroLike(rewriter, op, stamp);
		}
		if (isIntOrBoolZero(r)) {
			return l;
		}
		if (isIntOrBoolZero(l)) {
			return r;
		}
		break;
	}
	return canonicalizeBinaryComp(rewriter, op);
}

Operation* simplifyShift(Operation* op) {
	auto* shift = as<ShiftOperation>(op);
	if (isIntConstWithValue(shift->getRightInput(), 0)) {
		return shift->getLeftInput();
	}
	return nullptr;
}

Operation* simplifyCompare(FunctionRewriter& rewriter, CompareOperation* op) {
	return canonicalizeCompare(rewriter, op);
}

Operation* simplifyCast(FunctionRewriter& rewriter, CastOperation* op) {
	Operation* input = op->getInput();
	const auto outerStamp = op->getStamp();
	if (input->getStamp() == outerStamp) {
		return input;
	}
	auto* innerCast = dyn_cast<CastOperation>(input);
	if (innerCast == nullptr) {
		return nullptr;
	}
	const Type s0 = innerCast->getInput()->getStamp();
	const Type s1 = innerCast->getStamp();
	const Type s2 = outerStamp;
	if (!isInteger(s0) || !isInteger(s1) || !isInteger(s2)) {
		return nullptr;
	}
	const auto w0 = getBitWith(s0);
	const auto w1 = getBitWith(s1);
	const auto w2 = getBitWith(s2);
	const bool wideningSameSignedness = isSignedInteger(s0) == isSignedInteger(s1) &&
	                                    isSignedInteger(s1) == isSignedInteger(s2) && w0 <= w1 && w1 <= w2;
	const bool truncationChain = w0 >= w1 && w1 >= w2;
	if (!wideningSameSignedness && !truncationChain) {
		return nullptr;
	}
	return rewriter.createBefore<CastOperation>(op, op->getIdentifier(), innerCast->getInput(), s2);
}

Operation* simplifySelect(SelectOperation* op) {
	if (auto* cond = dyn_cast<ConstBooleanOperation>(op->getCondition())) {
		return cond->getValue() ? op->getTrueValue() : op->getFalseValue();
	}
	if (op->getTrueValue() == op->getFalseValue()) {
		return op->getTrueValue();
	}
	return nullptr;
}

bool isHandledKind(Operation::OperationType type) {
	switch (type) {
	case OpType::AddOp:
	case OpType::SubOp:
	case OpType::MulOp:
	case OpType::DivOp:
	case OpType::AndOp:
	case OpType::OrOp:
	case OpType::NotOp:
	case OpType::BinaryComp:
	case OpType::ShiftOp:
	case OpType::CompareOp:
	case OpType::CastOp:
	case OpType::SelectOp:
		return true;
	default:
		return false;
	}
}

Operation* trySimplify(FunctionRewriter& rewriter, Operation* op) {
	switch (op->getOperationType()) {
	case OpType::AddOp:
		return simplifyAdd(rewriter, as<AddOperation>(op));
	case OpType::SubOp:
		return simplifySub(rewriter, as<SubOperation>(op));
	case OpType::MulOp:
		return simplifyMul(rewriter, as<MulOperation>(op));
	case OpType::DivOp:
		return simplifyDiv(op);
	case OpType::AndOp:
		return simplifyAnd(rewriter, as<AndOperation>(op));
	case OpType::OrOp:
		return simplifyOr(rewriter, as<OrOperation>(op));
	case OpType::NotOp:
		return simplifyNot(rewriter, as<NotOperation>(op));
	case OpType::BinaryComp:
		return simplifyBinaryComp(rewriter, as<BinaryCompOperation>(op));
	case OpType::ShiftOp:
		return simplifyShift(op);
	case OpType::CompareOp:
		return simplifyCompare(rewriter, as<CompareOperation>(op));
	case OpType::CastOp:
		return simplifyCast(rewriter, as<CastOperation>(op));
	case OpType::SelectOp:
		return simplifySelect(as<SelectOperation>(op));
	default:
		return nullptr;
	}
}

/// Worklist-driven fixed point, mirroring
/// `ConstantFoldingAndCopyPropagationPass::applyToFunction`: every handled
/// op is visited at most once unless a fold changes one of its operands (its
/// consumers are re-enqueued) or it is itself a freshly created replacement
/// (canonicalized/collapsed ops are re-enqueued too, since one round can
/// otherwise miss a rule that a canonicalization just exposed).
bool applyToFunction(FunctionOperation& fn, common::Arena& arena) {
	FunctionRewriter rewriter(fn, arena);
	bool changed = false;

	std::deque<Operation*> worklist;
	std::unordered_set<Operation*> queued;
	auto enqueue = [&](Operation* op) {
		if (op != nullptr && isHandledKind(op->getOperationType()) && queued.insert(op).second) {
			worklist.push_back(op);
		}
	};

	for (auto* block : fn.getBasicBlocks()) {
		for (auto* op : block->getOperations()) {
			enqueue(op);
		}
	}

	while (!worklist.empty()) {
		Operation* op = worklist.front();
		worklist.pop_front();
		queued.erase(op);

		if (rewriter.definingBlock(op) == nullptr) {
			continue; // already erased by an earlier rule's cascade.
		}
		Operation* replacement = trySimplify(rewriter, op);
		if (replacement == nullptr) {
			continue;
		}
		changed = true;

		std::vector<Operation*> consumers;
		consumers.reserve(rewriter.useCount(op));
		for (const auto& use : rewriter.usesOf(op)) {
			consumers.push_back(use.user);
		}

		rewriter.replaceAllUses(op, replacement);
		rewriter.eraseIfDead(op); // op is now use-free; cascades into operands it orphaned.

		for (auto* consumer : consumers) {
			enqueue(consumer);
		}
		enqueue(replacement);
	}
	return changed;
}

} // namespace

bool AlgebraicSimplificationPass::apply(IRGraph& ir) {
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
