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
/// (LLVM attributes: https://llvm.org/docs/LangRef.html and https://llvm.org/doxygen/classllvm_1_1Attribute.html)
/// Useful information for MLIR/LLVM backend:
/// - eliminating function calls that have no effect
///		- (ModRefInfo::Ref or ModRefInfo::NoModRef) + willReturn=true + noUnwind=true
struct FunctionAttributes {
	/// Indicates whether a function reads(ref)/writes(mod) memory
	ModRefInfo modRefInfo = ModRefInfo::ModRef;
	/// Indicates that a call of this function will either exhibit undefined behavior or comes back and continues
	/// execution at a point in the existing call stack that includes the current invocation
	/// May still raise an exception, i.a., nounwind is not implied
	bool willReturn = false;
	/// Indicates that the function never raises an exception
	bool noUnwind = false;
};
} // namespace nautilus
