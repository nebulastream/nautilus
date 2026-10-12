
#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief Folds pure operations whose inputs are all compile-time constants
 * into a single `Const{Int,Float,Boolean}Operation` and rewires every use of
 * the folded operation to the new constant.
 *
 * Foldable operations:
 *   - Arithmetic: `AddOp`, `SubOp`, `MulOp`, `DivOp`, `ModOp` (int and float)
 *   - Logical:    `AndOp`, `OrOp`, `NotOp`
 *   - Bitwise:    `ShiftOp` (LS, RS), `BinaryCompOp` (BAND, BOR, XOR; int and
 *                 bool), `NegateOp` (bitwise NOT for ints, sign flip for floats)
 *   - Compare:    `CompareOp` (EQ, NE, LT, LE, GT, GE; EQ/NE only for bools)
 *   - Casts:      `CastOp` between integer, float and bool stamps
 *
 * Inputs read as constants are `ConstIntOp`, `ConstFloatOp` and
 * `ConstBooleanOp`; `ConstPtrOp` is deliberately excluded.
 *
 * Every fold reproduces what the unfolded operation computes on every
 * backend:
 *   - integer results are wrapped to the stamp's width and kept in the
 *     canonical form (sign-extended for signed, zero-extended for unsigned
 *     stamps); division, modulo, right shift and ordered compares use the
 *     stamp's signedness;
 *   - f32 arithmetic is computed in `float`, and every f32 constant is
 *     rounded to `float` (#542);
 *   - integer casts extend by the source's signedness; int->float rounds once,
 *     straight from the integer type; float->bool treats NaN as true.
 *
 * Operations whose result is undefined at runtime, or trap, are left in place
 * so they still execute (and fail) as written:
 *   - integer division or modulo by zero, and signed `MIN / -1`, `MIN % -1`;
 *   - a shift amount that is negative or not below the stamp's bit width;
 *   - float division by zero, and float modulo (never folded);
 *   - float->int casts of NaN, infinities or out-of-range values, and f64->f32
 *     casts of finite values beyond f32's range;
 *   - casts to or from pointers, and operand kinds an operation is not defined
 *     on (e.g. `Add(int, float)`, ordered compares on bools).
 *
 * Folding runs a worklist to a fixed point per function, so chains such as
 * `(5 + 3) * 2` or `(f64) -(3 + 4)` collapse in one invocation: the worklist
 * starts with the operations whose inputs are already constant, and each fold
 * adds the consumers it made foldable. A function with nothing to fold costs
 * one scan. Each folded constant takes over the folded operation's slot and
 * identifier (`FunctionRewriter::createReplacing`), and the constants it
 * orphaned are erased in one sweep per block at the end, so the pass is linear
 * in the number of operations and folds.
 */
class ConstantFoldingAndCopyPropagationPass : public IRPass {
public:
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "ConstantFoldingAndCopyPropagation";
	}
};

} // namespace nautilus::compiler::ir
