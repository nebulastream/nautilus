
#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include <string>

namespace nautilus::compiler::ir {

/**
 * @brief Abstract base for a pass that transforms or analyses an `IRGraph`.
 *
 * Passes are registered with an `IRPassManager` and executed in
 * registration order. Implementations must be idempotent and composable:
 * running the same pass twice or reordering passes in a well-formed
 * pipeline must not break correctness.
 *
 * Passes receive a mutable `IRGraph&`. Analysis-only passes should not
 * mutate the graph; there is currently no mechanical enforcement of that,
 * but the verifier will catch any invariant violations.
 */
class IRPass {
public:
	virtual ~IRPass() = default;

	/// Runs the transformation on @p ir. Called once per iteration of the
	/// `IRPassManager` group this pass belongs to (see
	/// `IRPassManager::addFixedPointGroup`); a plain `addPass` group runs
	/// its single pass exactly once. Returns `true` iff the pass changed
	/// the graph -- the pass manager uses this to skip `dumpAfterEachPass`
	/// on a no-op run and to decide whether a fixed-point group has
	/// converged (a round where every pass in the group returns `false`
	/// stops the group early, before `maxIterations` is reached).
	virtual bool apply(IRGraph& ir) = 0;

	/// A short identifier used for logging and for the per-pass IR-dump
	/// stage name (`after_<name>`). Lower-camel-case preferred.
	virtual std::string getName() const = 0;
};

} // namespace nautilus::compiler::ir
