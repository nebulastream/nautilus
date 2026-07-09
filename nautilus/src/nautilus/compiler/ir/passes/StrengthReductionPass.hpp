
#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief Replaces a per-iteration `base + inductionVar * constantStride`
 * pointer computation (the shape `array[i]` lowers to — there is no GEP-like
 * op, just a MulOperation feeding an AddOperation) with a single pointer
 * value that is incremented by `step * constantStride` each iteration.
 *
 * Scope is deliberately narrow (v1): only a "natural" single-latch,
 * single-preheader loop is considered (the loop header has exactly two
 * predecessors: one back-edge and one forward edge). The induction variable
 * must be a direct loop-header block argument whose back-edge value is
 * exactly `AddOperation(inductionVar, ConstIntOperation(step))` (either
 * operand order). The multiply's constant operand and the add's other
 * (base-pointer) operand must both be loop-invariant. The multiply must have
 * no other use in the function (so it is safe to neutralize once the
 * strength-reduced pointer replaces it) — anything not matching this exact
 * shape is left untouched.
 *
 * The rewrite never touches the original `base + index * stride` operation's
 * *consumers* (e.g. a LoadOperation reading `array[i]`): instead it mutates
 * that AddOperation in place (via its existing `setLeftInput`/`setRightInput`)
 * to become `newPointerPhi + 0`, so every existing consumer — whatever its
 * operation kind — sees the new value automatically through the pointer it
 * already holds, without needing a generic "rewrite every consumer type"
 * mechanism.
 *
 * Opt-in only (see "ir.enableStrengthReduction" in CompilationPipeline.cpp):
 * measured on the BC interpreter, this transform is *correct* but regressed
 * performance, because BC's per-op cost is dispatch-dominated rather than
 * ALU-dominated -- trading one multiply for a new stride-add plus a leftover
 * dead cast and an unfolded "ptr+0" was a net loss there. It may still be
 * worth enabling for an ALU-bound backend (e.g. MLIR's generated code),
 * which is why the pass stays available rather than being deleted.
 *
 * Default-flip status (design §4.3 "Enablement" / issue #347): the two P0
 * passes that remove exactly the documented residue now exist and run before
 * this pass -- `AlgebraicSimplificationPass` folds the `ptr + 0` back to the
 * base pointer, and `DeadCodeEliminationPass` sweeps the neutralized multiply
 * and the dead cast. The original blocker is therefore gone, but flipping the
 * default (renaming the option to `ir.disableStrengthReduction`) is still
 * gated on a fresh Release (Clang 21) `ExecutionBenchmark` sweep on `tbc`,
 * `bc`, and `asmjit` (off vs on, >= 5 runs, medians), which has not yet been
 * run. Record the medians here and flip only if no interpreter kernel
 * regresses > 2%.
 */
class StrengthReductionPass : public IRPass {
public:
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "StrengthReduction";
	}
};

} // namespace nautilus::compiler::ir
