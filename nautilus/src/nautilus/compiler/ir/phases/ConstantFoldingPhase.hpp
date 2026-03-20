#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include <memory>

namespace nautilus::compiler::ir {

/**
 * @brief Performs constant propagation and folding on the IR graph.
 *
 * This phase uses a lattice-based forward dataflow analysis to identify
 * constant values, then replaces operations with computed constants where possible.
 * Block arguments are handled as the equivalent of phi nodes.
 */
class ConstantFoldingPhase {
public:
	void apply(std::shared_ptr<IRGraph> ir);
};

} // namespace nautilus::compiler::ir
