#include "nautilus/nir/ConstPool.hpp"
#include <cstring>
#include <stdexcept>

namespace nautilus::nir {

namespace {
uint64_t mix(uint64_t h, uint64_t v) {
	h ^= v + 0x9e3779b97f4a7c15ull + (h << 6) + (h >> 2);
	return h;
}
} // namespace

void ConstPool::insertIndex(uint64_t hash, uint32_t index) {
	if ((records_.size() + 1) * 2 > slots_.size()) {
		growIndex();
	}
	const size_t mask = slots_.size() - 1;
	size_t slot = (hash ^ (hash >> 32)) & mask;
	while (slots_[slot] != EMPTY) {
		slot = (slot + 1) & mask;
	}
	slots_[slot] = index;
}

void ConstPool::growIndex() {
	const size_t capacity = slots_.empty() ? 32 : slots_.size() * 2;
	slots_.assign(capacity, EMPTY);
	const size_t mask = capacity - 1;
	for (uint32_t i = 0; i < hashes_.size(); ++i) {
		size_t slot = (hashes_[i] ^ (hashes_[i] >> 32)) & mask;
		while (slots_[slot] != EMPTY) {
			slot = (slot + 1) & mask;
		}
		slots_[slot] = i;
	}
}

Ref ConstPool::intern(TypeId type, Provenance provenance, uint64_t bits) {
	uint64_t h = mix(mix(static_cast<uint64_t>(type), static_cast<uint64_t>(provenance)), bits);
	if (!slots_.empty()) {
		const size_t mask = slots_.size() - 1;
		for (size_t slot = (h ^ (h >> 32)) & mask; slots_[slot] != EMPTY; slot = (slot + 1) & mask) {
			const auto candidate = slots_[slot];
			auto& r = records_[candidate];
			if (hashes_[candidate] == h && wideSizes_[candidate] == 0 && r.type == static_cast<uint8_t>(type) &&
			    r.provenance == static_cast<uint8_t>(provenance) && r.bits == bits) {
				return makeConstRef(candidate);
			}
		}
	}
	if (records_.size() >= MAX_CONSTANTS) {
		throw std::length_error("constant pool is full");
	}
	auto index = static_cast<uint32_t>(records_.size());
	insertIndex(h, index);
	records_.push_back(ConstRecord {static_cast<uint8_t>(type), static_cast<uint8_t>(provenance), 0, 0, bits});
	hashes_.push_back(h);
	wideSizes_.push_back(0);
	return makeConstRef(index);
}

Ref ConstPool::internBytes(TypeId type, Provenance provenance, std::span<const std::byte> bytes, uint32_t align) {
	uint64_t h = mix(static_cast<uint64_t>(type), static_cast<uint64_t>(provenance));
	for (auto b : bytes) {
		h = mix(h, static_cast<uint8_t>(b));
	}
	if (!slots_.empty()) {
		const size_t mask = slots_.size() - 1;
		for (size_t slot = (h ^ (h >> 32)) & mask; slots_[slot] != EMPTY; slot = (slot + 1) & mask) {
			const auto candidate = slots_[slot];
			auto& r = records_[candidate];
			if (hashes_[candidate] == h && wideSizes_[candidate] == bytes.size() &&
			    r.type == static_cast<uint8_t>(type) && r.provenance == static_cast<uint8_t>(provenance) &&
			    std::memcmp(bytes_.data() + r.byteOffset, bytes.data(), bytes.size()) == 0) {
				return makeConstRef(candidate);
			}
		}
	}
	if (records_.size() >= MAX_CONSTANTS) {
		throw std::length_error("constant pool is full");
	}
	auto index = static_cast<uint32_t>(records_.size());
	if (align > 1) {
		bytes_.resize((bytes_.size() + align - 1) / align * align, std::byte {0});
	}
	auto offset = static_cast<uint32_t>(bytes_.size());
	bytes_.insert(bytes_.end(), bytes.begin(), bytes.end());
	insertIndex(h, index);
	records_.push_back(ConstRecord {static_cast<uint8_t>(type), static_cast<uint8_t>(provenance), 0, offset, 0});
	hashes_.push_back(h);
	wideSizes_.push_back(static_cast<uint32_t>(bytes.size()));
	return makeConstRef(index);
}

void ConstPool::reset(std::vector<ConstRecord> records, std::vector<std::byte> bytes) {
	records_ = std::move(records);
	bytes_ = std::move(bytes);
	wideSizes_.assign(records_.size(), 0);
	hashes_.clear();
	hashes_.reserve(records_.size());
	for (auto& r : records_) {
		hashes_.push_back(mix(mix(static_cast<uint64_t>(r.type), static_cast<uint64_t>(r.provenance)), r.bits));
	}
	slots_.clear();
	while (slots_.size() < records_.size() * 2) {
		slots_.assign(slots_.empty() ? 32 : slots_.size() * 2, EMPTY);
	}
	const size_t mask = slots_.empty() ? 0 : slots_.size() - 1;
	for (uint32_t i = 0; i < records_.size(); ++i) {
		size_t slot = (hashes_[i] ^ (hashes_[i] >> 32)) & mask;
		while (slots_[slot] != EMPTY) {
			slot = (slot + 1) & mask;
		}
		slots_[slot] = i;
	}
}

} // namespace nautilus::nir
