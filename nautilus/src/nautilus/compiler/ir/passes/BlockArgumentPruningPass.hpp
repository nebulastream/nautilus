
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
 *  2. **Same-value pass-through**: every predecessor edge passes the
 *     identical value `v` for the slot, ignoring self-referential edges
 *     (a loop latch passing the header's own argument straight back), and
 *     `v`'s defining block dominates the target block. All uses of the
 *     argument are replaced by `v` and the slot is removed as in (1). This
 *     is the SSA-phi-elimination analog; it is what deletes loop-invariant
 *     loop-carried arguments (preheader and latch both pass `v`). The
 *     dominance requirement conservatively skips blocks not reachable from
 *     the entry (`Dominators::dominates` is false for unreachable targets).
 *
 * The entry block is never touched: its arguments are the function ABI.
 * Each pruned slot removes a per-edge-execution move in the interpreter
 * backends (per *iteration* for loop headers) and frees a register in the
 * direct-lowering backends.
 *
 * Pass-through pruning (2) replaces a block's own argument with a value
 * defined in another (dominating) block, so lowered code reads values
 * across block boundaries. The `bc`/`tbc` register allocators support
 * this by pinning any identifier that is read outside its defining block
 * (see `crossBlockIds` in the two lowering providers) -- their free-list
 * recycling is only valid for block-locally-used values. `asmjit` (real
 * liveness-based register allocation), `cpp` (function-scoped variables),
 * and `mlir` handle dominance-valid cross-block reads natively.
 *
 * Idempotent: a second run finds no unused or pass-through argument and
 * changes nothing.
 */
class BlockArgumentPruningPass : public IRPass {
public:
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "BlockArgumentPruning";
	}
};

} // namespace nautilus::compiler::ir
