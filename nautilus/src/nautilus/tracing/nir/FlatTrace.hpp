#pragma once

#include "nautilus/common/RegionAttributes.hpp"
#include "nautilus/nir/Module.hpp"
#include "nautilus/tracing/Snapshot.hpp"
#include "nautilus/tracing/TypedValueRef.hpp"
#include <cstdint>
#include <vector>

namespace nautilus::tracing::nirtrace {

/// The recording of one traced function (docs/plans/nautilus-0.4/01-tracing-frontend.md §6): NIR instructions at the
/// pre-SSA level in recording order, grouped into blocks that a control-flow merge splits and a region seam collapse
/// joins. The record index is the global id of an operation; a `val<T>` holds it as its value reference, and a tagged
/// operation is found again through the tag map by that id, which no split rewrites.

using FlatRegion = uint32_t;
inline constexpr FlatRegion NO_FLAT_REGION = 0xFFFFFFFFu;

/// An open-addressing map from the snapshot of an operation to the id of its first record.
class TagMap {
public:
	explicit TagMap(size_t expected = 128);
	/// The record id, or `nir::NONE`.
	[[nodiscard]] uint32_t find(const Snapshot& key) const;
	void set(const Snapshot& key, uint32_t op);
	void clear();
	[[nodiscard]] size_t size() const {
		return size_;
	}

private:
	struct Entry {
		Snapshot key; // an unset snapshot (no tag) marks a free slot: a real snapshot always has a tag
		uint32_t op = nir::NONE;
	};
	void grow();
	std::vector<Entry> table_;
	size_t mask_ = 0, size_ = 0;
};

enum RecordFlags : uint8_t {
	/// The first record of a traced operation. The operation may be several records (a cast chain, a copy and its
	/// reconciling assign); the cursor of a replay steps over all of them.
	RecGroup = 1,
	/// A `br` that the cursor passes through: the ones a merge or a region boundary adds.
	RecJump = 2,
};

/// What a replay needs to know about a record without decoding it.
struct RecordMeta {
	/// For the first record of an operation: the value the operation handed to the `val` (its ref and type).
	uint32_t result = 0;
	Type type = Type::v;
	uint8_t flags = 0;
};

struct FlatBlock {
	/// Record ids in execution order; strictly increasing, which is what lets a split find a position by search.
	std::vector<uint32_t> ops;
	std::vector<uint32_t> preds;
	FlatRegion region = NO_FLAT_REGION;
	bool merge = false;
};

struct FlatRegionSpec {
	RegionAttributes attributes;
	FlatRegion parent = NO_FLAT_REGION;
};

class FlatTrace {
public:
	explicit FlatTrace(nir::Module& module);

	// ---- the records ----------------------------------------------------------------------------------------

	std::vector<nir::Inst> recs;
	std::vector<RecordMeta> meta;
	/// The block of each record. Updated for the moved tail of a split.
	std::vector<uint32_t> blockOf;
	std::vector<FlatBlock> blocks;
	TagMap tags;

	/// Appends a record to the current block and returns its id. Record 0 is reserved: a value ref of 0 is "none".
	uint32_t append(const nir::Inst& inst, uint8_t flags = 0);
	/// Records a function parameter: a record of no block, which the function's entry block declares when it is written
	/// out.
	uint32_t addParameter(const nir::Inst& inst, Type type);
	/// The id the next record will get; the first record of an operation is marked by `markOperation`.
	[[nodiscard]] uint32_t nextId() const {
		return static_cast<uint32_t>(recs.size());
	}
	/// Declares the records from `first` on one traced operation that handed `result` to the `val`, and registers
	/// its snapshot. `snapshot` is null for an operation without one (the jumps a merge adds).
	void markOperation(uint32_t first, const Snapshot* snapshot, TypedValueRef result);

	// ---- blocks and the replay cursor -----------------------------------------------------------------------

	uint32_t createBlock();
	void setCurrentBlock(uint32_t index);
	[[nodiscard]] uint32_t currentBlock() const {
		return currentBlock_;
	}
	/// The record the replay is at, passing through jumps.
	[[nodiscard]] const nir::Inst& currentRecord();
	[[nodiscard]] uint32_t currentRecordId();
	/// The result of the operation under the cursor, then steps over it.
	TypedValueRef consumeOperation();

	/// A tagged snapshot that exists means the same operation was reached again: the tail of the other path merges
	/// with the current block and the recording returns false. True if the snapshot is new.
	bool checkTag(const Snapshot& snapshot);

	/// Appends a `br` to `target` (a jump a replay passes through) to the current block.
	uint32_t addJump(uint32_t target, const Snapshot* snapshot);

	/// Splits the block of record `op` there and joins the tail with the current block's path; see ExecutionTrace.
	uint32_t processControlFlowMerge(uint32_t op);

	// ---- regions ----------------------------------------------------------------------------------------------

	FlatRegion addRegion(const RegionAttributes& attributes, uint32_t entryBlock);
	FlatRegion setCurrentRegion(FlatRegion region);
	[[nodiscard]] FlatRegion commonRegionAncestor(FlatRegion first, FlatRegion second) const;
	/// Splices the blocks that only bounded region bodies out of the finished trace and renumbers the rest.
	void collapseRegionSeams();

	std::vector<FlatRegionSpec> regions;
	FlatRegion currentRegion = NO_FLAT_REGION;

	// ---- per function tables --------------------------------------------------------------------------------

	struct AllocaSpec {
		uint32_t size;
		uint32_t align;
	};
	std::vector<AllocaSpec> allocas;
	/// The records of the function's parameters, by position.
	std::vector<uint32_t> parameters;

	nir::Module& module() {
		return module_;
	}

	/// The false target and the probability of a `condbr`; its record holds the index of one of these in `c` until the
	/// function is written out, when the extra list the instruction needs is made.
	struct CondInfo {
		uint32_t probability;
		uint32_t falseBlock;
	};
	std::vector<CondInfo> conds;

	/// Calls `f(uint32_t& block)` for every block id a terminator record holds (`br`: the target; `condbr`: both).
	template <class F>
	void forEachTarget(nir::Inst& inst, F&& f) {
		if (inst.op == static_cast<uint16_t>(nir::Op::Br)) {
			f(inst.a);
		} else if (inst.op == static_cast<uint16_t>(nir::Op::CondBr)) {
			f(inst.b);
			f(conds[inst.c].falseBlock);
		}
	}

private:
	nir::Module& module_;
	uint32_t currentBlock_ = 0;
	uint32_t cursor_ = 0;
	/// A new record of a block that is not the current one never happens, so the position of the cursor in the current
	/// block is all a replay needs.
	void passJumps();
	uint32_t appendTo(uint32_t block, const nir::Inst& inst, uint8_t flags);
	[[nodiscard]] bool isJump(uint32_t id) const {
		return (meta[id].flags & RecJump) != 0;
	}
};

} // namespace nautilus::tracing::nirtrace
