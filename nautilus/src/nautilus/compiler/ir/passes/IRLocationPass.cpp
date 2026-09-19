#include "nautilus/compiler/ir/passes/IRLocationPass.hpp"
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
                     std::map<RegionKey, uint32_t>& cache, const std::map<RegionKey, uint32_t>& openLines) {
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
		chain = map.chains[internChain(function, spec->parent, map, cache, openLines)];
	}

	ResolvedRegion resolved;
	resolved.name = spec->attributes.hasName() ? spec->attributes.name : "<region>";
	if (spec->attributes.location.isKnown()) {
		resolved.file = spec->attributes.location.file;
		resolved.line = spec->attributes.location.line;
		resolved.column = spec->attributes.location.column;
	}
	if (auto it = openLines.find(key); it != openLines.end()) {
		resolved.irLine = it->second;
	}
	chain.push_back(std::move(resolved));

	const auto chainIndex = static_cast<uint32_t>(map.chains.size());
	map.chains.push_back(std::move(chain));
	cache.emplace(key, chainIndex);
	return chainIndex;
}

void recordRegion(const FunctionOperation& function, const Operation* operation, IRLocationMap& map,
                  std::map<RegionKey, uint32_t>& cache, const std::map<RegionKey, uint32_t>& openLines) {
	if (operation->getRegionIndex() == NO_REGION) {
		return;
	}
	map.operationChains.try_emplace(operation,
	                                internChain(function, operation->getRegionIndex(), map, cache, openLines));
}

void recordRegion(const FunctionOperation& function, const BasicBlock* block, IRLocationMap& map,
                  std::map<RegionKey, uint32_t>& cache, const std::map<RegionKey, uint32_t>& openLines) {
	if (block->getRegionIndex() == NO_REGION) {
		return;
	}
	map.blockChains.try_emplace(block, internChain(function, block->getRegionIndex(), map, cache, openLines));
}

/// Notes @p line as a candidate opening line for @p index and for every region
/// enclosing it: a region opens no later than the first line of anything traced
/// anywhere inside it, nested regions included.
void noteOpenLine(const FunctionOperation& function, RegionIndex index, uint32_t line,
                  std::map<RegionKey, uint32_t>& openLines) {
	if (line == 0) {
		return;
	}
	for (RegionIndex current = index; current != NO_REGION;) {
		const RegionKey key {&function, current};
		auto [it, inserted] = openLines.try_emplace(key, line);
		if (!inserted) {
			if (line >= it->second) {
				// This region and its ancestors already start no later.
				break;
			}
			it->second = line;
		}
		const RegionSpec* spec = function.findRegion(current);
		current = spec != nullptr ? spec->parent : NO_REGION;
	}
}

/// Calls @p visit with (function, region index, line) for everything the map
/// has a line for that was traced inside a region.
template <typename Visit>
void forEachRegionedObject(const IRGraph& graph, const IRLocationMap& map, Visit visit) {
	const auto visitBlock = [&](const FunctionOperation& function, const BasicBlock* block) {
		if (block->getRegionIndex() != NO_REGION) {
			visit(function, block->getRegionIndex(), map.lineOf(block));
		}
		for (const auto* operation : block->getOperations()) {
			if (operation->getRegionIndex() != NO_REGION) {
				visit(function, operation->getRegionIndex(), map.lineOf(operation));
			}
		}
	};
	for (const auto* function : graph.getFunctionOperations()) {
		for (const auto* block : function->getBasicBlocks()) {
			visitBlock(*function, block);
		}
		if (function->exceptionRegion.has_value()) {
			for (const auto& pad : function->exceptionRegion->pads) {
				visitBlock(*function, pad.block);
			}
		}
	}
}

} // namespace

IRLocationMap computeIRLocations(const IRGraph& graph, const IRPrintOptions& options) {
	IRLocationMap map;

	// Rendering and recording are one traversal, so the tables cannot describe
	// a layout the text does not have.
	RecordingSink sink(map);
	IRPrintOptions renderOptions = options;
	renderOptions.sink = &sink;
	map.text = graph.toString(renderOptions);

	// Regions come from the region table rather than the text, so they need a
	// second walk over the same snapshot. Two passes: where each region opens
	// has to be known before the chains that quote it are interned.
	std::map<RegionKey, uint32_t> openLines;
	forEachRegionedObject(graph, map, [&](const FunctionOperation& function, RegionIndex index, uint32_t line) {
		noteOpenLine(function, index, line, openLines);
	});

	std::map<RegionKey, uint32_t> chainCache;
	for (const auto* function : graph.getFunctionOperations()) {
		for (const auto* block : function->getBasicBlocks()) {
			recordRegion(*function, block, map, chainCache, openLines);
			for (const auto* operation : block->getOperations()) {
				recordRegion(*function, operation, map, chainCache, openLines);
			}
		}
		if (function->exceptionRegion.has_value()) {
			for (const auto& pad : function->exceptionRegion->pads) {
				recordRegion(*function, pad.block, map, chainCache, openLines);
				for (const auto* operation : pad.block->getOperations()) {
					recordRegion(*function, operation, map, chainCache, openLines);
				}
			}
		}
	}

	return map;
}
bool IRLocationPass::apply(IRGraph& ir) {
	result = std::make_shared<IRLocationMap>(computeIRLocations(ir, IRPrintOptions {}));
	// Published on the graph so a backend downstream of the pass manager reads
	// the map the pipeline produced rather than re-rendering the graph.
	ir.setLocationMap(result);
	return false;
}

} // namespace nautilus::compiler::ir
