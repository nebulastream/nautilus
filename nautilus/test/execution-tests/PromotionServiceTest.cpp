#include "catch2/catch_test_macros.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>

#if defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND)

#include "nautilus/compiler/PromotionService.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

// The tests of the promotion service (11-tiered-compilation.md §6). The tiers are `tbc` backends wrapped in a probe
// that counts the compiles, blocks them on a gate, or fails them, so every state of a job can be held still.

namespace nautilus::engine {

namespace {

using namespace std::chrono_literals;

val<int32_t> promotionAddOne(val<int32_t> x) {
	return x + 1;
}

/// Opens and closes; waiters also wake when their compile is cancelled.
class Gate {
public:
	explicit Gate(bool open) : open_(open) {
	}
	void open() {
		{
			const std::lock_guard<std::mutex> lock(mutex_);
			open_ = true;
		}
		changed_.notify_all();
	}
	void wait(const compiler::nirbackend::CancellationToken* cancellation) {
		std::unique_lock<std::mutex> lock(mutex_);
		while (!open_ && !(cancellation != nullptr && cancellation->cancelled())) {
			changed_.wait_for(lock, 1ms);
		}
	}

private:
	std::mutex mutex_;
	std::condition_variable changed_;
	bool open_;
};

enum class Fault { none, throwInt, throwRuntimeError };

/// What the probe backend of a test observed and how it behaves.
struct Probe {
	explicit Probe(bool open = true) : gate(open) {
	}
	std::atomic<int> entered {0};
	std::atomic<int> completed {0};
	std::atomic<int> cancelled {0};
	std::atomic<Fault> fault {Fault::none};
	Gate gate;
};

/// A `tbc` that counts, can wait on a gate (until the compile is cancelled) and can throw.
class ProbeBackend : public compiler::CompilationBackend {
public:
	explicit ProbeBackend(std::shared_ptr<Probe> probe) : probe_(std::move(probe)) {
	}

	std::unique_ptr<compiler::Executable> compile(const std::shared_ptr<compiler::ir::IRGraph>& ir,
	                                              const compiler::DumpHandler& dumpHandler,
	                                              const engine::Options& options,
	                                              compiler::CompilationStatistics* statistics,
	                                              const compiler::nirbackend::CancellationToken* cancellation) const override {
		probe_->entered.fetch_add(1);
		switch (probe_->fault.load()) {
		case Fault::throwInt:
			throw 42;
		case Fault::throwRuntimeError:
			throw std::runtime_error("injected");
		case Fault::none:
			break;
		}
		probe_->gate.wait(cancellation);
		auto executable = delegate().compile(ir, dumpHandler, options, statistics, cancellation);
		(executable ? probe_->completed : probe_->cancelled).fetch_add(1);
		return executable;
	}
	[[nodiscard]] compiler::ExceptionPropagationMode getExceptionPropagationMode() const override {
		return delegate().getExceptionPropagationMode();
	}
	[[nodiscard]] compiler::IROptimizationLevel irOptimizationLevel() const override {
		return delegate().irOptimizationLevel();
	}
	[[nodiscard]] bool compilesNir() const override {
		return true;
	}

private:
	static const compiler::CompilationBackend& delegate() {
		return *compiler::CompilationBackendRegistry::getInstance()->getBackend("tbc");
	}
	std::shared_ptr<Probe> probe_;
};

/// Registers a probe backend under a fresh name. The names sort after every real backend, so the registry's default
/// backend does not change.
std::string registerProbe(const std::shared_ptr<Probe>& probe) {
	static std::atomic<int> counter {0};
	auto name = "zz-promotion-probe-" + std::to_string(counter.fetch_add(1));
	compiler::CompilationBackendRegistry::getInstance()->registerBackend(name, std::make_unique<ProbeBackend>(probe));
	return name;
}

template <typename Predicate>
bool eventually(Predicate predicate, std::chrono::milliseconds timeout = 10s) {
	const auto deadline = std::chrono::steady_clock::now() + timeout;
	while (!predicate()) {
		if (std::chrono::steady_clock::now() > deadline) {
			return false;
		}
		std::this_thread::sleep_for(1ms);
	}
	return true;
}

/// An engine with a tiered compiler of the given tiers, and the arena pools it needs to outlive.
struct Fixture {
	Fixture(const std::string& tier0, const std::string& tier1, int threads, std::optional<int> maxQueued = {},
	        engine::PromotionPolicy policy = engine::PromotionPolicy::eager, Options options = Options()) {
		options.setOption("engine.tiered.promotionThreads", threads);
		if (maxQueued) {
			options.setOption("engine.tiered.maxQueuedPromotions", *maxQueued);
		}
		TieredCompilationConfig config;
		config.tier0.backend = tier0;
		config.tier1.backend = tier1;
		config.policy = policy;
		auto tieredJit = std::make_unique<compiler::TieredJITCompiler>(options, config, traceArenaPool, irArenaPool);
		jit = tieredJit.get();
		engine = std::make_unique<NautilusEngine>(std::move(tieredJit), options);
	}

	CompiledModule compileOne() const {
		auto module = engine->createModule();
		module.registerFunction("add_one", promotionAddOne);
		return module.compile();
	}

	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	compiler::TieredJITCompiler* jit = nullptr;
	std::unique_ptr<NautilusEngine> engine;
};

/// The number of threads of this process, or -1 where it cannot be read.
int threadCount() {
	std::ifstream status("/proc/self/status");
	std::string line;
	while (std::getline(status, line)) {
		if (line.rfind("Threads:", 0) == 0) {
			return std::stoi(line.substr(8));
		}
	}
	return -1;
}

/// The virtual memory of this process in KiB, or -1.
long virtualMemoryKiB() {
	std::ifstream status("/proc/self/status");
	std::string line;
	while (std::getline(status, line)) {
		if (line.rfind("VmSize:", 0) == 0) {
			return std::stol(line.substr(7));
		}
	}
	return -1;
}

#if defined(__SANITIZE_ADDRESS__) || defined(__SANITIZE_THREAD__)
constexpr bool SANITIZED = true;
#elif defined(__has_feature)
#if __has_feature(address_sanitizer) || __has_feature(thread_sanitizer)
constexpr bool SANITIZED = true;
#else
constexpr bool SANITIZED = false;
#endif
#else
constexpr bool SANITIZED = false;
#endif

/// Compiles @p modules short-lived modules on @p fixture, dropping each right away, and samples the thread count.
void runShortLivedModules(const Fixture& fixture, int modules, int baselineThreads, int allowedThreads,
                          long* memoryAfterWarmup, long* memoryAtEnd) {
	int maxThreads = 0;
	for (int i = 0; i < modules; ++i) {
		{
			auto compiled = fixture.compileOne();
			auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");
			REQUIRE(fn(i) == i + 1);
		}
		if (i % 64 == 0) {
			maxThreads = std::max(maxThreads, threadCount());
		}
		if (i == modules / 10) {
			*memoryAfterWarmup = virtualMemoryKiB();
		}
	}
	fixture.engine->waitForPromotions();
	maxThreads = std::max(maxThreads, threadCount());
	*memoryAtEnd = virtualMemoryKiB();
	if (baselineThreads > 0) {
		// ThreadSanitizer starts a background thread of its own with the first thread of the process
		REQUIRE(maxThreads <= baselineThreads + allowedThreads + (SANITIZED ? 1 : 0));
	}
}

} // namespace

TEST_CASE("Promotion Service - The Thread Count Is Bounded By The Pool") {
	const auto probe = std::make_shared<Probe>();
	const auto baseline = threadCount();
	{
		Fixture fixture("tbc", registerProbe(probe), 2);
		long warm = 0;
		long end = 0;
		// the caller's thread is in the baseline: only the workers are new
		runShortLivedModules(fixture, 1500, baseline, 2, &warm, &end);
		REQUIRE(fixture.jit->promotionWorkerCount() <= 2);
		REQUIRE(fixture.jit->promotionWorkerCount() >= 1);
		if (!SANITIZED && warm > 0) {
			// flat: a thread per promotion would map a stack (8 MiB) each
			REQUIRE(end - warm < 128 * 1024);
		}
	}
	if (baseline > 0) {
		// destroying the engine joins its workers
		REQUIRE(eventually([&] { return threadCount() <= baseline + (SANITIZED ? 1 : 0); }));
	}
}

TEST_CASE("Promotion Service - A Promotion Of A Dropped Module Is Cancelled While Queued", "[promotion]") {
	const auto probe = std::make_shared<Probe>(false);
	Fixture fixture("tbc", registerProbe(probe), 1);

	// A occupies the only worker, inside the backend
	auto a = fixture.compileOne();
	REQUIRE(eventually([&] { return probe->entered.load() == 1; }));
	REQUIRE(a.promotionStatus().state == PromotionState::running);
	{
		auto b = fixture.compileOne();
		REQUIRE(b.promotionStatus().state == PromotionState::queued);
		auto fn = b.getFunction<int32_t(int32_t)>("add_one");
		REQUIRE(fn(1) == 2);
	} // B is dropped while it waits
	probe->gate.open();
	REQUIRE(fixture.engine->waitForPromotions(30s));
	REQUIRE(probe->entered.load() == 1); // B was never compiled
	REQUIRE(a.promotionStatus().state == PromotionState::done);
	REQUIRE(a.tier() == Tier::tier1);
}

TEST_CASE("Promotion Service - A Promotion Of A Dropped Module Ends At The Next Stage Boundary", "[promotion]") {
	const auto probe = std::make_shared<Probe>(false);
	Fixture fixture("tbc", registerProbe(probe), 1);
	{
		auto compiled = fixture.compileOne();
		REQUIRE(eventually([&] { return probe->entered.load() == 1; }));
		REQUIRE(compiled.promotionStatus().state == PromotionState::running);
	} // dropped while the backend runs; the backend sees its token cancelled at the stage boundary
	REQUIRE(fixture.engine->waitForPromotions(30s));
	REQUIRE(probe->cancelled.load() == 1);
	REQUIRE(probe->completed.load() == 0);

	// the service goes on
	probe->gate.open();
	auto next = fixture.compileOne();
	REQUIRE(next.waitForPromotion(30s));
	REQUIRE(next.tier() == Tier::tier1);
}

TEST_CASE("Promotion Service - A Compile On Another Thread Does Not Block Behind A Wait", "[promotion]") {
	const auto probe = std::make_shared<Probe>(false);
	Fixture fixture("tbc", registerProbe(probe), 1);
	auto a = fixture.compileOne();
	REQUIRE(eventually([&] { return probe->entered.load() == 1; }));

	std::thread waiter([&] { fixture.engine->waitForPromotions(); });
	std::this_thread::sleep_for(20ms); // let it block
	auto compiling = std::async(std::launch::async, [&] {
		auto b = fixture.compileOne();
		return b.getFunction<int32_t(int32_t)>("add_one")(41);
	});
	// the promotion of A still blocks; compile() of B returns regardless (F4)
	REQUIRE(compiling.wait_for(20s) == std::future_status::ready);
	REQUIRE(compiling.get() == 42);
	probe->gate.open();
	waiter.join();
	REQUIRE(fixture.jit->allPromotionsComplete());
}

TEST_CASE("Promotion Service - Destroying The Engine Waits For At Most One Compile Per Worker", "[promotion]") {
	const auto probe = std::make_shared<Probe>(false);
	std::vector<CompiledModule> modules;
	{
		Fixture fixture("tbc", registerProbe(probe), 1);
		for (int i = 0; i < 100; ++i) {
			modules.push_back(fixture.compileOne());
		}
		REQUIRE(eventually([&] { return probe->entered.load() == 1; }));
		const auto start = std::chrono::steady_clock::now();
		fixture.engine.reset(); // the gate stays closed: the running compile ends because it is cancelled
		REQUIRE(std::chrono::steady_clock::now() - start < 20s);
	}
	REQUIRE(probe->entered.load() == 1); // the 99 queued jobs never ran
	int cancelled = 0;
	for (auto& module : modules) {
		cancelled += module.promotionStatus().state == PromotionState::cancelled;
		REQUIRE(module.tier() == Tier::tier0);
		REQUIRE(module.getFunction<int32_t(int32_t)>("add_one")(1) == 2);
	}
	REQUIRE(cancelled == 100);
}

TEST_CASE("Promotion Service - A Tier-1 Compile That Throws Leaves The Module On Tier 0", "[promotion]") {
	const auto probe = std::make_shared<Probe>();
	Fixture fixture("tbc", registerProbe(probe), 1);

	SECTION("a value that is not a std::exception") {
		probe->fault = Fault::throwInt;
		auto compiled = fixture.compileOne();
		REQUIRE(compiled.waitForPromotion(30s));
		const auto status = compiled.promotionStatus();
		REQUIRE(status.state == PromotionState::failed);
		REQUIRE_FALSE(status.error.empty());
		REQUIRE(compiled.tier() == Tier::tier0);
		REQUIRE(compiled.getFunction<int32_t(int32_t)>("add_one")(5) == 6);
	}
	SECTION("a std::exception") {
		probe->fault = Fault::throwRuntimeError;
		auto compiled = fixture.compileOne();
		REQUIRE(compiled.waitForPromotion(30s));
		REQUIRE(compiled.promotionStatus().state == PromotionState::failed);
		REQUIRE(compiled.promotionStatus().error == "injected");
		REQUIRE(compiled.tier() == Tier::tier0);
	}
	// a failure is not retried, and the worker and the service survive it
	REQUIRE(probe->entered.load() == 1);
	probe->fault = Fault::none;
	auto next = fixture.compileOne();
	REQUIRE(next.waitForPromotion(30s));
	REQUIRE(next.promotionStatus().state == PromotionState::done);
	REQUIRE(next.tier() == Tier::tier1);
}

TEST_CASE("Promotion Service - Policy Manual Promotes Only On promote()", "[promotion]") {
	const auto probe = std::make_shared<Probe>();
	Fixture fixture("tbc", registerProbe(probe), 2, {}, PromotionPolicy::manual);
	auto compiled = fixture.compileOne();
	auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

	REQUIRE(fixture.engine->waitForPromotions(30s)); // nothing is pending
	REQUIRE(compiled.waitForPromotion(0ms));
	REQUIRE(compiled.promotionStatus().state == PromotionState::none);
	REQUIRE(compiled.tier() == Tier::tier0);
	REQUIRE(probe->entered.load() == 0);
	REQUIRE(fn(1) == 2);

	// promote() from several threads at once queues one job
	constexpr int callers = 8;
	std::atomic<int> ready {0};
	std::atomic<bool> go {false};
	std::atomic<int> queued {0};
	std::vector<std::thread> threads;
	for (int i = 0; i < callers; ++i) {
		threads.emplace_back([&] {
			++ready;
			while (!go.load()) {
				std::this_thread::yield();
			}
			queued += compiled.promote();
		});
	}
	while (ready.load() < callers) {
		std::this_thread::yield();
	}
	go = true;
	for (auto& thread : threads) {
		thread.join();
	}
	REQUIRE(queued.load() == 1);
	REQUIRE(compiled.waitForPromotion(30s));
	REQUIRE(compiled.promotionStatus().state == PromotionState::done);
	REQUIRE(compiled.tier() == Tier::tier1);
	REQUIRE(probe->entered.load() == 1);
	REQUIRE_FALSE(compiled.promote());
	REQUIRE(fn(1) == 2);
}

TEST_CASE("Promotion Service - Policy Eager Queues One Job Per Module", "[promotion]") {
	const auto probe = std::make_shared<Probe>();
	Fixture fixture("tbc", registerProbe(probe), 2);
	std::vector<CompiledModule> modules;
	for (int i = 0; i < 20; ++i) {
		modules.push_back(fixture.compileOne());
	}
	REQUIRE(fixture.engine->waitForPromotions(60s));
	REQUIRE(probe->entered.load() == 20);
	for (auto& module : modules) {
		REQUIRE(module.promotionStatus().state == PromotionState::done);
		REQUIRE(module.tier() == Tier::tier1);
		REQUIRE_FALSE(module.promote()); // the job was queued by compile()
	}
}

TEST_CASE("Promotion Service - A Tier 1 That Finishes First Is Not Overwritten By Tier 0", "[promotion]") {
	const auto tier0 = std::make_shared<Probe>(false);
	const auto tier1 = std::make_shared<Probe>();
	Fixture fixture(registerProbe(tier0), registerProbe(tier1), 1);

	std::optional<CompiledModule> compiled;
	std::thread compiling([&] { compiled.emplace(fixture.compileOne()); });
	// the job is queued after conversion, while the tier-0 code generation is still blocked
	REQUIRE(eventually([&] { return tier1->entered.load() == 1 && tier0->entered.load() == 1; }));
	REQUIRE(fixture.engine->waitForPromotions(30s)); // tier 1 is installed
	tier0->gate.open();
	compiling.join();

	REQUIRE(tier0->completed.load() == 1); // the tier-0 compile ran to the end ...
	REQUIRE(compiled->tier() == Tier::tier1); // ... and its result was dropped
	REQUIRE(compiled->promotionStatus().state == PromotionState::done);
	const auto stats = compiled->getStatistics();
	REQUIRE(stats != nullptr);
	REQUIRE(std::get<std::string>(*stats->find("tier")) == "tier1");
	REQUIRE(compiled->getFunction<int32_t(int32_t)>("add_one")(9) == 10);
}

TEST_CASE("Promotion Service - The Admission Cap Skips Promotions", "[promotion]") {
	const auto probe = std::make_shared<Probe>(false);
	SECTION("a cap of one") {
		Fixture fixture("tbc", registerProbe(probe), 1, 1);
		auto a = fixture.compileOne();
		REQUIRE(eventually([&] { return probe->entered.load() == 1; }));
		auto b = fixture.compileOne(); // waits
		auto c = fixture.compileOne(); // beyond the cap
		REQUIRE(b.promotionStatus().state == PromotionState::queued);
		REQUIRE(c.promotionStatus().state == PromotionState::skipped);
		REQUIRE(c.tier() == Tier::tier0);
		REQUIRE(c.getFunction<int32_t(int32_t)>("add_one")(2) == 3);
		REQUIRE(c.waitForPromotion(0ms));
		probe->gate.open();
		REQUIRE(fixture.engine->waitForPromotions(30s));
		REQUIRE(a.tier() == Tier::tier1);
		REQUIRE(b.tier() == Tier::tier1);
		REQUIRE(c.tier() == Tier::tier0);
		REQUIRE(probe->entered.load() == 2);
	}
	SECTION("a cap of zero") {
		probe->gate.open();
		Fixture fixture("tbc", registerProbe(probe), 1, 0);
		auto a = fixture.compileOne();
		REQUIRE(a.promotionStatus().state == PromotionState::skipped);
		REQUIRE(fixture.engine->waitForPromotions(30s));
		REQUIRE(probe->entered.load() == 0);
		REQUIRE(fixture.jit->promotionWorkerCount() == 0);
	}
	SECTION("jobs of dropped modules do not count against the cap") {
		Fixture fixture("tbc", registerProbe(probe), 1, 1);
		auto a = fixture.compileOne();
		REQUIRE(eventually([&] { return probe->entered.load() == 1; }));
		{
			auto dropped = fixture.compileOne(); // fills the queue ...
		}                                        // ... and is dropped
		auto c = fixture.compileOne();
		REQUIRE(c.promotionStatus().state == PromotionState::queued);
		probe->gate.open();
		REQUIRE(fixture.engine->waitForPromotions(30s));
		REQUIRE(c.tier() == Tier::tier1);
	}
}

TEST_CASE("Promotion Service - waitForPromotion Waits For One Module", "[promotion]") {
	const auto probe = std::make_shared<Probe>(false);
	Fixture fixture("tbc", registerProbe(probe), 1);
	auto a = fixture.compileOne();
	REQUIRE_FALSE(a.waitForPromotion(20ms));
	REQUIRE_FALSE(fixture.engine->waitForPromotions(20ms));
	probe->gate.open();
	REQUIRE(a.waitForPromotion(30s));
	REQUIRE(a.promotionStatus().state == PromotionState::done);
}

TEST_CASE("Promotion Service - The Tier-1 Report Links The Tier-0 Unit", "[promotion]") {
	const auto probe = std::make_shared<Probe>();
	Fixture fixture("tbc", registerProbe(probe), 1, {}, PromotionPolicy::manual);
	auto compiled = fixture.compileOne();
	const auto tier0Stats = compiled.getStatistics();
	REQUIRE(tier0Stats != nullptr);
	const auto tier0UnitId = std::get<std::string>(*tier0Stats->find("compilation.unitId"));

	REQUIRE(compiled.promote());
	REQUIRE(compiled.waitForPromotion(30s));
	const auto stats = compiled.getStatistics();
	REQUIRE(stats != nullptr);
	REQUIRE(stats.get() != tier0Stats.get());
	REQUIRE(std::get<std::string>(*stats->find("tier")) == "tier1");
	REQUIRE(std::get<std::string>(*stats->find("compilation.tier0UnitId")) == tier0UnitId);
	REQUIRE(std::get<std::string>(*stats->find("compilation.unitId")) == tier0UnitId + "_tier1");
	REQUIRE(std::get<std::string>(*stats->find("promotion.status")) == "done");
	for (const auto* key : {"promotion.queueMs", "promotion.compileMs", "promotion.timeToTier1Ms"}) {
		INFO(key);
		REQUIRE(stats->contains(key));
		REQUIRE(std::get<double>(*stats->find(key)) >= 0.0);
	}
	REQUIRE(std::get<double>(*stats->find("promotion.timeToTier1Ms")) >=
	        std::get<double>(*stats->find("promotion.compileMs")));
}

TEST_CASE("Promotion Service - promote() After The Engine Is Gone Does Nothing", "[promotion]") {
	const auto probe = std::make_shared<Probe>();
	std::optional<CompiledModule> compiled;
	{
		Fixture fixture("tbc", registerProbe(probe), 1, {}, PromotionPolicy::manual);
		compiled.emplace(fixture.compileOne());
	}
	REQUIRE_FALSE(compiled->promote());
	REQUIRE(compiled->promotionStatus().state == PromotionState::cancelled);
	REQUIRE(compiled->tier() == Tier::tier0);
	REQUIRE(compiled->getFunction<int32_t(int32_t)>("add_one")(1) == 2);
	REQUIRE(probe->entered.load() == 0);
}

TEST_CASE("Promotion Service - An Interpreted Module Is Promoted On Request", "[promotion]") {
	const auto probe = std::make_shared<Probe>();
	Fixture fixture(INTERPRETER_BACKEND, registerProbe(probe), 1, {}, PromotionPolicy::manual);
	auto compiled = fixture.compileOne();
	auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");
	REQUIRE(compiled.tier() == Tier::interpreted);
	REQUIRE(fn(1) == 2);
	REQUIRE(compiled.promote());
	REQUIRE(compiled.waitForPromotion(30s));
	REQUIRE(compiled.tier() == Tier::tier1);
	REQUIRE(fn(2) == 3);
}

TEST_CASE("Promotion Service - A Single-Tier Compile Has Nothing To Promote", "[promotion]") {
	Options options;
	options.setOption("engine.backend", std::string("tbc"));
	NautilusEngine engine(options);
	auto module = engine.createModule();
	module.registerFunction("add_one", promotionAddOne);
	auto compiled = module.compile();
	REQUIRE(compiled.tier() == Tier::tier1);
	REQUIRE(compiled.promotionStatus().state == PromotionState::none);
	REQUIRE(compiled.waitForPromotion(0ms));
	REQUIRE_FALSE(compiled.promote());
	REQUIRE(engine.waitForPromotions(0ms));
}

TEST_CASE("Promotion Service - The Tier Options Can Be Set Alone", "[promotion]") {
	SECTION("tier 1 alone") {
		const auto probe = std::make_shared<Probe>();
		const auto name = registerProbe(probe);
		Options options;
		options.setOption("engine.tier1.backend", name);
		NautilusEngine engine(options);
		REQUIRE(engine.getNameOfBackend().find(name) != std::string::npos);
		auto module = engine.createModule();
		module.registerFunction("add_one", promotionAddOne);
		auto compiled = module.compile();
		REQUIRE(compiled.getFunction<int32_t(int32_t)>("add_one")(1) == 2);
		REQUIRE(compiled.waitForPromotion(30s));
		REQUIRE(probe->entered.load() == 1);
	}
	SECTION("tier 0 alone") {
		Options options;
		options.setOption("engine.tier0.backend", std::string("tbc"));
		NautilusEngine engine(options);
		const auto name = engine.getNameOfBackend();
		// the default tier 1 of this build; single-tier if it is tbc itself
		REQUIRE((name == "tbc" || name.rfind("tiered(tbc,", 0) == 0));
		auto module = engine.createModule();
		module.registerFunction("add_one", promotionAddOne);
		auto compiled = module.compile();
		REQUIRE(compiled.getFunction<int32_t(int32_t)>("add_one")(1) == 2);
	}
	SECTION("an unknown backend fails when the engine is created") {
		Options options;
		options.setOption("engine.tier1.backend", std::string("no-such-backend"));
		REQUIRE_THROWS(NautilusEngine(options));
		Options tier0;
		tier0.setOption("engine.tier0.backend", std::string("no-such-backend"));
		REQUIRE_THROWS(NautilusEngine(tier0));
	}
}

TEST_CASE("Promotion Service - The Options Of The Service Are Checked", "[promotion]") {
	SECTION("promotion=none is not a policy") {
		Options options;
		options.setOption("engine.tiered.promotion", std::string("none"));
		REQUIRE_THROWS_WITH(NautilusEngine(options), Catch::Matchers::ContainsSubstring("engine.backend"));
	}
	SECTION("promotion=manual") {
		Options options;
		options.setOption("engine.tier0.backend", std::string("tbc"));
		options.setOption("engine.tier1.backend", std::string("tbc"));
		options.setOption("engine.tiered.promotion", std::string("manual"));
		REQUIRE_NOTHROW(NautilusEngine(options));
	}
	SECTION("threads and cap") {
		Options options;
		options.setOption("engine.tiered.promotionThreads", 0);
		REQUIRE_THROWS(NautilusEngine(options));
		Options cap;
		cap.setOption("engine.tiered.maxQueuedPromotions", -1);
		REQUIRE_THROWS(NautilusEngine(cap));
	}
}

TEST_CASE("Promotion Service - The Deprecated Alias Compiles Single-Tier And Wins Over The Policy", "[promotion]") {
	const auto probe = std::make_shared<Probe>();
	const auto name = registerProbe(probe);
	Options options;
	options.setOption("engine.tier0.backend", std::string("tbc"));
	options.setOption("engine.tier1.backend", name);
	options.setOption("engine.tiered.backgroundPromotion", false);
	SECTION("alone") {
	}
	SECTION("with engine.tiered.promotion") {
		options.setOption("engine.tiered.promotion", std::string("manual"));
	}
	NautilusEngine engine(options);
	REQUIRE(engine.getNameOfBackend() == name);
	auto module = engine.createModule();
	module.registerFunction("add_one", promotionAddOne);
	auto compiled = module.compile();
	REQUIRE(compiled.tier() == Tier::tier1);
	REQUIRE(compiled.promotionStatus().state == PromotionState::none);
	REQUIRE(probe->entered.load() == 1); // compiled directly with the tier-1 backend
}

TEST_CASE("Promotion Service - Concurrent Compiles And Drops Share The Pool", "[promotion]") {
	const auto probe = std::make_shared<Probe>();
	Fixture fixture("tbc", registerProbe(probe), 2);
	std::atomic<int> errors {0};
	std::vector<std::thread> threads;
	for (int t = 0; t < 4; ++t) {
		threads.emplace_back([&, t] {
			for (int i = 0; i < 100; ++i) {
				auto compiled = fixture.compileOne();
				auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");
				if (fn(i + t) != i + t + 1) {
					++errors;
				}
				if (i % 3 == 0) {
					compiled.waitForPromotion(50ms);
				}
				if (fn(i) != i + 1) {
					++errors;
				}
			}
		});
	}
	for (auto& thread : threads) {
		thread.join();
	}
	REQUIRE(errors.load() == 0);
	REQUIRE(fixture.engine->waitForPromotions(60s));
	REQUIRE(fixture.jit->promotionWorkerCount() <= 2);
}

// The long-running engine of 11 §6: 40,000 short-lived modules on one engine (a tenth of them under a sanitizer, which
// runs about twenty times slower). It completes, the thread count stays at the pool size and the virtual memory flat.
TEST_CASE("Promotion Service - 40000 Short-Lived Modules", "[promotion]") {
	const auto probe = std::make_shared<Probe>();
	const auto baseline = threadCount();
	Fixture fixture("tbc", registerProbe(probe), 2);
	long warm = 0;
	long end = 0;
	runShortLivedModules(fixture, SANITIZED ? 4000 : 40000, baseline, 2, &warm, &end);
	if (!SANITIZED && warm > 0) {
		REQUIRE(end - warm < 128 * 1024);
	}
}

#ifdef ENABLE_MLIR_BACKEND
// The workers of the service own the MLIR contexts, and the contexts own no thread pool: the thread count does not
// depend on the hardware concurrency even though `mlir.enableMultithreading` is on.
TEST_CASE("Promotion Service - The Thread Count Is Bounded With MLIR Workers", "[promotion][mlir]") {
	const auto baseline = threadCount();
	{
		Options options;
		options.setOption("mlir.enableMultithreading", true);
		Fixture fixture("tbc", "mlir", 2, {}, PromotionPolicy::eager, options);
		std::vector<CompiledModule> modules;
		int maxThreads = 0;
		for (int i = 0; i < 40; ++i) {
			modules.push_back(fixture.compileOne());
			maxThreads = std::max(maxThreads, threadCount());
			if (modules.size() > 8) {
				modules.erase(modules.begin());
			}
		}
		REQUIRE(fixture.engine->waitForPromotions(120s));
		maxThreads = std::max(maxThreads, threadCount());
		for (auto& module : modules) {
			REQUIRE(module.tier() == Tier::tier1);
			REQUIRE(module.getFunction<int32_t(int32_t)>("add_one")(1) == 2);
		}
		if (baseline > 0) {
			// the workers, plus the pool allowance of 11 §6 (there is none: contexts are created without one)
			REQUIRE(maxThreads <= baseline + 2 + 2);
		}
	}
}
#endif

} // namespace nautilus::engine

#endif
