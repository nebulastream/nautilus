#include "nautilus/tracing/nir/FlatTrace.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <new>

namespace nautilus::tracing::nirtrace {

namespace {

constexpr size_t roundUpToPowerOfTwo(size_t n) {
	size_t p = 16;
	while (p < n) {
		p *= 2;
	}
	return p;
}

} // namespace

// ---- TagMap ------------------------------------------------------------------------------------------------------

TagMap::TagMap(size_t expected) {
	// kept at most 5/8 full
	capacity_ = roundUpToPowerOfTwo(expected * 2);
	table_ = static_cast<Entry*>(std::calloc(capacity_, sizeof(Entry)));
	if (table_ == nullptr) {
		throw std::bad_alloc();
	}
	mask_ = capacity_ - 1;
}

TagMap::~TagMap() {
	std::free(table_);
}

void TagMap::set(const Snapshot& key, uint32_t op) {
	if ((size_ + 1) * 8 > capacity_ * 5) { // at most 5/8 full
		grow();
	}
	size_t slot = hashOf(key) & mask_;
	while (table_[slot].epoch == epoch_) {
		if (table_[slot].key == key) {
			table_[slot].op = op;
			return;
		}
		slot = (slot + 1) & mask_;
	}
	table_[slot].key = key;
	table_[slot].op = op;
	table_[slot].epoch = epoch_;
	++size_;
}

void TagMap::erase(const Snapshot& key) {
	size_t hole = hashOf(key) & mask_;
	while (table_[hole].epoch == epoch_) {
		if (table_[hole].key == key) {
			break;
		}
		hole = (hole + 1) & mask_;
	}
	if (table_[hole].epoch != epoch_) {
		return;
	}
	--size_;
	// shift back the entries of the probe sequence that would not be found past the hole
	size_t next = hole;
	for (;;) {
		next = (next + 1) & mask_;
		if (table_[next].epoch != epoch_) {
			break;
		}
		const size_t home = hashOf(table_[next].key) & mask_;
		// the entry may move into the hole if its home is not in the cyclic range (hole, next]
		const bool between = hole <= next ? (hole < home && home <= next) : (hole < home || home <= next);
		if (!between) {
			table_[hole] = table_[next];
			hole = next;
		}
	}
	table_[hole].epoch = 0;
}

void TagMap::eraseFrom(uint32_t firstOp) {
	// the survivors are written back into a table of the next epoch
	std::vector<Entry> survivors;
	for (size_t i = 0; i < capacity_; ++i) {
		if (table_[i].epoch == epoch_ && table_[i].op < firstOp) {
			survivors.push_back(table_[i]);
		}
	}
	size_ = 0;
	if (++epoch_ == 0) {
		std::memset(static_cast<void*>(table_), 0, capacity_ * sizeof(Entry));
		epoch_ = 1;
	}
	for (const auto& entry : survivors) {
		set(entry.key, entry.op);
	}
}

void TagMap::grow() {
	Entry* old = table_;
	const size_t oldCapacity = capacity_;
	// fewer rehashes of the entries a long trace adds: four times up to 64 K slots, then twice
	capacity_ *= capacity_ < 65536 ? 4 : 2;
	table_ = static_cast<Entry*>(std::calloc(capacity_, sizeof(Entry)));
	if (table_ == nullptr) {
		table_ = old;
		capacity_ = oldCapacity;
		throw std::bad_alloc();
	}
	mask_ = capacity_ - 1;
	for (size_t i = 0; i < oldCapacity; ++i) {
		if (old[i].epoch == epoch_) {
			size_t slot = hashOf(old[i].key) & mask_;
			while (table_[slot].epoch == epoch_) {
				slot = (slot + 1) & mask_;
			}
			table_[slot] = old[i];
		}
	}
	std::free(old);
}

void TagMap::clear(size_t keepCapacity) {
	size_ = 0;
	if (capacity_ > keepCapacity) {
		// one very large trace must not leave every later small one a table it never fills
		std::free(table_);
		capacity_ = 64;
		table_ = static_cast<Entry*>(std::calloc(capacity_, sizeof(Entry)));
		if (table_ == nullptr) {
			throw std::bad_alloc();
		}
		mask_ = capacity_ - 1;
		epoch_ = 1;
		return;
	}
	if (++epoch_ == 0) {
		// the epoch wrapped: stale entries could look current again
		std::memset(static_cast<void*>(table_), 0, capacity_ * sizeof(Entry));
		epoch_ = 1;
	}
}

// ---- FlatTrace ---------------------------------------------------------------------------------------------------

FlatTrace::FlatTrace(nir::Module& module) : module_(&module) {
	recs.reserve(256);
	meta.reserve(256);
	blocks.reserve(8);
	// record 0 is "no value"
	recs.push_back(nir::Inst {});
	meta.push_back({});
	createBlock();
}

void FlatTrace::recycleBlocks() {
	for (auto& block : blocks) {
		// a vector that grew very large is not worth keeping
		if (block.ops.capacity() <= 4096 && spareVectors_.size() < 1024) {
			block.ops.clear();
			spareVectors_.push_back(std::move(block.ops));
		}
		if (block.preds.capacity() <= 64 && spareVectors_.size() < 1024) {
			block.preds.clear();
			spareVectors_.push_back(std::move(block.preds));
		}
	}
	blocks.clear();
}

void FlatTrace::reset(nir::Module& module) {
	module_ = &module;
	recycleBlocks();
	// a trace that outgrew the usual size does not keep its tables
	auto shrink = [](auto& v, size_t limit) {
		v.clear();
		if (v.capacity() > limit) {
			v = {};
			v.reserve(256);
		}
	};
	shrink(recs, 1u << 20);
	shrink(meta, 1u << 20);
	regionLog.clear();
	conds.clear();
	allocas.clear();
	parameters.clear();
	returns.clear();
	regions.clear();
	currentRegion = NO_FLAT_REGION;
	currentBlock_ = 0;
	cursor_ = 0;
	openCheckpoints_ = 0;
	predLog_.clear();
	tags.clear();
	recs.push_back(nir::Inst {});
	meta.push_back({});
	createBlock();
}

uint32_t FlatTrace::appendTo(uint32_t block, const nir::Inst& inst, uint8_t flags) {
	const auto id = static_cast<uint32_t>(recs.size());
	recs.push_back(inst);
	meta.push_back(RecordMeta {0, Type::v, flags, block});
	blocks[block].ops.push_back(id);
	return id;
}

uint32_t FlatTrace::append(const nir::Inst& inst, uint8_t flags) {
	const auto id = appendTo(currentBlock_, inst, flags);
	// the cursor of a recording is on the last record, where the branch of a traced condition reads its targets
	cursor_ = static_cast<uint32_t>(blocks[currentBlock_].ops.size() - 1);
	return id;
}

uint32_t FlatTrace::addParameter(const nir::Inst& inst, Type type) {
	const auto id = static_cast<uint32_t>(recs.size());
	recs.push_back(inst);
	meta.push_back(RecordMeta {id, type, 0});
	return id;
}

void FlatTrace::markOperation(uint32_t first, const Snapshot* snapshot, TypedValueRef result) {
	meta[first].flags |= RecGroup;
	meta[first].result = result.ref;
	meta[first].type = result.type;
	// a snapshot without a tag is the one of an operation recorded optimistically: it has no entry
	if (snapshot != nullptr && snapshot->getTag() != nullptr) {
		tags.set(*snapshot, first);
	}
}

uint32_t FlatTrace::createBlock() {
	const auto id = static_cast<uint32_t>(blocks.size());
	blocks.emplace_back();
	if (!spareVectors_.empty()) {
		blocks.back().ops = std::move(spareVectors_.back());
		spareVectors_.pop_back();
	}
	if (!spareVectors_.empty()) {
		blocks.back().preds = std::move(spareVectors_.back());
		spareVectors_.pop_back();
	}
	// a block belongs to the region that was open when it was made; a region's entry block is re-stamped by addRegion
	blocks.back().region = currentRegion;
	return id;
}

void FlatTrace::setCurrentBlock(uint32_t index) {
	if (index >= blocks.size()) {
		throw RuntimeException("Cannot set current block to out of bounds index: " + std::to_string(index));
	}
	cursor_ = 0;
	currentBlock_ = index;
}

void FlatTrace::passJumps() {
	for (;;) {
		const auto& ops = blocks[currentBlock_].ops;
		if (cursor_ >= ops.size()) {
			throw RuntimeException("Current operation index out of bounds: " + std::to_string(cursor_));
		}
		const auto id = ops[cursor_];
		if (!isJump(id)) {
			return;
		}
		setCurrentBlock(recs[id].a);
	}
}

const nir::Inst& FlatTrace::currentRecord() {
	passJumps();
	return recs[blocks[currentBlock_].ops[cursor_]];
}

uint32_t FlatTrace::currentRecordId() {
	passJumps();
	return blocks[currentBlock_].ops[cursor_];
}

TypedValueRef FlatTrace::consumeOperation() {
	// the replay's hot path: look through the jumps a merge or a region boundary added, take the operation's result,
	// and step over its records
	const FlatBlock* block = &blocks[currentBlock_];
	uint32_t first;
	for (;;) {
		if (cursor_ >= block->ops.size()) {
			throw RuntimeException("Current operation index out of bounds: " + std::to_string(cursor_));
		}
		first = block->ops[cursor_];
		if ((meta[first].flags & RecJump) == 0) {
			break;
		}
		setCurrentBlock(recs[first].a);
		block = &blocks[currentBlock_];
	}
	const auto& ops = block->ops;
	const TypedValueRef result(meta[first].result, meta[first].type);
	// the operation is the record and those after it that are not the start of another
	++cursor_;
	while (cursor_ < ops.size() && (meta[ops[cursor_]].flags & (RecGroup | RecJump)) == 0) {
		++cursor_;
	}
	if (cursor_ >= ops.size()) {
		throw RuntimeException("Operation index out of bounds: " + std::to_string(cursor_));
	}
	if (isJump(ops[cursor_])) {
		setCurrentBlock(recs[ops[cursor_]].a);
	}
	return result;
}

bool FlatTrace::checkTag(const Snapshot& snapshot) {
	// the operation exists: it is reached again, which is a control-flow merge
	const auto op = tags.find(snapshot);
	if (op != nir::NONE) {
		lastMerge_ = snapshot; // for the diagnostic of a constant loop, which this merge may turn out to be
		processControlFlowMerge(op);
		return false;
	}
	return true;
}

uint32_t FlatTrace::addJump(uint32_t target, const Snapshot* snapshot) {
	nir::Inst inst {};
	inst.op = static_cast<uint16_t>(nir::Op::Br);
	inst.a = target;
	const auto id = append(inst, RecJump);
	if (snapshot != nullptr) {
		markOperation(id, snapshot, TypedValueRef(0, Type::v));
	}
	return id;
}

uint32_t FlatTrace::processControlFlowMerge(uint32_t op) {
	const uint32_t referenceIndex = meta[op].block;
	if (referenceIndex == currentBlock_) {
		throw ConstantLoopError();
	}
	const uint32_t merged = createBlock();
	blocks[merged].merge = true;
	// the tail of the reference block from the operation on moves to the merge block; the ids of a block increase
	auto& reference = blocks[referenceIndex];
	const auto position =
	    static_cast<size_t>(std::lower_bound(reference.ops.begin(), reference.ops.end(), op) - reference.ops.begin());
	auto& mergeBlock = blocks[merged];
	mergeBlock.ops.assign(reference.ops.begin() + static_cast<std::ptrdiff_t>(position), reference.ops.end());
	for (auto id : mergeBlock.ops) {
		meta[id].block = merged;
	}
	reference.ops.resize(position);

	nir::Inst jump {};
	jump.op = static_cast<uint16_t>(nir::Op::Br);
	jump.a = merged;
	appendTo(referenceIndex, jump, RecJump);
	appendTo(currentBlock_, jump, RecJump);

	mergeBlock.preds = {referenceIndex, currentBlock_};
	const auto previousCurrent = currentBlock_;
	setCurrentBlock(merged);

	// the successors of the moved tail now have the merge block as their predecessor
	nir::Inst last = recs[blocks[merged].ops.back()];
	forEachTarget(last, [&](uint32_t& target) {
		auto& preds = blocks[target].preds;
		std::replace(preds.begin(), preds.end(), referenceIndex, merged);
		std::replace(preds.begin(), preds.end(), previousCurrent, merged);
	});
	return merged;
}

FlatTrace::Checkpoint FlatTrace::checkpoint(uint32_t entryBlock) {
	Checkpoint cp;
	cp.records = static_cast<uint32_t>(recs.size());
	cp.blocks = static_cast<uint32_t>(blocks.size());
	cp.conds = static_cast<uint32_t>(conds.size());
	cp.allocas = static_cast<uint32_t>(allocas.size());
	cp.returns = static_cast<uint32_t>(returns.size());
	cp.regions = static_cast<uint32_t>(regions.size());
	cp.entryBlock = entryBlock;
	cp.currentRegion = currentRegion;
	cp.predLog = predLog_.size();
	cp.regionLogSize = regionLog.size();
	++openCheckpoints_;
	return cp;
}

void FlatTrace::addPred(uint32_t block, uint32_t pred) {
	blocks[block].preds.push_back(pred);
	if (openCheckpoints_ > 0) {
		predLog_.push_back(block);
	}
}

void FlatTrace::release(const Checkpoint&) {
	--openCheckpoints_;
	if (openCheckpoints_ == 0) {
		predLog_.clear();
	}
}

void FlatTrace::rollback(const Checkpoint& cp) {
	tags.eraseFrom(cp.records);
	// the predecessors added to blocks that existed before the checkpoint (the others are dropped below)
	while (predLog_.size() > cp.predLog) {
		const auto block = predLog_.back();
		predLog_.pop_back();
		if (block < cp.blocks) {
			blocks[block].preds.pop_back();
		}
	}
	// the entry block of the scope keeps the records it had; every other older block got none
	auto& entry = blocks[cp.entryBlock].ops;
	entry.erase(std::lower_bound(entry.begin(), entry.end(), cp.records), entry.end());
	while (blocks.size() > cp.blocks) {
		if (blocks.back().ops.capacity() <= 4096 && spareVectors_.size() < 1024) {
			blocks.back().ops.clear();
			spareVectors_.push_back(std::move(blocks.back().ops));
		}
		blocks.pop_back();
	}
	recs.resize(cp.records);
	meta.resize(cp.records);
	conds.resize(cp.conds);
	allocas.resize(cp.allocas);
	returns.resize(cp.returns);
	regions.resize(cp.regions);
	regionLog.resize(cp.regionLogSize);
	currentRegion = cp.currentRegion;
	cursor_ = 0;
	release(cp);
}

FlatRegion FlatTrace::addRegion(const RegionAttributes& attributes, uint32_t entryBlock) {
	const auto index = static_cast<FlatRegion>(regions.size());
	regions.push_back(FlatRegionSpec {attributes, currentRegion});
	blocks[entryBlock].region = index;
	return index;
}

FlatRegion FlatTrace::setCurrentRegion(FlatRegion region) {
	const auto previous = currentRegion;
	currentRegion = region;
	regionLog.emplace_back(static_cast<uint32_t>(recs.size()), region);
	return previous;
}

FlatRegion FlatTrace::commonRegionAncestor(FlatRegion first, FlatRegion second) const {
	for (auto outer = first; outer != NO_FLAT_REGION && outer < regions.size(); outer = regions[outer].parent) {
		for (auto inner = second; inner != NO_FLAT_REGION && inner < regions.size(); inner = regions[inner].parent) {
			if (inner == outer) {
				return outer;
			}
		}
	}
	return NO_FLAT_REGION;
}

void FlatTrace::collapseRegionSeams() {
	if (regions.empty()) {
		return;
	}
	const auto blockCount = static_cast<uint32_t>(blocks.size());
	std::vector<uint8_t> dead(blockCount, 0);
	bool anyMerged = false;
	// One pass in index order is enough: along a chain of seams every link is absorbed by whichever block its
	// predecessor has become.
	for (uint32_t index = 1; index < blockCount; ++index) {
		auto& block = blocks[index];
		if (block.preds.size() != 1) {
			continue;
		}
		const auto predecessorIndex = block.preds.front();
		if (predecessorIndex == index || predecessorIndex >= blockCount || dead[predecessorIndex]) {
			continue;
		}
		auto& predecessor = blocks[predecessorIndex];
		if (predecessor.ops.empty()) {
			continue;
		}
		const auto jumpId = predecessor.ops.back();
		const auto& jump = recs[jumpId];
		if (jump.op != static_cast<uint16_t>(nir::Op::Br) || jump.a != index) {
			continue;
		}
		predecessor.ops.pop_back();
		for (auto id : block.ops) {
			meta[id].block = predecessorIndex;
		}
		predecessor.ops.insert(predecessor.ops.end(), block.ops.begin(), block.ops.end());
		if (!block.ops.empty()) {
			forEachTarget(recs[block.ops.back()], [&](uint32_t& successor) {
				auto& preds = blocks[successor].preds;
				std::replace(preds.begin(), preds.end(), index, predecessorIndex);
			});
		}
		// the block now holds code of two regions, so it names the innermost one that contains both
		predecessor.region = commonRegionAncestor(predecessor.region, block.region);
		block.ops.clear();
		block.preds.clear();
		dead[index] = 1;
		anyMerged = true;
	}
	if (!anyMerged) {
		return;
	}
	// renumber the survivors densely, and everything that names a block with them
	std::vector<uint32_t> renumbering(blockCount, 0);
	uint32_t next = 0;
	for (uint32_t index = 0; index < blockCount; ++index) {
		if (!dead[index]) {
			renumbering[index] = next++;
		}
	}
	std::vector<FlatBlock> survivors;
	survivors.reserve(next);
	for (uint32_t index = 0; index < blockCount; ++index) {
		if (dead[index]) {
			continue;
		}
		auto& block = blocks[index];
		for (auto& pred : block.preds) {
			pred = renumbering[pred];
		}
		if (!block.ops.empty()) {
			forEachTarget(recs[block.ops.back()], [&](uint32_t& target) { target = renumbering[target]; });
		}
		for (auto id : block.ops) {
			meta[id].block = renumbering[index];
		}
		survivors.push_back(std::move(block));
	}
	blocks = std::move(survivors);
	// the tag map names records, which stay valid, but no scope replays this trace any more
	tags.clear();
	currentBlock_ = 0;
	cursor_ = 0;
}

} // namespace nautilus::tracing::nirtrace
