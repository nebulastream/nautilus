#pragma once

#include "nautilus/compiler/ir/IRLocationMap.hpp"
#include "nautilus/compiler/ir/passes/IRPass.hpp"
#include <memory>
#include <string>

namespace nautilus::compiler::ir {

/**
 * @brief Analysis pass: records where every function, block and operation lands
 * in one rendering of the IR.
 *
 * Computes an `IRLocationMap` (see IRLocationMap.hpp) and keeps it as the
 * pass's result; the graph is left untouched and `apply` always reports no
 * change. The map is what lets the MLIR backend point DWARF line numbers at a
 * Nautilus-IR dump.
 *
 * The result is also published on the graph (`IRGraph::getLocationMap`), which
 * is how the MLIR backend reads it.
 *
 * Must be registered as the last pass in the pipeline. A map is a snapshot of
 * one rendering: any later pass that mints or removes an operation invalidates
 * every line in it.
 */
class IRLocationPass final : public IRPass {
public:
	bool apply(IRGraph& ir) override;

	std::string getName() const override {
		return "irLocations";
	}

	/// The map computed by the last `apply`, or nullptr before the first one.
	[[nodiscard]] const std::shared_ptr<IRLocationMap>& getResult() const {
		return result;
	}

private:
	std::shared_ptr<IRLocationMap> result;
};

/// Renders @p graph and returns where everything in it landed, together with
/// the flattened region chain of every operation.
///
/// Must run after the last pass that mutates @p graph -- see IRLocationMap.
IRLocationMap computeIRLocations(const IRGraph& graph, const IRPrintOptions& options);

} // namespace nautilus::compiler::ir
