#pragma once

#include "nautilus/JITCompiler.hpp"
#include "nautilus/options.hpp"
#include <functional>
#include <memory>
#include <string>

namespace nautilus::compiler {

class Executable;
class CompilationBackendRegistry;

/**
 * @brief Multi-tier JIT compiler that uses tiered compilation strategy.
 *
 * This compiler first executes code with a low-latency backend (e.g., bytecode interpreter)
 * and then transitions to a high-performance backend (e.g., MLIR) after a configurable
 * number of invocations. This approach balances startup time with peak performance.
 *
 * Example configuration:
 * - Tier 1: "bc" (bytecode interpreter) - low latency, fast compilation
 * - Tier 2: "mlir" - high performance, slower compilation
 * - Threshold: 100 invocations before tier 2 compilation starts
 */
class MultiTierJitCompiler {
public:
	using wrapper_function = std::function<void()>;

	/**
	 * @brief Construct a multi-tier JIT compiler with default options.
	 *
	 * Default configuration:
	 * - Tier 1 backend: "bc" (bytecode)
	 * - Tier 2 backend: "mlir"
	 * - Tier 2 threshold: 100 invocations
	 */
	MultiTierJitCompiler();

	/**
	 * @brief Construct a multi-tier JIT compiler with custom options.
	 *
	 * Supported options:
	 * - "engine.tier1.backend": Backend for tier 1 (default: "bc")
	 * - "engine.tier2.backend": Backend for tier 2 (default: "mlir")
	 * - "engine.tier2.threshold": Number of invocations before tier 2 compilation (default: 100)
	 *
	 * @param options Configuration options
	 */
	explicit MultiTierJitCompiler(engine::Options options);

	/**
	 * @brief Compile a function using the multi-tier strategy.
	 *
	 * This method:
	 * 1. Immediately compiles the function with the tier 1 backend
	 * 2. Returns an executable that tracks invocations
	 * 3. After reaching the threshold, compiles with tier 2 backend asynchronously
	 * 4. Automatically switches to tier 2 when ready
	 *
	 * The returned executable provides a stable function pointer that remains valid
	 * throughout all tier transitions.
	 *
	 * @param function The wrapper function to compile
	 * @return std::unique_ptr<Executable> with stable function pointer
	 */
	[[nodiscard]] std::unique_ptr<Executable> compile(wrapper_function function) const;

	virtual ~MultiTierJitCompiler();

	/**
	 * @brief Get the name of the tier 1 backend.
	 * @return Backend name (e.g., "bc", "cpp", "asmjit")
	 */
	std::string getTier1BackendName() const {
		return options.getOptionOrDefault<std::string>("engine.tier1.backend", "bc");
	}

	/**
	 * @brief Get the name of the tier 2 backend.
	 * @return Backend name (e.g., "mlir", "cpp")
	 */
	std::string getTier2BackendName() const {
		return options.getOptionOrDefault<std::string>("engine.tier2.backend", "mlir");
	}

	/**
	 * @brief Get the tier 2 compilation threshold.
	 * @return Number of invocations before tier 2 compilation starts
	 */
	uint64_t getTier2Threshold() const {
		return static_cast<uint64_t>(options.getOptionOrDefault<int>("engine.tier2.threshold", 100));
	}

	/**
	 * @brief Get a descriptive name for this compiler.
	 * @return String like "multi-tier(bc->mlir)"
	 */
	std::string getName() const;

private:
	const engine::Options options;
	const CompilationBackendRegistry* backends;
};

} // namespace nautilus::compiler
