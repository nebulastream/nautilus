#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/passes/IRPass.hpp"
#include <string>

namespace nautilus::compiler::ir {

/**
 * @brief Proves Nautilus-to-Nautilus calls noUnwind by whole-module
 * call-graph analysis, downgrading them from `CALL_WITH_EXCEPTION_HANDLING`
 * to plain `CALL`.
 *
 * The trace-time heuristic that decides a NautilusFunction call's noUnwind
 * attribute (`std::is_nothrow_invocable_v` over the wrapped callable) is
 * pessimistic for any call passing `val<T>` arguments by value: binding an
 * lvalue argument to a by-value parameter goes through `val<T>`'s copy
 * constructor, which is never `noexcept`, so the trait comes back false
 * regardless of whether the callee itself can actually throw. Every nested
 * Nautilus call in the module therefore starts out tagged
 * exception-handling even when it (and everything it calls) provably can't
 * unwind.
 *
 * Because every Nautilus function reachable from the compiled root is
 * traced into the same IRGraph as its own FunctionOperation, this pass can
 * inspect each callee's finished body directly instead of trusting the
 * trace-time guess. A function "canThrow" if it contains a raw external
 * call still marked exception-handling, an IndirectCallOperation (callee
 * unknown, always conservative), or a Nautilus-to-Nautilus call into a
 * function proven canThrow. This is computed as a fixed point starting
 * optimistic (canThrow = false for everyone): a pure recursive cycle with
 * no external throw source is proven noThrow as a whole, rather than
 * conservatively assumed throwing just because it's cyclic.
 *
 * Runs once, immediately before ExceptionRegionPreparationPass, so a
 * function proven noThrow skips landing-pad construction entirely.
 */
class NoThrowInferencePass : public IRPass {
public:
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "noThrowInference";
	}
};

} // namespace nautilus::compiler::ir
