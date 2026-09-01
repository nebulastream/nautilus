#pragma once

#include "nautilus/compiler/ir/ExceptionRegion.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/passes/IRPass.hpp"
#include <string>

namespace nautilus::compiler::ir {

/**
 * @brief Terminal pass that materializes exception-handling regions.
 *
 * Walks every FunctionOperation's basic blocks, collects every
 * potentially-throwing call (ProxyCallOperation / IndirectCallOperation with
 * `noUnwind == false`), and interns each call's ordered destructor list into
 * a shared landing-pad block. The result is attached to the FunctionOperation
 * as a `FunctionExceptionRegion` side table (see ir/ExceptionRegion.hpp).
 * Pad blocks and their destructor calls are arena-allocated and carry fresh
 * identifiers that never collide with the main CFG's.
 *
 * Idempotent: a function whose `exceptionRegion` is already set is skipped.
 */
class ExceptionRegionPreparationPass : public IRPass {
public:
	bool apply(IRGraph& ir) override;
	std::string getName() const override {
		return "exceptionRegionPreparation";
	}
};

} // namespace nautilus::compiler::ir
