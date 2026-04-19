#pragma once
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/options.hpp"
#include <map>
#include <memory>

namespace nautilus::compiler {
class Executable;
class CompilationStatistics;
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
	 *
	 * @param ir          IR graph to lower and compile.
	 * @param dumpHandler Handler for backend-specific dump artefacts.
	 * @param options     Engine options.
	 * @param statistics  Optional statistics sink; may be @c nullptr. When
	 *                    non-null, backends record per-phase timings and
	 *                    backend-specific metrics (e.g. code size) under
	 *                    backend-scoped keys.
	 * @return std::unique_ptr<Executable>
	 */
	virtual std::unique_ptr<Executable> compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler,
	                                            const engine::Options& options,
	                                            CompilationStatistics* statistics = nullptr) const = 0;

	virtual ~CompilationBackend();
};

class CompilationBackendRegistry {
public:
	static CompilationBackendRegistry* getInstance();
	void registerBackend(const std::string& name, std::unique_ptr<CompilationBackend> backend);
	const CompilationBackend* getBackend(const std::string& name) const;
	/// Returns the name of the first available backend, or empty string if none.
	std::string getDefaultBackendName() const;

private:
	CompilationBackendRegistry();
	std::map<std::string, std::unique_ptr<CompilationBackend>> items =
	    std::map<std::string, std::unique_ptr<CompilationBackend>>();
};

} // namespace nautilus::compiler
