#pragma once

#include <cstdint>

/// Contains function attributes that backends can use to improve code optimization
/// The struct 'FunctionAttributes' is the 'vehicle' that transports attributes from nautilus to the specific backend
namespace nautilus {
enum class ModRefInfo : uint8_t {
	NoModRef = 0,
	Ref = 1,
	Mod = 2,
	ModRef = 3,
};

/// Closely modeled after the LLVM function attributes, but may be used for any backend
/// (LLVM attributes: https://llvm.org/doxygen/classllvm_1_1Attribute.html)
struct FunctionAttributes {
	/// Indicates whether a function reads(ref)/writes(mod) memory
	ModRefInfo modRefInfo = ModRefInfo::ModRef;
};
} // namespace nautilus
