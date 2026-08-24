#pragma once

#include "nautilus/compiler/ir/ExceptionRegion.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include <cstddef>
#include <unordered_map>

namespace nautilus::compiler {

/**
 * @brief Read-only helper that answers captured-exception-transport queries
 *        over a function's `FunctionExceptionRegion` side table.
 *
 * The fallback backends (CPP, BC, TBC, AsmJit) use this to decide, for a
 * given potentially-throwing call, whether it needs a pending-exception check
 * plus a branch to a landing pad, and which pad to branch to. It holds a
 * non-owning pointer to the region (or nullptr when the function has none).
 *
 * Construct once per function and reuse across call sites: the constructor
 * builds a call -> call-site index so each lookup is O(1). Constructing one
 * per call site instead makes lowering quadratic in the number of calls.
 *
 * The capture wrapper itself (`captureThrowingCall<R, Args...>`) is generated
 * at the typed invoke() site and recorded on the IR call operation
 * (`ProxyCallOperation::getCaptureFunc()` / `IndirectCallOperation::getCaptureFunc()`);
 * it is not resolved here.
 */
class CapturedExceptionTransport {
public:
	CapturedExceptionTransport() = default;
	explicit CapturedExceptionTransport(const ir::FunctionOperation& fn);

	/// Returns true if @p call needs captured exception transport (a pending
	/// check after the call plus a branch to its landing pad).
	[[nodiscard]] bool callNeedsCapture(const ir::Operation* call) const;

	/// Returns the index of @p call's landing pad within the region's pad list,
	/// or `ir::noLandingPad` when the call is not an exceptional call site or
	/// has no destructors to run. Backends that name pads (labels, block
	/// indices) should key off this rather than the pad address.
	[[nodiscard]] size_t getPadIndexForCall(const ir::Operation* call) const;

	/// Returns the landing pad for @p call, or nullptr if the call is not an
	/// exceptional call site or has no destructors to run.
	[[nodiscard]] const ir::LandingPadBlock* getPadForCall(const ir::Operation* call) const;

	/// The recorded `captureThrowingCall<R, Args...>` thunk for @p call, or
	/// nullptr when the call has none.
	///
	/// A capture site without a thunk must be lowered as a *direct* call plus
	/// the pending-exception check. Routing it through a null thunk would call
	/// address 0, with the argument list shifted by the thunk's extra target
	/// parameter. Not every exceptional call site carries a thunk:
	/// `traceNautilusCallWithExceptionHandling` records none, since a nested
	/// Nautilus callee captures through its own transport.
	[[nodiscard]] static void* captureThunkFor(const ir::Operation* call);

	/// True when @p call both needs capture and has a thunk to capture through.
	[[nodiscard]] bool callNeedsCaptureThunk(const ir::Operation* call) const {
		return callNeedsCapture(call) && captureThunkFor(call) != nullptr;
	}

	/// The underlying side table, or nullptr when the function has none.
	[[nodiscard]] const ir::FunctionExceptionRegion* getRegion() const {
		return region_;
	}

	/// True when the function has at least one exceptional call site, i.e. it
	/// needs landing pads, an exceptional exit and a pushed ExceptionFrame.
	[[nodiscard]] bool hasExceptionalCallSites() const {
		return region_ != nullptr && !region_->callSites.empty();
	}

private:
	const ir::FunctionExceptionRegion* region_ = nullptr;
	/// call operation -> index into `region_->callSites`.
	std::unordered_map<const ir::Operation*, size_t> siteIndex_;
};

} // namespace nautilus::compiler
