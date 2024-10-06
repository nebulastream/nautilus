
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include <utility>

namespace nautilus::compiler::ir {

FunctionOperation::FunctionOperation(std::string name, std::vector<std::unique_ptr<BasicBlock>>& functionBasicBlocks,
                                     std::vector<Type> inputArgs, std::vector<std::string> inputArgNames,
                                     Type outputArg)
    : Operation(OperationType::FunctionOp, outputArg), name(std::move(name)),
      functionBasicBlocks(std::move(functionBasicBlocks)), inputArgs(std::move(inputArgs)),
      inputArgNames(std::move(inputArgNames)) {
}

const std::string& FunctionOperation::getName() const {
	return name;
}

const BasicBlock& FunctionOperation::getFunctionBasicBlock() const {
	return *functionBasicBlocks.front();
}

const std::vector<Type>& FunctionOperation::getInputArgs() const {
	return inputArgs;
}

const std::vector<std::unique_ptr<BasicBlock>>& FunctionOperation::getBasicBlocks() const {
	return functionBasicBlocks;
}

Type FunctionOperation::getOutputArg() const {
	return getStamp();
}

bool FunctionOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::FunctionOp;
}

const std::vector<std::string>& FunctionOperation::getInputArgNames() const {
	return inputArgNames;
}

} // namespace nautilus::compiler::ir
