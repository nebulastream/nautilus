
#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief Eliminates basic blocks that contain nothing but an unconditional
 * branch, rewiring their predecessors to skip over them.
 *
 * A block is a removal candidate iff
 *   - it is *not* the function's entry block,
 *   - its operation list contains exactly one op, a `BranchOperation`,
 *   - that branch's target is some block `t != self` (no self-loop).
 *
 * For each predecessor `p` that targets the candidate `v` via one or more
 * of `p`'s terminator invocations, the pass substitutes `v`'s outgoing
 * argument list into `p`'s invocation: block arguments are mapped to the
 * values `p` was passing for the corresponding argument slot; constants
 * pass through unchanged. Arity is preserved, so no buffer growth is
 * needed.
 *
 * The pass iterates to a fixed point per function so chains of empty
 * blocks collapse in a single invocation.
 */
class EmptyBlockEliminationPass : public IRPass {
public:
	void apply(IRGraph& ir) override;
	std::string getName() const override {
		return "EmptyBlockElimination";
	}
};

} // namespace nautilus::compiler::ir
