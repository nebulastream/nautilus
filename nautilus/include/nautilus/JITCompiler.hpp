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
class IJITCompiler {
public:
	using wrapper_function = std::function<void()>;

	virtual ~IJITCompiler() = default;

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

/**
 * @brief Standard single-tier JIT compiler implementation.
 *
 * Compiles functions using a single backend (selected via options).
 * This is the original Nautilus compilation strategy.
 */
class JITCompiler : public IJITCompiler {
public:
	JITCompiler();
	JITCompiler(engine::Options options);
	~JITCompiler() override;

	[[nodiscard]] std::unique_ptr<Executable> compile(wrapper_function function) const override;
	[[nodiscard]] std::unique_ptr<Executable> compile(std::list<CompilableFunction>& functions) const override;

	std::string getName() const override;
	const engine::Options& getOptions() const override {
		return options;
	}

	/**
	 * @brief Trace and convert functions to IR without backend compilation.
	 * @return Shared IR graph that can be compiled by any backend
	 */
	[[nodiscard]] std::shared_ptr<ir::IRGraph> compileToIR(std::list<CompilableFunction>& functions) const;

	/**
	 * @brief Compile a pre-built IR graph with a specific backend.
	 * @param ir The IR graph
	 * @param backendName Which backend to use
	 * @return Compiled executable
	 */
	[[nodiscard]] std::unique_ptr<Executable> compileIR(const std::shared_ptr<ir::IRGraph>& ir,
	                                                    const std::string& backendName) const;

private:
	const engine::Options options;
	const CompilationBackendRegistry* backends;
};

} // namespace nautilus::compiler
