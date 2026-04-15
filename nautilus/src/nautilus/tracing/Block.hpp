
#pragma once

#include "TraceOperation.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include <cinttypes>
#include <vector>

namespace nautilus::tracing {

struct operation_identifier {
	uint32_t blockIndex;
	uint32_t operationIndex;
};

/**
 * @brief Represents a basic block in a trace.
 *
 * Operations are stored as pointers that are owned by an Arena living in the
 * enclosing ExecutionTrace.  Keeping operations in the arena (rather than by
 * value inside a std::vector) delivers two benefits:
 *
 *   - pointer stability: addresses of operations are never invalidated when
 *     the operations vector or the blocks vector grows,
 *   - lower allocator pressure: TraceOperations (and their input vectors)
 *     are allocated from pre-allocated arena chunks instead of going through
 *     the general purpose allocator on every trace step.
 *
 * Moving operations between blocks is O(1) and never touches the heap: only
 * the pointer moves, the TraceOperation object itself stays put in the arena.
 */
class Block {
public:
	/**
	 * @brief Indicates if this is a control flow merge block -> such blocks could be loop headers.
	 */
	enum class Type : uint8_t { Default, ControlFlowMerge };

	/**
	 * @brief creates a new block with a specific block id.
	 * @param blockId
	 */
	explicit Block(uint32_t blockId);

	/**
	 * @brief Appends an (already arena-allocated) operation to this block.
	 */
	operation_identifier addOperation(TraceOperation* operation);

	/**
	 * @brief Adds a argument to the block
	 * @param value reference
	 */
	void addArgument(TypedValueRef ref);

	/**
	 * @brief defines the id of this block.
	 */
	uint32_t blockId;

	/**
	 * @brief defines the type of this block.
	 */
	Type type;

	/**
	 * @brief indicates a list of arguments that this block receives.
	 */
	std::vector<TypedValueRef> arguments;

	/**
	 * @brief Defines a list of operations this block contains.
	 *
	 * Pointers are non-owning; the referenced TraceOperations are owned by
	 * the ExecutionTrace's Arena.
	 */
	std::vector<TraceOperation*> operations;

	/**
	 * @brief Indicates successors of this block.
	 */
	std::vector<uint32_t> predecessors;
};

} // namespace nautilus::tracing
