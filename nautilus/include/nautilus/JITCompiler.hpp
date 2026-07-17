#pragma once

#include "nautilus/options.hpp"
#include <functional>
#include <list>
#include <memory>

namespace nautilus::engine::details {
struct ModuleState;
}

namespace nautilus::compiler {

class Executable;
class CompilationBackendRegistry;
class CompilableFunction;

namespace ir {
class IRGraph;
}

using CompilationUnitID = std::string;

/**
 * @brief Abstract interface for JIT compilers in Nautilus.
 *
 * Implementations provide different compilation strategies (single-tier, tiered, etc.)
 * while presenting a uniform interface to the engine and module system.
 */
class JITCompiler {
public:
	using wrapper_function = std::function<void()>;

	virtual ~JITCompiler() = default;

	/**
	 * @brief Compile a single wrapper function.
	 * @param function The traced wrapper function to compile
	 * @param moduleOptions Per-module options for this compilation (overrides
	 *        of the engine-wide defaults). Defaults to empty.
	 * @return Compiled executable
	 */
	[[nodiscard]] virtual std::unique_ptr<Executable>
	compile(wrapper_function function, const engine::ModuleOptions& moduleOptions = {}) const = 0;

	/**
	 * @brief Compile multiple named functions together as one compilation unit.
	 * @param functions List of named compilable functions to trace and compile together
	 * @param moduleOptions Per-module options for this compilation (overrides
	 *        of the engine-wide defaults). Defaults to empty.
	 * @return Executable containing all compiled functions, accessible by name
	 */
	[[nodiscard]] virtual std::unique_ptr<Executable>
	compile(std::list<CompilableFunction>& functions, const engine::ModuleOptions& moduleOptions = {}) const = 0;

	/**
	 * @brief Compile @p functions and publish the result into @p state.
	 *
	 * Sets @c state->executable to the produced executable. Compilers that
	 * promote in the background (tiered) additionally start an asynchronous
	 * promotion targeting @p state. Passing the module state lets each compile
	 * carry its own IR to its own promotion, so concurrent compile() calls on a
	 * shared engine never share compiler-level state.
	 *
	 * @param functions List of named compilable functions to trace and compile.
	 * @param moduleOptions Per-module options for this compilation.
	 * @param state Module state to populate (and, for tiered compilers, promote).
	 */
	virtual void compileModule(std::list<CompilableFunction>& functions, const engine::ModuleOptions& moduleOptions,
	                           std::shared_ptr<engine::details::ModuleState> state) const = 0;

	/**
	 * @brief Trace @p functions and return their IR in the portable text
	 * format instead of compiling a backend executable.
	 *
	 * The returned text contains everything needed to reconstruct the IR in
	 * another process (see IRSerializationUtil.hpp) and can later be compiled
	 * directly — skipping tracing — via @ref compileIRModule or the
	 * engine-level `NautilusEngine::loadModuleFromIR`.
	 *
	 * @param functions List of named compilable functions to trace.
	 * @param moduleOptions Per-module options for this compilation.
	 * @return Serialized Nautilus IR text.
	 */
	[[nodiscard]] virtual std::string compileToSerializedIR(std::list<CompilableFunction>& functions,
	                                                        const engine::ModuleOptions& moduleOptions) const = 0;

	/**
	 * @brief Compile a pre-built IR graph and publish the result into
	 * @p state, skipping the tracing frontend entirely.
	 *
	 * The counterpart of @ref compileModule for IR that was loaded from a
	 * file (or otherwise constructed) rather than traced. Tiered compilers
	 * compile tier 0 synchronously and promote to tier 1 in the background,
	 * exactly like the traced path; since a loaded module has no original
	 * callables to interpret, an interpreter tier 0 falls back to a
	 * synchronous tier-1 compile.
	 *
	 * @param ir The IR graph to compile.
	 * @param moduleOptions Per-module options for this compilation.
	 * @param state Module state to populate (and, for tiered compilers, promote).
	 */
	virtual void compileIRModule(std::shared_ptr<ir::IRGraph> ir, const engine::ModuleOptions& moduleOptions,
	                             std::shared_ptr<engine::details::ModuleState> state) const = 0;

	/**
	 * @brief Get the name of the primary compilation backend.
	 */
	virtual std::string getName() const = 0;

	/**
	 * @brief Get the engine options.
	 */
	virtual const engine::Options& getOptions() const = 0;
};

} // namespace nautilus::compiler
