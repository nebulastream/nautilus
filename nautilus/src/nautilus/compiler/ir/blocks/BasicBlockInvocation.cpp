
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

void BasicBlockInvocation::addArgument(common::Arena& arena, Operation* argument) {
	if (numInputs == capacity_) {
		// Grow geometrically so back-to-back appends amortise to O(1).
		// Block invocations very rarely exceed a handful of arguments, so
		// the initial small bucket avoids allocating a chunk for the
		// common case where only one or two arguments are added.
		uint32_t newCap = capacity_ == 0 ? 4u : capacity_ * 2u;
		auto* newInputs = static_cast<Operation**>(arena.allocate(sizeof(Operation*) * newCap, alignof(Operation*)));
		if (numInputs > 0) {
			std::copy(inputs, inputs + numInputs, newInputs);
		}
		inputs = newInputs;
		capacity_ = newCap;
	}
	inputs[numInputs++] = argument;
}

void BasicBlockInvocation::replaceArgument(Operation* toReplace, Operation* replaceWith) {
	std::replace(inputs, inputs + numInputs, toReplace, replaceWith);
}

void BasicBlockInvocation::replaceArgument(const Operation* toReplace, Operation* replaceWith) {
	std::replace(inputs, inputs + numInputs, const_cast<Operation*>(toReplace), replaceWith);
}

int BasicBlockInvocation::getOperationArgIndex(Operation* arg) {
	for (uint32_t i = 0; i < numInputs; i++) {
		if (inputs[i] == arg) {
			return static_cast<int>(i);
		}
	}
	return -1;
}

std::span<Operation* const> BasicBlockInvocation::getArguments() const {
	return getInputs();
}

bool BasicBlockInvocation::classof(const Operation* op) {
	return op->getOperationType() == OperationType::BlockInvocation;
}

} // namespace nautilus::compiler::ir
