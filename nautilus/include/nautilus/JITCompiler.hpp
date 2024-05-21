#pragma once

#include "nautilus/options.hpp"
#include <functional>
#include <memory>

namespace nautilus::compiler {

class Executable;
class CompilationBackendRegistry;
class JITCompiler {
public:
	using wrapper_function = std::function<void()>;

	JITCompiler();
	JITCompiler(engine::Options  options);

	[[nodiscard]] std::unique_ptr<Executable> compile(wrapper_function function) const;

	virtual ~JITCompiler();

private:
	const engine::Options options;
	std::unique_ptr<CompilationBackendRegistry> backends;
};


} // namespace nautilus::compiler
