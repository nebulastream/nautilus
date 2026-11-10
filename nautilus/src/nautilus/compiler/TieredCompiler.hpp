#pragma once

#include "CompilationPipeline.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/JITCompiler.hpp"
#include <chrono>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>

namespace nautilus::engine::details {
struct ModuleState;
}

namespace nautilus::compiler {
class PromotionJob;
class PromotionService;
} // namespace nautilus::compiler

namespace nautilus::engine {

/// Sentinel tier-0 backend name: skip the synchronous tier-0 compile and run
/// the module interpreted (direct invocation of the original callable) until
/// the background tier-1 compilation swaps in its executable.
inline constexpr auto INTERPRETER_BACKEND = "interpreter";

/// When a module is promoted to tier 1 (`engine.tiered.promotion`, 11-tiered-compilation.md §4.4).
enum class PromotionPolicy {
	/// Queued right after conversion, before the tier-0 code generation.
	eager,
	/// Only on `CompiledModule::promote()`.
	manual,
};

/**
 * @brief Configuration for a single compilation tier.
 */
struct TierConfig {
	std::string backend; // backend name ("tbc", "mlir", "cpp", "asmjit"), or
	                     // INTERPRETER_BACKEND for tier 0
};

/**
 * @brief Configuration for multi-tier JIT compilation.
 *
 * Tier 0 is the fast compilation tier used immediately.
 * Tier 1 is the high-performance tier compiled in the background.
 *
 * When constructed from options without explicit tier settings, tier 0 is
 * selected by availability in the order: asmjit, tbc, interpreter. The
 * interpreter tier runs the module uncompiled until tier 1 is promoted.
 */
struct TieredCompilationConfig {
	TierConfig tier0 {"tbc"};  // low-latency tier (compiled synchronously)
	TierConfig tier1 {"mlir"}; // high-performance tier (compiled in background)

	/// When true (default) the compiler runs two-tier: a fast tier-0 compile is
	/// returned immediately and the high-performance tier-1 backend is compiled
	/// in the background. When false the compiler runs single-tier: it compiles
	/// directly with the high-performance @ref tier1 backend and performs no
	/// tier-0 compile and no background promotion.
	bool backgroundPromotion {true};

	/// When a two-tier module is promoted. The options constructor reads it from `engine.tiered.promotion`.
	PromotionPolicy policy {PromotionPolicy::eager};
};

} // namespace nautilus::engine

namespace nautilus::compiler {

/**
 * @brief Multi-tier JIT compiler implementation.
 *
 * Compiles functions first with a fast backend (tier 0) for immediate execution,
 * then promotes to a high-performance backend (tier 1) in the background, on the
 * workers of the compiler's @ref PromotionService (one per compiler, hence one per
 * engine).
 *
 * This is the only JITCompiler strategy. Single-tier compilation is a
 * configuration of it: setting `engine.backend=<x>` compiles synchronously
 * with exactly that backend (tier1=<x>, no tier-0 compile, no promotion),
 * and `engine.tiered.backgroundPromotion=false` compiles single-tier with
 * the configured tier-1 backend.
 *
 * A promotion job holds the module state weakly and, when it completes, swaps the
 * executable and increments the version counter under the module's exclusive lock.
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
	 * @brief Compile tier-0 into @p state and queue the background tier-1 promotion.
	 *
	 * The IR produced for tier-0 is handed directly to the promotion for this
	 * module -- no compiler-level cache is shared between calls, so concurrent
	 * compileModule() invocations on a single shared compiler are race-free.
	 * With the policy `eager` the job is queued right after conversion, before
	 * the tier-0 code generation (11-tiered-compilation.md §4.5); with `manual`
	 * it waits for `CompiledModule::promote()`.
	 */
	void compileModule(std::list<CompilableFunction>& functions, const engine::ModuleOptions& moduleOptions,
	                   std::shared_ptr<engine::details::ModuleState> state) const override;

	std::string getName() const override;
	const engine::Options& getOptions() const override;

	/// Blocks until no promotion of this compiler is queued or running. No lock is held while waiting.
	bool waitForPromotions(std::chrono::milliseconds timeout = std::chrono::milliseconds::max()) const override;

	/// True if no promotion is queued or running.
	bool allPromotionsComplete() const;

	/// The promotion workers created so far: at most `engine.tiered.promotionThreads`, however many modules were
	/// compiled.
	size_t promotionWorkerCount() const;

private:
	/// Called with the converted graph, before the tier backend compiles it.
	using GraphReady = std::function<void(const std::shared_ptr<ir::IRGraph>&)>;

	/**
	 * @brief Trace and compile @p functions with the named @p backend.
	 *
	 * @p tierLabel is recorded into the compilation statistics ("tier0"/"tier1").
	 * @p optimization is how much of the IR optimization pipeline runs before
	 * the graph is compiled; the caller decides it from every backend that
	 * will compile this graph, the promotion target included, since the graph
	 * is traced once and shared between the tiers.
	 * @p graphReady, if set, is called between conversion and the backend: the
	 * point where an eager promotion starts (§4.5).
	 */
	[[nodiscard]] std::unique_ptr<Executable> compileTier(std::list<CompilableFunction>& functions,
	                                                      const engine::ModuleOptions& moduleOptions,
	                                                      const std::string& backend, const std::string& tierLabel,
	                                                      IROptimizationLevel optimization,
	                                                      const GraphReady& graphReady = {}) const;

	/// The promotion job of @p state: compiles @p ir with the tier-1 backend on a worker. Installs the job as the
	/// state's promotion control and, under the policy `manual`, parks it.
	[[nodiscard]] std::shared_ptr<PromotionJob>
	createPromotion(const std::shared_ptr<engine::details::ModuleState>& state, std::shared_ptr<ir::IRGraph> ir,
	                const engine::ModuleOptions& options) const;

	/// Whether tier 1 can compile @p ir while tier 0 still does: both backends read the immutable NIR module
	/// and neither builds the 0.3 IR into the graph.
	[[nodiscard]] bool canStartEarly(const ir::IRGraph& ir) const;

	CompilationPipeline pipeline_;
	engine::TieredCompilationConfig config_;
	/// Created with the compiler, starts its first worker with the first job. Shared with the promotion controls of
	/// the modules, weakly, so that `promote()` after the engine is gone is a no-op.
	std::shared_ptr<PromotionService> service_;
};

} // namespace nautilus::compiler
