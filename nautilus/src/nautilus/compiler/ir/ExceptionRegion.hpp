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
};

struct ExceptionalCallSite {
	const Operation* call;
	std::optional<CleanupPadId> cleanup;
};

struct FunctionExceptionRegion {
	std::vector<CleanupPad> pads;
	std::vector<ExceptionalCallSite> callSites;
};

} // namespace nautilus::compiler::ir
