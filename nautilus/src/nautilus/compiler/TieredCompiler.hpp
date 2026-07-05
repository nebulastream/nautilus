#pragma once

#include "CompilationPipeline.hpp"
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

/// Sentinel tier-0 backend name: skip the synchronous tier-0 compile and run
/// the module interpreted (direct invocation of the original callable) until
/// the background tier-1 compilation swaps in its executable.
inline constexpr auto INTERPRETER_BACKEND = "interpreter";

/**
 * @brief Configuration for a single compilation tier.
 */
struct TierConfig {
	std::string backend; // backend name ("bc", "mlir", "cpp", "asmjit"), or
	                     // INTERPRETER_BACKEND for tier 0
};

/**
 * @brief Configuration for multi-tier JIT compilation.
 *
 * Tier 0 is the fast compilation tier used immediately.
 * Tier 1 is the high-performance tier compiled in the background.
 *
 * When constructed from options without explicit tier settings, tier 0 is
 * selected by availability in the order: asmjit, bc, interpreter. The
 * interpreter tier runs the module uncompiled until tier 1 is promoted.
 */
struct TieredCompilationConfig {
	TierConfig tier0 {"bc"};   // low-latency tier (compiled synchronously)
	TierConfig tier1 {"mlir"}; // high-performance tier (compiled in background)

	/// When true (default) the compiler runs two-tier: a fast tier-0 compile is
	/// returned immediately and the high-performance tier-1 backend is compiled
	/// in the background. When false the compiler runs single-tier: it compiles
	/// directly with the high-performance @ref tier1 backend and performs no
	/// tier-0 compile and no background promotion.
	bool backgroundPromotion {true};
};

} // namespace nautilus::engine

namespace nautilus::compiler {

/**
 * @brief Multi-tier JIT compiler implementation.
 *
 * Compiles functions first with a fast backend (tier 0) for immediate execution,
 * then promotes to a high-performance backend (tier 1) in the background.
 *
 * This is the only JITCompiler strategy. Single-tier compilation is a
 * configuration of it: setting `engine.backend=<x>` compiles synchronously
 * with exactly that backend (tier1=<x>, no tier-0 compile, no promotion),
 * and `engine.tiered.backgroundPromotion=false` compiles single-tier with
 * the configured tier-1 backend.
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
	 * @brief Trace and compile @p functions with the named @p backend, returning
	 * the IR via @p outIR so callers can hand it to background promotion.
	 *
	 * @p tierLabel is recorded into the compilation statistics ("tier0"/"tier1").
	 */
	[[nodiscard]] std::unique_ptr<Executable> compileTier(std::list<CompilableFunction>& functions,
	                                                      const engine::ModuleOptions& moduleOptions,
	                                                      const std::string& backend, const std::string& tierLabel,
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

	CompilationPipeline pipeline_;
	engine::TieredCompilationConfig config_;

	mutable std::vector<std::thread> promotionThreads_;
	mutable std::mutex threadsMutex_;
	mutable std::atomic<int> pendingPromotions_ {0};
};

} // namespace nautilus::compiler
