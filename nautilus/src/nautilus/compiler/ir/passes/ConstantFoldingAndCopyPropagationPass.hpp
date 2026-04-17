
#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief Folds pure binary operations whose operands are both compile-time
 * constants into a single `Const{Int,Float,Boolean}Operation` and propagates
 * the freshly minted constant into every downstream use.
 *
 * Foldable operation set:
 *   - Arithmetic: `AddOp`, `SubOp`, `MulOp`, `DivOp`, `ModOp`
 *   - Logical:    `AndOp`, `OrOp`
 *   - Bitwise:    `ShiftOp` (LS, RS), `BinaryCompOp` (BAND, BOR, XOR)
 *   - Compare:    `CompareOp` (EQ, NE, LT, LE, GT, GE)
 *
 * Operand kinds treated as constant match `Operation::isConstOperation()`,
 * i.e. `ConstIntOp`, `ConstFloatOp`, `ConstBooleanOp` — `ConstPtrOp` is
 * deliberately excluded.
 *
 * Guards that skip the fold (the original op remains in place so it still
 * executes at runtime):
 *   - integer / float divide- or mod-by-zero;
 *   - negative shift amount;
 *   - float modulo (not considered foldable by this pass);
 *   - operand kind mismatch (e.g. `Add(int, float)`, bool mixed with int).
 *
 * Copy propagation walks every operation after each fold round and rewires
 * consumers to the replacement constant. Supported consumer slots:
 *   - `BinaryOperation::setLeftInput` / `setRightInput`
 *   - `IfOperation::setBooleanValue`
 *   - `ReturnOperation::setReturnValue`
 *   - Every `BasicBlockInvocation` argument reachable via
 *     `getSuccessorInvocations()` on a terminator operation.
 *
 * The pass iterates to a fixed point per function, so chained folds such
 * as `(5 + 3) * 2` collapse to `16` in a single invocation.
 */
class ConstantFoldingAndCopyPropagationPass : public IRPass {
public:
	void apply(IRGraph& ir) override;
	std::string getName() const override {
		return "ConstantFoldingAndCopyPropagation";
	}
};

} // namespace nautilus::compiler::ir
