
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"

namespace nautilus::compiler::ir {

FunctionAddressOfOperation::FunctionAddressOfOperation(const std::string& functionSymbol,
                                                       const std::string& functionName, void* functionPtr,
                                                       OperationIdentifier identifier)
    : Operation(Operation::OperationType::FunctionAddressOfOp, identifier, Type::ptr, {}),
      functionSymbol(functionSymbol), functionName(functionName), functionPtr(functionPtr) {
}

const std::string& FunctionAddressOfOperation::getFunctionSymbol() const {
	return functionSymbol;
}

const std::string& FunctionAddressOfOperation::getFunctionName() const {
	return functionName;
}

void* FunctionAddressOfOperation::getFunctionPtr() {
	return functionPtr;
}

} // namespace nautilus::compiler::ir
