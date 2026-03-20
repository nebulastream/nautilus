
#include "nautilus/compiler/ir/phases/ConstantFoldingPhase.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
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
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nautilus::compiler::ir {

namespace {

// --- Lattice ---

struct ConstantValue {
	enum Kind { Unknown, Constant, Overdefined };
	Kind kind = Unknown;
	int64_t intValue = 0;
	double floatValue = 0.0;
	bool boolValue = false;
	Type type = Type::v;

	static ConstantValue makeUnknown() {
		return {Unknown, 0, 0.0, false, Type::v};
	}

	static ConstantValue makeOverdefined() {
		return {Overdefined, 0, 0.0, false, Type::v};
	}

	static ConstantValue makeInt(int64_t val, Type t) {
		return {Constant, val, 0.0, false, t};
	}

	static ConstantValue makeFloat(double val, Type t) {
		return {Constant, 0, val, false, t};
	}

	static ConstantValue makeBool(bool val) {
		return {Constant, 0, 0.0, val, Type::b};
	}

	bool isConstant() const {
		return kind == Constant;
	}

	bool isOverdefined() const {
		return kind == Overdefined;
	}

	bool isUnknown() const {
		return kind == Unknown;
	}

	bool equals(const ConstantValue& other) const {
		if (kind != other.kind) {
			return false;
		}
		if (kind != Constant) {
			return true;
		}
		if (type != other.type) {
			return false;
		}
		if (type == Type::b) {
			return boolValue == other.boolValue;
		}
		if (isFloat(type)) {
			return floatValue == other.floatValue;
		}
		return intValue == other.intValue;
	}
};

ConstantValue merge(const ConstantValue& a, const ConstantValue& b) {
	if (a.isUnknown()) {
		return b;
	}
	if (b.isUnknown()) {
		return a;
	}
	if (a.isOverdefined() || b.isOverdefined()) {
		return ConstantValue::makeOverdefined();
	}
	// Both constant
	if (a.equals(b)) {
		return a;
	}
	return ConstantValue::makeOverdefined();
}

// --- Predecessor map: target block → list of (source block, invocation) ---

struct PredecessorEdge {
	BasicBlock* sourceBlock;
	BasicBlockInvocation* invocation;
};

using PredecessorMap = std::unordered_map<const BasicBlock*, std::vector<PredecessorEdge>>;

PredecessorMap buildPredecessorMap(FunctionOperation* func) {
	PredecessorMap predMap;
	for (auto& block : func->getBasicBlocks()) {
		if (block->getOperations().empty()) {
			continue;
		}
		auto* terminator = block->getOperations().back().get();
		auto opType = terminator->getOperationType();
		if (opType == Operation::OperationType::BranchOp) {
			auto* branchOp = static_cast<BranchOperation*>(terminator);
			auto& inv = branchOp->getNextBlockInvocation();
			predMap[inv.getBlock()].push_back({block.get(), &branchOp->getNextBlockInvocation()});
		} else if (opType == Operation::OperationType::IfOp) {
			auto* ifOp = static_cast<IfOperation*>(terminator);
			auto& trueInv = ifOp->getTrueBlockInvocation();
			auto& falseInv = ifOp->getFalseBlockInvocation();
			predMap[trueInv.getBlock()].push_back({block.get(), &ifOp->getTrueBlockInvocation()});
			predMap[falseInv.getBlock()].push_back({block.get(), &ifOp->getFalseBlockInvocation()});
		}
		// ReturnOp has no successors
	}
	return predMap;
}

// --- Transfer functions ---

using LatticeState = std::unordered_map<const Operation*, ConstantValue>;

ConstantValue getLatticeValue(const LatticeState& state, const Operation* op) {
	auto it = state.find(op);
	if (it != state.end()) {
		return it->second;
	}
	return ConstantValue::makeUnknown();
}

ConstantValue evaluateArithmeticInt(Operation::OperationType opType, int64_t left, int64_t right, Type resultType) {
	switch (opType) {
	case Operation::OperationType::AddOp:
		return ConstantValue::makeInt(left + right, resultType);
	case Operation::OperationType::SubOp:
		return ConstantValue::makeInt(left - right, resultType);
	case Operation::OperationType::MulOp:
		return ConstantValue::makeInt(left * right, resultType);
	case Operation::OperationType::DivOp:
		if (right == 0) {
			return ConstantValue::makeOverdefined();
		}
		return ConstantValue::makeInt(left / right, resultType);
	case Operation::OperationType::ModOp:
		if (right == 0) {
			return ConstantValue::makeOverdefined();
		}
		return ConstantValue::makeInt(left % right, resultType);
	default:
		return ConstantValue::makeOverdefined();
	}
}

ConstantValue evaluateArithmeticFloat(Operation::OperationType opType, double left, double right, Type resultType) {
	switch (opType) {
	case Operation::OperationType::AddOp:
		return ConstantValue::makeFloat(left + right, resultType);
	case Operation::OperationType::SubOp:
		return ConstantValue::makeFloat(left - right, resultType);
	case Operation::OperationType::MulOp:
		return ConstantValue::makeFloat(left * right, resultType);
	case Operation::OperationType::DivOp:
		return ConstantValue::makeFloat(left / right, resultType);
	default:
		return ConstantValue::makeOverdefined();
	}
}

bool evaluateComparison(CompareOperation::Comparator comp, int64_t left, int64_t right) {
	switch (comp) {
	case CompareOperation::EQ:
		return left == right;
	case CompareOperation::NE:
		return left != right;
	case CompareOperation::LT:
		return left < right;
	case CompareOperation::LE:
		return left <= right;
	case CompareOperation::GT:
		return left > right;
	case CompareOperation::GE:
		return left >= right;
	}
	return false;
}

bool evaluateFloatComparison(CompareOperation::Comparator comp, double left, double right) {
	switch (comp) {
	case CompareOperation::EQ:
		return left == right;
	case CompareOperation::NE:
		return left != right;
	case CompareOperation::LT:
		return left < right;
	case CompareOperation::LE:
		return left <= right;
	case CompareOperation::GT:
		return left > right;
	case CompareOperation::GE:
		return left >= right;
	}
	return false;
}

ConstantValue evaluateBinaryOp(const BinaryOperation* binOp, const ConstantValue& leftVal,
                               const ConstantValue& rightVal) {
	auto opType = binOp->getOperationType();
	auto resultType = binOp->getStamp();

	if (leftVal.isOverdefined() || rightVal.isOverdefined()) {
		return ConstantValue::makeOverdefined();
	}
	if (leftVal.isUnknown() || rightVal.isUnknown()) {
		return ConstantValue::makeUnknown();
	}

	// Both are Constant
	if (opType == Operation::OperationType::CompareOp) {
		auto* cmpOp = static_cast<const CompareOperation*>(binOp);
		if (isFloat(leftVal.type)) {
			return ConstantValue::makeBool(evaluateFloatComparison(cmpOp->getComparator(), leftVal.floatValue, rightVal.floatValue));
		}
		return ConstantValue::makeBool(evaluateComparison(cmpOp->getComparator(), leftVal.intValue, rightVal.intValue));
	}

	if (opType == Operation::OperationType::AndOp) {
		return ConstantValue::makeBool(leftVal.boolValue && rightVal.boolValue);
	}
	if (opType == Operation::OperationType::OrOp) {
		return ConstantValue::makeBool(leftVal.boolValue || rightVal.boolValue);
	}

	if (opType == Operation::OperationType::ShiftOp) {
		auto* shiftOp = static_cast<const ShiftOperation*>(binOp);
		if (shiftOp->getType() == ShiftOperation::LS) {
			return ConstantValue::makeInt(leftVal.intValue << rightVal.intValue, resultType);
		}
		return ConstantValue::makeInt(leftVal.intValue >> rightVal.intValue, resultType);
	}

	if (opType == Operation::OperationType::BinaryComp) {
		auto* bCompOp = static_cast<const BinaryCompOperation*>(binOp);
		switch (bCompOp->getType()) {
		case BinaryCompOperation::BAND:
			return ConstantValue::makeInt(leftVal.intValue & rightVal.intValue, resultType);
		case BinaryCompOperation::BOR:
			return ConstantValue::makeInt(leftVal.intValue | rightVal.intValue, resultType);
		case BinaryCompOperation::XOR:
			return ConstantValue::makeInt(leftVal.intValue ^ rightVal.intValue, resultType);
		}
	}

	// Arithmetic operations
	if (isFloat(leftVal.type)) {
		return evaluateArithmeticFloat(opType, leftVal.floatValue, rightVal.floatValue, resultType);
	}
	return evaluateArithmeticInt(opType, leftVal.intValue, rightVal.intValue, resultType);
}

ConstantValue evaluateCast(const ConstantValue& inputVal, Type targetType) {
	if (!inputVal.isConstant()) {
		return inputVal;
	}

	// Bool to integer
	if (inputVal.type == Type::b && isInteger(targetType)) {
		return ConstantValue::makeInt(inputVal.boolValue ? 1 : 0, targetType);
	}

	// Integer to float
	if (isInteger(inputVal.type) && isFloat(targetType)) {
		return ConstantValue::makeFloat(static_cast<double>(inputVal.intValue), targetType);
	}

	// Float to integer
	if (isFloat(inputVal.type) && isInteger(targetType)) {
		return ConstantValue::makeInt(static_cast<int64_t>(inputVal.floatValue), targetType);
	}

	// Integer to integer (width change)
	if (isInteger(inputVal.type) && isInteger(targetType)) {
		return ConstantValue::makeInt(inputVal.intValue, targetType);
	}

	// Float to float (precision change)
	if (isFloat(inputVal.type) && isFloat(targetType)) {
		return ConstantValue::makeFloat(inputVal.floatValue, targetType);
	}

	return ConstantValue::makeOverdefined();
}

ConstantValue transferOperation(const Operation* op, const LatticeState& state) {
	auto opType = op->getOperationType();

	switch (opType) {
	case Operation::OperationType::ConstIntOp: {
		auto* constOp = static_cast<const ConstIntOperation*>(op);
		return ConstantValue::makeInt(constOp->getValue(), constOp->getStamp());
	}
	case Operation::OperationType::ConstFloatOp: {
		auto* constOp = static_cast<const ConstFloatOperation*>(op);
		return ConstantValue::makeFloat(constOp->getValue(), constOp->getStamp());
	}
	case Operation::OperationType::ConstBooleanOp: {
		auto* constOp = static_cast<const ConstBooleanOperation*>(op);
		return ConstantValue::makeBool(constOp->getValue());
	}
	case Operation::OperationType::AddOp:
	case Operation::OperationType::SubOp:
	case Operation::OperationType::MulOp:
	case Operation::OperationType::DivOp:
	case Operation::OperationType::ModOp:
	case Operation::OperationType::CompareOp:
	case Operation::OperationType::AndOp:
	case Operation::OperationType::OrOp:
	case Operation::OperationType::ShiftOp:
	case Operation::OperationType::BinaryComp: {
		auto* binOp = static_cast<const BinaryOperation*>(op);
		auto leftVal = getLatticeValue(state, binOp->getLeftInput());
		auto rightVal = getLatticeValue(state, binOp->getRightInput());
		return evaluateBinaryOp(binOp, leftVal, rightVal);
	}
	case Operation::OperationType::NotOp: {
		auto* notOp = static_cast<const NotOperation*>(op);
		auto inputVal = getLatticeValue(state, notOp->getInput());
		if (inputVal.isConstant()) {
			return ConstantValue::makeBool(!inputVal.boolValue);
		}
		return inputVal; // propagate Unknown or Overdefined
	}
	case Operation::OperationType::NegateOp: {
		auto* negOp = static_cast<const NegateOperation*>(op);
		auto inputVal = getLatticeValue(state, negOp->getInput());
		if (inputVal.isConstant()) {
			if (isFloat(inputVal.type)) {
				return ConstantValue::makeFloat(-inputVal.floatValue, inputVal.type);
			}
			return ConstantValue::makeInt(-inputVal.intValue, inputVal.type);
		}
		return inputVal;
	}
	case Operation::OperationType::CastOp: {
		auto inputVal = getLatticeValue(state, op->getInputs()[0]);
		return evaluateCast(inputVal, op->getStamp());
	}
	case Operation::OperationType::SelectOp: {
		auto* selOp = static_cast<const SelectOperation*>(op);
		auto condVal = getLatticeValue(state, selOp->getCondition());
		auto trueVal = getLatticeValue(state, selOp->getTrueValue());
		auto falseVal = getLatticeValue(state, selOp->getFalseValue());
		if (condVal.isConstant()) {
			return condVal.boolValue ? trueVal : falseVal;
		}
		if (condVal.isOverdefined()) {
			return merge(trueVal, falseVal);
		}
		return ConstantValue::makeUnknown();
	}
	default:
		// Memory ops, calls, etc. → Overdefined
		return ConstantValue::makeOverdefined();
	}
}

// --- ID allocator ---

uint32_t findMaxOperationId(FunctionOperation* func) {
	uint32_t maxId = 0;
	for (auto& block : func->getBasicBlocks()) {
		for (auto& arg : block->getArguments()) {
			maxId = std::max(maxId, arg->getIdentifier().getId());
		}
		for (auto& op : block->getOperations()) {
			maxId = std::max(maxId, op->getIdentifier().getId());
		}
	}
	return maxId;
}

// --- Analysis context ---

class ConstantPropagationAnalysis {
public:
	ConstantPropagationAnalysis(FunctionOperation* func) : func_(func), predMap_(buildPredecessorMap(func)) {
	}

	void run() {
		auto& blocks = func_->getBasicBlocks();
		if (blocks.empty()) {
			return;
		}

		// Initialize entry block arguments as Overdefined (function parameters)
		auto& entryBlock = blocks[0];
		for (auto& arg : entryBlock->getArguments()) {
			state_[arg.get()] = ConstantValue::makeOverdefined();
		}

		// Seed worklist with all blocks
		std::queue<BasicBlock*> worklist;
		std::unordered_set<const BasicBlock*> inWorklist;
		for (auto& block : blocks) {
			worklist.push(block.get());
			inWorklist.insert(block.get());
		}

		// Iterate until fixpoint
		while (!worklist.empty()) {
			auto* block = worklist.front();
			worklist.pop();
			inWorklist.erase(block);

			bool changed = processBlock(block);
			if (changed) {
				// Add successor blocks to worklist
				addSuccessors(block, worklist, inWorklist);
			}
		}
	}

	const LatticeState& getState() const {
		return state_;
	}

private:
	bool processBlock(BasicBlock* block) {
		bool changed = false;

		// Process block arguments (phi-equivalent)
		auto predIt = predMap_.find(block);
		if (predIt != predMap_.end()) {
			auto& args = block->getArguments();
			for (size_t i = 0; i < args.size(); i++) {
				auto mergedVal = ConstantValue::makeUnknown();
				for (auto& edge : predIt->second) {
					auto& invArgs = edge.invocation->getArguments();
					if (i < invArgs.size()) {
						auto incomingVal = getLatticeValue(state_, invArgs[i]);
						mergedVal = merge(mergedVal, incomingVal);
					}
				}
				changed |= updateState(args[i].get(), mergedVal);
			}
		}

		// Process operations
		for (auto& op : block->getOperations()) {
			auto opType = op->getOperationType();
			// Skip terminators and block invocations
			if (opType == Operation::OperationType::BranchOp || opType == Operation::OperationType::IfOp ||
			    opType == Operation::OperationType::ReturnOp || opType == Operation::OperationType::BlockInvocation) {
				continue;
			}
			auto newVal = transferOperation(op.get(), state_);
			changed |= updateState(op.get(), newVal);
		}

		return changed;
	}

	bool updateState(const Operation* op, const ConstantValue& newVal) {
		auto it = state_.find(op);
		if (it == state_.end()) {
			if (newVal.isUnknown()) {
				return false;
			}
			state_[op] = newVal;
			return true;
		}
		if (it->second.equals(newVal)) {
			return false;
		}
		it->second = newVal;
		return true;
	}

	void addSuccessors(BasicBlock* block, std::queue<BasicBlock*>& worklist,
	                   std::unordered_set<const BasicBlock*>& inWorklist) {
		if (block->getOperations().empty()) {
			return;
		}
		auto* terminator = block->getOperations().back().get();
		auto opType = terminator->getOperationType();
		if (opType == Operation::OperationType::BranchOp) {
			auto* branchOp = static_cast<BranchOperation*>(terminator);
			auto* target = const_cast<BasicBlock*>(branchOp->getNextBlockInvocation().getBlock());
			if (inWorklist.find(target) == inWorklist.end()) {
				worklist.push(target);
				inWorklist.insert(target);
			}
		} else if (opType == Operation::OperationType::IfOp) {
			auto* ifOp = static_cast<IfOperation*>(terminator);
			auto* trueTarget = const_cast<BasicBlock*>(ifOp->getTrueBlockInvocation().getBlock());
			auto* falseTarget = const_cast<BasicBlock*>(ifOp->getFalseBlockInvocation().getBlock());
			if (inWorklist.find(trueTarget) == inWorklist.end()) {
				worklist.push(trueTarget);
				inWorklist.insert(trueTarget);
			}
			if (inWorklist.find(falseTarget) == inWorklist.end()) {
				worklist.push(falseTarget);
				inWorklist.insert(falseTarget);
			}
		}
	}

	FunctionOperation* func_;
	PredecessorMap predMap_;
	LatticeState state_;
};

// --- Transformation pass ---

void replaceInputInOperation(Operation* op, Operation* oldInput, Operation* newInput) {
	auto opType = op->getOperationType();

	// BinaryOperation subclasses
	if (auto* binOp = dynamic_cast<BinaryOperation*>(op)) {
		if (binOp->getLeftInput() == oldInput) {
			binOp->setLeftInput(newInput);
		}
		if (binOp->getRightInput() == oldInput) {
			binOp->setRightInput(newInput);
		}
		return;
	}

	if (opType == Operation::OperationType::NotOp) {
		auto* notOp = static_cast<NotOperation*>(op);
		if (notOp->getInput() == oldInput) {
			notOp->setInput(newInput);
		}
		return;
	}

	if (opType == Operation::OperationType::NegateOp) {
		auto* negOp = static_cast<NegateOperation*>(op);
		if (negOp->getInput() == oldInput) {
			negOp->setInput(newInput);
		}
		return;
	}

	if (opType == Operation::OperationType::CastOp) {
		auto* castOp = static_cast<CastOperation*>(op);
		if (castOp->getInput() == oldInput) {
			castOp->setInput(newInput);
		}
		return;
	}

	if (opType == Operation::OperationType::SelectOp) {
		auto* selOp = static_cast<SelectOperation*>(op);
		if (selOp->getCondition() == oldInput) {
			selOp->setCondition(newInput);
		}
		if (selOp->getTrueValue() == oldInput) {
			selOp->setTrueValue(newInput);
		}
		if (selOp->getFalseValue() == oldInput) {
			selOp->setFalseValue(newInput);
		}
		return;
	}

	if (opType == Operation::OperationType::ReturnOp) {
		auto* retOp = static_cast<ReturnOperation*>(op);
		if (retOp->hasReturnValue() && retOp->getReturnValue() == oldInput) {
			retOp->setReturnValue(newInput);
		}
		return;
	}

	if (opType == Operation::OperationType::IfOp) {
		auto* ifOp = static_cast<IfOperation*>(op);
		if (ifOp->getBooleanValue() == oldInput) {
			ifOp->setBooleanValue(newInput);
		}
		ifOp->getTrueBlockInvocation().replaceArgument(oldInput, newInput);
		ifOp->getFalseBlockInvocation().replaceArgument(oldInput, newInput);
		return;
	}

	if (opType == Operation::OperationType::BranchOp) {
		auto* branchOp = static_cast<BranchOperation*>(op);
		branchOp->getNextBlockInvocation().replaceArgument(oldInput, newInput);
		return;
	}
}

std::unique_ptr<Operation> createConstantOp(const ConstantValue& val, uint32_t& nextId) {
	OperationIdentifier id(nextId++);
	if (val.type == Type::b) {
		return std::make_unique<ConstBooleanOperation>(id, val.boolValue);
	}
	if (isFloat(val.type)) {
		return std::make_unique<ConstFloatOperation>(id, val.floatValue, val.type);
	}
	return std::make_unique<ConstIntOperation>(id, val.intValue, val.type);
}

void applyFolding(FunctionOperation* func, const LatticeState& state) {
	uint32_t nextId = findMaxOperationId(func) + 1;

	// Build a map: operations that can be replaced → constant value
	// We skip operations that are already constants (no need to replace them)
	std::unordered_map<const Operation*, ConstantValue> foldableOps;
	for (auto& [op, val] : state) {
		if (val.isConstant() && !op->isConstOperation() &&
		    op->getOperationType() != Operation::OperationType::BasicBlockArgument) {
			foldableOps[op] = val;
		}
	}

	// Also collect block arguments that are constant
	std::unordered_map<const Operation*, ConstantValue> foldableArgs;
	for (auto& [op, val] : state) {
		if (val.isConstant() && op->getOperationType() == Operation::OperationType::BasicBlockArgument) {
			foldableArgs[op] = val;
		}
	}

	if (foldableOps.empty() && foldableArgs.empty()) {
		return;
	}

	// For each foldable operation, create a constant and replace all uses
	std::unordered_map<const Operation*, Operation*> replacementMap;

	// First: create replacement constants for foldable non-arg operations
	for (auto& block : func->getBasicBlocks()) {
		std::vector<std::pair<Operation*, ConstantValue>> toReplace;

		for (auto& op : block->getOperations()) {
			auto it = foldableOps.find(op.get());
			if (it != foldableOps.end()) {
				toReplace.push_back({op.get(), it->second});
			}
		}

		for (auto& [op, val] : toReplace) {
			auto constOp = createConstantOp(val, nextId);
			auto* constPtr = constOp.get();
			block->addOperationBefore(op, constOp);
			replacementMap[op] = constPtr;
		}
	}

	// Create replacement constants for foldable block arguments
	for (auto& block : func->getBasicBlocks()) {
		for (auto& arg : block->getArguments()) {
			auto it = foldableArgs.find(arg.get());
			if (it != foldableArgs.end()) {
				auto constOp = createConstantOp(it->second, nextId);
				auto* constPtr = constOp.get();
				// Insert at the beginning of the block's operations
				if (!block->getOperations().empty()) {
					auto* firstOp = block->getOperations().front().get();
					block->addOperationBefore(firstOp, constOp);
				} else {
					block->addOperation(std::move(constOp));
				}
				replacementMap[arg.get()] = constPtr;
			}
		}
	}

	if (replacementMap.empty()) {
		return;
	}

	// Replace all uses across all blocks
	for (auto& block : func->getBasicBlocks()) {
		for (auto& op : block->getOperations()) {
			for (auto& [oldOp, newOp] : replacementMap) {
				replaceInputInOperation(op.get(), const_cast<Operation*>(oldOp), newOp);
			}
		}
	}
}

} // anonymous namespace

void ConstantFoldingPhase::apply(std::shared_ptr<IRGraph> ir) {
	for (auto& func : ir->getFunctionOperations()) {
		ConstantPropagationAnalysis analysis(func.get());
		analysis.run();
		applyFolding(func.get(), analysis.getState());
	}
}

} // namespace nautilus::compiler::ir
