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
 * @brief Multi-tier JIT compiler using tiered compilation to balance startup latency with peak performance.
 *
 * Tier 1 (e.g., "bc") provides fast compilation; tier 2 (e.g., "mlir") provides optimized code
 * after a configurable invocation threshold.
 */
class MultiTierJitCompiler {
public:
	using wrapper_function = std::function<void()>;

	MultiTierJitCompiler();
	explicit MultiTierJitCompiler(engine::Options options);
	virtual ~MultiTierJitCompiler();

	/**
	 * @brief Compile a function using the multi-tier strategy.
	 *
	 * Immediately compiles with the tier 1 backend and returns an executable that
	 * tracks invocations. After reaching the threshold, tier 2 compilation starts
	 * asynchronously and the executable switches when ready.
	 */
	[[nodiscard]] std::unique_ptr<Executable> compile(wrapper_function function) const;

	std::string getTier1BackendName() const {
		return options.getOptionOrDefault<std::string>("engine.multiTier.tier1Backend", "bc");
	}

	std::string getTier2BackendName() const {
		return options.getOptionOrDefault<std::string>("engine.multiTier.tier2Backend", "mlir");
	}

	uint64_t getTier2Threshold() const {
		return static_cast<uint64_t>(options.getOptionOrDefault<int>("engine.multiTier.tier2Threshold", 100));
	}

	std::string getName() const;

private:
	const engine::Options options;
	const CompilationBackendRegistry* backends_;
};

} // namespace nautilus::compiler
