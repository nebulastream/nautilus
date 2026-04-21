
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include <algorithm>
#include <cassert>
#include <utility>

namespace nautilus::compiler::ir {

FunctionOperation::FunctionOperation(std::string name, std::vector<BasicBlock*> functionBasicBlocks,
                                     std::vector<Type> inputArgs, std::vector<std::string> inputArgNames,
                                     Type outputArg, std::unordered_map<std::string, std::string> attributes)
    : Operation(OperationType::FunctionOp, outputArg), name(std::move(name)),
      functionBasicBlocks(std::move(functionBasicBlocks)), inputArgs(std::move(inputArgs)),
      inputArgNames(std::move(inputArgNames)), attributes(std::move(attributes)) {
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

const std::vector<BasicBlock*>& FunctionOperation::getBasicBlocks() const {
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

BasicBlock* FunctionOperation::getEntryBlock() const {
	return functionBasicBlocks.empty() ? nullptr : functionBasicBlocks.front();
}

void FunctionOperation::detachBasicBlock(BasicBlock* block) {
	assert(block != nullptr && "cannot detach null block");
	assert(block != getEntryBlock() && "cannot detach the function's entry block");
	assert(block->getPredecessors().empty() && "cannot detach a block that still has predecessors");
	auto it = std::find(functionBasicBlocks.begin(), functionBasicBlocks.end(), block);
	assert(it != functionBasicBlocks.end() && "block is not owned by this function");
	functionBasicBlocks.erase(it);
	// The BasicBlock's storage stays live in the arena; not freed here.
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
