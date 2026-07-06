
#pragma once

#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nautilus::compiler::ir {

class FunctionOperation;
class BasicBlockArgument;
class BasicBlockInvocation;

/**
 * @brief Shared natural-loop recognizer for the IR passes.
 *
 * The matcher was originally private to `StrengthReductionPass.cpp`; it is
 * extracted here (behavior-preserving) so both `StrengthReductionPass` and
 * `LoopInvariantCodeMotionPass` share one definition of "a loop this pass is
 * allowed to touch". Scope is deliberately narrow: only a single-latch,
 * single-preheader natural loop is recognized. Anything more general
 * (multiple latches/preheaders, irreducible control flow) is conservatively
 * skipped -- the passes built on this leave such loops untouched.
 */

/// Forward-reachability (via successor edges, >= 1 step) from every block,
/// memoized per block. Function CFGs here are small (tens of blocks), so a
/// plain per-block BFS is more than fast enough.
class Reachability {
public:
	const std::unordered_set<BasicBlock*>& from(BasicBlock* start);

private:
	std::unordered_map<BasicBlock*, std::unordered_set<BasicBlock*>> cache_;
};

/// A recognized natural loop: `header` has exactly two predecessor edges, one
/// forward edge from `preheader` and one back edge from `latch`, each a single
/// invocation whose argument arity matches the header. `body` is the header
/// plus every block reachable from it that can also reach the latch. The
/// preheader is guaranteed to lie *outside* `body`.
struct NaturalLoop {
	BasicBlock* header;
	BasicBlock* preheader;
	BasicBlock* latch;
	BasicBlockInvocation* preheaderInv;
	BasicBlockInvocation* latchInv;
	std::unordered_set<BasicBlock*> body;
};

/// Returns every natural loop in @p fn, in block-list order of the headers.
std::vector<NaturalLoop> findNaturalLoops(FunctionOperation& fn);

/// A control-flow edge into a successor rebinds every argument to a *fresh*
/// `BasicBlockArgument` owned by that successor -- even a plain pass-through
/// edge mints a new object that is only value-equivalent to the source, not
/// pointer-equal. This walks every pass-through edge inside @p loopBody
/// starting from @p ivArg and returns the full set of objects that all
/// represent the same loop-carried value (including @p ivArg itself). The
/// insert-based visited check naturally stops the walk at the back edge
/// (whose argument is a *different* object) without special-casing it.
std::unordered_set<Operation*> computeIvAliases(BasicBlock* header, BasicBlockArgument* ivArg,
                                                const std::unordered_set<BasicBlock*>& loopBody);

/// For every header argument slot, records whether the loop merely threads the
/// same value through unchanged every iteration (a "pass-through" phi) rather
/// than recomputing it. A slot is a pass-through iff the latch's supplied value
/// for that slot is itself a member of the slot's own alias set (see
/// @ref computeIvAliases). Returns a map from every alias object of every
/// pass-through slot to `true`; members of a real induction/accumulator slot
/// are omitted, so callers treat "not present" as unknown rather than "not
/// invariant".
std::unordered_map<Operation*, bool> computeHeaderPassThroughMap(BasicBlock* header, BasicBlockInvocation* latchInv,
                                                                 const std::unordered_set<BasicBlock*>& loopBody);

/// Returns true if @p op is loop-invariant: either its defining block is
/// outside @p loopBody outright, or it is a member of a header argument slot
/// that @p passThroughMap identified as a pass-through. Operations absent from
/// @p definingBlock (e.g. a function argument on the entry block) are treated
/// as invariant.
bool isLoopInvariant(Operation* op, const std::unordered_map<Operation*, BasicBlock*>& definingBlock,
                     const std::unordered_set<BasicBlock*>& loopBody,
                     const std::unordered_map<Operation*, bool>& passThroughMap);

/// Builds the `operation -> defining block` map for @p fn (block arguments and
/// operations alike), the shape @ref isLoopInvariant consumes.
std::unordered_map<Operation*, BasicBlock*> computeDefiningBlocks(FunctionOperation& fn);

} // namespace nautilus::compiler::ir
