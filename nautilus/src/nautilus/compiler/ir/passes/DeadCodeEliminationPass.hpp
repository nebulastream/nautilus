
#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief Removes pure operations that have zero uses, cascading into
 * operands that become use-free as a result.
 *
 * A thin driver over `FunctionRewriter::eraseIfDead` (design
 * docs/design/ir-pass-improvements.md §4.3-A): every function is opened in
 * one rewriter session and the cascade is seeded from every operation in
 * every block. Only operations that are pure (`isPureOp`, per
 * `OperationProperties.hpp` -- includes `Constant*` ops, excludes loads,
 * stores, and calls) and use-free are ever erased; block arguments are left
 * untouched (`BlockArgumentPruningPass` owns that invariant), and an
 * operation used only as a `BasicBlockInvocation` argument is not use-free
 * and therefore survives.
 */
class DeadCodeEliminationPass : public IRPass {
public:
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "DeadCodeElimination";
	}
};

} // namespace nautilus::compiler::ir
