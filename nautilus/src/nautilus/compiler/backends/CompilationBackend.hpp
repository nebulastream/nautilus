#pragma once
#include "nautilus/common/ExceptionPropagation.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/options.hpp"
#include <cstdint>
#include <map>
#include <memory>

namespace nautilus::compiler {
class Executable;
class CompilationStatistics;
namespace ir {
class IRGraph;
}

/**
 * @brief How much of the IR optimization pipeline runs on a graph before a
 * backend compiles it.
 *
 * The IR passes exist for the backends that execute the IR as it is -- the
 * interpreters and the assembler backend -- where every operation and block
 * argument removed is a dispatch or a register saved at run time. A backend
 * with an optimizing pipeline of its own (MLIR, through LLVM) performs the
 * same folding, simplification and dead-code removal itself, and running
 * those passes first only costs compile time. Block arguments are the
 * exception: LLVM turns them into phis and pays for every one the trace
 * left behind (measured on the #492 repro, MLIR compilation of a graph with
 * unpruned arguments was 7-18% slower end to end than with them pruned, while
 * the other passes made no difference to it), so a self-optimizing backend
 * still wants them pruned.
 */
enum class IROptimizationLevel : uint8_t {
	/// Nothing beyond the analyses every backend needs (`ir.runOptimizationPasses=false`).
	None,
	/// Block-argument pruning only.
	ArgumentPruning,
	/// Every optimization pass.
	Full,
};

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
	 * @brief How much of the IR optimization pipeline this backend wants run
	 * on a graph before it compiles it. See `IROptimizationLevel`.
	 *
	 * The pipeline runs the highest level any backend that will compile the
	 * graph asks for; the exception-handling analyses every backend lowers
	 * landing pads from run at every level.
	 */
	[[nodiscard]] virtual IROptimizationLevel irOptimizationLevel() const {
		return IROptimizationLevel::Full;
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
