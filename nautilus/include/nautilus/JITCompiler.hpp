#pragma once

#include "nautilus/options.hpp"
#include <functional>
#include <list>
#include <memory>

namespace nautilus::compiler {

class Executable;
class CompilationBackendRegistry;
class CompilableFunction;

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

	virtual ~JITCompiler();

	std::string getName() const {
		return options.getOptionOrDefault<std::string>("engine.backend", "mlir");
	}

private:
	const engine::Options options;
	const CompilationBackendRegistry* backends;
};

} // namespace nautilus::compiler
