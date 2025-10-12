#pragma once

#include "nautilus/options.hpp"
#include <functional>
#include <memory>

namespace nautilus::compiler {
namespace ir {
class IRGraph;
}
class Executable;
class CompilationBackendRegistry;

using CompilationUnitID = std::string;

class JITCompiler {
public:
	using wrapper_function = std::function<void()>;

	JITCompiler();
	JITCompiler(engine::Options options);

	[[nodiscard]] std::unique_ptr<Executable> compile(wrapper_function function) const;
	[[nodiscard]] std::unique_ptr<Executable> compile(std::shared_ptr<ir::IRGraph>& irGraph) const;

	virtual ~JITCompiler();

private:
	const engine::Options options;
	const CompilationBackendRegistry* backends;
};

} // namespace nautilus::compiler
