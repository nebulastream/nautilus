#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/Module.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/logging.hpp"

#ifdef ENABLE_COMPILER

#include "nautilus/CompilableFunction.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/PromotionService.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <mutex>

namespace nautilus::compiler {

/// Appends @p statistics as one JSON line to the file named by the
/// NAUTILUS_STATISTICS_FILE environment variable, if it is set. This measures
/// an embedding application's compilations without changing how that
/// application configures nautilus.
static void appendStatisticsToFile(const CompilationStatistics& statistics) {
	const char* path = std::getenv("NAUTILUS_STATISTICS_FILE");
	if (path == nullptr || *path == '\0') {
		return;
	}
	static std::mutex fileMutex;
	const std::lock_guard<std::mutex> lock(fileMutex);
	std::ofstream out(path, std::ios::app);
	out << statistics.toJson() << '\n';
	if (!out) {
		log::warn("Could not append compilation statistics to {}", path);
	}
}

namespace {

double millisecondsBetween(std::chrono::steady_clock::time_point from, std::chrono::steady_clock::time_point to) {
	return std::chrono::duration<double, std::milli>(to - from).count();
}

/// The settings of the promotion service in @p options (`engine.tiered.promotionThreads`,
/// `engine.tiered.maxQueuedPromotions`).
PromotionServiceConfig promotionServiceConfig(const engine::Options& options) {
	PromotionServiceConfig config;
	config.threads = defaultPromotionThreads();
	if (options.hasOption("engine.tiered.promotionThreads")) {
		const auto threads = options.getOptionOrDefault("engine.tiered.promotionThreads", 0);
		if (threads < 1) {
			throw RuntimeException("engine.tiered.promotionThreads must be at least 1, not " + std::to_string(threads));
		}
		config.threads = static_cast<std::size_t>(threads);
	}
	if (options.hasOption("engine.tiered.maxQueuedPromotions")) {
		const auto cap = options.getOptionOrDefault("engine.tiered.maxQueuedPromotions", 0);
		if (cap < 0) {
			throw RuntimeException("engine.tiered.maxQueuedPromotions must not be negative, not " +
			                       std::to_string(cap));
		}
		config.maxQueued = static_cast<std::size_t>(cap);
		config.bounded = true;
	}
	return config;
}

/// The module's side of its promotion job: `promote()` queues it, and destroying the module's state cancels it.
class JobControl final : public engine::details::PromotionControl {
public:
	JobControl(std::shared_ptr<PromotionJob> job, std::weak_ptr<PromotionService> service)
	    : job_(std::move(job)), service_(std::move(service)) {
	}
	~JobControl() override {
		job_->cancel();
	}
	bool promote() override {
		const auto service = service_.lock();
		return service != nullptr && service->submit(job_);
	}

private:
	std::shared_ptr<PromotionJob> job_;
	std::weak_ptr<PromotionService> service_;
};

} // namespace

// --- TieredJITCompiler ---

TieredJITCompiler::TieredJITCompiler(engine::Options options, common::ArenaPool& traceArenaPool,
                                     common::ArenaPool& irArenaPool)
    : pipeline_(options, traceArenaPool, irArenaPool) {
	// checked whether or not this engine promotes, so a wrong value is not found only after a configuration change
	const auto serviceConfig = promotionServiceConfig(options);
	// An explicitly selected backend pins single-tier compilation: the engine
	// compiles synchronously with exactly this backend and never promotes.
	// It takes precedence over the tier options.
	auto explicitBackend = options.getOptionOrDefault<std::string>("engine.backend", "");
	if (!explicitBackend.empty()) {
		config_.tier1.backend = explicitBackend;
		config_.backgroundPromotion = false;
		return;
	}

	// Each tier option can be set alone; the other keeps its default (11 §4.7). A backend that is not available fails
	// here, not at the first compile.
	auto* registry = CompilationBackendRegistry::getInstance();
	const auto tier0 = options.getOptionOrDefault<std::string>("engine.tier0.backend", "");
	const auto tier1 = options.getOptionOrDefault<std::string>("engine.tier1.backend", "");
	if (!tier1.empty() && !registry->hasBackend(tier1)) {
		throw RuntimeException("engine.tier1.backend: the backend '" + tier1 + "' is not available in this build");
	}
	if (!tier0.empty() && tier0 != engine::INTERPRETER_BACKEND && !registry->hasBackend(tier0)) {
		throw RuntimeException("engine.tier0.backend: the backend '" + tier0 + "' is not available in this build");
	}

	// The defaults pick from what this build actually registered, so default options keep working in reduced builds.
	config_.tier1.backend = tier1;
	if (tier1.empty()) {
		config_.tier1.backend = registry->hasBackend("mlir") ? "mlir" : registry->getDefaultBackendName();
		if (config_.tier1.backend.empty()) {
			config_.tier1.backend = "mlir";
		}
	}
	// Tier-0 preference order: asmjit, tbc, interpreter. The interpreter tier is always available -- the module runs
	// uncompiled until the background tier-1 promotion swaps in its executable.
	config_.tier0.backend = tier0;
	if (tier0.empty()) {
		if (registry->hasBackend("asmjit")) {
			config_.tier0.backend = "asmjit";
		} else if (registry->hasBackend("tbc")) {
			config_.tier0.backend = "tbc";
		} else {
			config_.tier0.backend = engine::INTERPRETER_BACKEND;
		}
	}
	if (config_.tier0.backend == config_.tier1.backend) {
		// Promoting to the tier-0 backend would be a no-op; compile single-tier instead.
		config_.backgroundPromotion = false;
	}

	const auto promotion = options.getOptionOrDefault<std::string>("engine.tiered.promotion", "");
	if (promotion == "manual") {
		config_.policy = engine::PromotionPolicy::manual;
	} else if (promotion == "none") {
		// `none` is the internal target of the deprecated alias only (11 §4.4)
		throw RuntimeException("engine.tiered.promotion=none is not a policy; use engine.backend=<x> to compile "
		                       "single-tier with one backend");
	} else if (!promotion.empty() && promotion != "eager") {
		throw RuntimeException("engine.tiered.promotion does not accept '" + promotion + "'; use eager or manual");
	}
	// The deprecated alias wins over its target (11 §4.7)
	if (options.hasOption("engine.tiered.backgroundPromotion")) {
		log::warn("The option 'engine.tiered.backgroundPromotion' is deprecated; use 'engine.tiered.promotion', or "
		          "'engine.backend' to compile single-tier.");
		if (!options.getOptionOrDefault("engine.tiered.backgroundPromotion", true)) {
			config_.backgroundPromotion = false;
			if (!promotion.empty()) {
				log::warn("Both 'engine.tiered.backgroundPromotion' and 'engine.tiered.promotion' are set; the "
				          "deprecated alias wins.");
			}
		}
	}
	if (config_.backgroundPromotion) {
		service_ = std::make_shared<PromotionService>(serviceConfig);
	}
}

TieredJITCompiler::TieredJITCompiler(engine::Options options, engine::TieredCompilationConfig config,
                                     common::ArenaPool& traceArenaPool, common::ArenaPool& irArenaPool)
    : pipeline_(options, traceArenaPool, irArenaPool), config_(std::move(config)) {
	const auto serviceConfig = promotionServiceConfig(options);
	if (config_.backgroundPromotion) {
		service_ = std::make_shared<PromotionService>(serviceConfig);
	}
}

TieredJITCompiler::~TieredJITCompiler() {
	// Cancels the queued promotions and waits for the running ones, at most one compile per worker.
	if (service_) {
		service_->shutdown();
	}
}

std::unique_ptr<Executable> TieredJITCompiler::compile(wrapper_function function,
                                                       const engine::ModuleOptions& moduleOptions) const {
	static constexpr auto ROOT_FUNCTION_NAME = "execute";
	auto rootFunction = CompilableFunction(ROOT_FUNCTION_NAME, function);
	std::list<CompilableFunction> functionsToTrace;
	functionsToTrace.push_back(rootFunction);
	return compile(functionsToTrace, moduleOptions);
}

std::unique_ptr<Executable> TieredJITCompiler::compileTier(std::list<CompilableFunction>& functions,
                                                           const engine::ModuleOptions& moduleOptions,
                                                           const std::string& backend, const std::string& tierLabel,
                                                           IROptimizationLevel optimization,
                                                           const GraphReady& graphReady) const {
	// One shared statistics object covers the entire tier compile so the
	// user's CompiledModule::getStatistics() reflects tracing + IR passes +
	// backend work in a single report.
	auto statistics = std::make_shared<CompilationStatistics>();
	const auto compilationStart = std::chrono::steady_clock::now();

	auto ir = pipeline_.compileToIR(functions, moduleOptions, statistics.get(), optimization);
	if (graphReady) {
		graphReady(ir);
	}
	auto executable = pipeline_.compileIR(ir, backend, moduleOptions, statistics.get());

	statistics->recordTimingMs("compilation.totalMs", compilationStart);
	statistics->set("tier", tierLabel);

	if (moduleOptions.getOptionOrDefault("engine.logStatistics", false)) {
		const auto id = statistics->find("compilation.unitId") != nullptr
		                    ? std::get<std::string>(*statistics->find("compilation.unitId"))
		                    : std::string {};
		log::info("\n{}", statistics->formatReport(id, backend));
	}
	appendStatisticsToFile(*statistics);

	executable->setCompilationStatistics(std::static_pointer_cast<const CompilationStatistics>(std::move(statistics)));
	return executable;
}

std::unique_ptr<Executable> TieredJITCompiler::compile(std::list<CompilableFunction>& functions,
                                                       const engine::ModuleOptions& moduleOptions) const {
	if (!config_.backgroundPromotion || config_.tier0.backend == engine::INTERPRETER_BACKEND) {
		// Single-tier: compile directly with the high-performance backend.
		// The interpreter tier-0 also lands here: without a module state
		// there is nothing to run interpreted, so compile tier-1 up front.
		return compileTier(functions, moduleOptions, config_.tier1.backend, "tier1",
		                   pipeline_.irOptimizationLevel({config_.tier1.backend}));
	}
	// Two-tier: return the fast tier-0 executable (no module state to promote
	// into, so tier 0 is the graph's only consumer).
	return compileTier(functions, moduleOptions, config_.tier0.backend, "tier0",
	                   pipeline_.irOptimizationLevel({config_.tier0.backend}));
}

bool TieredJITCompiler::canStartEarly(const ir::IRGraph& ir) const {
	// Tier 1 may read the graph while tier 0 does only if neither builds the 0.3 IR into it: the backends that
	// compile NIR read the immutable module (§4.5); the others lower it in `prepareFor`, which writes the graph.
	if (!CompilationPipeline::carriesOnlyNir(ir)) {
		return false;
	}
	auto* registry = CompilationBackendRegistry::getInstance();
	return registry->getBackend(config_.tier0.backend)->compilesNir() &&
	       registry->getBackend(config_.tier1.backend)->compilesNir();
}

std::shared_ptr<PromotionJob>
TieredJITCompiler::createPromotion(const std::shared_ptr<engine::details::ModuleState>& state,
                                   std::shared_ptr<ir::IRGraph> ir, const engine::ModuleOptions& moduleOptions) const {
	// The unit ID of the tier-1 compile is the tier-0 one plus `_tier1`: the dumps of one module sit side by side
	// and the report links its tier-0 report (11 §4.8).
	auto tier0UnitId = CompilationPipeline::unitIdOf(*ir);
	auto options = moduleOptions;
	// Workers own no thread pool: a promotion lowers one module, and a context with a pool of its own would cost
	// `promotionThreads x hardware_concurrency` threads (11 §4.1).
	options.setOption("mlir.enableMultithreading", false);
	PromotionJob::CompileFunction compile =
	    [pipeline = &pipeline_, backend = config_.tier1.backend, tier0UnitId = std::move(tier0UnitId),
	     ir = std::move(ir), options = std::move(options)](const PromotionRun& run) -> std::unique_ptr<Executable> {
		const auto unitId = tier0UnitId + "_tier1";
		auto statistics = std::make_shared<CompilationStatistics>();
		statistics->set("compilation.unitId", unitId);
		statistics->set("compilation.tier0UnitId", tier0UnitId);
		statistics->set("tier", std::string {"tier1"});
		statistics->set("promotion.queueMs", millisecondsBetween(run.submitted, run.started));

		const auto recordFailure = [&](const std::string& message) {
			statistics->set("promotion.status", std::string {"failed"});
			statistics->set("promotion.error", message);
			appendStatisticsToFile(*statistics);
		};
		std::unique_ptr<Executable> executable;
		try {
			executable = pipeline->compileIR(ir, backend, options, statistics.get(), unitId, &run.cancellation);
		} catch (const std::exception& e) {
			recordFailure(e.what());
			throw;
		} catch (...) {
			recordFailure("an exception that is not derived from std::exception");
			throw;
		}
		if (executable == nullptr) {
			return nullptr; // cancelled at a stage boundary
		}
		const auto finished = std::chrono::steady_clock::now();
		statistics->set("compilation.totalMs", millisecondsBetween(run.started, finished));
		statistics->set("promotion.compileMs", millisecondsBetween(run.started, finished));
		statistics->set("promotion.timeToTier1Ms", millisecondsBetween(run.submitted, finished));
		statistics->set("promotion.status", std::string {"done"});
		if (options.getOptionOrDefault("engine.logStatistics", false)) {
			log::info("\n{}", statistics->formatReport(unitId, backend));
		}
		appendStatisticsToFile(*statistics);
		executable->setCompilationStatistics(
		    std::static_pointer_cast<const CompilationStatistics>(std::move(statistics)));
		return executable;
	};
	auto job = std::make_shared<PromotionJob>(state, std::move(compile));
	state->promotionControl = std::make_unique<JobControl>(job, service_);
	if (config_.policy == engine::PromotionPolicy::manual) {
		service_->park(job);
	}
	return job;
}

void TieredJITCompiler::compileModule(std::list<CompilableFunction>& functions,
                                      const engine::ModuleOptions& moduleOptions,
                                      std::shared_ptr<engine::details::ModuleState> state) const {
	if (!config_.backgroundPromotion) {
		// Single-tier: compile directly with the high-performance backend, no promotion.
		state->executable = compileTier(functions, moduleOptions, config_.tier1.backend, "tier1",
		                                pipeline_.irOptimizationLevel({config_.tier1.backend}));
		state->tier.store(engine::Tier::tier1, std::memory_order_relaxed);
		return;
	}
	const bool eager = config_.policy == engine::PromotionPolicy::eager;
	if (config_.tier0.backend == engine::INTERPRETER_BACKEND) {
		// Interpreter tier-0: leave the executable null so ModuleFunction
		// invokes the original callables directly, and only trace to IR for
		// the background tier-1 promotion. When the promotion completes, the
		// executable swap and version bump re-resolve all function handles.
		auto tracedIR = pipeline_.compileToIR(functions, moduleOptions, nullptr,
		                                      pipeline_.irOptimizationLevel({config_.tier1.backend}));
		auto job = createPromotion(state, std::move(tracedIR), moduleOptions);
		if (eager) {
			service_->submit(job);
		}
		return;
	}
	// Two-tier: fast tier-0 now, then promote to tier-1 in the background using
	// this module's own IR -- so both tiers' backends have a say in optimizing it.
	// With the policy `eager` the promotion is queued as soon as the graph exists, before the tier-0 code generation.
	std::shared_ptr<PromotionJob> job;
	bool submitted = false;
	auto tier0 = compileTier(functions, moduleOptions, config_.tier0.backend, "tier0",
	                         pipeline_.irOptimizationLevel({config_.tier0.backend, config_.tier1.backend}),
	                         [&](const std::shared_ptr<ir::IRGraph>& ir) {
		                         job = createPromotion(state, ir, moduleOptions);
		                         if (eager && canStartEarly(*ir)) {
			                         service_->submit(job);
			                         submitted = true;
		                         }
	                         });
	{
		// Install tier 0 only while no tier 1 is: the promotion may have finished first, and then tier 0 is
		// dropped. Installing it does not bump the version -- no handle exists before compileModule returns, and
		// new handles start unresolved (§4.5).
		const std::unique_lock<std::shared_mutex> lock(state->mutex);
		if (state->version.load(std::memory_order_acquire) == 0) {
			state->executable = std::move(tier0);
			state->tier.store(engine::Tier::tier0, std::memory_order_relaxed);
		}
	}
	tier0.reset(); // a tier-0 executable that lost against tier 1 is freed here, outside the lock
	if (eager && !submitted) {
		service_->submit(job);
	}
}

bool TieredJITCompiler::waitForPromotions(std::chrono::milliseconds timeout) const {
	return service_ == nullptr || service_->waitIdle(timeout);
}

bool TieredJITCompiler::allPromotionsComplete() const {
	return service_ == nullptr || service_->idle();
}

size_t TieredJITCompiler::promotionWorkerCount() const {
	return service_ == nullptr ? 0 : service_->workerCount();
}

std::string TieredJITCompiler::getName() const {
	if (!config_.backgroundPromotion) {
		// Single-tier mode reports the bare backend name so callers of
		// NautilusEngine::getNameOfBackend() can gate on it directly.
		return config_.tier1.backend;
	}
	return "tiered(" + config_.tier0.backend + "," + config_.tier1.backend + ")";
}

const engine::Options& TieredJITCompiler::getOptions() const {
	return pipeline_.getOptions();
}

} // namespace nautilus::compiler

#else

#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus::compiler {

TieredJITCompiler::TieredJITCompiler(engine::Options, common::ArenaPool& traceArenaPool, common::ArenaPool& irArenaPool)
    : pipeline_(engine::Options(), traceArenaPool, irArenaPool) {
}
TieredJITCompiler::TieredJITCompiler(engine::Options, engine::TieredCompilationConfig,
                                     common::ArenaPool& traceArenaPool, common::ArenaPool& irArenaPool)
    : pipeline_(engine::Options(), traceArenaPool, irArenaPool) {
}
TieredJITCompiler::~TieredJITCompiler() = default;
std::unique_ptr<Executable> TieredJITCompiler::compile(wrapper_function, const engine::ModuleOptions&) const {
	throw RuntimeException("Jit not initialised");
}
std::unique_ptr<Executable> TieredJITCompiler::compile(std::list<CompilableFunction>&,
                                                       const engine::ModuleOptions&) const {
	throw RuntimeException("Jit not initialised");
}
void TieredJITCompiler::compileModule(std::list<CompilableFunction>&, const engine::ModuleOptions&,
                                      std::shared_ptr<engine::details::ModuleState>) const {
	throw RuntimeException("Jit not initialised");
}
bool TieredJITCompiler::waitForPromotions(std::chrono::milliseconds) const {
	return true;
}
bool TieredJITCompiler::allPromotionsComplete() const {
	return true;
}
size_t TieredJITCompiler::promotionWorkerCount() const {
	return 0;
}
std::string TieredJITCompiler::getName() const {
	return "";
}
const engine::Options& TieredJITCompiler::getOptions() const {
	return pipeline_.getOptions();
}

} // namespace nautilus::compiler

#endif
