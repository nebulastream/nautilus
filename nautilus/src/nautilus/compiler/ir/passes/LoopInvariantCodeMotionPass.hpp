
#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief Hoists loop-invariant pure operations out of a natural loop's body
 * into its preheader, so they execute once per loop entry instead of once per
 * iteration.
 *
 * This is the highest per-hit payoff of the IR passes for the dispatch-bound
 * interpreters (`bc`/`tbc`): every hoisted operation saves
 * `(iterations - 1)` dispatches; for `asmjit` it removes re-computation from
 * the loop body. It is also the highest-risk pass, so it ships **opt-in**
 * (`ir.enableLICM`, default off) and is sequenced as a single-shot pass after
 * the main cleanup group (see design §4.3-G / §4.4).
 *
 * Scope (v1), deliberately conservative:
 *  - only the single-latch / single-preheader natural loops recognized by
 *    `LoopInfo::findNaturalLoops` are considered;
 *  - only *pure* operations (`isPureOp`) are hoisted -- loads, stores and
 *    calls are never moved (no alias analysis in v1);
 *  - an operation is hoisted only when *all* of its operands are
 *    loop-invariant (defined outside the loop, a pass-through loop argument,
 *    or already hoisted). Chains hoist transitively via an internal fixed
 *    point;
 *  - `DivOp`/`ModOp` are **never** speculated into the preheader unless their
 *    divisor is a non-zero integer constant -- hoisting a trapping division
 *    across the loop-taken check is unsound when the loop body would not have
 *    executed;
 *  - loops whose body contains a **nested** loop are skipped entirely. A value
 *    hoisted out of the outer loop would have a live range spanning the inner
 *    loop's back edge, which the direct-lowering interpreter backends
 *    (`bc`/`tbc`/`asmjit`) do not keep live -- in trace IR, every value that
 *    crosses a loop is a block argument, so this pattern is otherwise never
 *    generated. Single-loop hoists are backend-correct; nested handling would
 *    require threading the hoisted value through the inner loop's arguments and
 *    is left to a later revision.
 *
 * The hoisted operation keeps its identity (it is *moved*, not cloned, so
 * every existing consumer sees the same value); moving a definition into the
 * preheader -- which dominates the whole loop -- can never break SSA
 * dominance, and hoisting only side-effect-free operations makes the
 * reordering semantics-preserving.
 */
class LoopInvariantCodeMotionPass : public IRPass {
public:
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "LoopInvariantCodeMotion";
	}
};

} // namespace nautilus::compiler::ir
