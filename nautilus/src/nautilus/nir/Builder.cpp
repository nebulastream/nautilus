#include "nautilus/nir/Builder.hpp"
#include <cassert>
#include <stdexcept>

namespace nautilus::nir {

uint32_t Builder::beginFunction(uint32_t nameId, uint32_t signature, Visibility visibility, Level level,
                                uint32_t attrs) {
	assert(fn_ == NONE && "a function is already open");
	FunctionRecord r {};
	r.nameId = nameId;
	r.signature = signature;
	r.firstInst = static_cast<uint32_t>(m_.insts.size());
	r.firstBlock = static_cast<uint32_t>(m_.blocks.size());
	r.firstAlloca = static_cast<uint32_t>(m_.allocas.size());
	r.attrs = attrs;
	r.level = static_cast<uint8_t>(level);
	r.visibility = static_cast<uint8_t>(visibility);
	m_.functions.push_back(r);
	fn_ = static_cast<uint32_t>(m_.functions.size() - 1);
	useCount_.clear();
	return fn_;
}

uint32_t Builder::addAlloca(uint32_t size, uint32_t align) {
	assert(fn_ != NONE);
	m_.allocas.push_back(AllocaRecord {size, align});
	return fn().allocaCount++;
}

void Builder::closeBlock() {
	if (!blockOpen_) {
		return;
	}
	if (level() == Level::PreSSA) {
		auto& b = m_.blocks.back();
		b.first = m_.addExtra(blockIds_);
		b.count = static_cast<uint32_t>(blockIds_.size());
		blockIds_.clear();
	}
	blockOpen_ = false;
}

uint32_t Builder::beginBlock(uint16_t argCount, uint16_t flags) {
	assert(fn_ != NONE);
	closeBlock();
	BlockRecord b {};
	b.first = level() == Level::SSA ? instCount() : 0;
	b.argCount = level() == Level::SSA ? argCount : 0;
	b.flags = flags;
	m_.blocks.push_back(b);
	blockOpen_ = true;
	return fn().blockCount++;
}

Ref Builder::emit(Op op, TypeId type, uint8_t flags, uint32_t a, uint32_t b, uint32_t c) {
	return emit(Inst {static_cast<uint16_t>(op), static_cast<uint8_t>(type), flags, a, b, c});
}

Ref Builder::emit(const Inst& inst) {
	assert(blockOpen_ && "emit needs an open block");
	auto id = instCount();
	if (id > MAX_VALUE_ID) {
		throw std::length_error("function has too many instructions");
	}
	m_.insts.push_back(inst);
	useCount_.push_back(0);
	if (level() == Level::SSA) {
		m_.blocks.back().count++;
	} else {
		blockIds_.push_back(id);
	}
	forEachRef(inst, m_.extra.data(), level(), [&](Ref r) {
		if (isValueRef(r) && r < useCount_.size()) {
			useCount_[r]++;
		}
	});
	return id;
}

void Builder::endFunction() {
	assert(fn_ != NONE);
	closeBlock();
	fn().instCount = instCount();
	fn_ = NONE;
}

} // namespace nautilus::nir
