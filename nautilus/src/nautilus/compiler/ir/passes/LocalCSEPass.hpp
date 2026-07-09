
#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief Block-local common-subexpression elimination via value numbering.
 *
 * Within each basic block, pure operations are keyed by
 * `(opType, result stamp, canonicalized operand identities, op-specific
 * payload)` -- the payload being the comparator of a compare, the kind of a
 * shift/bitwise op, or the value of a constant. The first occurrence of a key
 * is the representative; every later operation with the same key has its uses
 * rewired to the representative (`replaceAllUses`) and is then erased.
 * Commutative operations (`add`, `mul`, `and`, `or`, the bitwise ops, and
 * `eq`/`ne` compares) normalize operand order so `a+b` and `b+a` share a key
 * (the algebraic-simplification pass' constant-to-the-right canonicalization
 * already helps here).
 *
 * Scope (v1), deliberately conservative:
 *  - **block-local only** -- no dominator-scoped numbering. After block
 *    merging, trace IR has long straight-line blocks, which is where the
 *    duplicates live;
 *  - **memory operations are excluded** -- loads are never CSE'd (there is no
 *    alias analysis, so two loads of the same address are not provably equal);
 *    they are not pure, so `isPureOp` already excludes them;
 *  - **constants are excluded** -- although pure, merging two equal constants
 *    can make one constant feed both a store/edge and another use, a sharing
 *    shape the direct-lowering interpreter backends mis-lower (their
 *    constant-image / edge-copy machinery assumes trace IR's
 *    one-constant-per-use form). Constant duplication is already swept by
 *    constant folding + DCE, so little is lost.
 *
 * Erasing a duplicate is safe with respect to the value table: a duplicate
 * shares its exact operand objects with its representative, so removing it can
 * never orphan (and thus never invalidate) another key's representative.
 */
class LocalCSEPass : public IRPass {
public:
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "LocalCSE";
	}
};

} // namespace nautilus::compiler::ir
