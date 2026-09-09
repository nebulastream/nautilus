#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstdint>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

namespace nautilus::compiler::ir {

class BasicBlock;
class FunctionOperation;

/// One region() level of an operation's nesting, already resolved against the
/// enclosing FunctionOperation's region table (docs/region.md) so a consumer
/// never walks `RegionSpec::parent` itself.
struct ResolvedRegion {
	/// The region's name, or "<region>" when it was opened without one.
	std::string name;
	/// The region() call site's C++ source position; `file` is empty when unknown.
	std::string file;
	uint32_t line = 0;
	uint32_t column = 0;
};

/// A region nesting chain, outermost first. Empty for an operation that was not
/// traced inside any region.
using RegionChain = std::vector<ResolvedRegion>;

/// Where every IR object appears in one specific rendering of one specific
/// IRGraph, plus that rendering's text.
///
/// Produced by computeIRLocations(), which renders the graph and records the
/// line numbers *as it emits them* -- no consumer ever re-parses `text` to
/// recover a position. The MLIR backend uses this to tag emitted MLIR ops with
/// FileLineColLocs pointing into the dump it writes to disk.
///
/// Keyed by IR node address rather than by `$N` identifier: Nautilus restarts
/// `$N` and `Block_N` at 0 for every function, so an identifier is only unique
/// within one function, whereas the arena keeps every node's address unique and
/// stable for as long as the graph -- and therefore for as long as this map,
/// which is bound to the graph state it was computed from.
///
/// A map is a snapshot. Any pass that mints or removes an operation after
/// computeIRLocations() has run invalidates every line number in it, so it must
/// be computed after the last mutating pass.
struct IRLocationMap {
	/// The rendered dump, byte-identical to `IRGraph::toString(options)`.
	std::string text;

	/// 1-based line in `text` where each object's own text begins. An
	/// operation whose rendering spans several lines (a source-location or
	/// region trailer) is recorded at its first line.
	std::unordered_map<const Operation*, uint32_t> operationLines;
	std::unordered_map<const BasicBlock*, uint32_t> blockLines;
	std::unordered_map<const FunctionOperation*, uint32_t> functionLines;

	/// Interned region chains. Operations in the same region share one entry,
	/// which lets a backend cache whatever it builds per chain (a
	/// DISubprogram, say) by index instead of by structural comparison.
	std::vector<RegionChain> chains;
	/// Index into `chains` for every operation that has a region. Operations
	/// with NO_REGION are absent.
	std::unordered_map<const Operation*, uint32_t> operationChains;
	/// The same for blocks: a block carries its own region, which is what a
	/// block-argument store belongs to (the store exists at the block's entry,
	/// before any of its operations).
	std::unordered_map<const BasicBlock*, uint32_t> blockChains;

	/// The line @p operation starts on, or 0 if it was not rendered.
	[[nodiscard]] uint32_t lineOf(const Operation* operation) const;
	[[nodiscard]] uint32_t lineOf(const BasicBlock* block) const;
	[[nodiscard]] uint32_t lineOf(const FunctionOperation* function) const;

	/// @p operation's region nesting, outermost first; empty when it has none.
	[[nodiscard]] const RegionChain& chainOf(const Operation* operation) const;
	[[nodiscard]] const RegionChain& chainOf(const BasicBlock* block) const;

	/// The index in `chains` of @p operation's chain, or `NO_CHAIN`. A
	/// consumer that caches per chain -- a DWARF subprogram, say -- keys on
	/// this rather than comparing chains structurally.
	static constexpr uint32_t NO_CHAIN = std::numeric_limits<uint32_t>::max();
	[[nodiscard]] uint32_t chainIndexOf(const Operation* operation) const;
	[[nodiscard]] uint32_t chainIndexOf(const BasicBlock* block) const;
};

/// Renders @p graph and returns where everything in it landed, together with
/// the flattened region chain of every operation.
///
/// Must run after the last pass that mutates @p graph -- see IRLocationMap.
IRLocationMap computeIRLocations(const IRGraph& graph, const IRPrintOptions& options);

} // namespace nautilus::compiler::ir
