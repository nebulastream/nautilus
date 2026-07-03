
#pragma once

#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include <cstddef>
#include <unordered_map>
#include <vector>

namespace nautilus::compiler::ir {

class FunctionOperation;

/**
 * @brief Dominator tree over one function's control-flow graph.
 *
 * Computed with the iterative Cooper-Harvey-Kennedy algorithm over reverse
 * post-order from the entry block. Construction requires the function's
 * predecessor lists to already be valid (`IRPassManager::run` calls
 * `rebuildPredecessorLists` once at the start of a pipeline run; standalone
 * users must call it themselves first).
 *
 * Blocks unreachable from the entry block (e.g. a loop left dangling after
 * branch folding, before the unreachable-block sweep runs) are handled as
 * follows: `isReachable` is false for them, they are absent from
 * `reversePostOrder()`, and `dominates(x, unreachableBlock)` is false for
 * every `x` (including `unreachableBlock` itself). Construction never
 * throws on such graphs. Consumers must not ask for the dominance of an
 * unreachable block's *contents* against reachable code -- there is none.
 */
class Dominators {
public:
	explicit Dominators(const FunctionOperation& fn);

	/// True iff `a` dominates `b`. A block dominates itself. False if `b`
	/// is unreachable from the entry block (regardless of `a`).
	[[nodiscard]] bool dominates(const BasicBlock* a, const BasicBlock* b) const;

	/// True iff `block` is reachable from the function's entry block.
	[[nodiscard]] bool isReachable(const BasicBlock* block) const;

	/// Reachable blocks in reverse post-order, starting with the entry block.
	[[nodiscard]] const std::vector<const BasicBlock*>& reversePostOrder() const;

private:
	std::vector<const BasicBlock*> rpo_;
	std::unordered_map<const BasicBlock*, size_t> rpoIndex_;
	/// idom_[i] is the RPO index of the immediate dominator of rpo_[i]; the
	/// entry block (index 0) is its own immediate dominator.
	std::vector<size_t> idom_;
};

} // namespace nautilus::compiler::ir
