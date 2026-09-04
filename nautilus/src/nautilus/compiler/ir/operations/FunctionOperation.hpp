
#pragma once

#include "nautilus/common/RegionAttributes.hpp"
#include "nautilus/compiler/ir/ExceptionRegion.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstddef>
#include <optional>
#include <unordered_map>

namespace nautilus::compiler::ir {

/// Per-function record of one stack allocation.  Each AllocaOperation in the
/// IR references one of these by index; backends emit the real `alloca`
/// instruction (or equivalent) once per entry in the function prologue.
struct AllocaSpec {
	size_t size;
	size_t align;
};

/// Per-function record of one region() call site (docs/region.md) whose body was traced
/// into this function: what the call site said about itself, and which region encloses it.
///
/// Operations reference these by index through Operation::getRegionIndex(). The region's
/// boundary blocks are deliberately not part of this: by the time the block-cleanup passes
/// are done, the entry and exit block a region added are merged away, while the operations
/// that came out of the body keep pointing here.
struct RegionSpec {
	RegionAttributes attributes;
	/// The region this one sits inside, or NO_REGION for a region opened directly in the
	/// function body. Walking this chain gives an operation's full region nesting.
	RegionIndex parent = NO_REGION;
	/// Identity of this region within the module, assigned once by the trace-to-IR
	/// conversion and never reused. Operations and blocks reference a region by its
	/// index in *this* function's table, which says nothing outside the function; the id
	/// is what a diagnostic, a dump or a cross-function report can name a region by.
	uint32_t id = 0;
};

class FunctionOperation : public Operation {
public:
	explicit FunctionOperation(std::string name, std::vector<BasicBlock*> functionBasicBlocks,
	                           std::vector<Type> inputArgs, std::vector<std::string> inputArgNames, Type outputArg,
	                           std::vector<AllocaSpec> allocaSpecs = {},
	                           std::unordered_map<std::string, std::string> attributes = {},
	                           std::vector<RegionSpec> regionSpecs = {});

	~FunctionOperation() = default;

	[[nodiscard]] const std::string& getName() const;

	const BasicBlock& getFunctionBasicBlock() const;
	const std::vector<BasicBlock*>& getBasicBlocks() const;

	/// Returns the function's entry block (the first entry of
	/// `functionBasicBlocks`). Passes should use this helper rather than
	/// positional access so the entry-block convention is documented at
	/// every use site.
	[[nodiscard]] BasicBlock* getEntryBlock() const;

	/// Unlinks @p block from the function's block list. This is *not* a
	/// free — every block is arena-allocated and persists in the arena
	/// until the enclosing `IRGraph` is destroyed. The method just removes
	/// the pointer from `functionBasicBlocks`, making the block
	/// unreachable from the CFG. Asserts that @p block is neither the
	/// entry block nor still has predecessors (those are pass bugs).
	void detachBasicBlock(BasicBlock* block);

	[[nodiscard]] const std::vector<Type>& getInputArgs() const;

	[[nodiscard]] Type getOutputArg() const;

	[[nodiscard]] const std::vector<std::string>& getInputArgNames() const;

	/// Returns the function's alloca table.  Each entry corresponds to one
	/// stack allocation site observed during tracing; AllocaOperation nodes
	/// in this function's body reference entries by index.
	[[nodiscard]] const std::vector<AllocaSpec>& getAllocaSpecs() const;

	/// Returns the function's region table.  Empty for a function traced without
	/// region(), or traced by a tracer that inlines region bodies; otherwise indexed by
	/// Operation::getRegionIndex().
	[[nodiscard]] const std::vector<RegionSpec>& getRegionSpecs() const;

	/// Returns the region @p index describes, or nullptr for NO_REGION and for an index
	/// this function has no entry for.
	[[nodiscard]] const RegionSpec* findRegion(RegionIndex index) const;

	/// True when @p inner is @p outer or is nested inside it. NO_REGION is the outermost
	/// scope -- the function body -- so everything is nested in it.
	[[nodiscard]] bool isRegionNestedIn(RegionIndex inner, RegionIndex outer) const;

	/// The innermost region containing both @p first and @p second: their nearest common
	/// ancestor in the nesting, NO_REGION if they share none.
	///
	/// This is what a block's region has to widen to when code from two regions ends up
	/// in one block -- which is what the block-cleanup passes do to a region's seams.
	[[nodiscard]] RegionIndex commonRegionAncestor(RegionIndex first, RegionIndex second) const;

	[[nodiscard]] bool hasAttribute(const std::string& key) const;
	[[nodiscard]] std::optional<std::string> getAttribute(const std::string& key) const;

	/// Exception-region side table, populated by
	/// ExceptionRegionPreparationPass (terminal pass). Empty until that pass
	/// runs; `std::nullopt` means the pass has not yet visited this function.
	std::optional<FunctionExceptionRegion> exceptionRegion;

	static bool classof(const Operation* Op);

private:
	std::string name;
	std::vector<BasicBlock*> functionBasicBlocks;
	std::vector<Type> inputArgs;
	std::vector<std::string> inputArgNames;
	std::vector<AllocaSpec> allocaSpecs;
	std::unordered_map<std::string, std::string> attributes;
	std::vector<RegionSpec> regionSpecs;
};
} // namespace nautilus::compiler::ir
