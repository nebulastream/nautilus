#pragma once

#include "nautilus/compiler/ir/passes/IRPass.hpp"

namespace nautilus::compiler::ir {

/**
 * Computes the static destructor state at every potentially throwing call.
 *
 * The result is attached to each FunctionOperation as a side-table and does
 * not alter the ordinary CFG. Backends materialize the logical pads only
 * after all normal IR optimization has completed.
 */
class ExceptionCleanupPreparationPass : public IRPass {
public:
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "ExceptionCleanupPreparation";
	}
};

} // namespace nautilus::compiler::ir
