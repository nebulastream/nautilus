#pragma once

#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <span>

namespace nautilus::compiler::ir {

/**
 * @brief Argument list passed when control flow enters a successor block.
 *
 * BasicBlockInvocation is always an embedded sub-object of a terminator
 * Operation (BranchOperation, IfOperation): both branches of an `if` and
 * the single successor of a `br` each own one. The argument array is built
 * incrementally during IR construction (one argument is appended per
 * matching block-argument slot) and frozen once the terminator is fully
 * wired.
 *
 * Storage is allocated from the surrounding `common::Arena`. Each
 * `addArgument` walks into a fresh, doubled buffer when the existing
 * one runs out of room; the previous buffer is released back to the
 * arena on the next `softReset` (the arena owns it, not the
 * invocation), so the small amount of fragmentation is bounded by the
 * arena lifetime. In practice block invocations have 0–4 arguments so
 * the doubling growth never advances past the second buffer.
 */
class BasicBlockInvocation : public Operation {
public:
	BasicBlockInvocation();

	~BasicBlockInvocation() = default;

	void setBlock(BasicBlock* block);

	[[nodiscard]] const BasicBlock* getBlock() const;

	/// Appends @p argument, allocating a larger backing buffer from
	/// @p arena when the current one is full.
	void addArgument(common::Arena& arena, Operation* argument);

	void replaceArgument(Operation* toReplace, Operation* replaceWith);

	void replaceArgument(const Operation* toReplace, Operation* replaceWith);

	std::span<Operation* const> getArguments() const;

	/**
	 * @brief Iterate over args, find arg that matches Operation* and return index.
	 *
	 * @return int: arg index for provided Operation*. Is -1 if no arg that matches Operation* was found.
	 */
	int getOperationArgIndex(Operation*);

	static bool classof(const Operation* op);

private:
	// Arguments are stored in the base Operation::inputs buffer — block-argument
	// edges are real SSA value edges, so generic passes that walk getInputs()
	// see them automatically without a special case. `capacity_` tracks the
	// physical buffer size so growth can avoid reallocating per push when the
	// caller appends multiple arguments back-to-back.
	BasicBlock* basicBlock = nullptr;
	uint32_t capacity_ = 0;
};

} // namespace nautilus::compiler::ir
