#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include <cstdint>
#include <string>

namespace nautilus {

using AllocaIndex = uint32_t;

enum class CleanupEffectKind : uint8_t {
	ActivateAfterSuccess,
	DeactivateBeforeCall,
};

struct CleanupEffect {
	CleanupEffectKind kind;
	AllocaIndex alloca;

	constexpr bool operator==(const CleanupEffect&) const = default;
};

struct DestructorSpec {
	void* functionPtr;
	std::string functionName;
	FunctionAttributes attributes;
};

struct ExceptionCaptureSpec {
	void* functionPtr;
};

} // namespace nautilus
