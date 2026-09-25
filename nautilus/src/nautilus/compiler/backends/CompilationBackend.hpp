#pragma once
#include "nautilus/common/ExceptionPropagation.hpp"
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

	[[nodiscard]] virtual ExceptionPropagationMode getExceptionPropagationMode() const {
		return ExceptionPropagationMode::NativeUnwind;
	}

	/**
	 * @brief Whether this backend gains anything from the Nautilus-IR
	 * optimization group.
	 *
	 * The group is the fixed-point set CompilationPipeline::compileToIR runs
	 * between IR generation and the terminal passes: constant folding,
	 * algebraic simplification, constant-branch folding, empty-block
	 * elimination, block merging, dead-code elimination, block-argument
	 * pruning, plus the function-attribute inference that feeds them. The
	 * terminal passes (no-throw inference, exception-region preparation, the
	 * debug location map) are not part of it and always run.
	 *
	 * A backend that executes or lowers the IR as it is (the interpreters,
	 * the direct-to-machine-code lowerings) returns true: every operation
	 * the group removes is one it would otherwise dispatch or emit. A
	 * backend that hands the IR to an optimizing compiler of its own returns
	 * false: that compiler performs each of these transformations itself, so
	 * running them on the Nautilus IR first spends compile time and changes
	 * nothing about the code that comes out. The pipeline skips the group
	 * when every backend that will consume a graph says false, unless
	 * `ir.forceOptimizationPasses` is set.
	 */
	[[nodiscard]] virtual bool benefitsFromIROptimizationPasses() const {
		return true;
	}

	virtual ~CompilationBackend();
};

class CompilationBackendRegistry {
public:
	static CompilationBackendRegistry* getInstance();
	void registerBackend(const std::string& name, std::unique_ptr<CompilationBackend> backend);
	const CompilationBackend* getBackend(const std::string& name) const;
	/// Returns true if a backend with the given name is registered.
	bool hasBackend(const std::string& name) const;
	/// Returns the name of the first available backend, or empty string if none.
	std::string getDefaultBackendName() const;

private:
	CompilationBackendRegistry();
	std::map<std::string, std::unique_ptr<CompilationBackend>> items =
	    std::map<std::string, std::unique_ptr<CompilationBackend>>();
};

} // namespace nautilus::compiler
