
#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/passes/IRPass.hpp"
#include <cstdint>
#include <optional>
#include <vector>

namespace nautilus::compiler::ir {

/// A shared landing-pad block. The block holds the ordered destructor calls
/// (reverse of the construction order) that must run when the associated call
/// unwinds. It is not part of the function's main CFG; it lives only in the
/// `FunctionExceptionRegion` side table and is lowered by the backend.
struct LandingPadBlock {
	BasicBlock* block = nullptr;
};

/// A call site that may unwind. `pad` is `nullptr` when the call has no
/// destructors to run (the backend still needs the site to attach an
/// exception handler to, but there is no cleanup to perform).
struct ExceptionalCallSite {
	Operation* call = nullptr;
	LandingPadBlock* pad = nullptr;
};

/// Per-function side table produced by ExceptionRegionPreparationPass:
/// the landing pads keyed by the ordered destructor set, plus the call sites
/// that reference them.
struct FunctionExceptionRegion {
	std::vector<LandingPadBlock> pads;
	std::vector<ExceptionalCallSite> callSites;
};

/**
 * @brief Terminal pass that materializes exception-handling regions.
 *
 * Walks every FunctionOperation's basic blocks, collects every
 * potentially-throwing call (ProxyCallOperation / IndirectCallOperation with
 * `noUnwind == false`), and interns each call's ordered destructor list into
 * a shared landing-pad block. The result is attached to the FunctionOperation
 * as a `FunctionExceptionRegion` side table. Pad blocks and their destructor
 * calls are arena-allocated and carry fresh identifiers that never collide
 * with the main CFG's.
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
