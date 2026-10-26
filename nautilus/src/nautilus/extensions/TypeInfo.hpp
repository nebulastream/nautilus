#pragma once

#include <cstdint>
#include <string>

namespace nautilus::ext {

/// How a backend keeps a value of an extension type (docs/plans/nautilus-0.4/04-extensions.md §3.2).
enum class RegClass : uint8_t { GP, FP, Vector, MultiPart };

/// An extension type: a registered, named layout. The registry assigns its `TypeId` (16 to 255).
struct TypeInfo {
	/// "simd.vec<f32,8>", "builtin.i128": `<plugin>.<ctor>`, optionally followed by `<arg,...>`.
	std::string name;
	uint32_t version = 1;
	/// The oldest image version of this type that still loads (13-nir-spec.md §8.4).
	uint32_t minCompatibleVersion = 1;
	uint16_t size = 0, align = 1;
	RegClass regClass = RegClass::GP;
	/// More than one for multi-part values (an i128 is two GP parts).
	uint8_t parts = 1;
	/// Payloads and constants contain no process addresses.
	bool portable = true;
	/// The host CPU supports this instance natively; an unavailable instance still loads (13 §8.4).
	bool available = true;
	/// The C type that stands for it in native signatures (`__m256`), empty if none (13 §4.5).
	std::string cabi;
};

} // namespace nautilus::ext
