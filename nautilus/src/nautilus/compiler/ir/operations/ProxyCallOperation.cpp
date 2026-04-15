
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include <utility>

namespace nautilus::compiler::ir {
ProxyCallOperation::ProxyCallOperation(common::Arena& arena, OperationIdentifier identifier,
                                       std::span<Operation* const> inputArguments, Type resultType)
    : Operation(arena, Operation::OperationType::ProxyCallOp, identifier, resultType, inputArguments) {
}

ProxyCallOperation::ProxyCallOperation(common::Arena& arena, const std::string& functionSymbol,
                                       const std::string& functionName, void* functionPtr,
                                       OperationIdentifier identifier, std::span<Operation* const> inputArguments,
                                       Type resultType, const FunctionAttributes fnAttrs)
    : Operation(arena, Operation::OperationType::ProxyCallOp, identifier, resultType, inputArguments),
      mangedFunctionSymbol(functionSymbol), functionName(functionName), functionPtr(functionPtr), fnAttrs(fnAttrs) {
}

std::span<Operation* const> ProxyCallOperation::getInputArguments() const {
	return getInputs();
}

void ProxyCallOperation::setInputArguments(common::Arena& arena, std::span<Operation* const> newInputArguments) {
	// Re-allocate a new buffer of the requested size from the arena and
	// rebind the inputs view. The previous buffer is left in the arena
	// (it is reclaimed in bulk on the next reset).
	if (newInputArguments.empty()) {
		this->inputs = nullptr;
		this->numInputs = 0;
		return;
	}
	auto* newInputs = allocateInputs(arena, newInputArguments.size());
	std::copy(newInputArguments.begin(), newInputArguments.end(), newInputs);
	this->inputs = newInputs;
	this->numInputs = static_cast<uint32_t>(newInputArguments.size());
}

const std::string& ProxyCallOperation::getFunctionName() const {
	return functionName;
}

const std::string& ProxyCallOperation::getFunctionSymbol() const {
	return mangedFunctionSymbol;
}

void* ProxyCallOperation::getFunctionPtr() {
	return functionPtr;
}

const FunctionAttributes& ProxyCallOperation::getFunctionAttributes() const {
	return fnAttrs;
}

bool ProxyCallOperation::classof(const Operation* op) {
	return op->getOperationType() == OperationType::ProxyCallOp;
}

} // namespace nautilus::compiler::ir
