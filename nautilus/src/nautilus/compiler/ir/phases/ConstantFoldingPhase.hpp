#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/options.hpp"
#include <memory>

namespace nautilus::compiler::ir {

/**
 * @brief Constant folding phase that evaluates operations with constant operands at compile time.
 *
 * For each operation where all inputs are constants, this phase computes the result
 * and replaces the operation with a new constant operation. This is applied iteratively
 * until no more folding is possible.
 *
 * Controlled by the following options:
 *   - "optimization.constantFolding" (bool, default: true) — enable intra-block constant folding
 *   - "optimization.interBlockConstantPropagation" (bool, default: true) — enable inter-block propagation
 */
class ConstantFoldingPhase {
public:
	/**
	 * @brief Applies constant folding to the supplied IR graph using default options (all enabled).
	 * @param ir IR graph that the constant folding is applied to.
	 */
	void apply(std::shared_ptr<IRGraph> ir);

	/**
	 * @brief Applies constant folding to the supplied IR graph with configurable options.
	 * @param ir IR graph that the constant folding is applied to.
	 * @param options Engine options controlling which sub-passes are enabled.
	 */
	void apply(std::shared_ptr<IRGraph> ir, const engine::Options& options);
};

} // namespace nautilus::compiler::ir
