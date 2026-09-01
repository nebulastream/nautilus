#pragma once

#include <cstddef>
#include <vector>

namespace nautilus::compiler::ir {

class BasicBlock;
class Operation;

/// A shared landing-pad block. The block holds the ordered destructor calls
/// (reverse of the construction order) that must run when the associated call
/// unwinds. It is not part of the function's main CFG; it lives only in the
/// `FunctionExceptionRegion` side table and is lowered by the backend.
struct LandingPadBlock {
	BasicBlock* block = nullptr;
};

/// Sentinel for `ExceptionalCallSite::padIndex`: the call is an exceptional
/// call site, but has no destructors to run.
inline constexpr size_t noLandingPad = static_cast<size_t>(-1);

/// A call site that may unwind. `padIndex` is `noLandingPad` when the call has
/// no destructors to run (the backend still needs the site to attach an
/// exception handler to, but there is no cleanup to perform).
///
/// The pad is referenced by index rather than by pointer so that
/// `FunctionExceptionRegion` stays a well-behaved value type: a copy of the
/// region keeps its call sites pointing at the copy's own pads.
struct ExceptionalCallSite {
	Operation* call = nullptr;
	size_t padIndex = noLandingPad;

	[[nodiscard]] bool hasPad() const {
		return padIndex != noLandingPad;
	}
};

/// Per-function side table produced by ExceptionRegionPreparationPass:
/// the landing pads keyed by the ordered destructor set, plus the call sites
/// that reference them.
struct FunctionExceptionRegion {
	std::vector<LandingPadBlock> pads;
	std::vector<ExceptionalCallSite> callSites;

	/// The landing pad for @p site, or nullptr when the site has no cleanup.
	[[nodiscard]] const LandingPadBlock* padFor(const ExceptionalCallSite& site) const {
		return site.hasPad() ? &pads[site.padIndex] : nullptr;
	}
};

} // namespace nautilus::compiler::ir
