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

Ref ConstPool::intern(TypeId type, Provenance provenance, uint64_t bits) {
	uint64_t h = mix(mix(static_cast<uint64_t>(type), static_cast<uint64_t>(provenance)), bits);
	auto range = index_.equal_range(h);
	for (auto it = range.first; it != range.second; ++it) {
		auto& r = records_[it->second];
		if (wideSizes_[it->second] == 0 && r.type == static_cast<uint8_t>(type) &&
		    r.provenance == static_cast<uint8_t>(provenance) && r.bits == bits) {
			return makeConstRef(it->second);
		}
	}
	if (records_.size() >= MAX_CONSTANTS) {
		throw std::length_error("constant pool is full");
	}
	auto index = static_cast<uint32_t>(records_.size());
	records_.push_back(ConstRecord {static_cast<uint8_t>(type), static_cast<uint8_t>(provenance), 0, 0, bits});
	wideSizes_.push_back(0);
	index_.emplace(h, index);
	return makeConstRef(index);
}

Ref ConstPool::internBytes(TypeId type, Provenance provenance, std::span<const std::byte> bytes, uint32_t align) {
	uint64_t h = mix(static_cast<uint64_t>(type), static_cast<uint64_t>(provenance));
	for (auto b : bytes) {
		h = mix(h, static_cast<uint8_t>(b));
	}
	auto range = index_.equal_range(h);
	for (auto it = range.first; it != range.second; ++it) {
		auto& r = records_[it->second];
		if (wideSizes_[it->second] == bytes.size() && r.type == static_cast<uint8_t>(type) &&
		    r.provenance == static_cast<uint8_t>(provenance) &&
		    std::memcmp(bytes_.data() + r.byteOffset, bytes.data(), bytes.size()) == 0) {
			return makeConstRef(it->second);
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
	records_.push_back(ConstRecord {static_cast<uint8_t>(type), static_cast<uint8_t>(provenance), 0, offset, 0});
	wideSizes_.push_back(static_cast<uint32_t>(bytes.size()));
	index_.emplace(h, index);
	return makeConstRef(index);
}

} // namespace nautilus::nir
