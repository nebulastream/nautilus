
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

	/// Runs the transformation on @p ir. Called exactly once per
	/// `IRPassManager::run()` invocation, in registration order.
	virtual void apply(IRGraph& ir) = 0;

	/// A short identifier used for logging and for the per-pass IR-dump
	/// stage name (`after_<name>`). Lower-camel-case preferred.
	virtual std::string getName() const = 0;
};

} // namespace nautilus::compiler::ir
