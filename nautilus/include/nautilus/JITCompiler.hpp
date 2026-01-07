#pragma once

#include "nautilus/options.hpp"
#include <functional>
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

	virtual ~JITCompiler();

	std::string getName() const {
		return options.getOptionOrDefault<std::string>("engine.backend", "mlir");
	}

private:
	const engine::Options options;
	const CompilationBackendRegistry* backends;
};

} // namespace nautilus::compiler
