
#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief Local, pattern-per-op algebraic rewrites (design
 * docs/design/ir-pass-improvements.md §4.3-B).
 *
 * Every rule either replaces the op's uses with an existing operand (the op
 * itself then dies in `DeadCodeEliminationPass`) or folds it to a freshly
 * created constant/operation. Implements, exactly:
 *
 *  - Additive/multiplicative identities: `x+0`, `0+x`, `x-0` -> `x` and
 *    `x-x` -> `0` for **integers only** (float additive identities are not
 *    IEEE-exact for -0.0/NaN and are never folded); `x*1`, `1*x`, `x/1` ->
 *    `x` for integers and floats alike (exact under IEEE 754); `x*0`,
 *    `0*x` -> `0` for integers only (float `0*x` is not folded -- NaN/-0.0);
 *    `ptr + 0` (pointer-typed add with an integer constant 0) -> the base
 *    pointer.
 *  - Bitwise: `x&x`, `x|x` -> `x`; `x^x` -> `0`; `x&0` -> `0`; `x|0`, `x^0`,
 *    `x<<0`, `x>>0` -> `x`.
 *  - Boolean: `and(x,true)` -> `x`, `and(x,false)` -> `false`, `or` duals,
 *    `not(not(x))` -> `x`, `not(compare)` -> the comparison with an
 *    inverted comparator.
 *  - Casts: cast to the identical stamp -> the operand; cast-of-cast
 *    collapses to a single cast only when provably lossless (an integer
 *    widening chain of uniform signedness, or a pure integer truncation
 *    chain) -- anything else is left alone.
 *  - `select(constBool, a, b)` -> `a`/`b`; `select(c, x, x)` -> `x`.
 *  - Canonicalization: for commutative ops (`add`, `mul`, logical `and`/
 *    `or`, bitwise `and`/`or`/`xor`, `compare`), a constant left operand is
 *    swapped to the right (comparators are mirrored, not inverted, to
 *    preserve semantics) so the existing per-backend right-operand
 *    immediate folding sees it.
 *
 * A thin worklist-driven fixed point per function, structurally mirroring
 * `ConstantFoldingAndCopyPropagationPass`: every replacement goes through
 * `FunctionRewriter::replaceAllUses` + `eraseIfDead`, and a replaced op's
 * consumers are re-examined since their new operand may now be foldable.
 */
class AlgebraicSimplificationPass : public IRPass {
public:
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "AlgebraicSimplification";
	}
};

} // namespace nautilus::compiler::ir
