
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include <algorithm>

namespace nautilus::compiler::ir {

BasicBlockInvocation::BasicBlockInvocation() : Operation(OperationType::BlockInvocation, Type::v) {
}

void BasicBlockInvocation::setBlock(BasicBlock* block) {
	// Pure setter: predecessor bookkeeping is the responsibility of the
	// enclosing `BasicBlock` helpers that know which block owns this
	// invocation (`BasicBlock::addNextBlock`, `replaceTerminatorOperation`,
	// `replaceSuccessor`). An invocation sub-object does not have a stable
	// pointer back to its enclosing block, and adding one would bloat the
	// hot path for every IR op.
	this->basicBlock = block;
}

const BasicBlock* BasicBlockInvocation::getBlock() const {
	return basicBlock;
}

void BasicBlockInvocation::addArgument(common::Arena& arena, Operation* argument) {
	const std::size_t size = inputs.size();
	if (size == capacity_) {
		// Grow geometrically so back-to-back appends amortise to O(1).
		// Block invocations very rarely exceed a handful of arguments, so
		// the initial small bucket avoids allocating a chunk for the
		// common case where only one or two arguments are added.
		uint32_t newCap = capacity_ == 0 ? 4u : capacity_ * 2u;
		auto* newInputs = static_cast<Operation**>(arena.allocate(sizeof(Operation*) * newCap, alignof(Operation*)));
		if (size > 0) {
			std::copy(inputs.begin(), inputs.end(), newInputs);
		}
		inputs = std::span<Operation*>(newInputs, size);
		capacity_ = newCap;
	}
	// Extend the span by one slot and fill it. The underlying buffer has
	// `capacity_` elements, so widening the view is always in-bounds.
	Operation** data = inputs.data();
	inputs = std::span<Operation*>(data, size + 1);
	data[size] = argument;
}

void BasicBlockInvocation::clearArguments() {
	// Reset the span and capacity so the invocation looks empty. The
	// previous arena buffer (if any) stays live in the arena until the
	// enclosing IR is destroyed; its slot is simply unreferenced, which
	// is the normal arena ownership model.
	inputs = std::span<Operation*>();
	capacity_ = 0;
}

void BasicBlockInvocation::replaceArgument(Operation* toReplace, Operation* replaceWith) {
	std::replace(inputs.begin(), inputs.end(), toReplace, replaceWith);
}

void BasicBlockInvocation::replaceArgument(const Operation* toReplace, Operation* replaceWith) {
	std::replace(inputs.begin(), inputs.end(), const_cast<Operation*>(toReplace), replaceWith);
}

int BasicBlockInvocation::getOperationArgIndex(Operation* arg) {
	for (std::size_t i = 0; i < inputs.size(); i++) {
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
