
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include <algorithm>

namespace nautilus::compiler::ir {

BasicBlockInvocation::BasicBlockInvocation() : Operation(OperationType::BlockInvocation, Type::v) {
}

void BasicBlockInvocation::setBlock(BasicBlock* block) {
	this->basicBlock = block;
}

const BasicBlock* BasicBlockInvocation::getBlock() const {
	return basicBlock;
}

void BasicBlockInvocation::addArgument(Operation* argument) {
	inputs.emplace_back(argument);
}

void BasicBlockInvocation::removeArgument(uint64_t argumentIndex) {
	inputs.erase(inputs.begin() + argumentIndex);
}

void BasicBlockInvocation::replaceArgument(Operation* toReplace, Operation* replaceWith) {
	std::replace(inputs.begin(), inputs.end(), toReplace, replaceWith);
}

void BasicBlockInvocation::replaceArgument(const Operation* toReplace, Operation* replaceWith) {
	std::replace(inputs.begin(), inputs.end(), const_cast<Operation*>(toReplace), replaceWith);
}

int BasicBlockInvocation::getOperationArgIndex(Operation* arg) {
	for (uint64_t i = 0; i < inputs.size(); i++) {
		if (inputs[i] == arg) {
			return i;
		}
	}
	return -1;
}

const std::vector<Operation*>& BasicBlockInvocation::getArguments() const {
	return inputs;
}

bool BasicBlockInvocation::classof(const Operation* op) {
	return op->getOperationType() == OperationType::BlockInvocation;
}

} // namespace nautilus::compiler::ir
