
#include "nautilus/compiler/ir/passes/ConstantFoldingAndCopyPropagationPass.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <cstdint>
#include <deque>
#include <unordered_set>
#include <vector>

namespace nautilus::compiler::ir {

namespace {

enum class ConstKind : uint8_t { None, Int, Float, Bool };

struct ConstValue {
	ConstKind kind = ConstKind::None;
	int64_t i = 0;
	double f = 0.0;
	bool b = false;
};

ConstValue readConstant(const Operation* op) {
	if (op == nullptr) {
		return {};
	}
	switch (op->getOperationType()) {
	case Operation::OperationType::ConstIntOp: {
		ConstValue v;
		v.kind = ConstKind::Int;
		v.i = as<ConstIntOperation>(op)->getValue();
		return v;
	}
	case Operation::OperationType::ConstFloatOp: {
		ConstValue v;
		v.kind = ConstKind::Float;
		v.f = as<ConstFloatOperation>(op)->getValue();
		return v;
	}
	case Operation::OperationType::ConstBooleanOp: {
		ConstValue v;
		v.kind = ConstKind::Bool;
		v.b = as<ConstBooleanOperation>(op)->getValue();
		return v;
	}
	default:
		return {};
	}
}

/// Truncate a 64-bit-computed fold result down to its stamp's actual bit
/// width. Every fold below (Add/Sub/Mul/Div/Mod/Shl/Shr) computes correct
/// 64-bit-wraparound arithmetic, but a narrower stamp (e.g. i32) needs the
/// result wrapped to *its* width too -- otherwise the resulting
/// ConstIntOperation holds a value outside its declared stamp's range (e.g.
/// folding `-613191811i32 << 2` gives the 64-bit value -2452767244, which
/// doesn't fit in an i32). Most backends happen to re-truncate when they
/// materialize a typed constant (an explicit cast around a C++ literal, an
/// MLIR typed attribute, ...), which is why this stayed masked everywhere
/// except a backend that loads the raw 64-bit value into a same-width
/// register unmodified.
int64_t truncateToStamp(int64_t value, Type stamp) {
	switch (stamp) {
	case Type::i8:
		return static_cast<int64_t>(static_cast<int8_t>(value));
	case Type::ui8:
		return static_cast<int64_t>(static_cast<uint8_t>(value));
	case Type::i16:
		return static_cast<int64_t>(static_cast<int16_t>(value));
	case Type::ui16:
		return static_cast<int64_t>(static_cast<uint16_t>(value));
	case Type::i32:
		return static_cast<int64_t>(static_cast<int32_t>(value));
	case Type::ui32:
		return static_cast<int64_t>(static_cast<uint32_t>(value));
	default:
		return value; // i64, ui64 -- already full width.
	}
}

/// Every folded replacement is inserted immediately before the op it
/// replaces (via `FunctionRewriter::createBefore`) and keeps that op's exact
/// identifier -- this is a copy-propagating in-place swap, not a fresh
/// value, so reusing the identifier keeps IR dumps stable and lets any
/// (unexpected) stray reference to the old op still resolve to a valid,
/// harmless value instead of a dangling one.
Operation* makeIntConst(FunctionRewriter& rewriter, Operation* anchor, OperationIdentifier id, int64_t value,
                        Type stamp) {
	return rewriter.createBefore<ConstIntOperation>(anchor, id, truncateToStamp(value, stamp), stamp);
}

Operation* makeFloatConst(FunctionRewriter& rewriter, Operation* anchor, OperationIdentifier id, double value,
                          Type stamp) {
	return rewriter.createBefore<ConstFloatOperation>(anchor, id, value, stamp);
}

Operation* makeBoolConst(FunctionRewriter& rewriter, Operation* anchor, OperationIdentifier id, bool value) {
	return rewriter.createBefore<ConstBooleanOperation>(anchor, id, value);
}

/// Attempts to fold an arithmetic op whose operands are both `int64_t`
/// constants. Returns `nullptr` when semantics forbid the fold
/// (divide/mod by zero).
Operation* foldIntArithmetic(FunctionRewriter& rewriter, const Operation& op, Operation::OperationType kind, int64_t l,
                             int64_t r) {
	const auto stamp = op.getStamp();
	const auto id = op.getIdentifier();
	auto* anchor = const_cast<Operation*>(&op);
	// Division and modulo are sign-sensitive: an unsigned stamp must fold with
	// unsigned semantics, otherwise operands above INT64_MAX are treated as
	// negative. Add/Sub/Mul share the same bit pattern for both signednesses.
	const bool isUnsigned = isUnsignedInteger(stamp);
	switch (kind) {
	case Operation::OperationType::AddOp:
		return makeIntConst(rewriter, anchor, id,
		                    static_cast<int64_t>(static_cast<uint64_t>(l) + static_cast<uint64_t>(r)), stamp);
	case Operation::OperationType::SubOp:
		return makeIntConst(rewriter, anchor, id,
		                    static_cast<int64_t>(static_cast<uint64_t>(l) - static_cast<uint64_t>(r)), stamp);
	case Operation::OperationType::MulOp:
		return makeIntConst(rewriter, anchor, id,
		                    static_cast<int64_t>(static_cast<uint64_t>(l) * static_cast<uint64_t>(r)), stamp);
	case Operation::OperationType::DivOp:
		if (r == 0) {
			return nullptr;
		}
		if (isUnsigned) {
			return makeIntConst(rewriter, anchor, id,
			                    static_cast<int64_t>(static_cast<uint64_t>(l) / static_cast<uint64_t>(r)), stamp);
		}
		return makeIntConst(rewriter, anchor, id, l / r, stamp);
	case Operation::OperationType::ModOp:
		if (r == 0) {
			return nullptr;
		}
		if (isUnsigned) {
			return makeIntConst(rewriter, anchor, id,
			                    static_cast<int64_t>(static_cast<uint64_t>(l) % static_cast<uint64_t>(r)), stamp);
		}
		return makeIntConst(rewriter, anchor, id, l % r, stamp);
	default:
		return nullptr;
	}
}

Operation* foldFloatArithmetic(FunctionRewriter& rewriter, const Operation& op, Operation::OperationType kind, double l,
                               double r) {
	const auto stamp = op.getStamp();
	const auto id = op.getIdentifier();
	auto* anchor = const_cast<Operation*>(&op);
	switch (kind) {
	case Operation::OperationType::AddOp:
		return makeFloatConst(rewriter, anchor, id, l + r, stamp);
	case Operation::OperationType::SubOp:
		return makeFloatConst(rewriter, anchor, id, l - r, stamp);
	case Operation::OperationType::MulOp:
		return makeFloatConst(rewriter, anchor, id, l * r, stamp);
	case Operation::OperationType::DivOp:
		if (r == 0.0) {
			return nullptr;
		}
		return makeFloatConst(rewriter, anchor, id, l / r, stamp);
	case Operation::OperationType::ModOp:
		// Float modulo is not folded: it is rarely worth the complexity and
		// keeping it out avoids introducing IEEE-edge-case differences in
		// reference dumps.
		return nullptr;
	default:
		return nullptr;
	}
}

Operation* foldCompareInt(FunctionRewriter& rewriter, const Operation& op, CompareOperation::Comparator cmp, int64_t l,
                          int64_t r, bool isUnsigned) {
	const auto id = op.getIdentifier();
	auto* anchor = const_cast<Operation*>(&op);
	// Ordered comparisons are sign-sensitive. EQ/NE compare bit patterns and are
	// the same either way; LT/LE/GT/GE must use unsigned comparison for unsigned
	// operands, otherwise values above INT64_MAX compare as negative.
	const auto ul = static_cast<uint64_t>(l);
	const auto ur = static_cast<uint64_t>(r);
	switch (cmp) {
	case CompareOperation::EQ:
		return makeBoolConst(rewriter, anchor, id, l == r);
	case CompareOperation::NE:
		return makeBoolConst(rewriter, anchor, id, l != r);
	case CompareOperation::LT:
		return makeBoolConst(rewriter, anchor, id, isUnsigned ? ul < ur : l < r);
	case CompareOperation::LE:
		return makeBoolConst(rewriter, anchor, id, isUnsigned ? ul <= ur : l <= r);
	case CompareOperation::GT:
		return makeBoolConst(rewriter, anchor, id, isUnsigned ? ul > ur : l > r);
	case CompareOperation::GE:
		return makeBoolConst(rewriter, anchor, id, isUnsigned ? ul >= ur : l >= r);
	}
	return nullptr;
}

Operation* foldCompareFloat(FunctionRewriter& rewriter, const Operation& op, CompareOperation::Comparator cmp, double l,
                            double r) {
	const auto id = op.getIdentifier();
	auto* anchor = const_cast<Operation*>(&op);
	switch (cmp) {
	case CompareOperation::EQ:
		return makeBoolConst(rewriter, anchor, id, l == r);
	case CompareOperation::NE:
		return makeBoolConst(rewriter, anchor, id, l != r);
	case CompareOperation::LT:
		return makeBoolConst(rewriter, anchor, id, l < r);
	case CompareOperation::LE:
		return makeBoolConst(rewriter, anchor, id, l <= r);
	case CompareOperation::GT:
		return makeBoolConst(rewriter, anchor, id, l > r);
	case CompareOperation::GE:
		return makeBoolConst(rewriter, anchor, id, l >= r);
	}
	return nullptr;
}

Operation* foldCompareBool(FunctionRewriter& rewriter, const Operation& op, CompareOperation::Comparator cmp, bool l,
                           bool r) {
	const auto id = op.getIdentifier();
	auto* anchor = const_cast<Operation*>(&op);
	switch (cmp) {
	case CompareOperation::EQ:
		return makeBoolConst(rewriter, anchor, id, l == r);
	case CompareOperation::NE:
		return makeBoolConst(rewriter, anchor, id, l != r);
	default:
		// Ordered comparisons on booleans are not meaningful here; skip the
		// fold rather than inventing a semantics.
		return nullptr;
	}
}

Operation* tryFold(FunctionRewriter& rewriter, Operation* op) {
	const auto* binary = dyn_cast<BinaryOperation>(op);
	if (binary == nullptr) {
		return nullptr;
	}
	const auto left = readConstant(binary->getLeftInput());
	const auto right = readConstant(binary->getRightInput());
	if (left.kind == ConstKind::None || right.kind == ConstKind::None) {
		return nullptr;
	}

	const auto opKind = op->getOperationType();
	const auto id = op->getIdentifier();
	const auto stamp = op->getStamp();

	switch (opKind) {
	case Operation::OperationType::AddOp:
	case Operation::OperationType::SubOp:
	case Operation::OperationType::MulOp:
	case Operation::OperationType::DivOp:
	case Operation::OperationType::ModOp:
		if (left.kind == ConstKind::Int && right.kind == ConstKind::Int) {
			return foldIntArithmetic(rewriter, *op, opKind, left.i, right.i);
		}
		if (left.kind == ConstKind::Float && right.kind == ConstKind::Float) {
			return foldFloatArithmetic(rewriter, *op, opKind, left.f, right.f);
		}
		return nullptr;

	case Operation::OperationType::AndOp:
		// `AndOp` is logical-AND; its stamp is always `Type::b`. Bitwise AND
		// on integers lives in `BinaryComp::BAND`, handled below.
		if (left.kind == ConstKind::Bool && right.kind == ConstKind::Bool) {
			return makeBoolConst(rewriter, op, id, left.b && right.b);
		}
		return nullptr;

	case Operation::OperationType::OrOp:
		if (left.kind == ConstKind::Bool && right.kind == ConstKind::Bool) {
			return makeBoolConst(rewriter, op, id, left.b || right.b);
		}
		return nullptr;

	case Operation::OperationType::ShiftOp: {
		if (left.kind != ConstKind::Int || right.kind != ConstKind::Int) {
			return nullptr;
		}
		if (right.i < 0) {
			return nullptr;
		}
		const auto shiftOp = as<ShiftOperation>(op);
		const auto amount = static_cast<uint64_t>(right.i) & 63u;
		if (shiftOp->getType() == ShiftOperation::LS) {
			const auto shifted = static_cast<int64_t>(static_cast<uint64_t>(left.i) << amount);
			return makeIntConst(rewriter, op, id, shifted, stamp);
		}
		// Right shift is sign-sensitive: unsigned operands need a logical shift
		// (zero-fill), signed operands an arithmetic shift (sign-fill).
		if (isUnsignedInteger(stamp)) {
			return makeIntConst(rewriter, op, id, static_cast<int64_t>(static_cast<uint64_t>(left.i) >> amount), stamp);
		}
		return makeIntConst(rewriter, op, id, left.i >> amount, stamp);
	}

	case Operation::OperationType::BinaryComp: {
		const auto binOp = as<BinaryCompOperation>(op);
		if (left.kind == ConstKind::Int && right.kind == ConstKind::Int) {
			switch (binOp->getType()) {
			case BinaryCompOperation::BAND:
				return makeIntConst(rewriter, op, id, left.i & right.i, stamp);
			case BinaryCompOperation::BOR:
				return makeIntConst(rewriter, op, id, left.i | right.i, stamp);
			case BinaryCompOperation::XOR:
				return makeIntConst(rewriter, op, id, left.i ^ right.i, stamp);
			}
			return nullptr;
		}
		if (left.kind == ConstKind::Bool && right.kind == ConstKind::Bool) {
			switch (binOp->getType()) {
			case BinaryCompOperation::BAND:
				return makeBoolConst(rewriter, op, id, left.b && right.b);
			case BinaryCompOperation::BOR:
				return makeBoolConst(rewriter, op, id, left.b || right.b);
			case BinaryCompOperation::XOR:
				return makeBoolConst(rewriter, op, id, left.b != right.b);
			}
			return nullptr;
		}
		return nullptr;
	}

	case Operation::OperationType::CompareOp: {
		const auto cmpOp = as<CompareOperation>(op);
		const auto cmp = cmpOp->getComparator();
		if (left.kind == ConstKind::Int && right.kind == ConstKind::Int) {
			// CompareOperation's stamp is bool; signedness comes from the
			// operand type instead.
			const bool isUnsigned = isUnsignedInteger(binary->getLeftInput()->getStamp());
			return foldCompareInt(rewriter, *op, cmp, left.i, right.i, isUnsigned);
		}
		if (left.kind == ConstKind::Float && right.kind == ConstKind::Float) {
			return foldCompareFloat(rewriter, *op, cmp, left.f, right.f);
		}
		if (left.kind == ConstKind::Bool && right.kind == ConstKind::Bool) {
			return foldCompareBool(rewriter, *op, cmp, left.b, right.b);
		}
		return nullptr;
	}

	default:
		return nullptr;
	}
}

/// Worklist-driven fixed point: every binary op is folded at most once
/// unless a later fold turns one of its operands into a fresh constant, in
/// which case its (single) re-examination is re-queued explicitly. This
/// replaces the previous whole-function rescan per round (O(ops) per round,
/// O(rounds) rounds) with O(ops + folds) total work.
bool applyToFunction(FunctionOperation& fn, common::Arena& arena) {
	FunctionRewriter rewriter(fn, arena);
	bool changed = false;

	std::deque<Operation*> worklist;
	std::unordered_set<Operation*> queued;
	auto enqueue = [&](Operation* op) {
		if (op != nullptr && isa<BinaryOperation>(op) && queued.insert(op).second) {
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
			continue; // already erased by an earlier fold's cascade.
		}
		Operation* folded = tryFold(rewriter, op);
		if (folded == nullptr) {
			continue;
		}
		folded->setSourceTag(op->getSourceTag());
		changed = true;

		// Snapshot op's consumers before replaceAllUses moves them onto
		// `folded` -- those consumers may now themselves be foldable.
		std::vector<Operation*> consumers;
		consumers.reserve(rewriter.useCount(op));
		for (const auto& use : rewriter.usesOf(op)) {
			consumers.push_back(use.user);
		}

		rewriter.replaceAllUses(op, folded);
		rewriter.eraseIfDead(op); // op is now use-free; cascades into operands it orphaned.

		for (auto* consumer : consumers) {
			enqueue(consumer);
		}
	}
	return changed;
}

} // namespace

bool ConstantFoldingAndCopyPropagationPass::apply(IRGraph& ir) {
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
