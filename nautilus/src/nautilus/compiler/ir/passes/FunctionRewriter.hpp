
#pragma once

#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory_resource>
#include <span>
#include <vector>

namespace nautilus::compiler::ir {

class IRGraph;

class FunctionOperation;

/**
 * @brief A per-function mutation session: use tracking, identifier minting,
 * safe erase, insertion, atomic block-argument mutation, and CFG rewiring.
 *
 * Every planned IR pass performs the same five mutations -- replace uses,
 * erase, create/insert, change block-argument arity, rewire CFG edges --
 * and today each was either missing, unsafe, or reinvented per pass (see
 * `docs/design/ir-abstraction-milestone.md` for the motivating history:
 * issues #327 and #321). `FunctionRewriter` is the single place those five
 * primitives live from now on.
 *
 * Construction is O(function): it builds an external use-edge table
 * (`Operation` stays a packed 32-byte arena object -- no intrusive
 * use-lists) and computes the next fresh identifier once. Every primitive
 * after that is O(affected uses/args/edges), never O(function) again --
 * this is what turns a per-pass whole-function rescan into an incremental
 * update.
 *
 * The table is one open-addressing array over entries bump-allocated from a
 * session-owned monotonic buffer and released in bulk with the session: one
 * hash per value, the defining block and the first two uses in the entry
 * itself, no per-value allocation. Entries are never given back individually
 * -- an erased value's entry is cleared, a use list that grows leaves its old
 * storage behind -- which is bounded by a constant factor of the final table.
 * `IRPassManager` shares one session per function between the passes of a run
 * (see `RewriterCache`), so a pass usually finds the table already built.
 *
 * Session discipline: while a rewriter is live, every mutation of its
 * function must go through it. Bypassing it (raw `BasicBlock::removeOperation`,
 * `Operation::setInput`, ...) leaves the use table stale. This is the same
 * contract `BasicBlock::getPredecessors()` already has for CFG edits.
 *
 * `erase`, `removeBlockArgument`, and `eraseBlock` throw `RuntimeException`
 * on a contract violation (a live op, an entry block, ...) rather than
 * asserting, so violations are testable with `REQUIRE_THROWS_AS` and stay
 * active in Release test runs.
 */
class FunctionRewriter {
public:
	/// A single use edge: `user` reads the tracked value at operand index
	/// `operandIndex`. `user` may be a plain operation (e.g. an `AddOperation`
	/// reading one of its operands) or a `BasicBlockInvocation` (an
	/// argument edge on a block-CFG edge) -- both are representable
	/// uniformly because invocation arguments live in the same
	/// `Operation::inputs` span every other operation kind uses.
	///
	/// A call's destructor addresses (DestructorOperands.hpp) are tracked as
	/// uses too, so `replaceAllUses` rewrites them and a value whose only
	/// consumer is a landing pad is not seen as dead. Their `operandIndex` is
	/// `destructorOperandBase + i` for destructor `i`, which keeps them
	/// distinct from the `Operation::inputs` slots.
	struct Use {
		Operation* user;
		uint32_t operandIndex;
	};

	static constexpr uint32_t destructorOperandBase = 0x80000000u;

	/// One block-argument slot, for @ref removeBlockArguments.
	struct BlockArgumentSlot {
		BasicBlock* block;
		size_t index;
	};

	/// @param ir  Optional. When supplied, dead-code elimination resolves a
	///            call's callee through the module function table and can drop
	///            a call the callee proves has no observable effect. Without
	///            it the rewriter falls back to the per-opcode answer, under
	///            which no call is ever removable.
	FunctionRewriter(FunctionOperation& fn, common::Arena& arena, const IRGraph* ir = nullptr);

	// ── Use queries (uniform over operand edges AND invocation arguments) ──

	/// Returns every use of @p op: operand edges and invocation-argument
	/// edges alike. O(1) plus the size of the result.
	[[nodiscard]] std::span<const Use> usesOf(const Operation* op) const;

	/// O(1) plus the size of the result (a lookup, not a scan).
	[[nodiscard]] size_t useCount(const Operation* op) const;

	[[nodiscard]] bool hasUses(const Operation* op) const;

	/// Returns the block that defines @p op: the block whose operation list
	/// contains it, or the block that owns it as a block argument. Returns
	/// `nullptr` for an operation this session does not track (e.g. never
	/// part of the function, or already erased).
	[[nodiscard]] BasicBlock* definingBlock(const Operation* op) const;

	// ── Value rewiring ──

	/// Rewires every use of @p from (operand slots and invocation arguments)
	/// to @p to. O(uses(from)). After the call `useCount(from) == 0`; @p to
	/// gains exactly the moved uses and no other operand's use count changes.
	void replaceAllUses(Operation* from, Operation* to);

	// ── Identifiers ──

	/// Strictly greater than every identifier that existed in the function
	/// at session construction and every identifier this session has
	/// already minted. Never reused, including across erases.
	[[nodiscard]] OperationIdentifier freshId();

	// ── Creation & insertion ──

	/// Arena-allocates a `T` (injecting the arena as the first constructor
	/// argument, matching `BasicBlock::addOperation`'s convention -- callers
	/// pass the identifier, typically `freshId()`, as part of @p args like
	/// any other constructor parameter), registers its operand uses, and
	/// inserts it immediately before @p block's terminator.
	template <typename T, typename... Args>
	T* createBeforeTerminator(BasicBlock* block, Args&&... args) {
		T* op = arena_.create<T>(arena_, std::forward<Args>(args)...);
		Operation* terminator = block->getTerminatorOp();
		block->addOperationBefore(terminator, op);
		defBlockOf(op) = block;
		registerUses(op);
		return op;
	}

	/// As above, but inserts immediately before @p anchor (which must
	/// already be tracked by this session, i.e. a live operation in the
	/// function).
	template <typename T, typename... Args>
	T* createBefore(Operation* anchor, Args&&... args) {
		T* op = arena_.create<T>(arena_, std::forward<Args>(args)...);
		BasicBlock* block = definingBlock(anchor);
		block->addOperationBefore(anchor, op);
		defBlockOf(op) = block;
		registerUses(op);
		return op;
	}

	// ── Erasure ──

	/// Precondition: `useCount(op) == 0` and `op` is not a terminator.
	/// Detaches @p op from its block and unregisters its operand uses.
	/// Throws `RuntimeException` (mutating nothing) on a live op or a
	/// terminator.
	void erase(Operation* op);

	/// `erase(op)` if `op` is pure (`isPureOp` is the only purity oracle)
	/// and use-free, then cascades into operands that became use-free and
	/// pure as a result. Idempotent: calling it again on an already-erased
	/// operation is a no-op. Returns the number of operations erased.
	size_t eraseIfDead(Operation* op) {
		return eraseIfDead(op, ir_);
	}

	/// As above, resolving calls through @p module instead of the session's
	/// own (see the constructor): a session shared between passes is built
	/// without one, and dead-code elimination supplies it per call.
	size_t eraseIfDead(Operation* op, const IRGraph* module);

	// ── Block arguments (both sides of the arity invariant, atomically) ──

	/// Precondition: the argument has no uses. Removes argument slot @p
	/// index of @p block *and* the @p index-th argument of every invocation
	/// targeting @p block (including multiple invocations from the same
	/// predecessor). Throws `RuntimeException` (mutating nothing) if the
	/// argument has uses. A single-slot @ref removeBlockArguments.
	void removeBlockArgument(BasicBlock* block, size_t index);

	/// Removes every listed slot at once: any number of blocks, any number
	/// of slots per block, in any order (a slot listed twice counts once).
	/// Same precondition and exception as @ref removeBlockArgument, checked
	/// for every slot before anything is mutated.
	///
	/// Removing a slot shifts the operand index of every later argument of
	/// every invocation targeting that block, and the use table records
	/// those indices. Rather than dropping and re-deriving each invocation's
	/// use edges one slot at a time -- which scans the full use list of
	/// every value the invocation passes, once per removed slot -- this
	/// walks the use list of each value any rewritten invocation passes
	/// exactly once and patches or drops its entries in place. For a value
	/// passed through every block of the function that is the difference
	/// between O(blocks) and O(blocks^2) per pruning round.
	void removeBlockArguments(std::span<const BlockArgumentSlot> slots);

	/// Appends an argument of the given @p stamp to @p block and exactly one
	/// value per invocation targeting @p block: @p valueForEdge is invoked
	/// once per such invocation (not once per predecessor block -- a
	/// predecessor with two invocations targeting @p block yields two
	/// calls) to supply that edge's value. Returns the freshly minted
	/// argument.
	BasicBlockArgument* addBlockArgument(BasicBlock* block, Type stamp,
	                                     const std::function<Operation*(BasicBlockInvocation&)>& valueForEdge);

	/// Replaces every argument of @p inv with @p newArgs, in order. Unlike
	/// @ref removeBlockArgument / @ref addBlockArgument this does not touch
	/// @p inv's target block's argument list -- it exists for passes that
	/// retarget an invocation to a block with a different arity/argument
	/// shape (e.g. `EmptyBlockEliminationPass` substituting a skipped
	/// block's outgoing arguments for its predecessor's incoming ones) and
	/// therefore need to replace the whole argument list in one step rather
	/// than incrementally. Old argument use edges are dropped and new ones
	/// registered; @p inv's target is unchanged (pair with
	/// `setInvocationTarget` when the target changes too).
	void setInvocationArguments(BasicBlockInvocation& inv, std::span<Operation* const> newArgs);

	// ── CFG ──

	/// Retargets @p inv to @p newTarget. Predecessor lists of the old and
	/// new target stay consistent (mirrors `BasicBlock::replaceSuccessor`,
	/// generalized to a single invocation rather than every edge between
	/// two blocks).
	void setInvocationTarget(BasicBlockInvocation& inv, BasicBlock* newTarget);

	/// Replaces @p block's terminator with @p newTerminator. Predecessor
	/// lists of the old and new terminator's successors stay consistent
	/// (delegates to `BasicBlock::replaceTerminatorOperation`); the use
	/// table is updated for both terminators' operands and invocation
	/// arguments.
	void replaceTerminator(BasicBlock* block, Operation* newTerminator);

	/// Precondition: @p block has no predecessors and is not the function's
	/// entry block. Detaches @p block (`FunctionOperation::detachBasicBlock`)
	/// and unregisters every use originating in it, including removing
	/// @p block's own predecessor-list entry from every block it still
	/// targets. Throws `RuntimeException` (mutating nothing) otherwise.
	void eraseBlock(BasicBlock* block);

	/// Merges @p succ into @p pred: every use of each of @p succ's block
	/// arguments is rewired to the corresponding invocation argument of
	/// @p pred's branch (via @ref replaceAllUses), @p succ's operations are
	/// spliced onto the end of @p pred in place of the branch, @p succ's
	/// terminator becomes @p pred's terminator (its successors' predecessor
	/// lists are updated, once per invocation edge), and @p succ is detached.
	/// Operation identity is preserved: nothing is cloned, no identifier is
	/// reused or renumbered.
	///
	/// Preconditions (throws `RuntimeException`, mutating nothing):
	/// @p pred's terminator is an unconditional branch targeting @p succ,
	/// @p succ's only predecessor edge is that branch, `succ != pred`, and
	/// @p succ is not the function's entry block.
	void mergeIntoPredecessor(BasicBlock* pred, BasicBlock* succ);

private:
	/// Registers `Use{user, i}` for every non-null operand of @p user,
	/// including its destructor addresses.
	void registerUses(Operation* user);
	/// Removes every `Use` entry recorded for @p user (from every operand's
	/// use list). Used before erasing @p user and before re-deriving its
	/// use edges after an arity-changing mutation (e.g. removing an
	/// invocation argument shifts every later operand index, so this
	/// session re-derives the invocation's use edges from scratch rather
	/// than patching indices in place).
	void unregisterUser(Operation* user);

	/// The uses of one value: contiguous (so `usesOf` can hand out a span),
	/// the first `inlineCapacity` in place -- most values have one or two --
	/// and the rest in storage from the session's buffer. Growth abandons the
	/// old storage, like any allocation from that buffer.
	class UseList {
	public:
		static constexpr uint32_t inlineCapacity = 2;

		[[nodiscard]] const Use* data() const {
			return heap_ != nullptr ? heap_ : inline_;
		}
		Use* data() {
			return heap_ != nullptr ? heap_ : inline_;
		}
		[[nodiscard]] uint32_t size() const {
			return size_;
		}
		[[nodiscard]] bool empty() const {
			return size_ == 0;
		}
		[[nodiscard]] std::span<const Use> span() const {
			return {data(), size_};
		}
		Use& operator[](size_t i) {
			return data()[i];
		}
		void push_back(const Use& use, std::pmr::memory_resource& memory) {
			if (size_ == capacity_) {
				grow(memory);
			}
			data()[size_++] = use;
		}
		/// Shrinks to the first @p n uses.
		void truncate(uint32_t n) {
			size_ = n;
		}
		void clear() {
			size_ = 0;
		}

	private:
		void grow(std::pmr::memory_resource& memory);

		Use inline_[inlineCapacity];
		Use* heap_ = nullptr;
		uint32_t size_ = 0;
		uint32_t capacity_ = inlineCapacity;
	};

	/// Everything the session knows about one value. `block` is null for a
	/// value that is not (or no longer) defined in the function -- an operand
	/// that is only used, or an erased definition.
	struct Entry {
		const Operation* key;
		BasicBlock* block = nullptr;
		UseList uses;
	};

	/// Open-addressing table from a value to its `Entry`. Entries live in the
	/// session's buffer and never move, so a span from `usesOf` stays valid
	/// while other values are inserted; the probe array holds pointers to
	/// them and is the only thing a rehash copies. Entries are never removed
	/// -- erasing a value clears its entry -- which keeps probing simple and
	/// is bounded by the values the session ever saw.
	class ValueTable {
	public:
		ValueTable(std::pmr::memory_resource& memory, size_t expectedValues);
		[[nodiscard]] Entry* find(const Operation* value) const;
		Entry& operator[](const Operation* value);

	private:
		[[nodiscard]] size_t slotOf(const Operation* value) const;
		void rehash(size_t capacity);

		std::pmr::memory_resource& memory_;
		Entry** slots_ = nullptr;
		size_t capacity_ = 0;
		size_t count_ = 0;
	};

	[[nodiscard]] BasicBlock*& defBlockOf(const Operation* value) {
		return table_[value].block;
	}
	/// Clears everything known about @p value (its definition and its uses).
	void forget(const Operation* value);

	FunctionOperation& fn_;
	common::Arena& arena_;
	const IRGraph* ir_ = nullptr;
	/// Backs `table_`; declared before it so it outlives it.
	std::pmr::monotonic_buffer_resource tableMemory_;
	ValueTable table_;
	uint32_t nextId_ = 0;
};

} // namespace nautilus::compiler::ir
