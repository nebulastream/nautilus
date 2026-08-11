#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace nautilus {

using AllocaIndex = uint32_t;
using CleanupStateId = uint32_t;

inline constexpr CleanupStateId EMPTY_CLEANUP_STATE = 0;

enum class CallKind : uint8_t {
	Regular,
	WithExceptionHandling,
};

[[nodiscard]] constexpr CallKind classifyCallKind(FunctionAttributes attributes, CleanupStateId cleanupState) noexcept {
	return !attributes.noUnwind && cleanupState != EMPTY_CLEANUP_STATE ? CallKind::WithExceptionHandling
	                                                                   : CallKind::Regular;
}

struct CleanupState {
	std::vector<AllocaIndex> active;
	bool operator==(const CleanupState&) const = default;
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
