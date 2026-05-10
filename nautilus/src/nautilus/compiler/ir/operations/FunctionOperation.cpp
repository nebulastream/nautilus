
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include <utility>

namespace nautilus::compiler::ir {

FunctionOperation::FunctionOperation(std::string name, std::vector<std::unique_ptr<BasicBlock>>& functionBasicBlocks,
                                     std::vector<Type> inputArgs, std::vector<std::string> inputArgNames,
                                     Type outputArg, std::vector<AllocaSpec> allocaSpecs,
                                     std::unordered_map<std::string, std::string> attributes)
    : Operation(OperationType::FunctionOp, outputArg), name(std::move(name)),
      functionBasicBlocks(std::move(functionBasicBlocks)), inputArgs(std::move(inputArgs)),
      inputArgNames(std::move(inputArgNames)), allocaSpecs(std::move(allocaSpecs)), attributes(std::move(attributes)) {
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

BasicBlock* FunctionOperation::getEntryBlock() const {
	return functionBasicBlocks.empty() ? nullptr : functionBasicBlocks.front().get();
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

const std::vector<AllocaSpec>& FunctionOperation::getAllocaSpecs() const {
	return allocaSpecs;
}

bool FunctionOperation::hasAttribute(const std::string& key) const {
	return attributes.contains(key);
}

std::optional<std::string> FunctionOperation::getAttribute(const std::string& key) const {
	auto it = attributes.find(key);
	if (it != attributes.end()) {
		return it->second;
	}
	return std::nullopt;
}

} // namespace nautilus::compiler::ir
