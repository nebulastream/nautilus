#include "nautilus/nir/Builder.hpp"
#include <cassert>
#include <numeric>
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
	if (level() == Level::SSA) {
		m_.blocks.back().count++;
	} else {
		blockIds_.push_back(id);
	}
	if (trackUses_) {
		useCount_.push_back(0);
		forEachRef(inst, m_.extra.data(), level(), [&](Ref r) {
			if (isValueRef(r) && r < useCount_.size()) {
				useCount_[r]++;
			}
		});
	}
	return id;
}

Ref Builder::appendPreSSABlock(std::span<const Inst> insts) {
	assert(fn_ != NONE && !blockOpen_ && level() == Level::PreSSA);
	const auto first = instCount();
	if (static_cast<uint64_t>(first) + insts.size() > static_cast<uint64_t>(MAX_VALUE_ID) + 1) {
		throw std::length_error("function has too many instructions");
	}
	m_.insts.insert(m_.insts.end(), insts.begin(), insts.end());
	BlockRecord b {};
	b.first = static_cast<uint32_t>(m_.extra.size());
	b.count = static_cast<uint32_t>(insts.size());
	m_.blocks.push_back(b);
	m_.extra.resize(m_.extra.size() + insts.size());
	std::iota(m_.extra.end() - static_cast<std::ptrdiff_t>(insts.size()), m_.extra.end(), first);
	fn().blockCount++;
	return first;
}

void Builder::reserve(size_t insts, size_t extraWords) {
	m_.insts.reserve(m_.insts.size() + insts);
	m_.extra.reserve(m_.extra.size() + extraWords);
}

void Builder::endFunction() {
	assert(fn_ != NONE);
	closeBlock();
	fn().instCount = instCount();
	fn_ = NONE;
}

} // namespace nautilus::nir
