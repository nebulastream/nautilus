#pragma once

#include "nautilus/common/ExceptionCleanup.hpp"
#include <cstdint>
#include <optional>
#include <vector>

namespace nautilus::compiler::ir {

class Operation;

using CleanupPadId = uint32_t;

struct CleanupPad {
	CleanupPadId id;
	std::vector<AllocaIndex> active;
	bool operator==(const CleanupPad&) const = default;
};

struct ExceptionalCallSite {
	const Operation* call;
	std::optional<CleanupPadId> cleanup;
	bool operator==(const ExceptionalCallSite&) const = default;
};

struct FunctionExceptionRegion {
	std::vector<CleanupPad> pads;
	std::vector<ExceptionalCallSite> callSites;
	bool operator==(const FunctionExceptionRegion&) const = default;
};

} // namespace nautilus::compiler::ir
