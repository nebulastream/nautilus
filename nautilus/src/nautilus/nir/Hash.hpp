#pragma once

#include "nautilus/common/OptionRegistry.hpp"
#include "nautilus/nir/Canonicalize.hpp"
#include <array>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace nautilus::nir {

/// An XXH3-128 value. The key is 16 bytes: the low 64 bits, then the high 64 bits, each little-endian (13 §7.2).
struct Hash128 {
	uint64_t low = 0;
	uint64_t high = 0;

	std::array<uint8_t, 16> bytes() const;
	/// 32 lowercase hex digits of the key bytes.
	std::string hex() const;
	bool operator==(const Hash128&) const = default;
};

/// `XXH3_128bits_withSeed(data, size, 0)`.
Hash128 hashBytes(std::span<const uint8_t> data);

/// The structural hash of 13-nir-spec.md §7.2: XXH3-128 over bytes [48, totalSize) of the canonical image of the
/// module canonicalized as if it were anonymous and without debug sections, then the options block of the
/// code-affecting options in effect. The module must be at the SSA level. A cache key, not a security property.
Hash128 structuralHash(const Module& m, const EffectiveOptions& options, const CanonOptions& canon = {});

/// The key of compiled code that inlined imports (§7.3): XXH3-128 over the module's key, a u32 count and, per
/// inlined exporter sorted by name, `u32 nameLength`, the name bytes and the exporter's structural hash.
Hash128 keyWithInlinedImports(const Hash128& moduleKey, std::vector<std::pair<std::string, Hash128>> exporters);

} // namespace nautilus::nir
