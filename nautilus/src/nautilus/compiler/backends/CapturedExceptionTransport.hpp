#pragma once

#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/passes/ExceptionRegionPreparationPass.hpp"
#include <span>

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
 * The capture wrapper itself (`captureThrowingCall<R, Args...>`) is generated
 * at the typed invoke() site and recorded on the IR call operation
 * (`ProxyCallOperation::getCaptureFunc()` / `IndirectCallOperation::getCaptureFunc()`);
 * it is not resolved here.
 */
class CapturedExceptionTransport {
public:
	explicit CapturedExceptionTransport(const ir::FunctionOperation& fn);

	/// Returns true if @p call needs captured exception transport (a pending
	/// check after the call plus a branch to its landing pad).
	[[nodiscard]] bool callNeedsCapture(const ir::Operation* call) const;

	/// Returns the landing pad for @p call, or nullptr if the call is not an
	/// exceptional call site or has no destructors to run.
	[[nodiscard]] const ir::LandingPadBlock* getPadForCall(const ir::Operation* call) const;

private:
	const ir::ExceptionalCallSite* findCallSite(const ir::Operation* call) const;

	const ir::FunctionExceptionRegion* region_ = nullptr;
};

} // namespace nautilus::compiler
