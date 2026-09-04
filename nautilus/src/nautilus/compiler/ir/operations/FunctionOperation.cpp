
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include <algorithm>
#include <cassert>
#include <utility>

namespace nautilus::compiler::ir {

FunctionOperation::FunctionOperation(std::string name, std::vector<BasicBlock*> functionBasicBlocks,
                                     std::vector<Type> inputArgs, std::vector<std::string> inputArgNames,
                                     Type outputArg, std::vector<AllocaSpec> allocaSpecs,
                                     std::unordered_map<std::string, std::string> attributes,
                                     std::vector<RegionSpec> regionSpecs)
    : Operation(OperationType::FunctionOp, outputArg), name(std::move(name)),
      functionBasicBlocks(std::move(functionBasicBlocks)), inputArgs(std::move(inputArgs)),
      inputArgNames(std::move(inputArgNames)), allocaSpecs(std::move(allocaSpecs)), attributes(std::move(attributes)),
      regionSpecs(std::move(regionSpecs)) {
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

const std::vector<AllocaSpec>& FunctionOperation::getAllocaSpecs() const {
	return allocaSpecs;
}

const std::vector<RegionSpec>& FunctionOperation::getRegionSpecs() const {
	return regionSpecs;
}

const RegionSpec* FunctionOperation::findRegion(RegionIndex index) const {
	if (index >= regionSpecs.size()) {
		return nullptr;
	}
	return &regionSpecs[index];
}

bool FunctionOperation::isRegionNestedIn(RegionIndex inner, RegionIndex outer) const {
	if (outer == NO_REGION) {
		return true;
	}
	for (auto current = inner; current != NO_REGION;) {
		if (current == outer) {
			return true;
		}
		const auto* spec = findRegion(current);
		if (spec == nullptr) {
			return false;
		}
		current = spec->parent;
	}
	return false;
}

RegionIndex FunctionOperation::commonRegionAncestor(RegionIndex first, RegionIndex second) const {
	// Walking `first`'s chain and testing each link against `second` costs at most
	// depth^2 comparisons on a nesting that is a handful of levels deep in practice.
	for (auto current = first; current != NO_REGION;) {
		if (isRegionNestedIn(second, current)) {
			return current;
		}
		const auto* spec = findRegion(current);
		if (spec == nullptr) {
			break;
		}
		current = spec->parent;
	}
	return NO_REGION;
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
