#pragma once

#include "nautilus/options.hpp"
#include <functional>
#include <list>
#include <memory>

namespace nautilus::compiler {

class Executable;
class CompilationBackendRegistry;
class CompilableFunction;

namespace ir {
class IRGraph;
class IRPass;
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
	 * @return Compiled executable
	 */
	[[nodiscard]] virtual std::unique_ptr<Executable> compile(wrapper_function function) const = 0;

	/**
	 * @brief Compile multiple named functions together as one compilation unit.
	 * @param functions List of named compilable functions to trace and compile together
	 * @return Executable containing all compiled functions, accessible by name
	 */
	[[nodiscard]] virtual std::unique_ptr<Executable> compile(std::list<CompilableFunction>& functions) const = 0;

	/**
	 * @brief Get the name of the primary compilation backend.
	 */
	virtual std::string getName() const = 0;

	/**
	 * @brief Get the engine options.
	 */
	virtual const engine::Options& getOptions() const = 0;

	/**
	 * @brief Trace and convert functions to IR without backend code generation.
	 *
	 * Enables callers (e.g. `NautilusModule::compile()` when plugin IR passes
	 * are registered) to inspect or mutate the IR between the frontend and
	 * the backend.
	 *
	 * @param functions Named compilable functions to trace and convert.
	 * @return A shared IR graph that can be lowered via @ref compileIR.
	 */
	[[nodiscard]] virtual std::shared_ptr<ir::IRGraph> compileToIR(std::list<CompilableFunction>& functions) const = 0;

	/**
	 * @brief Lower a pre-built IR graph with the named backend.
	 *
	 * Intended to be called after @ref compileToIR has produced an IR
	 * graph and any plugin IR passes have run.
	 */
	[[nodiscard]] virtual std::unique_ptr<Executable> compileIR(const std::shared_ptr<ir::IRGraph>& ir,
	                                                            const std::string& backendName) const = 0;

	/**
	 * @brief Register a plugin IR pass to be applied during every subsequent
	 * compilation, alongside the built-in passes the compiler runs.
	 *
	 * Ownership of the pass moves into the compiler. Implementations append
	 * it to the same `IRPassManager` pipeline they construct internally, so
	 * plugin and built-in passes share dump/verify/statistics handling.
	 */
	virtual void addIRPass(std::unique_ptr<ir::IRPass> pass) = 0;
};

} // namespace nautilus::compiler
