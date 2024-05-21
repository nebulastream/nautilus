
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"

namespace nautilus::compiler::ir {
IfOperation::IfOperation(Operation* booleanValue) : Operation(Operation::OperationType::IfOp, Type::v, {booleanValue}) {
}

Operation* IfOperation::getValue() {
	return inputs[0];
}

BasicBlockInvocation& IfOperation::getTrueBlockInvocation() {
	return trueBlockInvocation;
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

std::string IfOperation::toString() {
	std::string baseString = "if " + getValue()->getIdentifier().toString() + " ? Block_" +
	                         trueBlockInvocation.getBlock()->getIdentifier() + '(';
	// "if " + comparatorOp->getIdentifier() + " ? Block_" + trueBlockInvocation.getBlock()->getIdentifier() + '(';
	if (trueBlockInvocation.getArguments().size() > 0) {
		baseString += trueBlockInvocation.getArguments()[0]->getIdentifier().toString();
		for (int i = 1; i < (int) trueBlockInvocation.getArguments().size(); ++i) {
			baseString += ", " + trueBlockInvocation.getArguments().at(i)->getIdentifier().toString();
		}
	}
	if (falseBlockInvocation.getBlock()) {
		baseString += ") : Block_" + falseBlockInvocation.getBlock()->getIdentifier() + '(';
		if (falseBlockInvocation.getArguments().size() > 0) {
			baseString += falseBlockInvocation.getArguments()[0]->getIdentifier().toString();
			for (int i = 1; i < (int) falseBlockInvocation.getArguments().size(); ++i) {
				baseString += ", " + falseBlockInvocation.getArguments().at(i)->getIdentifier().toString();
			}
		}
	}
	return baseString += ')';
}

bool IfOperation::hasFalseCase() {
	return this->falseBlockInvocation.getBlock() != nullptr;
}
} // namespace nautilus::compiler::ir
