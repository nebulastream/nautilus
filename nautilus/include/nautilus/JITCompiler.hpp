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

class JITCompiler {
public:
	using wrapper_function = std::function<void()>;

	JITCompiler();
	JITCompiler(engine::Options options);

	[[nodiscard]] std::unique_ptr<Executable> compile(wrapper_function function) const;

	/**
	 * @brief Compiles multiple named functions together as one compilation unit.
	 * @param functions List of named compilable functions to trace and compile together
	 * @return Executable containing all compiled functions, accessible by name
	 */
	[[nodiscard]] std::unique_ptr<Executable> compile(std::list<CompilableFunction>& functions) const;

	/**
	 * @brief Trace and convert functions to IR without backend compilation.
	 * @param functions List of named compilable functions to trace
	 * @return Shared IR graph that can be compiled by any backend
	 */
	[[nodiscard]] std::shared_ptr<ir::IRGraph> compileToIR(std::list<CompilableFunction>& functions) const;

	/**
	 * @brief Compile a pre-built IR graph with a specific backend.
	 * @param ir The IR graph (can be shared between tiers)
	 * @param backendName Which backend to use for compilation
	 * @return Compiled executable
	 */
	[[nodiscard]] std::unique_ptr<Executable> compileIR(const std::shared_ptr<ir::IRGraph>& ir,
	                                                    const std::string& backendName) const;

	virtual ~JITCompiler();

	std::string getName() const;

	const engine::Options& getOptions() const {
		return options;
	}

private:
	const engine::Options options;
	const CompilationBackendRegistry* backends;
};

} // namespace nautilus::compiler
