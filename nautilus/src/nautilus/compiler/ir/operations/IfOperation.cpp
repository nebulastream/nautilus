
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"

namespace nautilus::compiler::ir {
IfOperation::IfOperation(common::Arena& arena, Operation* booleanValue, double probability)
    : Operation(arena, Operation::OperationType::IfOp, OperationIdentifier {0}, Type::v, {booleanValue}),
      probability(probability) {
}

Operation* IfOperation::getValue() const {
	return inputs[0];
}

const BasicBlockInvocation& IfOperation::getTrueBlockInvocation() const {
	return trueBlockInvocation;
}

BasicBlockInvocation& IfOperation::getTrueBlockInvocation() {
	return trueBlockInvocation;
}

const BasicBlockInvocation& IfOperation::getFalseBlockInvocation() const {
	return falseBlockInvocation;
}

BasicBlockInvocation& IfOperation::getFalseBlockInvocation() {
	return falseBlockInvocation;
}

void IfOperation::setTrueBlockInvocation(BasicBlock* trueBlockInvocation) {
	this->trueBlockInvocation.setBlock(trueBlockInvocation);
}

void IfOperation::setFalseBlockInvocation(BasicBlock* falseBlockInvocation) {
	this->falseBlockInvocation.setBlock(falseBlockInvocation);
}

BasicBlock* IfOperation::getMergeBlock() {
	return mergeBlock;
}

Operation* IfOperation::getBooleanValue() {
	return inputs[0];
}

void IfOperation::setBooleanValue(Operation* newBooleanValue) {
	this->inputs[0] = newBooleanValue;
}

void IfOperation::setMergeBlock(BasicBlock* mergeBlock) {
	this->mergeBlock = mergeBlock;
}

bool IfOperation::hasFalseCase() {
	return this->falseBlockInvocation.getBlock() != nullptr;
}

double IfOperation::getProbability() const {
	return this->probability;
}

bool IfOperation::classof(const Operation* op) {
	return op->getOperationType() == OperationType::IfOp;
}
} // namespace nautilus::compiler::ir
