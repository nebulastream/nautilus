#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * Resolves the trace-recorded destructor state at every potentially throwing call.
 *
 * The result is attached to each FunctionOperation as a side-table and does
 * not alter the ordinary CFG. Backends materialize the logical pads only
 * after all normal IR optimization has completed. Lifetime transitions are
 * intentionally absent from the ordinary IR: exact state identity is part of
 * tracing and only calls retain a state reference.
 */
class ExceptionCleanupPreparationPass : public IRPass {
public:
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "ExceptionCleanupPreparation";
	}
};

} // namespace nautilus::compiler::ir
