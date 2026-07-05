
#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief Removes redundant block arguments (design
 * docs/design/ir-pass-improvements.md §4.3-E).
 *
 * The only pass allowed to change block-argument arity, and it does so
 * exclusively through `FunctionRewriter::removeBlockArgument`, which keeps
 * the block's argument list and every targeting invocation's argument list
 * in sync atomically (the verifier's arity check guards the invariant).
 *
 * Two prunes per block, run to a per-function fixed point (removing one
 * argument can leave another without uses):
 *
 *  1. **Unused argument**: a block argument with zero uses -- operand and
 *     invocation-argument uses both count -- is removed together with the
 *     corresponding argument slot of every invocation targeting the block.
 *  2. **Same-value pass-through** (opt-in, see below): every predecessor
 *     edge passes the identical value `v` for the slot, ignoring
 *     self-referential edges (a loop latch passing the header's own
 *     argument straight back), and `v`'s defining block dominates the
 *     target block. All uses of the argument are replaced by `v` and the
 *     slot is removed as in (1). This is the SSA-phi-elimination analog;
 *     it is what deletes loop-invariant loop-carried arguments (preheader
 *     and latch both pass `v`). The dominance requirement conservatively
 *     skips blocks not reachable from the entry (`Dominators::dominates`
 *     is false for unreachable targets).
 *
 * The entry block is never touched: its arguments are the function ABI.
 * Each pruned slot removes a per-edge-execution move in the interpreter
 * backends (per *iteration* for loop headers).
 *
 * Pass-through pruning (2) is IR-correct but ships **disabled by default**
 * (`ir.enableBlockArgumentPassThroughPruning`): it replaces a block's own
 * argument with a value defined in another block, and today the
 * direct-lowering backends (`bc`, `tbc`, `asmjit`) assume the trace-IR
 * discipline that a block only reads its own arguments and its own local
 * definitions. Their register allocation frees a value's register at its
 * last static reference in *emission* order (see
 * `BCLoweringProvider::useValue`), which is only sound under that
 * discipline -- a cross-block read introduced by this prune can observe a
 * register that was freed and reallocated by a block emitted later but
 * executed earlier (observed as an ifSumLoop infinite loop and a fuzzer
 * pointer-index mismatch on `tbc`). Flip the default only after those
 * backends compute real cross-block liveness. Unused-argument pruning (1)
 * never introduces cross-block reads and is unaffected.
 *
 * Idempotent: a second run finds no unused or pass-through argument and
 * changes nothing.
 */
class BlockArgumentPruningPass : public IRPass {
public:
	explicit BlockArgumentPruningPass(bool enablePassThroughPruning = false)
	    : enablePassThroughPruning(enablePassThroughPruning) {
	}
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "BlockArgumentPruning";
	}

private:
	bool enablePassThroughPruning;
};

} // namespace nautilus::compiler::ir
