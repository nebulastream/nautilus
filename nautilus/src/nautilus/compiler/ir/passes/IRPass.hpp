
#pragma once

#include <string>

namespace nautilus::compiler::ir {

class IRGraph;

/**
 * @brief Abstract base for a pass that transforms or analyses an `IRGraph`.
 *
 * Passes are registered with an `IRPassManager` and executed in registration
 * order. Implementations must be idempotent and composable: running the same
 * pass twice or reordering passes in a well-formed pipeline must not break
 * correctness.
 *
 * The same interface is used both for built-in compiler passes (constant
 * folding, empty-block elimination, ...) and for user-supplied plugin passes
 * registered via `NautilusModule::addIRPass()`. The friendly alias
 * `nautilus::IRPass` is provided below for plugin code.
 *
 * Plugin passes that produce plugin-owned state (e.g. a profiler) should hold
 * that state via `std::shared_ptr` and hand a copy of the handle to the user
 * via their plugin-specific entry point, so the state outlives both the
 * `NautilusModule` and the pass.
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

namespace nautilus {

/// Plugin-facing alias for the compiler's IR pass interface.
using IRPass = compiler::ir::IRPass;

} // namespace nautilus
