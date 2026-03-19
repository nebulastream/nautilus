
#include "nautilus/compiler/ir/phases/ConstantFoldingPhase.hpp"
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
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"

namespace nautilus::compiler::ir {

namespace {

/**
 * @brief Replaces all uses of oldOp with newOp across all blocks in a function.
 *
 * Updates operation inputs and block invocation arguments.
 */
void replaceAllUses(FunctionOperation& func, Operation* oldOp, Operation* newOp) {
	for (auto& block : func.getBasicBlocks()) {
		for (auto& op : block->getOperations()) {
			op->replaceInputWith(oldOp, newOp);

			// Handle block invocation arguments in terminator operations
			if (auto* branchOp = dynamic_cast<BranchOperation*>(op.get())) {
				branchOp->getNextBlockInvocation().replaceArgument(oldOp, newOp);
			} else if (auto* ifOp = dynamic_cast<IfOperation*>(op.get())) {
				ifOp->getTrueBlockInvocation().replaceArgument(oldOp, newOp);
				ifOp->getFalseBlockInvocation().replaceArgument(oldOp, newOp);
			}
		}
	}
}

/**
 * @brief Attempts to fold a binary arithmetic operation on two integer constants.
 * @return A new ConstIntOperation if foldable, nullptr otherwise.
 */
Operation* tryFoldIntArithmetic(Operation::OperationType opType, OperationIdentifier id, int64_t left, int64_t right,
                                Type stamp) {
	switch (opType) {
	case Operation::OperationType::AddOp:
		return new ConstIntOperation(id, left + right, stamp);
	case Operation::OperationType::SubOp:
		return new ConstIntOperation(id, left - right, stamp);
	case Operation::OperationType::MulOp:
		return new ConstIntOperation(id, left * right, stamp);
	case Operation::OperationType::DivOp:
		if (right == 0) {
			return nullptr;
		}
		return new ConstIntOperation(id, left / right, stamp);
	case Operation::OperationType::ModOp:
		if (right == 0) {
			return nullptr;
		}
		return new ConstIntOperation(id, left % right, stamp);
	default:
		return nullptr;
	}
}

/**
 * @brief Attempts to fold a binary arithmetic operation on two float constants.
 * @return A new ConstFloatOperation if foldable, nullptr otherwise.
 */
Operation* tryFoldFloatArithmetic(Operation::OperationType opType, OperationIdentifier id, double left, double right,
                                  Type stamp) {
	switch (opType) {
	case Operation::OperationType::AddOp:
		return new ConstFloatOperation(id, left + right, stamp);
	case Operation::OperationType::SubOp:
		return new ConstFloatOperation(id, left - right, stamp);
	case Operation::OperationType::MulOp:
		return new ConstFloatOperation(id, left * right, stamp);
	case Operation::OperationType::DivOp:
		if (right == 0.0) {
			return nullptr;
		}
		return new ConstFloatOperation(id, left / right, stamp);
	default:
		return nullptr;
	}
}

/**
 * @brief Attempts to fold a comparison on two integer constants.
 */
Operation* tryFoldIntCompare(OperationIdentifier id, int64_t left, int64_t right,
                             CompareOperation::Comparator comparator) {
	bool result;
	switch (comparator) {
	case CompareOperation::EQ:
		result = (left == right);
		break;
	case CompareOperation::NE:
		result = (left != right);
		break;
	case CompareOperation::LT:
		result = (left < right);
		break;
	case CompareOperation::LE:
		result = (left <= right);
		break;
	case CompareOperation::GT:
		result = (left > right);
		break;
	case CompareOperation::GE:
		result = (left >= right);
		break;
	default:
		return nullptr;
	}
	return new ConstBooleanOperation(id, result);
}

/**
 * @brief Attempts to fold a comparison on two float constants.
 */
Operation* tryFoldFloatCompare(OperationIdentifier id, double left, double right,
                               CompareOperation::Comparator comparator) {
	bool result;
	switch (comparator) {
	case CompareOperation::EQ:
		result = (left == right);
		break;
	case CompareOperation::NE:
		result = (left != right);
		break;
	case CompareOperation::LT:
		result = (left < right);
		break;
	case CompareOperation::LE:
		result = (left <= right);
		break;
	case CompareOperation::GT:
		result = (left > right);
		break;
	case CompareOperation::GE:
		result = (left >= right);
		break;
	default:
		return nullptr;
	}
	return new ConstBooleanOperation(id, result);
}

/**
 * @brief Attempts to fold a single operation if all its inputs are constants.
 * @return A newly allocated constant operation if foldable, nullptr otherwise.
 */
Operation* tryFold(const std::unique_ptr<Operation>& op) {
	auto opType = op->getOperationType();
	auto id = op->getIdentifier();
	auto stamp = op->getStamp();

	// Binary arithmetic: Add, Sub, Mul, Div, Mod
	if (opType == Operation::OperationType::AddOp || opType == Operation::OperationType::SubOp ||
	    opType == Operation::OperationType::MulOp || opType == Operation::OperationType::DivOp ||
	    opType == Operation::OperationType::ModOp) {
		auto* binOp = static_cast<BinaryOperation*>(op.get());
		auto* left = binOp->getLeftInput();
		auto* right = binOp->getRightInput();

		// Both int constants
		if (auto* leftConst = left->dynCast<ConstIntOperation>()) {
			if (auto* rightConst = right->dynCast<ConstIntOperation>()) {
				return tryFoldIntArithmetic(opType, id, leftConst->getValue(), rightConst->getValue(), stamp);
			}
		}
		// Both float constants
		if (auto* leftConst = left->dynCast<ConstFloatOperation>()) {
			if (auto* rightConst = right->dynCast<ConstFloatOperation>()) {
				return tryFoldFloatArithmetic(opType, id, leftConst->getValue(), rightConst->getValue(), stamp);
			}
		}
		return nullptr;
	}

	// Logical: And, Or
	if (opType == Operation::OperationType::AndOp || opType == Operation::OperationType::OrOp) {
		auto* binOp = static_cast<BinaryOperation*>(op.get());
		auto* leftConst = binOp->getLeftInput()->dynCast<ConstBooleanOperation>();
		auto* rightConst = binOp->getRightInput()->dynCast<ConstBooleanOperation>();
		if (leftConst && rightConst) {
			bool result = (opType == Operation::OperationType::AndOp)
			                  ? (leftConst->getValue() && rightConst->getValue())
			                  : (leftConst->getValue() || rightConst->getValue());
			return new ConstBooleanOperation(id, result);
		}
		return nullptr;
	}

	// Not (unary boolean)
	if (opType == Operation::OperationType::NotOp) {
		auto* notOp = static_cast<NotOperation*>(op.get());
		if (auto* inputConst = notOp->getInput()->dynCast<ConstBooleanOperation>()) {
			return new ConstBooleanOperation(id, !inputConst->getValue());
		}
		return nullptr;
	}

	// Negate (unary arithmetic)
	if (opType == Operation::OperationType::NegateOp) {
		auto* input = op->getInputs()[0];
		if (auto* intConst = input->dynCast<ConstIntOperation>()) {
			return new ConstIntOperation(id, -intConst->getValue(), stamp);
		}
		if (auto* floatConst = input->dynCast<ConstFloatOperation>()) {
			return new ConstFloatOperation(id, -floatConst->getValue(), stamp);
		}
		return nullptr;
	}

	// Compare
	if (opType == Operation::OperationType::CompareOp) {
		auto* cmpOp = static_cast<CompareOperation*>(op.get());
		auto* left = cmpOp->getLeftInput();
		auto* right = cmpOp->getRightInput();
		auto comparator = cmpOp->getComparator();

		if (auto* leftConst = left->dynCast<ConstIntOperation>()) {
			if (auto* rightConst = right->dynCast<ConstIntOperation>()) {
				return tryFoldIntCompare(id, leftConst->getValue(), rightConst->getValue(), comparator);
			}
		}
		if (auto* leftConst = left->dynCast<ConstFloatOperation>()) {
			if (auto* rightConst = right->dynCast<ConstFloatOperation>()) {
				return tryFoldFloatCompare(id, leftConst->getValue(), rightConst->getValue(), comparator);
			}
		}
		return nullptr;
	}

	// BinaryComp (bitwise BAND, BOR, XOR)
	if (opType == Operation::OperationType::BinaryComp) {
		auto* binCompOp = static_cast<BinaryCompOperation*>(op.get());
		auto* leftConst = binCompOp->getLeftInput()->dynCast<ConstIntOperation>();
		auto* rightConst = binCompOp->getRightInput()->dynCast<ConstIntOperation>();
		if (leftConst && rightConst) {
			int64_t result;
			switch (binCompOp->getType()) {
			case BinaryCompOperation::BAND:
				result = leftConst->getValue() & rightConst->getValue();
				break;
			case BinaryCompOperation::BOR:
				result = leftConst->getValue() | rightConst->getValue();
				break;
			case BinaryCompOperation::XOR:
				result = leftConst->getValue() ^ rightConst->getValue();
				break;
			}
			return new ConstIntOperation(id, result, stamp);
		}
		return nullptr;
	}

	// Shift (LS, RS)
	if (opType == Operation::OperationType::ShiftOp) {
		auto* shiftOp = static_cast<ShiftOperation*>(op.get());
		auto* leftConst = shiftOp->getLeftInput()->dynCast<ConstIntOperation>();
		auto* rightConst = shiftOp->getRightInput()->dynCast<ConstIntOperation>();
		if (leftConst && rightConst) {
			int64_t result = (shiftOp->getType() == ShiftOperation::LS)
			                     ? (leftConst->getValue() << rightConst->getValue())
			                     : (leftConst->getValue() >> rightConst->getValue());
			return new ConstIntOperation(id, result, stamp);
		}
		return nullptr;
	}

	// Select with constant condition
	if (opType == Operation::OperationType::SelectOp) {
		auto* selectOp = static_cast<SelectOperation*>(op.get());
		if (auto* condConst = selectOp->getCondition()->dynCast<ConstBooleanOperation>()) {
			// We can't fold to a constant, but we can redirect uses to the selected input.
			// Return the selected operation directly — the caller will handle the reference update.
			// We use nullptr here since we handle select specially outside.
		}
		return nullptr;
	}

	return nullptr;
}

/**
 * @brief Returns true if the operation is a constant (int, float, or boolean).
 */
bool isConstant(Operation* op) {
	auto type = op->getOperationType();
	return type == Operation::OperationType::ConstIntOp || type == Operation::OperationType::ConstFloatOp ||
	       type == Operation::OperationType::ConstBooleanOp;
}

/**
 * @brief Returns true if two constant operations hold the same value.
 */
bool sameConstantValue(Operation* a, Operation* b) {
	if (a == b) {
		return true;
	}
	if (a->getOperationType() != b->getOperationType()) {
		return false;
	}
	if (auto* ai = a->dynCast<ConstIntOperation>()) {
		return ai->getValue() == b->dynCast<ConstIntOperation>()->getValue();
	}
	if (auto* af = a->dynCast<ConstFloatOperation>()) {
		return af->getValue() == b->dynCast<ConstFloatOperation>()->getValue();
	}
	if (auto* ab = a->dynCast<ConstBooleanOperation>()) {
		return ab->getValue() == b->dynCast<ConstBooleanOperation>()->getValue();
	}
	return false;
}

/**
 * @brief Collects all BasicBlockInvocations that target the given block.
 *
 * Scans all blocks in the function and inspects their terminators
 * (BranchOperation and IfOperation) to find invocations targeting the block.
 */
std::vector<const BasicBlockInvocation*> getIncomingInvocations(FunctionOperation& func, const BasicBlock* target) {
	std::vector<const BasicBlockInvocation*> result;
	for (auto& block : func.getBasicBlocks()) {
		auto* terminator = block->getTerminatorOp();
		if (auto* branchOp = dynamic_cast<BranchOperation*>(terminator)) {
			if (branchOp->getNextBlockInvocation().getBlock() == target) {
				result.push_back(&branchOp->getNextBlockInvocation());
			}
		} else if (auto* ifOp = dynamic_cast<IfOperation*>(terminator)) {
			if (ifOp->getTrueBlockInvocation().getBlock() == target) {
				result.push_back(&ifOp->getTrueBlockInvocation());
			}
			if (ifOp->getFalseBlockInvocation().getBlock() == target) {
				result.push_back(&ifOp->getFalseBlockInvocation());
			}
		}
	}
	return result;
}

/**
 * @brief Returns true if any operation in the function references the given operation as an input.
 */
bool hasUses(FunctionOperation& func, Operation* op) {
	for (auto& block : func.getBasicBlocks()) {
		for (auto& blockOp : block->getOperations()) {
			for (auto* input : blockOp->getInputs()) {
				if (input == op) {
					return true;
				}
			}
			// Check block invocation arguments in terminators
			if (auto* branchOp = dynamic_cast<BranchOperation*>(blockOp.get())) {
				for (auto* arg : branchOp->getNextBlockInvocation().getArguments()) {
					if (arg == op) {
						return true;
					}
				}
			} else if (auto* ifOp = dynamic_cast<IfOperation*>(blockOp.get())) {
				for (auto* arg : ifOp->getTrueBlockInvocation().getArguments()) {
					if (arg == op) {
						return true;
					}
				}
				for (auto* arg : ifOp->getFalseBlockInvocation().getArguments()) {
					if (arg == op) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

/**
 * @brief Creates a new constant operation with the same value as the source but a new identifier.
 *
 * The new constant is created with the block argument's identifier so it can be resolved
 * in the target block's value frame during code generation.
 */
Operation* cloneConstant(Operation* source, OperationIdentifier newId) {
	if (auto* intConst = source->dynCast<ConstIntOperation>()) {
		return new ConstIntOperation(newId, intConst->getValue(), source->getStamp());
	}
	if (auto* floatConst = source->dynCast<ConstFloatOperation>()) {
		return new ConstFloatOperation(newId, floatConst->getValue(), source->getStamp());
	}
	if (auto* boolConst = source->dynCast<ConstBooleanOperation>()) {
		return new ConstBooleanOperation(newId, boolConst->getValue());
	}
	return nullptr;
}

/**
 * @brief Propagates constants across basic block boundaries.
 *
 * For each block argument, if all predecessors pass the same constant value,
 * creates a new constant in the target block and replaces uses of the block argument.
 *
 * @return true if any block arguments were replaced with constants.
 */
bool propagateConstantsAcrossBlocks(FunctionOperation& func) {
	bool anyChanged = false;
	for (auto& block : func.getBasicBlocks()) {
		auto& blockArgs = block->getArguments();
		if (blockArgs.empty()) {
			continue;
		}

		auto invocations = getIncomingInvocations(func, block.get());
		if (invocations.empty()) {
			continue;
		}

		for (size_t argIdx = 0; argIdx < blockArgs.size(); argIdx++) {
			// Skip block arguments that are no longer used (already propagated)
			if (!hasUses(func, blockArgs[argIdx].get())) {
				continue;
			}

			auto& passedArgs = invocations[0]->getArguments();
			if (argIdx >= passedArgs.size()) {
				continue;
			}
			auto* firstValue = passedArgs[argIdx];
			if (!isConstant(firstValue)) {
				continue;
			}

			// Check that all predecessors pass the same constant at this position
			bool allSame = true;
			for (size_t p = 1; p < invocations.size(); p++) {
				auto& args = invocations[p]->getArguments();
				if (argIdx >= args.size() || !sameConstantValue(firstValue, args[argIdx])) {
					allSame = false;
					break;
				}
			}
			if (!allSame) {
				continue;
			}

			// Create a new constant in this block with the block argument's identifier,
			// so backends can resolve it in the local value frame.
			auto* newConst = cloneConstant(firstValue, blockArgs[argIdx]->getIdentifier());
			if (!newConst) {
				continue;
			}
			replaceAllUses(func, blockArgs[argIdx].get(), newConst);
			// Insert the constant before the first operation so it is available to all uses.
			auto newConstPtr = std::unique_ptr<Operation>(newConst);
			block->addOperationBefore(block->getOperationAt(0), newConstPtr);
			anyChanged = true;
		}
	}
	return anyChanged;
}

/**
 * @brief Converts IfOperations with constant boolean conditions to unconditional BranchOperations.
 *
 * When constant folding produces a ConstBooleanOperation that feeds an IfOperation's condition,
 * the conditional branch is replaced with an unconditional jump to the taken block.
 *
 * @return true if any branches were simplified.
 */
bool simplifyConstantBranches(FunctionOperation& func) {
	bool anyChanged = false;
	for (auto& block : func.getBasicBlocks()) {
		auto* terminator = block->getTerminatorOp();
		if (terminator->getOperationType() != Operation::OperationType::IfOp) {
			continue;
		}
		auto* ifOp = static_cast<IfOperation*>(terminator);
		auto* condConst = ifOp->getBooleanValue()->dynCast<ConstBooleanOperation>();
		if (!condConst) {
			continue;
		}

		// Pick the taken branch based on the constant condition value
		auto& takenInvocation =
		    condConst->getValue() ? ifOp->getTrueBlockInvocation() : ifOp->getFalseBlockInvocation();

		// Create an unconditional branch to the taken block
		auto* branchOp = new BranchOperation();
		auto& nextInvocation = branchOp->getNextBlockInvocation();
		nextInvocation.setBlock(const_cast<BasicBlock*>(takenInvocation.getBlock()));
		for (auto* arg : takenInvocation.getArguments()) {
			nextInvocation.addArgument(arg);
		}

		block->replaceTerminatorOperation(branchOp);
		anyChanged = true;
	}
	return anyChanged;
}

/**
 * @brief Processes a single function, folding constants until a fixed point.
 *
 * Iterates through intra-block constant folding, inter-block constant propagation,
 * and branch simplification until no more changes occur.
 *
 * @return true if any operations were folded or branches simplified.
 */
bool foldFunction(FunctionOperation& func) {
	bool anyChanged = false;
	bool changed = true;
	while (changed) {
		changed = false;

		// Intra-block: fold operations with constant inputs
		bool localChanged = true;
		while (localChanged) {
			localChanged = false;
			for (auto& block : func.getBasicBlocks()) {
				auto& ops = block->getOperations();
				for (size_t i = 0; i < ops.size(); i++) {
					auto* replacement = tryFold(ops[i]);
					if (replacement) {
						auto* oldOp = ops[i].get();
						replaceAllUses(func, oldOp, replacement);
						block->replaceOperation(i, replacement);
						localChanged = true;
						changed = true;
						anyChanged = true;
						break; // Restart scanning this block since operations changed
					}

					// Special case: Select with constant condition — redirect uses
					if (ops[i]->getOperationType() == Operation::OperationType::SelectOp) {
						auto* selectOp = static_cast<SelectOperation*>(ops[i].get());
						if (auto* condConst = selectOp->getCondition()->dynCast<ConstBooleanOperation>()) {
							auto* selectedValue =
							    condConst->getValue() ? selectOp->getTrueValue() : selectOp->getFalseValue();
							replaceAllUses(func, ops[i].get(), selectedValue);
							// Since the select's uses are already redirected, it becomes dead code.
							localChanged = true;
							changed = true;
							anyChanged = true;
							break;
						}
					}
				}
			}
		}

		// Inter-block: propagate constants through block arguments
		if (propagateConstantsAcrossBlocks(func)) {
			changed = true;
			anyChanged = true;
		}

		// Simplify conditional branches with constant conditions
		if (simplifyConstantBranches(func)) {
			changed = true;
			anyChanged = true;
		}
	}

	return anyChanged;
}

} // anonymous namespace

void ConstantFoldingPhase::apply(std::shared_ptr<IRGraph> ir) {
	for (auto& funcOp : ir->getFunctionOperations()) {
		foldFunction(*funcOp);
	}
}

} // namespace nautilus::compiler::ir
