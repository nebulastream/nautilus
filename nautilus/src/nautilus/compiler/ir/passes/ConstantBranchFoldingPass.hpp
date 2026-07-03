
#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief Folds statically-decidable `IfOperation`s into `BranchOperation`s
 * and sweeps the blocks that fall out of the CFG as a result (design
 * docs/design/ir-pass-improvements.md §4.3-C).
 *
 * Two coupled steps, run to a per-function fixed point (folding one `if`
 * can turn a formerly-conditional predecessor edge into groundwork for
 * folding another):
 *
 *  1. Every `IfOperation` whose condition is a `ConstBooleanOperation` is
 *     replaced by a `BranchOperation` that adopts the taken side's
 *     `BasicBlockInvocation` (target and argument list). An `IfOperation`
 *     whose true and false invocations target the same block with
 *     identical argument lists is folded the same way regardless of the
 *     condition's constness.
 *  2. Unreachable-block sweep: every block not reachable from the
 *     function's entry block via successor edges is detached. This also
 *     removes unreachable *cycles* (mutually-referential blocks with no
 *     path from entry) that would otherwise never individually satisfy
 *     `FunctionRewriter::eraseBlock`'s "no predecessors" precondition --
 *     every predecessor of an unreachable block is itself unreachable (by
 *     definition of forward reachability), so every unreachable block's
 *     predecessor list is safe to clear before erasure.
 *
 * The entry block is never a candidate for either step.
 */
class ConstantBranchFoldingPass : public IRPass {
public:
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "ConstantBranchFolding";
	}
};

} // namespace nautilus::compiler::ir
