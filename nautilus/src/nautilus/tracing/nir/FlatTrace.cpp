#include "nautilus/tracing/nir/FlatTrace.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>

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
	// kept at most half full
	table_.resize(roundUpToPowerOfTwo(expected * 2));
	mask_ = table_.size() - 1;
}

uint32_t TagMap::find(const Snapshot& key) const {
	size_t slot = std::hash<Snapshot> {}(key) &mask_;
	while (table_[slot].key.getTag() != nullptr) {
		if (table_[slot].key == key) {
			return table_[slot].op;
		}
		slot = (slot + 1) & mask_;
	}
	return nir::NONE;
}

void TagMap::set(const Snapshot& key, uint32_t op) {
	if ((size_ + 1) * 2 > table_.size()) {
		grow();
	}
	size_t slot = std::hash<Snapshot> {}(key) &mask_;
	while (table_[slot].key.getTag() != nullptr) {
		if (table_[slot].key == key) {
			table_[slot].op = op;
			return;
		}
		slot = (slot + 1) & mask_;
	}
	table_[slot] = {key, op};
	++size_;
}

void TagMap::grow() {
	std::vector<Entry> old = std::move(table_);
	table_.assign(old.size() * 2, Entry {});
	mask_ = table_.size() - 1;
	size_ = 0;
	for (const auto& entry : old) {
		if (entry.key.getTag() != nullptr) {
			set(entry.key, entry.op);
		}
	}
}

void TagMap::clear() {
	// keeps the capacity, like the tables the other tracer pools
	std::fill(table_.begin(), table_.end(), Entry {});
	size_ = 0;
}

// ---- FlatTrace ---------------------------------------------------------------------------------------------------

FlatTrace::FlatTrace(nir::Module& module) : module_(module) {
	recs.reserve(256);
	meta.reserve(256);
	blockOf.reserve(256);
	blocks.reserve(8);
	// record 0 is "no value"
	recs.push_back(nir::Inst {});
	meta.push_back({});
	blockOf.push_back(0);
	createBlock();
}

uint32_t FlatTrace::appendTo(uint32_t block, const nir::Inst& inst, uint8_t flags) {
	const auto id = static_cast<uint32_t>(recs.size());
	recs.push_back(inst);
	meta.push_back(RecordMeta {0, Type::v, flags});
	blockOf.push_back(block);
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
	blockOf.push_back(0);
	return id;
}

void FlatTrace::markOperation(uint32_t first, const Snapshot* snapshot, TypedValueRef result) {
	meta[first].flags |= RecGroup;
	meta[first].result = result.ref;
	meta[first].type = result.type;
	if (snapshot != nullptr) {
		tags.set(*snapshot, first);
	}
}

uint32_t FlatTrace::createBlock() {
	const auto id = static_cast<uint32_t>(blocks.size());
	blocks.emplace_back();
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
	passJumps();
	const auto& ops = blocks[currentBlock_].ops;
	const auto first = ops[cursor_];
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
	const uint32_t referenceIndex = blockOf[op];
	if (referenceIndex == currentBlock_) {
		throw RuntimeException("Invalid trace. This is maybe caused by a constant loop.");
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
		blockOf[id] = merged;
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

FlatRegion FlatTrace::addRegion(const RegionAttributes& attributes, uint32_t entryBlock) {
	const auto index = static_cast<FlatRegion>(regions.size());
	regions.push_back(FlatRegionSpec {attributes, currentRegion});
	blocks[entryBlock].region = index;
	return index;
}

FlatRegion FlatTrace::setCurrentRegion(FlatRegion region) {
	const auto previous = currentRegion;
	currentRegion = region;
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
			blockOf[id] = predecessorIndex;
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
			blockOf[id] = renumbering[index];
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
