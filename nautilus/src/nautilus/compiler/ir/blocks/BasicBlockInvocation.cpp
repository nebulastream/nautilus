
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"

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
	this->operations.emplace_back(argument);
}

void BasicBlockInvocation::removeArgument(uint64_t argumentIndex) {
	operations.erase(operations.begin() + argumentIndex);
}

void BasicBlockInvocation::replaceArgument(Operation* toReplace, Operation* replaceWith) {
	auto argumentIndex = 0;
	for (const auto& argument : operations) {
		if (argument == toReplace) {
			operations.at(argumentIndex) = replaceWith;
		}
		++argumentIndex;
	}
}

void BasicBlockInvocation::replaceArgument(const Operation* toReplace, Operation* replaceWith) {
	auto argumentIndex = 0;
	for (const auto& argument : operations) {
		if (argument == toReplace) {
			operations.at(argumentIndex) = replaceWith;
		}
		++argumentIndex;
	}
}

int BasicBlockInvocation::getOperationArgIndex(Operation* arg) {
	for (uint64_t i = 0; i < operations.size(); i++) {
		if (operations[i] == arg) {
			return i;
		}
	}
	return -1;
}

const std::vector<Operation*>& BasicBlockInvocation::getArguments() const {
	return operations;
}

} // namespace nautilus::compiler::ir
