
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"

namespace nautilus::compiler::ir {
IfOperation::IfOperation(Operation* booleanValue, double probability)
    : Operation(Operation::OperationType::IfOp, Type::v, {booleanValue}), probability(probability) {
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

BasicBlockPtr IfOperation::getMergeBlock() {
	return mergeBlock.lock();
}

Operation* IfOperation::getBooleanValue() {
	return inputs[0];
}

void IfOperation::setBooleanValue(Operation* newBooleanValue) {
	this->inputs[0] = newBooleanValue;
}

void IfOperation::setMergeBlock(BasicBlockPtr mergeBlock) {
	this->mergeBlock = mergeBlock;
}

bool IfOperation::hasFalseCase() {
	return this->falseBlockInvocation.getBlock() != nullptr;
}

double IfOperation::getProbability() const {
	return this->probability;
}
} // namespace nautilus::compiler::ir
