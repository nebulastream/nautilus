#pragma once

#include "LegacyCompiler.hpp"
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
class TieredJITCompiler : public JITCompiler {
public:
	/// Construct a tiered compiler that draws tier-0 trace arenas from
	/// @p traceArenaPool and IR-graph arenas from @p irArenaPool.  Both pools
	/// are internally synchronized and must outlive the compiler.
	TieredJITCompiler(engine::Options options, common::ArenaPool& traceArenaPool, common::ArenaPool& irArenaPool);
	TieredJITCompiler(engine::Options options, engine::TieredCompilationConfig config,
	                  common::ArenaPool& traceArenaPool, common::ArenaPool& irArenaPool);
	~TieredJITCompiler() override;

	[[nodiscard]] std::unique_ptr<Executable> compile(wrapper_function function,
	                                                  const engine::ModuleOptions& moduleOptions = {}) const override;
	[[nodiscard]] std::unique_ptr<Executable> compile(std::list<CompilableFunction>& functions,
	                                                  const engine::ModuleOptions& moduleOptions = {}) const override;

	/**
	 * @brief Compile tier-0 into @p state and start background tier-1 promotion.
	 *
	 * The IR produced for tier-0 is handed directly to the promotion for this
	 * module — no compiler-level cache is shared between calls, so concurrent
	 * compileModule() invocations on a single shared compiler are race-free.
	 */
	void compileModule(std::list<CompilableFunction>& functions, const engine::ModuleOptions& moduleOptions,
	                   std::shared_ptr<engine::details::ModuleState> state) const override;

	std::string getName() const override;
	const engine::Options& getOptions() const override;

	/**
	 * @brief Block until all pending background promotions have completed.
	 */
	void waitForPendingPromotions() const;

	/**
	 * @brief Returns true if all background promotions have completed.
	 */
	bool allPromotionsComplete() const;

private:
	/**
	 * @brief Trace and compile the tier-0 executable, returning the IR via
	 * @p outIR so callers can hand it to background promotion.
	 */
	[[nodiscard]] std::unique_ptr<Executable> compileTier0(std::list<CompilableFunction>& functions,
	                                                       const engine::ModuleOptions& moduleOptions,
	                                                       std::shared_ptr<ir::IRGraph>& outIR) const;

	/**
	 * @brief Start background tier-1 promotion of @p ir targeting @p state.
	 *
	 * Compiles @p ir with the tier-1 backend on a background thread using
	 * @p options.  When done, the executable in the module state is swapped and
	 * the version counter is incremented, causing all ModuleFunction handles to
	 * re-resolve.  The IR and options are owned per call, so nothing is shared
	 * between concurrent compiles.
	 */
	void promoteAsync(std::weak_ptr<engine::details::ModuleState> state, std::shared_ptr<ir::IRGraph> ir,
	                  engine::ModuleOptions options) const;

	LegacyCompiler baseCompiler_;
	engine::TieredCompilationConfig config_;

	mutable std::vector<std::thread> promotionThreads_;
	mutable std::mutex threadsMutex_;
	mutable std::atomic<int> pendingPromotions_ {0};
};

} // namespace nautilus::compiler
