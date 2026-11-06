
#include "nautilus/compiler/ir/passes/ConstantFoldingAndCopyPropagationPass.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
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
#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <type_traits>
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
	return rewriter.createReplacing<ConstIntOperation>(anchor, id, truncateToStamp(value, stamp), stamp);
}

/// `ConstFloatOperation` stores a `double` for both float stamps, so an f32
/// constant must be rounded to `float` here; otherwise it keeps precision and
/// range f32 does not have (an f32 underflow staying nonzero, an f32 overflow
/// staying finite) and every later fold sees the unrounded value.
Operation* makeFloatConst(FunctionRewriter& rewriter, Operation* anchor, OperationIdentifier id, double value,
                          Type stamp) {
	if (stamp == Type::f32) {
		value = static_cast<double>(static_cast<float>(value));
	}
	return rewriter.createReplacing<ConstFloatOperation>(anchor, id, value, stamp);
}

Operation* makeBoolConst(FunctionRewriter& rewriter, Operation* anchor, OperationIdentifier id, bool value) {
	return rewriter.createReplacing<ConstBooleanOperation>(anchor, id, value);
}

/// `MIN / -1` and `MIN % -1` overflow for a signed stamp: the quotient does
/// not fit, x86 `idiv` traps on it, and LLVM treats it as undefined. Folding
/// it would either crash the compiler itself (for i64, the fold's own `l / r`
/// is that division) or invent a result the unfolded program never computes,
/// so the op is left in place.
bool isSignedMinByMinusOne(int64_t l, int64_t r, Type stamp) {
	if (!isSignedInteger(stamp) || r != -1) {
		return false;
	}
	const int width = getBitWith(stamp);
	const int64_t min = width == 64 ? std::numeric_limits<int64_t>::min() : -(int64_t {1} << (width - 1));
	return l == min;
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
		if (isSignedMinByMinusOne(l, r, stamp)) {
			return nullptr;
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
		if (isSignedMinByMinusOne(l, r, stamp)) {
			return nullptr;
		}
		return makeIntConst(rewriter, anchor, id, l % r, stamp);
	default:
		return nullptr;
	}
}

/// Computes `l OP r` in `T`, so an f32 fold rounds (and over/underflows)
/// exactly like the f32 operation it replaces.
template <typename T>
std::optional<double> computeFloatArithmetic(Operation::OperationType kind, T l, T r) {
	switch (kind) {
	case Operation::OperationType::AddOp:
		return static_cast<double>(l + r);
	case Operation::OperationType::SubOp:
		return static_cast<double>(l - r);
	case Operation::OperationType::MulOp:
		return static_cast<double>(l * r);
	case Operation::OperationType::DivOp:
		if (r == T(0)) {
			return std::nullopt;
		}
		return static_cast<double>(l / r);
	case Operation::OperationType::ModOp:
		// Float modulo is not folded: it is rarely worth the complexity and
		// keeping it out avoids introducing IEEE-edge-case differences in
		// reference dumps.
		return std::nullopt;
	default:
		return std::nullopt;
	}
}

Operation* foldFloatArithmetic(FunctionRewriter& rewriter, const Operation& op, Operation::OperationType kind, double l,
                               double r) {
	const auto stamp = op.getStamp();
	const auto result = stamp == Type::f32
	                        ? computeFloatArithmetic<float>(kind, static_cast<float>(l), static_cast<float>(r))
	                        : computeFloatArithmetic<double>(kind, l, r);
	if (!result) {
		return nullptr;
	}
	return makeFloatConst(rewriter, const_cast<Operation*>(&op), op.getIdentifier(), *result, stamp);
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

/// Canonical `ConstIntOperation` payload for @p value converted to integer
/// stamp @p stamp: wrapped to the stamp's width, sign-extended for signed
/// stamps and zero-extended for unsigned ones.
template <typename T>
int64_t canonicalInt(T value, Type stamp) {
	if constexpr (std::is_unsigned_v<T>) {
		return truncateToStamp(static_cast<int64_t>(static_cast<uint64_t>(value)), stamp);
	} else {
		return truncateToStamp(static_cast<int64_t>(value), stamp);
	}
}

/// Converts the integer constant @p value of stamp @p from to the float type
/// @p to directly from its integer type, so the result is rounded once (going
/// through `double` first would round an i64/ui64 twice on the way to f32).
double intToFloat(int64_t value, Type from, Type to) {
	if (isUnsignedInteger(from)) {
		const auto u = static_cast<uint64_t>(value);
		return to == Type::f32 ? static_cast<double>(static_cast<float>(u)) : static_cast<double>(u);
	}
	return to == Type::f32 ? static_cast<double>(static_cast<float>(value)) : static_cast<double>(value);
}

/// Converts the float constant @p value to integer stamp @p to, truncating
/// toward zero. Returns `std::nullopt` for NaN, infinities and values out of
/// the target's range: that conversion is undefined (LLVM poison, C++ UB) and
/// the backends disagree on its result.
std::optional<int64_t> floatToInt(double value, Type to) {
	if (!std::isfinite(value)) {
		return std::nullopt;
	}
	const int width = getBitWith(to);
	const double t = std::trunc(value);
	if (isUnsignedInteger(to)) {
		// Valid range is [0, 2^width); 2^width is exact as a double.
		if (t < 0.0 || t >= std::ldexp(1.0, width)) {
			return std::nullopt;
		}
		return canonicalInt(static_cast<uint64_t>(t), to);
	}
	// Valid range is [-2^(width-1), 2^(width-1)); both bounds are exact.
	if (t < -std::ldexp(1.0, width - 1) || t >= std::ldexp(1.0, width - 1)) {
		return std::nullopt;
	}
	return canonicalInt(static_cast<int64_t>(t), to);
}

/// Folds a cast of a constant, matching the backends' lowering (see
/// `MLIRLoweringProvider::visitCast`): integer widening extends by the
/// *source* signedness, narrowing truncates, same-width casts reinterpret;
/// int->float rounds once by source signedness; float->int truncates toward
/// zero and is only folded when in range; int/float->bool is `!= 0` (NaN is
/// true). Pointer and bool sources are left alone.
Operation* foldCast(FunctionRewriter& rewriter, Operation* op, const ConstValue& in) {
	const auto from = as<CastOperation>(op)->getInput()->getStamp();
	const auto to = op->getStamp();
	const auto id = op->getIdentifier();
	if (in.kind == ConstKind::Int && isInteger(from)) {
		if (isInteger(to)) {
			return makeIntConst(rewriter, op, id, in.i, to);
		}
		if (isFloat(to)) {
			return makeFloatConst(rewriter, op, id, intToFloat(in.i, from, to), to);
		}
		if (to == Type::b) {
			return makeBoolConst(rewriter, op, id, in.i != 0);
		}
		return nullptr;
	}
	if (in.kind == ConstKind::Float && isFloat(from)) {
		if (isFloat(to)) {
			// f64 -> f32 rounds to nearest; a finite value beyond f32's range
			// would overflow, which C++ leaves undefined, so it is not folded.
			if (to == Type::f32 && std::isfinite(in.f) &&
			    std::fabs(in.f) > static_cast<double>(std::numeric_limits<float>::max())) {
				return nullptr;
			}
			return makeFloatConst(rewriter, op, id, in.f, to);
		}
		if (isInteger(to)) {
			const auto value = floatToInt(in.f, to);
			return value ? makeIntConst(rewriter, op, id, *value, to) : nullptr;
		}
		if (to == Type::b) {
			return makeBoolConst(rewriter, op, id, in.f != 0.0);
		}
	}
	return nullptr;
}

/// Folds the single-operand ops: logical `Not` on a bool, `Negate` (bitwise
/// NOT for integers, IEEE sign flip for floats) and `Cast`.
Operation* tryFoldUnary(FunctionRewriter& rewriter, Operation* op) {
	const auto kind = op->getOperationType();
	if (kind != Operation::OperationType::NotOp && kind != Operation::OperationType::NegateOp &&
	    kind != Operation::OperationType::CastOp) {
		return nullptr;
	}
	const auto in = readConstant(op->getInputs()[0]);
	const auto id = op->getIdentifier();
	const auto stamp = op->getStamp();
	switch (kind) {
	case Operation::OperationType::NotOp:
		return in.kind == ConstKind::Bool ? makeBoolConst(rewriter, op, id, !in.b) : nullptr;
	case Operation::OperationType::NegateOp:
		if (in.kind == ConstKind::Int && isInteger(stamp)) {
			return makeIntConst(rewriter, op, id, ~in.i, stamp);
		}
		if (in.kind == ConstKind::Float && isFloat(stamp)) {
			return makeFloatConst(rewriter, op, id, -in.f, stamp);
		}
		return nullptr;
	default:
		return foldCast(rewriter, op, in);
	}
}

/// Ops `tryFold` can turn into a constant once all their inputs are constant.
bool isFoldCandidate(const Operation* op) {
	switch (op->getOperationType()) {
	case Operation::OperationType::NotOp:
	case Operation::OperationType::NegateOp:
	case Operation::OperationType::CastOp:
		return true;
	default:
		return isa<BinaryOperation>(op);
	}
}

Operation* tryFold(FunctionRewriter& rewriter, Operation* op) {
	const auto* binary = dyn_cast<BinaryOperation>(op);
	if (binary == nullptr) {
		return tryFoldUnary(rewriter, op);
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
		// A negative amount or one of at least the stamp's bit width is
		// undefined (LLVM poison), and the backends disagree on its result
		// (x86 masks the amount to the register width, others do not), so
		// there is no single answer to fold to.
		if (right.i < 0 || right.i >= getBitWith(stamp)) {
			return nullptr;
		}
		const auto shiftOp = as<ShiftOperation>(op);
		const auto amount = static_cast<uint64_t>(right.i);
		// The shifted operand may carry a different stamp than the result
		// (e.g. a ui32 operand under an i32 result); reinterpret it at the
		// result's width and signedness first, as the backends do.
		const auto value = truncateToStamp(left.i, stamp);
		if (shiftOp->getType() == ShiftOperation::LS) {
			const auto shifted = static_cast<int64_t>(static_cast<uint64_t>(value) << amount);
			return makeIntConst(rewriter, op, id, shifted, stamp);
		}
		// Right shift is sign-sensitive: unsigned operands need a logical shift
		// (zero-fill), signed operands an arithmetic shift (sign-fill).
		if (isUnsignedInteger(stamp)) {
			return makeIntConst(rewriter, op, id, static_cast<int64_t>(static_cast<uint64_t>(value) >> amount), stamp);
		}
		return makeIntConst(rewriter, op, id, value >> amount, stamp);
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

/// True if every input of @p op is a constant `tryFold` can read.
bool hasOnlyConstantInputs(const Operation* op) {
	const auto inputs = op->getInputs();
	return !inputs.empty() && std::all_of(inputs.begin(), inputs.end(),
	                                      [](const Operation* in) { return readConstant(in).kind != ConstKind::None; });
}

bool isReadyToFold(const Operation* op) {
	return isFoldCandidate(op) && hasOnlyConstantInputs(op);
}

/// Worklist-driven fixed point. The worklist only ever holds ops whose
/// inputs are all constant: the initial scan seeds it, and a successful fold
/// adds exactly those of its consumers that became ready. A function with
/// nothing to fold therefore costs one scan, without building a rewriter --
/// the common case once the pipeline group has reached its fixed point.
///
/// Each fold swaps the new constant into the folded op's slot and defers
/// removing the constants it orphaned to one sweep per block at the end, so
/// the pass is O(ops + folds) rather than O(folds * block size).
bool applyToFunction(FunctionOperation& fn, common::Arena& arena) {
	std::vector<Operation*> worklist;
	for (auto* block : fn.getBasicBlocks()) {
		for (auto* op : block->getOperations()) {
			if (isReadyToFold(op)) {
				worklist.push_back(op);
			}
		}
	}
	if (worklist.empty()) {
		return false;
	}

	ScopedRewriter scopedRewriter(fn, arena);
	FunctionRewriter& rewriter = *scopedRewriter;
	bool changed = false;
	std::vector<Operation*> consumers;
	std::vector<Operation*> operands;
	while (!worklist.empty()) {
		Operation* op = worklist.back();
		worklist.pop_back();
		// Already folded: an op is pushed once per consumer edge that made it
		// ready, so it can come up again after its first visit replaced it.
		if (rewriter.definingBlock(op) == nullptr) {
			continue;
		}
		// Snapshot what the fold moves away from `op`: its consumers (now
		// possibly ready) and its operands (now possibly dead).
		consumers.clear();
		for (const auto& use : rewriter.usesOf(op)) {
			consumers.push_back(use.user);
		}
		operands.assign(op->getInputs().begin(), op->getInputs().end());

		Operation* folded = tryFold(rewriter, op);
		if (folded == nullptr) {
			continue;
		}
		folded->setSourceTag(op->getSourceTag());
		folded->setRegionIndex(op->getRegionIndex());
		changed = true;

		for (auto* operand : operands) {
			rewriter.eraseIfDeadDeferred(operand);
		}
		for (auto* consumer : consumers) {
			if (isReadyToFold(consumer)) {
				worklist.push_back(consumer);
			}
		}
	}
	rewriter.compactErasedOperations();
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
