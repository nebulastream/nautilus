#pragma once

#include "nautilus/nir/Records.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <unordered_map>
#include <vector>

namespace nautilus::nir {

/// The module's constants (13-nir-spec.md §4.2), interned by (type, provenance, value bytes).
/// Interning happens once per created constant; operands refer to constants by index and never hash.
class ConstPool {
public:
	/// A constant of at most 8 bytes; `bits` is the zero-extended bit pattern.
	Ref intern(TypeId type, Provenance provenance, uint64_t bits);
	/// A constant wider than 8 bytes (extension types), whose `size` bytes live in constBytes.
	Ref internBytes(TypeId type, Provenance provenance, std::span<const std::byte> bytes);

	uint32_t size() const {
		return static_cast<uint32_t>(records_.size());
	}
	const ConstRecord& at(uint32_t index) const {
		return records_[index];
	}
	const ConstRecord& record(Ref ref) const {
		return records_[constIndexOf(ref)];
	}
	std::span<const ConstRecord> records() const {
		return records_;
	}
	std::span<const std::byte> bytes() const {
		return bytes_;
	}
	/// The `size` bytes of a wide constant.
	std::span<const std::byte> wideBytes(const ConstRecord& r, size_t size) const {
		return std::span<const std::byte>(bytes_).subspan(r.byteOffset, size);
	}

private:
	std::vector<ConstRecord> records_;
	std::vector<std::byte> bytes_;
	// hash of (type, provenance, value) -> indices with that hash
	std::unordered_multimap<uint64_t, uint32_t> index_;
	std::vector<uint32_t> wideSizes_; // per constant: byte size, 0 for narrow
};

} // namespace nautilus::nir
