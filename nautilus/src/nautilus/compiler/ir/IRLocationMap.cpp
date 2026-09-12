#include "nautilus/compiler/ir/IRLocationMap.hpp"
#include "nautilus/compiler/ir/ExceptionRegion.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include <map>
#include <utility>

namespace nautilus::compiler::ir {

namespace {

/// Fills an IRLocationMap's line tables from the renderer's callbacks.
///
/// `try_emplace` rather than assignment: an operation can be rendered more than
/// once (a landing pad's body is printed both in its block and in the trailing
/// `exception_region:` section), and the first rendering is the one a consumer
/// means when it asks where the operation is.
class RecordingSink final : public IRLineSink {
public:
	explicit RecordingSink(IRLocationMap& map) : map(map) {
	}

	void line(uint32_t lineNo, const void* owner, IRLineKind kind) override {
		switch (kind) {
		case IRLineKind::Function:
			map.functionLines.try_emplace(static_cast<const FunctionOperation*>(owner), lineNo);
			break;
		case IRLineKind::Block:
			map.blockLines.try_emplace(static_cast<const BasicBlock*>(owner), lineNo);
			break;
		case IRLineKind::Operation:
			map.operationLines.try_emplace(static_cast<const Operation*>(owner), lineNo);
			break;
		}
	}

private:
	IRLocationMap& map;
};

/// Identity of one region: its index is only meaningful within its own
/// function's table, so the function is part of the key.
using RegionKey = std::pair<const FunctionOperation*, RegionIndex>;

/// Returns the index in `map.chains` of @p index's flattened nesting chain,
/// building it (and its ancestors) on first request. One entry per region, so
/// every operation in a region shares a chain index.
uint32_t internChain(const FunctionOperation& function, RegionIndex index, IRLocationMap& map,
                     std::map<RegionKey, uint32_t>& cache) {
	const RegionKey key {&function, index};
	if (auto it = cache.find(key); it != cache.end()) {
		return it->second;
	}

	const RegionSpec* spec = function.findRegion(index);
	// A dangling index cannot name a chain; treat it as no nesting. Cached so a
	// malformed graph costs one lookup, not one per operation.
	if (spec == nullptr) {
		const auto chainIndex = static_cast<uint32_t>(map.chains.size());
		map.chains.emplace_back();
		cache.emplace(key, chainIndex);
		return chainIndex;
	}

	// Outermost first; the parent's chain is a prefix of this one.
	RegionChain chain;
	if (spec->parent != NO_REGION) {
		chain = map.chains[internChain(function, spec->parent, map, cache)];
	}

	ResolvedRegion resolved;
	resolved.name = spec->attributes.hasName() ? spec->attributes.name : "<region>";
	if (spec->attributes.location.isKnown()) {
		resolved.file = spec->attributes.location.file;
		resolved.line = spec->attributes.location.line;
		resolved.column = spec->attributes.location.column;
	}
	chain.push_back(std::move(resolved));

	const auto chainIndex = static_cast<uint32_t>(map.chains.size());
	map.chains.push_back(std::move(chain));
	cache.emplace(key, chainIndex);
	return chainIndex;
}

void recordRegion(const FunctionOperation& function, const Operation* operation, IRLocationMap& map,
                  std::map<RegionKey, uint32_t>& cache) {
	if (operation->getRegionIndex() == NO_REGION) {
		return;
	}
	map.operationChains.try_emplace(operation, internChain(function, operation->getRegionIndex(), map, cache));
}

void recordRegion(const FunctionOperation& function, const BasicBlock* block, IRLocationMap& map,
                  std::map<RegionKey, uint32_t>& cache) {
	if (block->getRegionIndex() == NO_REGION) {
		return;
	}
	map.blockChains.try_emplace(block, internChain(function, block->getRegionIndex(), map, cache));
}

} // namespace

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

IRLocationMap computeIRLocations(const IRGraph& graph, const IRPrintOptions& options) {
	IRLocationMap map;

	// Rendering and recording are one traversal, so the tables cannot describe
	// a layout the text does not have.
	RecordingSink sink(map);
	IRPrintOptions renderOptions = options;
	renderOptions.sink = &sink;
	map.text = graph.toString(renderOptions);

	// Regions come from the region table rather than the text, so they need a
	// second walk over the same snapshot.
	std::map<RegionKey, uint32_t> chainCache;
	for (const auto* function : graph.getFunctionOperations()) {
		for (const auto* block : function->getBasicBlocks()) {
			recordRegion(*function, block, map, chainCache);
			for (const auto* operation : block->getOperations()) {
				recordRegion(*function, operation, map, chainCache);
			}
		}
		if (function->exceptionRegion.has_value()) {
			for (const auto& pad : function->exceptionRegion->pads) {
				recordRegion(*function, pad.block, map, chainCache);
				for (const auto* operation : pad.block->getOperations()) {
					recordRegion(*function, operation, map, chainCache);
				}
			}
		}
	}

	return map;
}

} // namespace nautilus::compiler::ir
