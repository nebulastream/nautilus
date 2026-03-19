#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include <memory>

namespace nautilus::compiler::ir {

/**
 * @brief Constant folding phase that evaluates operations with constant operands at compile time.
 *
 * For each operation where all inputs are constants, this phase computes the result
 * and replaces the operation with a new constant operation. This is applied iteratively
 * until no more folding is possible.
 */
class ConstantFoldingPhase {
public:
	/**
	 * @brief Applies constant folding to the supplied IR graph.
	 * @param ir IR graph that the constant folding is applied to.
	 */
	void apply(std::shared_ptr<IRGraph> ir);
};

} // namespace nautilus::compiler::ir
