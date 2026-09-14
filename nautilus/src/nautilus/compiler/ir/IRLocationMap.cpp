#include "nautilus/compiler/ir/IRLocationMap.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"

namespace nautilus::compiler::ir {


uint32_t IRLocationMap::lineOf(const Operation* operation) const {
	auto it = operationLines.find(operation);
	return it != operationLines.end() ? it->second : 0;
}

uint32_t IRLocationMap::lineOf(const BasicBlock* block) const {
	auto it = blockLines.find(block);
	return it != blockLines.end() ? it->second : 0;
}

uint32_t IRLocationMap::lineOf(const FunctionOperation* function) const {
	auto it = functionLines.find(function);
	return it != functionLines.end() ? it->second : 0;
}

const RegionChain& IRLocationMap::chainOf(const Operation* operation) const {
	static const RegionChain empty;
	auto it = operationChains.find(operation);
	return it != operationChains.end() ? chains[it->second] : empty;
}

const RegionChain& IRLocationMap::chainOf(const BasicBlock* block) const {
	static const RegionChain empty;
	auto it = blockChains.find(block);
	return it != blockChains.end() ? chains[it->second] : empty;
}

uint32_t IRLocationMap::chainIndexOf(const Operation* operation) const {
	auto it = operationChains.find(operation);
	return it != operationChains.end() ? it->second : NO_CHAIN;
}

uint32_t IRLocationMap::chainIndexOf(const BasicBlock* block) const {
	auto it = blockChains.find(block);
	return it != blockChains.end() ? it->second : NO_CHAIN;
}

} // namespace nautilus::compiler::ir
