#pragma once
#include <cstdint>

namespace nautilus::compiler {
enum class ExceptionPropagationMode : uint8_t {
	NativeUnwind,
	CapturedHostRethrow,
};
} // namespace nautilus::compiler
