
#pragma once

#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <compare>
#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace nautilus::compiler::ir {

/**
 * @brief Strong typedef for a BasicBlock id.
 *
 * Mirrors `OperationIdentifier`: a `constexpr` value type wrapping a
 * `uint32_t`, so passing it by value is trivial and raw integer ids
 * cannot be accidentally mixed with op ids in containers or function
 * signatures. Implicit construction from `uint32_t` is allowed so
 * call sites that already hold a raw block-id (e.g. the tracing layer)
 * don't need per-site conversions.
 */
class BlockIdentifier {
public:
	constexpr BlockIdentifier(uint32_t id) : id(id) {
	}

	constexpr auto operator<=>(const BlockIdentifier&) const = default;
	constexpr bool operator==(const BlockIdentifier&) const = default;

	[[nodiscard]] constexpr uint32_t getId() const {
		return id;
	}

private:
	uint32_t id;
};

/**
 * @brief A BasicBlock used for control flow in Nautilus IR.
 *
 * A BasicBlock owns a list of `Operation*` and `BasicBlockArgument*`
 * pointers. The pointed-to objects are not owned by the block: every
 * Operation and BasicBlockArgument is allocated from the `common::Arena`
 * that is passed in at construction and lives for as long as that arena
 * lives. Destroying a BasicBlock therefore does not free the operations
 * it contains; the arena does, in bulk, when it is reset or destroyed.
 *
 * Rationale: the IR graph is produced once per compile and discarded at
 * the next compile cycle. Bulk arena deallocation is both faster and more
 * cache-friendly than per-op `delete`, and since every edge in the IR is
 * already a raw `Operation*` (`Operation::inputs`, `BasicBlockInvocation::
 * basicBlock`) the arena's stable-pointer guarantee matches the existing
 * invariants exactly.
 */
class BasicBlock {
public:
	/**
	 * @brief Constructs an (empty) BasicBlock.
	 *
	 * All subsequent Operations minted via @ref addOperation are allocated
	 * from @p arena.  The arena must outlive the block.
	 */
	explicit BasicBlock(common::Arena& arena, BlockIdentifier identifier, std::vector<BasicBlockArgument*> arguments);

	~BasicBlock();

	[[nodiscard]] BlockIdentifier getIdentifier() const;

	[[nodiscard]] const std::vector<Operation*>& getOperations() const;

	[[nodiscard]] Operation* getOperationAt(size_t index);

	[[nodiscard]] Operation* getTerminatorOp();

	[[nodiscard]] const std::vector<BasicBlockArgument*>& getArguments() const;

	/// Allocates a new operation of type T in the arena and appends it to
	/// this block. Returns the freshly created operation.
	///
	/// The owning arena is always injected as the first constructor
	/// argument so operations that need it (to allocate their inputs
	/// buffer) have it and those that don't simply ignore the reference.
	/// This keeps the factory uniform and makes the call site the
	/// single source of truth for where the arena comes from.
	template <typename T, typename... Args>
	T* addOperation(Args&&... args) {
		auto* op = arena_->create<T>(*arena_, std::forward<Args>(args)...);
		operations.push_back(op);
		return op;
	}

	/// Appends an already-arena-allocated operation to this block.
	BasicBlock* addOperation(Operation* operation);

	BasicBlock* addNextBlock(BasicBlock* nextBlock);

	void addNextBlock(BasicBlock* nextBlock, std::span<Operation* const> inputArguments);

	BasicBlock* addTrueBlock(BasicBlock* thenBlock);

	BasicBlock* addFalseBlock(BasicBlock* elseBlock);

	void replaceOperation(size_t operationIndex, Operation* operation);

	void removeOperation(Operation* operation);

	void addOperationBefore(Operation* before, Operation* operation);

	uint64_t getIndexOfArgument(Operation* arg);

	void replaceTerminatorOperation(Operation* newTerminatorOperation);

	[[nodiscard]] std::pair<const BasicBlock*, const BasicBlock*> getNextBlocks();

	/// Returns the blocks whose terminator targets this block. The list is
	/// maintained as an invariant by the CFG-wiring helpers
	/// (`addNextBlock`, `replaceTerminatorOperation`, `replaceSuccessor`,
	/// `BasicBlockInvocation::setBlock`). It is a plain heap `std::vector`,
	/// not arena-backed, because predecessors are a derived cache that
	/// grows and shrinks during transformation.
	[[nodiscard]] const std::vector<BasicBlock*>& getPredecessors() const;

	/// Returns the target blocks of this block's terminator. Delegates to
	/// `ir::getSuccessors` on the terminator op.
	[[nodiscard]] std::vector<BasicBlock*> getSuccessors();

	/// Redirects every edge from this block to @p from into @p to.
	///
	/// Walks the terminator's outgoing invocations; for each whose target
	/// is @p from, retargets it to @p to. Keeps the predecessor lists of
	/// both @p from and @p to consistent. The arity of every invocation's
	/// argument list is unchanged.
	void replaceSuccessor(BasicBlock* from, BasicBlock* to);

	// ── Predecessor-invariant maintenance (called by BasicBlockInvocation) ──
	/// Appends @p predecessor to this block's predecessor list. Intended
	/// for internal use by the CFG-wiring helpers — callers at the pass
	/// level should go through the high-level APIs (`addNextBlock`,
	/// `replaceSuccessor`, `BasicBlockInvocation::setBlock`).
	void addPredecessor(BasicBlock* predecessor);

	/// Removes one occurrence of @p predecessor from the predecessor list.
	/// A block may appear more than once (e.g. both arms of an `if` that
	/// target the same block); callers must invoke this once per edge they
	/// are removing, matching the number of invocations they tear down.
	void removePredecessor(BasicBlock* predecessor);

	/// Clears the predecessor list. Used by the pass manager to rebuild
	/// the predecessor invariant from the CFG at the start of a run.
	void clearPredecessors();

private:
	common::Arena* arena_;
	BlockIdentifier identifier;
	std::vector<Operation*> operations;
	std::vector<BasicBlockArgument*> arguments;
	std::vector<BasicBlock*> predecessors;
};

} // namespace nautilus::compiler::ir

namespace std {
template <>
struct hash<nautilus::compiler::ir::BlockIdentifier> {
	std::size_t operator()(nautilus::compiler::ir::BlockIdentifier k) const noexcept {
		return std::hash<uint32_t> {}(k.getId());
	}
};
} // namespace std
