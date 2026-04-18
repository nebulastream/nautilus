#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/Module.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"

#ifdef ENABLE_COMPILER

#include "nautilus/CompilableFunction.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include <chrono>
#include <thread>

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

TieredJITCompiler::TieredJITCompiler(engine::Options options, common::Arena& arena, common::ArenaPool& irArenaPool)
    : baseCompiler_(options, arena, irArenaPool) {
	auto tier0 = options.getOptionOrDefault<std::string>("engine.tier0.backend", "");
	auto tier1 = options.getOptionOrDefault<std::string>("engine.tier1.backend", "");
	if (!tier0.empty() && !tier1.empty()) {
		config_.tier0.backend = tier0;
		config_.tier1.backend = tier1;
	}
}

TieredJITCompiler::TieredJITCompiler(engine::Options options, engine::TieredCompilationConfig config,
                                     common::Arena& arena, common::ArenaPool& irArenaPool)
    : baseCompiler_(options, arena, irArenaPool), config_(std::move(config)) {
}

TieredJITCompiler::~TieredJITCompiler() {
	waitForPendingPromotions();
}

std::unique_ptr<Executable> TieredJITCompiler::compile(wrapper_function function) const {
	static constexpr auto ROOT_FUNCTION_NAME = "execute";
	auto rootFunction = CompilableFunction(ROOT_FUNCTION_NAME, function);
	std::list<CompilableFunction> functionsToTrace;
	functionsToTrace.push_back(rootFunction);
	return compile(functionsToTrace);
}

std::unique_ptr<Executable> TieredJITCompiler::compile(std::list<CompilableFunction>& functions) const {
	// One shared statistics object covers the entire tier-0 compile so the
	// user's CompiledModule::getStatistics() reflects tracing + IR passes +
	// tier-0 backend work in a single report.
	auto statistics = std::make_shared<CompilationStatistics>();
	const auto compilationStart = std::chrono::steady_clock::now();

	auto ir = baseCompiler_.compileToIR(functions, statistics.get());
	auto tier0Executable = baseCompiler_.compileIR(ir, config_.tier0.backend, statistics.get());

	statistics->recordTimingMs("compilation.totalMs", compilationStart);
	statistics->set("tier", std::string {"tier0"});

	if (baseCompiler_.getOptions().getOptionOrDefault("engine.logStatistics", false)) {
		const auto id = statistics->find("compilation.unitId") != nullptr
		                    ? std::get<std::string>(*statistics->find("compilation.unitId"))
		                    : std::string {};
		log::info("\n{}", statistics->formatReport(id, config_.tier0.backend));
	}

	tier0Executable->setCompilationStatistics(
	    std::static_pointer_cast<const CompilationStatistics>(std::move(statistics)));

	// Cache the IR for the upcoming promoteAsync() call
	lastCachedIR_ = std::move(ir);

	return tier0Executable;
}

void TieredJITCompiler::promoteAsync(std::weak_ptr<engine::details::ModuleState> state) const {
	auto cachedIR = std::move(lastCachedIR_);
	if (!cachedIR) {
		return;
	}

	pendingPromotions_.fetch_add(1, std::memory_order_relaxed);

	std::lock_guard<std::mutex> lock(threadsMutex_);
	promotionThreads_.emplace_back([weakState = std::move(state), ir = std::move(cachedIR), config = config_,
	                                options = baseCompiler_.getOptions(), &pendingCount = pendingPromotions_]() {
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
	return "tiered(" + config_.tier0.backend + "," + config_.tier1.backend + ")";
}

const engine::Options& TieredJITCompiler::getOptions() const {
	return baseCompiler_.getOptions();
}

std::shared_ptr<ir::IRGraph> TieredJITCompiler::compileToIR(std::list<CompilableFunction>& functions) const {
	return baseCompiler_.compileToIR(functions);
}

std::unique_ptr<Executable> TieredJITCompiler::compileIR(const std::shared_ptr<ir::IRGraph>& ir,
                                                         const std::string& backendName) const {
	return baseCompiler_.compileIR(ir, backendName);
}

void TieredJITCompiler::addIRPass(std::unique_ptr<ir::IRPass> pass) {
	// Plugin passes are stored on the inner LegacyCompiler so they participate
	// in both tier-0 (synchronous) and tier-1 (background promotion) compiles.
	baseCompiler_.addIRPass(std::move(pass));
}

} // namespace nautilus::compiler

#else

namespace nautilus::compiler {

TieredJITCompiler::TieredJITCompiler(engine::Options, common::Arena& arena, common::ArenaPool& irArenaPool)
    : baseCompiler_(engine::Options(), arena, irArenaPool) {
}
TieredJITCompiler::TieredJITCompiler(engine::Options, engine::TieredCompilationConfig, common::Arena& arena,
                                     common::ArenaPool& irArenaPool)
    : baseCompiler_(engine::Options(), arena, irArenaPool) {
}
TieredJITCompiler::~TieredJITCompiler() = default;
std::unique_ptr<Executable> TieredJITCompiler::compile(wrapper_function) const {
	throw RuntimeException("Jit not initialised");
}
std::unique_ptr<Executable> TieredJITCompiler::compile(std::list<CompilableFunction>&) const {
	throw RuntimeException("Jit not initialised");
}
std::shared_ptr<ir::IRGraph> TieredJITCompiler::compileToIR(std::list<CompilableFunction>&) const {
	throw RuntimeException("Jit not initialised");
}
std::unique_ptr<Executable> TieredJITCompiler::compileIR(const std::shared_ptr<ir::IRGraph>&,
                                                         const std::string&) const {
	throw RuntimeException("Jit not initialised");
}
void TieredJITCompiler::addIRPass(std::unique_ptr<ir::IRPass>) {
	throw RuntimeException("Jit not initialised");
}
void TieredJITCompiler::promoteAsync(std::weak_ptr<engine::details::ModuleState>) const {
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
	return baseCompiler_.getOptions();
}

} // namespace nautilus::compiler

#endif
