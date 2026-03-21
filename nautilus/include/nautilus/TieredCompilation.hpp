#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/JITCompiler.hpp"
#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>

namespace nautilus::engine::details {
struct ModuleState;
}

namespace nautilus::engine {

/**
 * @brief Configuration for a single compilation tier.
 */
struct TierConfig {
	std::string backend; // backend name ("bc", "mlir", "cpp", "asmjit")
};

/**
 * @brief Configuration for multi-tier JIT compilation.
 *
 * Tier 0 is the fast compilation tier used immediately.
 * Tier 1 is the high-performance tier compiled in the background.
 */
struct TieredCompilationConfig {
	TierConfig tier0 {"bc"};   // low-latency tier (compiled synchronously)
	TierConfig tier1 {"mlir"}; // high-performance tier (compiled in background)
};

} // namespace nautilus::engine

namespace nautilus::compiler {

/**
 * @brief Multi-tier JIT compiler implementation.
 *
 * Compiles functions first with a fast backend (tier 0) for immediate execution,
 * then promotes to a high-performance backend (tier 1) in the background.
 *
 * The compiler keeps a reference to the module state and directly swaps the
 * executable and increments the version counter when tier-1 compilation completes.
 * This eliminates the need for swap callbacks on executables.
 */
class TieredJITCompiler : public IJITCompiler {
public:
	TieredJITCompiler(engine::Options options, engine::TieredCompilationConfig config);
	~TieredJITCompiler() override;

	[[nodiscard]] std::unique_ptr<Executable> compile(wrapper_function function) const override;
	[[nodiscard]] std::unique_ptr<Executable> compile(std::list<CompilableFunction>& functions) const override;

	std::string getName() const override;
	const engine::Options& getOptions() const override;

	/**
	 * @brief Start background tier-1 promotion targeting the given module state.
	 *
	 * Uses the IR cached from the most recent compile() call to compile with the
	 * tier-1 backend in a background thread. When compilation completes, the
	 * executable in the module state is swapped and the version counter is
	 * incremented, causing all ModuleFunction handles to re-resolve.
	 *
	 * @param state Weak reference to the module state to promote
	 */
	void promoteAsync(std::weak_ptr<engine::details::ModuleState> state) const;

	/**
	 * @brief Block until all pending background promotions have completed.
	 */
	void waitForPendingPromotions() const;

	/**
	 * @brief Returns true if all background promotions have completed.
	 */
	bool allPromotionsComplete() const;

private:
	JITCompiler baseCompiler_;
	engine::TieredCompilationConfig config_;

	// Mutable because compile() is const but needs to cache IR for promotion
	mutable std::shared_ptr<ir::IRGraph> lastCachedIR_;
	mutable std::vector<std::thread> promotionThreads_;
	mutable std::mutex threadsMutex_;
	mutable std::atomic<int> pendingPromotions_ {0};
};

} // namespace nautilus::compiler
