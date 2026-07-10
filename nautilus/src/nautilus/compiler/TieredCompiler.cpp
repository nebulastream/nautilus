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
#include "nautilus/exceptions/RuntimeException.hpp"
#include <chrono>
#include <thread>

#ifdef ENABLE_TRACING
#include "nautilus/compiler/ir/util/IRSerializationUtil.hpp"
#endif

namespace nautilus::compiler {

static std::string createPromotionUnitID() {
	auto now = std::chrono::system_clock::now();
	auto time_t = std::chrono::system_clock::to_time_t(now);
	std::tm tm {};
	localtime_r(&time_t, &tm);
	char buf[64];
	std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d_%02d-%02d-%02d_tier1", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
	              tm.tm_hour, tm.tm_min, tm.tm_sec);
	return std::string(buf);
}

// --- TieredJITCompiler ---

TieredJITCompiler::TieredJITCompiler(engine::Options options, common::ArenaPool& traceArenaPool,
                                     common::ArenaPool& irArenaPool)
    : pipeline_(options, traceArenaPool, irArenaPool) {
	// An explicitly selected backend pins single-tier compilation: the engine
	// compiles synchronously with exactly this backend and never promotes.
	// It takes precedence over the tier options.
	auto explicitBackend = options.getOptionOrDefault<std::string>("engine.backend", "");
	if (!explicitBackend.empty()) {
		config_.tier1.backend = explicitBackend;
		config_.backgroundPromotion = false;
		return;
	}
	auto tier0 = options.getOptionOrDefault<std::string>("engine.tier0.backend", "");
	auto tier1 = options.getOptionOrDefault<std::string>("engine.tier1.backend", "");
	if (!tier0.empty() && !tier1.empty()) {
		config_.tier0.backend = tier0;
		config_.tier1.backend = tier1;
	}
	config_.backgroundPromotion = options.getOptionOrDefault("engine.tiered.backgroundPromotion", true);
	if (tier0.empty() || tier1.empty()) {
		// Default tiers: pick from what this build actually registered, so
		// default options keep working in reduced builds; explicit user
		// choices above stay strict and fail at compile time.
		auto* registry = CompilationBackendRegistry::getInstance();
		if (!registry->hasBackend(config_.tier1.backend)) {
			auto fallback = registry->getDefaultBackendName();
			if (!fallback.empty()) {
				config_.tier1.backend = fallback;
			}
		}
		// Tier-0 preference order: asmjit, bc, interpreter. The interpreter
		// tier is always available — the module runs uncompiled until the
		// background tier-1 promotion swaps in its executable.
		if (registry->hasBackend("asmjit")) {
			config_.tier0.backend = "asmjit";
		} else if (registry->hasBackend("bc")) {
			config_.tier0.backend = "bc";
		} else {
			config_.tier0.backend = engine::INTERPRETER_BACKEND;
		}
		if (config_.tier0.backend == config_.tier1.backend) {
			// Promoting to the tier-0 backend would be a no-op; compile
			// single-tier instead.
			config_.backgroundPromotion = false;
		}
	}
}

TieredJITCompiler::TieredJITCompiler(engine::Options options, engine::TieredCompilationConfig config,
                                     common::ArenaPool& traceArenaPool, common::ArenaPool& irArenaPool)
    : pipeline_(options, traceArenaPool, irArenaPool), config_(std::move(config)) {
}

TieredJITCompiler::~TieredJITCompiler() {
	waitForPendingPromotions();
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
                                                           std::shared_ptr<ir::IRGraph>& outIR) const {
	// One shared statistics object covers the entire tier compile so the
	// user's CompiledModule::getStatistics() reflects tracing + IR passes +
	// backend work in a single report.
	auto statistics = std::make_shared<CompilationStatistics>();
	const auto compilationStart = std::chrono::steady_clock::now();

	auto ir = pipeline_.compileToIR(functions, moduleOptions, statistics.get());
	auto executable = pipeline_.compileIR(ir, backend, moduleOptions, statistics.get());

	statistics->recordTimingMs("compilation.totalMs", compilationStart);
	statistics->set("tier", tierLabel);

	if (moduleOptions.getOptionOrDefault("engine.logStatistics", false)) {
		const auto id = statistics->find("compilation.unitId") != nullptr
		                    ? std::get<std::string>(*statistics->find("compilation.unitId"))
		                    : std::string {};
		log::info("\n{}", statistics->formatReport(id, backend));
	}

	executable->setCompilationStatistics(std::static_pointer_cast<const CompilationStatistics>(std::move(statistics)));

	// Hand the IR back to the caller so it can drive background promotion.
	outIR = std::move(ir);
	return executable;
}

std::unique_ptr<Executable> TieredJITCompiler::compile(std::list<CompilableFunction>& functions,
                                                       const engine::ModuleOptions& moduleOptions) const {
	std::shared_ptr<ir::IRGraph> ir;
	if (!config_.backgroundPromotion || config_.tier0.backend == engine::INTERPRETER_BACKEND) {
		// Single-tier: compile directly with the high-performance backend.
		// The interpreter tier-0 also lands here: without a module state
		// there is nothing to run interpreted, so compile tier-1 up front.
		return compileTier(functions, moduleOptions, config_.tier1.backend, "tier1", ir);
	}
	// Two-tier: return the fast tier-0 executable (no module state to promote into).
	return compileTier(functions, moduleOptions, config_.tier0.backend, "tier0", ir);
}

void TieredJITCompiler::compileModule(std::list<CompilableFunction>& functions,
                                      const engine::ModuleOptions& moduleOptions,
                                      std::shared_ptr<engine::details::ModuleState> state) const {
	std::shared_ptr<ir::IRGraph> ir;
	if (!config_.backgroundPromotion) {
		// Single-tier: compile directly with the high-performance backend, no promotion.
		state->executable = compileTier(functions, moduleOptions, config_.tier1.backend, "tier1", ir);
		return;
	}
	if (config_.tier0.backend == engine::INTERPRETER_BACKEND) {
		// Interpreter tier-0: leave the executable null so ModuleFunction
		// invokes the original callables directly, and only trace to IR for
		// the background tier-1 promotion. When the promotion completes, the
		// executable swap and version bump re-resolve all function handles.
		auto tracedIR = pipeline_.compileToIR(functions, moduleOptions);
		promoteAsync(state, std::move(tracedIR), moduleOptions);
		return;
	}
	// Two-tier: fast tier-0 now, then promote to tier-1 in the background using
	// this module's own IR.
	state->executable = compileTier(functions, moduleOptions, config_.tier0.backend, "tier0", ir);
	promoteAsync(state, std::move(ir), moduleOptions);
}

std::unique_ptr<Executable> TieredJITCompiler::compileIRTier(const std::shared_ptr<ir::IRGraph>& ir,
                                                             const engine::ModuleOptions& moduleOptions,
                                                             const std::string& backend,
                                                             const std::string& tierLabel) const {
	auto statistics = std::make_shared<CompilationStatistics>();
	const auto compilationStart = std::chrono::steady_clock::now();

	auto executable = pipeline_.compileIR(ir, backend, moduleOptions, statistics.get());

	statistics->recordTimingMs("compilation.totalMs", compilationStart);
	statistics->set("tier", tierLabel);

	if (moduleOptions.getOptionOrDefault("engine.logStatistics", false)) {
		const auto id = statistics->find("compilation.unitId") != nullptr
		                    ? std::get<std::string>(*statistics->find("compilation.unitId"))
		                    : std::string {};
		log::info("\n{}", statistics->formatReport(id, backend));
	}

	executable->setCompilationStatistics(std::static_pointer_cast<const CompilationStatistics>(std::move(statistics)));
	return executable;
}

std::string TieredJITCompiler::compileToSerializedIR(std::list<CompilableFunction>& functions,
                                                     const engine::ModuleOptions& moduleOptions) const {
#ifdef ENABLE_TRACING
	auto ir = pipeline_.compileToIR(functions, moduleOptions);
	return ir::serializeIR(*ir);
#else
	(void) functions;
	(void) moduleOptions;
	throw RuntimeException("Serializing IR requires a build with ENABLE_TRACING");
#endif
}

void TieredJITCompiler::compileIRModule(std::shared_ptr<ir::IRGraph> ir, const engine::ModuleOptions& moduleOptions,
                                        std::shared_ptr<engine::details::ModuleState> state) const {
	if (!config_.backgroundPromotion || config_.tier0.backend == engine::INTERPRETER_BACKEND) {
		// Single-tier, or interpreter tier 0: a loaded module has no original
		// callables to run interpreted, so compile tier 1 synchronously.
		state->executable = compileIRTier(ir, moduleOptions, config_.tier1.backend, "tier1");
		return;
	}
	// Two-tier: fast tier-0 now, then promote to tier-1 in the background.
	state->executable = compileIRTier(ir, moduleOptions, config_.tier0.backend, "tier0");
	promoteAsync(state, std::move(ir), moduleOptions);
}

void TieredJITCompiler::promoteAsync(std::weak_ptr<engine::details::ModuleState> state, std::shared_ptr<ir::IRGraph> ir,
                                     engine::ModuleOptions options) const {
	if (!ir) {
		return;
	}

	pendingPromotions_.fetch_add(1, std::memory_order_relaxed);

	std::lock_guard<std::mutex> lock(threadsMutex_);
	// The IR and per-module options are owned per call and moved into the
	// background thread, so concurrent promotions never share state.
	promotionThreads_.emplace_back([weakState = std::move(state), ir = std::move(ir), config = config_,
	                                options = std::move(options), &pendingCount = pendingPromotions_]() {
		try {
			auto* registry = CompilationBackendRegistry::getInstance();
			auto* backend = registry->getBackend(config.tier1.backend);

			auto compilationId = createPromotionUnitID();
			auto dumpHandler = DumpHandler(options, compilationId);

			auto statistics = std::make_shared<CompilationStatistics>();
			statistics->set("compilation.unitId", compilationId);
			statistics->set("tier", std::string {"tier1"});
			statistics->set("backend.name", config.tier1.backend);

			const auto promotionStart = std::chrono::steady_clock::now();
			auto tier1Executable = backend->compile(ir, dumpHandler, options, statistics.get());
			statistics->recordTimingMs("compilation.totalMs", promotionStart);
			tier1Executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());

			if (options.getOptionOrDefault("engine.logStatistics", false)) {
				log::info("\n{}", statistics->formatReport(compilationId, config.tier1.backend));
			}

			tier1Executable->setCompilationStatistics(
			    std::static_pointer_cast<const CompilationStatistics>(std::move(statistics)));

			if (auto s = weakState.lock()) {
				std::unique_lock<std::shared_mutex> lock(s->mutex);
				s->executable = std::move(tier1Executable);
				s->version.fetch_add(1, std::memory_order_release);
			}
		} catch (const std::exception& e) {
			log::error("Tier 1 promotion failed: {}", e.what());
		}

		pendingCount.fetch_sub(1, std::memory_order_release);
	});
}

void TieredJITCompiler::waitForPendingPromotions() const {
	std::lock_guard<std::mutex> lock(threadsMutex_);
	for (auto& t : promotionThreads_) {
		if (t.joinable()) {
			t.join();
		}
	}
	promotionThreads_.clear();
}

bool TieredJITCompiler::allPromotionsComplete() const {
	return pendingPromotions_.load(std::memory_order_acquire) == 0;
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
std::unique_ptr<Executable> TieredJITCompiler::compileTier(std::list<CompilableFunction>&, const engine::ModuleOptions&,
                                                           const std::string&, const std::string&,
                                                           std::shared_ptr<ir::IRGraph>&) const {
	throw RuntimeException("Jit not initialised");
}
void TieredJITCompiler::compileModule(std::list<CompilableFunction>&, const engine::ModuleOptions&,
                                      std::shared_ptr<engine::details::ModuleState>) const {
	throw RuntimeException("Jit not initialised");
}
std::unique_ptr<Executable> TieredJITCompiler::compileIRTier(const std::shared_ptr<ir::IRGraph>&,
                                                             const engine::ModuleOptions&, const std::string&,
                                                             const std::string&) const {
	throw RuntimeException("Jit not initialised");
}
std::string TieredJITCompiler::compileToSerializedIR(std::list<CompilableFunction>&,
                                                     const engine::ModuleOptions&) const {
	throw RuntimeException("Jit not initialised");
}
void TieredJITCompiler::compileIRModule(std::shared_ptr<ir::IRGraph>, const engine::ModuleOptions&,
                                        std::shared_ptr<engine::details::ModuleState>) const {
	throw RuntimeException("Jit not initialised");
}
void TieredJITCompiler::promoteAsync(std::weak_ptr<engine::details::ModuleState>, std::shared_ptr<ir::IRGraph>,
                                     engine::ModuleOptions) const {
}
void TieredJITCompiler::waitForPendingPromotions() const {
}
bool TieredJITCompiler::allPromotionsComplete() const {
	return true;
}
std::string TieredJITCompiler::getName() const {
	return "";
}
const engine::Options& TieredJITCompiler::getOptions() const {
	return pipeline_.getOptions();
}

} // namespace nautilus::compiler

#endif
