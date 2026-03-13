
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include <utility>

namespace nautilus::compiler::ir {

IndirectCallOperation::IndirectCallOperation(OperationIdentifier identifier, Operation* functionPtrOperand,
                                             std::vector<Operation*> inputArguments, Type resultType,
                                             FunctionAttributes fnAttrs)
    : Operation(Operation::OperationType::IndirectCallOp, identifier, resultType), fnAttrs(std::move(fnAttrs)) {
	// inputs[0] = function pointer operand; inputs[1..] = call arguments
	inputs.reserve(1 + inputArguments.size());
	inputs.push_back(functionPtrOperand);
	inputs.insert(inputs.end(), inputArguments.begin(), inputArguments.end());
}

Operation* IndirectCallOperation::getFunctionPtrOperand() const {
	return inputs[0];
}

std::vector<Operation*> IndirectCallOperation::getInputArguments() const {
	return std::vector<Operation*>(inputs.begin() + 1, inputs.end());
}

const FunctionAttributes& IndirectCallOperation::getFunctionAttributes() const {
	return fnAttrs;
}

} // namespace nautilus::compiler::ir
