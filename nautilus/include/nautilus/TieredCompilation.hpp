#pragma once

#include <string>

namespace nautilus::engine {

/**
 * @brief Configuration for a single compilation tier.
 */
struct TierConfig {
	std::string backend; // backend name ("bc", "mlir", "cpp", "asmjit")
	int threshold;       // invocation count before promoting to next tier (0 = disabled)
};

/**
 * @brief Configuration for multi-tier JIT compilation.
 *
 * Tier 0 is the fast compilation tier used immediately when compileTiered() is called.
 * Tier 1 is the high-performance tier compiled in the background after the threshold
 * number of invocations has been reached.
 */
struct TieredCompilationConfig {
	TierConfig tier0 {"bc", 0};       // low-latency tier (compiled immediately)
	TierConfig tier1 {"mlir", 1000};  // high-performance tier (background promotion)
};

} // namespace nautilus::engine
