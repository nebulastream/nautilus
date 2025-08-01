#pragma once
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/options.hpp"
#include <map>
#include <memory>

namespace nautilus::compiler {
class Executable;
namespace ir {
class IRGraph;
}

/**
 * @brief The compilation backend, compiles a ir graph to an executable.
 */
class CompilationBackend {
public:
	/**
	 * @brief Compiles ir graph to executable.
	 * @return std::unique_ptr<Executable>
	 */
	virtual std::unique_ptr<Executable> compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler,
	                                            const engine::Options& options) const = 0;

	virtual ~CompilationBackend();
};

class CompilationBackendRegistry {
public:
	static const CompilationBackendRegistry* getInstance();
	const CompilationBackend* getBackend(const std::string& name) const;

private:
	CompilationBackendRegistry();
	std::map<std::string, std::unique_ptr<CompilationBackend>> items =
	    std::map<std::string, std::unique_ptr<CompilationBackend>>();
};

} // namespace nautilus::compiler
