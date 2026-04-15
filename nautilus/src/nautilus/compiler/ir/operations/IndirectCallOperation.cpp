
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include <utility>

namespace nautilus::compiler::ir {

namespace {
// Builds a contiguous arena-allocated input array of the form
// `[functionPtrOperand, inputArguments[0], inputArguments[1], ...]` and returns
// it. We can't use the Operation constructor's initializer_list overload here
// because the argument count is dynamic.
Operation** buildIndirectCallInputs(common::Arena& arena, Operation* functionPtrOperand,
                                    std::span<Operation* const> inputArguments) {
	const std::size_t total = 1 + inputArguments.size();
	auto* buffer = static_cast<Operation**>(arena.allocate(sizeof(Operation*) * total, alignof(Operation*)));
	buffer[0] = functionPtrOperand;
	std::copy(inputArguments.begin(), inputArguments.end(), buffer + 1);
	return buffer;
}
} // namespace

IndirectCallOperation::IndirectCallOperation(common::Arena& arena, OperationIdentifier identifier,
                                             Operation* functionPtrOperand, std::span<Operation* const> inputArguments,
                                             Type resultType, FunctionAttributes fnAttrs)
    : Operation(Operation::OperationType::IndirectCallOp, identifier, resultType), fnAttrs(std::move(fnAttrs)) {
	// inputs[0] = function pointer operand; inputs[1..] = call arguments
	this->inputs = buildIndirectCallInputs(arena, functionPtrOperand, inputArguments);
	this->numInputs = static_cast<uint32_t>(1 + inputArguments.size());
}

Operation* IndirectCallOperation::getFunctionPtrOperand() const {
	return inputs[0];
}

std::span<Operation* const> IndirectCallOperation::getInputArguments() const {
	return getInputs().subspan(1);
}

const FunctionAttributes& IndirectCallOperation::getFunctionAttributes() const {
	return fnAttrs;
}

bool IndirectCallOperation::classof(const Operation* op) {
	return op->getOperationType() == OperationType::IndirectCallOp;
}

} // namespace nautilus::compiler::ir
