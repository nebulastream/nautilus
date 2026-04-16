
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
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <cstdint>
#include <unordered_map>
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

Operation* makeIntConst(common::Arena& arena, OperationIdentifier id, int64_t value, Type stamp) {
	return arena.create<ConstIntOperation>(arena, id, value, stamp);
}

Operation* makeFloatConst(common::Arena& arena, OperationIdentifier id, double value, Type stamp) {
	return arena.create<ConstFloatOperation>(arena, id, value, stamp);
}

Operation* makeBoolConst(common::Arena& arena, OperationIdentifier id, bool value) {
	return arena.create<ConstBooleanOperation>(arena, id, value);
}

/// Attempts to fold an arithmetic op whose operands are both `int64_t`
/// constants. Returns `nullptr` when semantics forbid the fold
/// (divide/mod by zero).
Operation* foldIntArithmetic(common::Arena& arena, const Operation& op, Operation::OperationType kind, int64_t l,
                             int64_t r) {
	const auto stamp = op.getStamp();
	const auto id = op.getIdentifier();
	switch (kind) {
	case Operation::OperationType::AddOp:
		return makeIntConst(arena, id, static_cast<int64_t>(static_cast<uint64_t>(l) + static_cast<uint64_t>(r)),
		                    stamp);
	case Operation::OperationType::SubOp:
		return makeIntConst(arena, id, static_cast<int64_t>(static_cast<uint64_t>(l) - static_cast<uint64_t>(r)),
		                    stamp);
	case Operation::OperationType::MulOp:
		return makeIntConst(arena, id, static_cast<int64_t>(static_cast<uint64_t>(l) * static_cast<uint64_t>(r)),
		                    stamp);
	case Operation::OperationType::DivOp:
		if (r == 0) {
			return nullptr;
		}
		return makeIntConst(arena, id, l / r, stamp);
	case Operation::OperationType::ModOp:
		if (r == 0) {
			return nullptr;
		}
		return makeIntConst(arena, id, l % r, stamp);
	default:
		return nullptr;
	}
}

Operation* foldFloatArithmetic(common::Arena& arena, const Operation& op, Operation::OperationType kind, double l,
                               double r) {
	const auto stamp = op.getStamp();
	const auto id = op.getIdentifier();
	switch (kind) {
	case Operation::OperationType::AddOp:
		return makeFloatConst(arena, id, l + r, stamp);
	case Operation::OperationType::SubOp:
		return makeFloatConst(arena, id, l - r, stamp);
	case Operation::OperationType::MulOp:
		return makeFloatConst(arena, id, l * r, stamp);
	case Operation::OperationType::DivOp:
		if (r == 0.0) {
			return nullptr;
		}
		return makeFloatConst(arena, id, l / r, stamp);
	case Operation::OperationType::ModOp:
		// Float modulo is not folded: it is rarely worth the complexity and
		// keeping it out avoids introducing IEEE-edge-case differences in
		// reference dumps.
		return nullptr;
	default:
		return nullptr;
	}
}

Operation* foldCompareInt(common::Arena& arena, const Operation& op, CompareOperation::Comparator cmp, int64_t l,
                          int64_t r) {
	const auto id = op.getIdentifier();
	switch (cmp) {
	case CompareOperation::EQ:
		return makeBoolConst(arena, id, l == r);
	case CompareOperation::NE:
		return makeBoolConst(arena, id, l != r);
	case CompareOperation::LT:
		return makeBoolConst(arena, id, l < r);
	case CompareOperation::LE:
		return makeBoolConst(arena, id, l <= r);
	case CompareOperation::GT:
		return makeBoolConst(arena, id, l > r);
	case CompareOperation::GE:
		return makeBoolConst(arena, id, l >= r);
	}
	return nullptr;
}

Operation* foldCompareFloat(common::Arena& arena, const Operation& op, CompareOperation::Comparator cmp, double l,
                            double r) {
	const auto id = op.getIdentifier();
	switch (cmp) {
	case CompareOperation::EQ:
		return makeBoolConst(arena, id, l == r);
	case CompareOperation::NE:
		return makeBoolConst(arena, id, l != r);
	case CompareOperation::LT:
		return makeBoolConst(arena, id, l < r);
	case CompareOperation::LE:
		return makeBoolConst(arena, id, l <= r);
	case CompareOperation::GT:
		return makeBoolConst(arena, id, l > r);
	case CompareOperation::GE:
		return makeBoolConst(arena, id, l >= r);
	}
	return nullptr;
}

Operation* foldCompareBool(common::Arena& arena, const Operation& op, CompareOperation::Comparator cmp, bool l,
                           bool r) {
	const auto id = op.getIdentifier();
	switch (cmp) {
	case CompareOperation::EQ:
		return makeBoolConst(arena, id, l == r);
	case CompareOperation::NE:
		return makeBoolConst(arena, id, l != r);
	default:
		// Ordered comparisons on booleans are not meaningful here; skip the
		// fold rather than inventing a semantics.
		return nullptr;
	}
}

Operation* tryFold(common::Arena& arena, Operation* op) {
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
			return foldIntArithmetic(arena, *op, opKind, left.i, right.i);
		}
		if (left.kind == ConstKind::Float && right.kind == ConstKind::Float) {
			return foldFloatArithmetic(arena, *op, opKind, left.f, right.f);
		}
		return nullptr;

	case Operation::OperationType::AndOp:
		// `AndOp` is logical-AND; its stamp is always `Type::b`. Bitwise AND
		// on integers lives in `BinaryComp::BAND`, handled below.
		if (left.kind == ConstKind::Bool && right.kind == ConstKind::Bool) {
			return makeBoolConst(arena, id, left.b && right.b);
		}
		return nullptr;

	case Operation::OperationType::OrOp:
		if (left.kind == ConstKind::Bool && right.kind == ConstKind::Bool) {
			return makeBoolConst(arena, id, left.b || right.b);
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
			return makeIntConst(arena, id, shifted, stamp);
		}
		return makeIntConst(arena, id, left.i >> amount, stamp);
	}

	case Operation::OperationType::BinaryComp: {
		const auto binOp = as<BinaryCompOperation>(op);
		if (left.kind == ConstKind::Int && right.kind == ConstKind::Int) {
			switch (binOp->getType()) {
			case BinaryCompOperation::BAND:
				return makeIntConst(arena, id, left.i & right.i, stamp);
			case BinaryCompOperation::BOR:
				return makeIntConst(arena, id, left.i | right.i, stamp);
			case BinaryCompOperation::XOR:
				return makeIntConst(arena, id, left.i ^ right.i, stamp);
			}
			return nullptr;
		}
		if (left.kind == ConstKind::Bool && right.kind == ConstKind::Bool) {
			switch (binOp->getType()) {
			case BinaryCompOperation::BAND:
				return makeBoolConst(arena, id, left.b && right.b);
			case BinaryCompOperation::BOR:
				return makeBoolConst(arena, id, left.b || right.b);
			case BinaryCompOperation::XOR:
				return makeBoolConst(arena, id, left.b != right.b);
			}
			return nullptr;
		}
		return nullptr;
	}

	case Operation::OperationType::CompareOp: {
		const auto cmpOp = as<CompareOperation>(op);
		const auto cmp = cmpOp->getComparator();
		if (left.kind == ConstKind::Int && right.kind == ConstKind::Int) {
			return foldCompareInt(arena, *op, cmp, left.i, right.i);
		}
		if (left.kind == ConstKind::Float && right.kind == ConstKind::Float) {
			return foldCompareFloat(arena, *op, cmp, left.f, right.f);
		}
		if (left.kind == ConstKind::Bool && right.kind == ConstKind::Bool) {
			return foldCompareBool(arena, *op, cmp, left.b, right.b);
		}
		return nullptr;
	}

	default:
		return nullptr;
	}
}

/// Rewires one invocation's arguments so that every entry matching a key in
/// @p replacements is swapped for the corresponding value. Takes a snapshot
/// before mutating because `replaceArgument` walks the same arguments span.
void propagateInInvocation(BasicBlockInvocation& inv, const std::unordered_map<Operation*, Operation*>& replacements) {
	const auto args = inv.getArguments();
	std::vector<Operation*> snapshot(args.begin(), args.end());
	for (auto* arg : snapshot) {
		auto it = replacements.find(arg);
		if (it != replacements.end()) {
			inv.replaceArgument(arg, it->second);
		}
	}
}

void propagateReplacements(FunctionOperation& fn, const std::unordered_map<Operation*, Operation*>& replacements) {
	for (auto* block : fn.getBasicBlocks()) {
		for (auto* op : block->getOperations()) {
			if (auto* bin = dyn_cast<BinaryOperation>(op)) {
				if (auto it = replacements.find(bin->getLeftInput()); it != replacements.end()) {
					bin->setLeftInput(it->second);
				}
				if (auto it = replacements.find(bin->getRightInput()); it != replacements.end()) {
					bin->setRightInput(it->second);
				}
			}
			if (auto* ifOp = dyn_cast<IfOperation>(op)) {
				if (auto it = replacements.find(ifOp->getBooleanValue()); it != replacements.end()) {
					ifOp->setBooleanValue(it->second);
				}
			}
			if (auto* retOp = dyn_cast<ReturnOperation>(op)) {
				if (retOp->hasReturnValue()) {
					if (auto it = replacements.find(retOp->getReturnValue()); it != replacements.end()) {
						retOp->setReturnValue(it->second);
					}
				}
			}
			for (auto* inv : getSuccessorInvocations(*op)) {
				if (inv != nullptr) {
					propagateInInvocation(*inv, replacements);
				}
			}
		}
	}
}

/// Counts the total number of operations across every block in @p fn. Used
/// only to compute a generous fixed-point iteration bound.
size_t countOps(const FunctionOperation& fn) {
	size_t total = 0;
	for (const auto* block : fn.getBasicBlocks()) {
		total += block->getOperations().size();
	}
	return total;
}

void applyToFunction(common::Arena& arena, FunctionOperation& fn) {
	const size_t iterationBound = countOps(fn) * 4u + 8u;
	size_t iterations = 0;
	bool changed = true;
	while (changed && iterations++ < iterationBound) {
		changed = false;
		std::unordered_map<Operation*, Operation*> replacements;

		// Phase 1: fold foldable binary ops in place.
		for (auto* block : fn.getBasicBlocks()) {
			const auto& ops = block->getOperations();
			for (size_t i = 0; i < ops.size(); ++i) {
				Operation* op = ops[i];
				if (!isa<BinaryOperation>(op)) {
					continue;
				}
				if (auto* folded = tryFold(arena, op)) {
					block->replaceOperation(i, folded);
					replacements.emplace(op, folded);
					changed = true;
				}
			}
		}

		if (replacements.empty()) {
			break;
		}

		// Phase 2: propagate every replacement into its downstream uses so
		// the next iteration can see freshly-constant operands.
		propagateReplacements(fn, replacements);
	}
}

} // namespace

void ConstantFoldingAndCopyPropagationPass::apply(IRGraph& ir) {
	common::Arena& arena = ir.getArena();
	for (auto* fn : ir.getFunctionOperations()) {
		if (fn != nullptr) {
			applyToFunction(arena, *fn);
		}
	}
}

} // namespace nautilus::compiler::ir
