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
};

} // namespace nautilus::compiler
