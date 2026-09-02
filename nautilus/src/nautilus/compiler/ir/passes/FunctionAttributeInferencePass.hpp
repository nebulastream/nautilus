#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief Derives what each in-module function actually does to memory.
 *
 * Every traced call to a Nautilus function passes a default-constructed
 * `FunctionAttributes`: ModRef, `willReturn = false`, `noUnwind = false`.
 * That is the most pessimistic answer possible, and it is applied to the one
 * kind of callee whose whole body is right here in the same graph -- the only
 * case where the answer can be *derived* rather than declared.
 *
 * This pass derives it. For each internal function it walks the body and
 * asks: does it load, does it store, can it fail to return, can it unwind --
 * charging every call it makes with whatever that callee does. The result
 * lands on the callee's function-table entry, where `effectsOf` reads it, so
 * CSE, LICM and DCE can move or drop a call that provably has no observable
 * effect.
 *
 * Soundness comes from being conservative wherever the analysis cannot see:
 *
 *  - A cycle in the call graph (direct or mutual recursion) makes every
 *    function in it pessimistic. A fixed point over recursion would be more
 *    precise; single-pass with recursion pessimistic is what this does.
 *  - Any loop in a function makes it `willReturn = false`. A terminating loop
 *    is misjudged here, which costs optimisation and never correctness.
 *  - An indirect call, or a call whose callee does not resolve, makes the
 *    caller fully pessimistic: the target is a runtime value.
 *
 * Analysis only -- it mutates the function table, never the graph -- so it
 * reports no change and is idempotent by construction.
 */
class FunctionAttributeInferencePass : public IRPass {
public:
	bool apply(IRGraph& ir) override;

	std::string getName() const override {
		return "functionAttributeInference";
	}
};

} // namespace nautilus::compiler::ir
