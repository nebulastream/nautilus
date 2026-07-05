
#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief Merges single-predecessor blocks into their predecessor (design
 * docs/design/ir-pass-improvements.md §4.3-D).
 *
 * A block `s` is merged into `p` iff `p` ends in an unconditional
 * `BranchOperation` targeting `s`, `s`'s only predecessor edge is that
 * branch, `s != p`, and `s` is not the entry block. Every use of each of
 * `s`'s block arguments is rewired to the corresponding invocation argument
 * (via `FunctionRewriter::replaceAllUses`, so operand and invocation-argument
 * uses alike are covered), `s`'s operations are spliced onto the end of `p`,
 * and `p` adopts `s`'s terminator. Runs to a per-function fixed point, so
 * chains of single-predecessor blocks collapse into one block.
 *
 * This is the generalization of `EmptyBlockEliminationPass` for the
 * single-predecessor case; the empty-block pass remains for the
 * multi-predecessor case it uniquely handles. Each merged block removes a
 * jump dispatch plus one full set of edge-copy moves per execution in the
 * interpreter backends.
 *
 * Idempotent: a second run finds no unconditional-branch edge to a
 * single-predecessor block and changes nothing.
 */
class BlockMergingPass : public IRPass {
public:
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "BlockMerging";
	}
};

} // namespace nautilus::compiler::ir
