#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/RuntimeBinding.hpp"
#include "nautilus/common/ExecutableImage.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/compiler/backends/mlir/ExceptionPersonality.hpp"
#include "nautilus/function.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_std.hpp"
#include <array>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#ifdef __linux__
#include <dlfcn.h>
#include <sys/personality.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace nautilus::engine {
namespace {

class TemporaryCacheDirectory {
public:
	TemporaryCacheDirectory() {
		static std::atomic<uint64_t> sequence {0};
		path_ = std::filesystem::temp_directory_path() /
		        ("nautilus-module-cache-test-" +
		         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + "-" +
		         std::to_string(sequence.fetch_add(1)));
		std::filesystem::create_directories(path_);
	}

	~TemporaryCacheDirectory() {
		std::error_code error;
		std::filesystem::remove_all(path_, error);
	}

	const std::filesystem::path& path() const {
		return path_;
	}

private:
	std::filesystem::path path_;
};

std::filesystem::path findArtifact(const std::filesystem::path& directory, std::string_view extension) {
	for (const auto& entry : std::filesystem::directory_iterator(directory)) {
		if (entry.path().extension() == extension) {
			return entry.path();
		}
	}
	return {};
}

std::string cacheStat(const CompiledModule& module, const std::string& name) {
	auto statistics = module.getStatistics();
	REQUIRE(statistics != nullptr);
	const auto* value = statistics->find(name);
	REQUIRE(value != nullptr);
	return std::get<std::string>(*value);
}

int64_t cacheIntStat(const CompiledModule& module, const std::string& name) {
	auto statistics = module.getStatistics();
	REQUIRE(statistics != nullptr);
	const auto* value = statistics->find(name);
	REQUIRE(value != nullptr);
	return std::get<int64_t>(*value);
}

struct GuardedCacheState {
	int64_t total = 0;
	int64_t live = 0;
	int64_t calls = 0;
	int64_t cleanupOrder = 0;

	int64_t readTotal() {
		return total;
	}
};

template <int32_t Marker>
struct GuardedCacheResource {
	GuardedCacheState* state;

	explicit GuardedCacheResource(GuardedCacheState* state) noexcept : state(state) {
		++state->live;
	}

	~GuardedCacheResource() noexcept {
		--state->live;
		state->cleanupOrder = state->cleanupOrder * 10 + Marker;
	}
};

int64_t guardedCacheProxy(GuardedCacheState* state, int64_t value) {
	++state->calls;
	if (value < 0) {
		throw std::runtime_error("guarded cache proxy");
	}
	state->total += value;
	return state->total;
}

void rawCleanup(int32_t* value) noexcept {
	++*value;
}

int32_t rawCleanupProxy(int32_t value) {
	if (value < 0) {
		throw std::runtime_error("raw cleanup proxy");
	}
	return value;
}

#ifdef __linux__
CompiledModule compileAbsoluteValueModule(const std::filesystem::path& cacheDirectory, int& traces) {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.Blob.CacheDir", cacheDirectory.string());
	options.setOption("engine.Blob.CacheKey", std::string("fresh-process-proxy-module-v1"));
	options.setOption("mlir.enableMultithreading", false);

	NautilusEngine engine(options);
	auto module = engine.createModule();
	module.registerFunction<val<int32_t>(val<int32_t>)>("absolute_value", [&traces](val<int32_t> value) {
		++traces;
		auto* absoluteValue = static_cast<int (*)(int)>(&std::abs);
		return invoke(absoluteValue, value);
	});
	return module.compile();
}
#endif

} // namespace

TEST_CASE("Persistent cache remains disabled without both cache options", "[cache][dispatch]") {
	for (const auto* setting : {"neither", "directory", "key"}) {
		DYNAMIC_SECTION(setting) {
			TemporaryCacheDirectory cache;
			Options options;
			options.setOption("engine.backend", std::string("mlir"));
			options.setOption("mlir.enableMultithreading", false);
			if (std::string_view(setting) == "directory") {
				options.setOption("engine.Blob.CacheDir", cache.path().string());
			} else if (std::string_view(setting) == "key") {
				options.setOption("engine.Blob.CacheKey", std::string("disabled-cache"));
			}
			int traces = 0;
			NautilusEngine engine(options);
			for (int iteration = 0; iteration < 2; ++iteration) {
				const auto before = traces;
				auto module = engine.createModule();
				module.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&](val<int32_t> value) {
					++traces;
					return value + 1;
				});
				auto compiled = module.compile();
				REQUIRE(compiled.getFunction<int32_t(int32_t)>("execute")(41) == 42);
				REQUIRE(traces > before);
				REQUIRE(cacheStat(compiled, "backend.name") == "mlir");
				REQUIRE(cacheStat(compiled, "tier") == "tier1");
				REQUIRE(compiled.getStatistics()->contains("compilation.totalMs"));
				REQUIRE(compiled.getStatistics()->contains("tracing.ms"));
				REQUIRE_FALSE(compiled.getStatistics()->contains("cache.eligible"));
			}
			REQUIRE(std::filesystem::is_empty(cache.path()));
		}
	}
}

TEST_CASE("Persistent cache falls back on non-Linux platforms", "[cache][dispatch]") {
#ifdef __linux__
	SKIP("Non-Linux cache fallback is checked on non-Linux platforms");
#endif
	TemporaryCacheDirectory cache;
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.Blob.CacheDir", cache.path().string());
	options.setOption("engine.Blob.CacheKey", std::string("unsupported-platform-cache"));
	options.setOption("mlir.enableMultithreading", false);
	int traces = 0;
	NautilusEngine engine(options);
	for (int iteration = 0; iteration < 2; ++iteration) {
		const auto before = traces;
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&](val<int32_t> value) {
			++traces;
			return value + 1;
		});
		auto compiled = module.compile();
		auto execute = compiled.getFunction<int32_t(int32_t)>("execute");
		REQUIRE(execute(41) == 42);
		REQUIRE(execute(-7) == -6);
		REQUIRE(traces > before);
		REQUIRE(cacheStat(compiled, "backend.name") == "mlir");
		REQUIRE(cacheStat(compiled, "tier") == "tier1");
		REQUIRE(cacheIntStat(compiled, "cache.eligible") == 0);
		REQUIRE(cacheIntStat(compiled, "cache.tracingRan") == 1);
		REQUIRE(cacheStat(compiled, "cache.object") == "not_used");
		REQUIRE(cacheStat(compiled, "cache.mlir") == "not_used");
		REQUIRE(cacheStat(compiled, "cache.fallback") == "missing_compiler_identity");
		REQUIRE(compiled.getStatistics()->contains("compilation.totalMs"));
		REQUIRE(compiled.getStatistics()->contains("tracing.ms"));
		REQUIRE(std::filesystem::is_empty(cache.path()));
	}
}

TEST_CASE("Persistent cache preserves debug and profiling source generation", "[cache][dispatch]") {
	for (const auto* setting : {"debug", "perf", "perf.sample"}) {
		DYNAMIC_SECTION(setting) {
			TemporaryCacheDirectory cache;
			Options options;
			options.setOption("engine.backend", std::string("mlir"));
			options.setOption("engine.Blob.CacheDir", cache.path().string());
			options.setOption("engine.Blob.CacheKey", std::string("debug-cache"));
			options.setOption("mlir.enableMultithreading", false);
			options.setOption(setting, true);
			int traces = 0;
			NautilusEngine engine(options);
			for (int iteration = 0; iteration < 2; ++iteration) {
				const auto before = traces;
				auto module = engine.createModule();
				module.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&](val<int32_t> value) {
					++traces;
					return value + 1;
				});
				auto compiled = module.compile();
				REQUIRE(compiled.getFunction<int32_t(int32_t)>("execute")(41) == 42);
				REQUIRE(traces > before);
				REQUIRE(cacheIntStat(compiled, "cache.eligible") == 0);
				REQUIRE(cacheIntStat(compiled, "cache.tracingRan") == 1);
				REQUIRE(cacheStat(compiled, "cache.fallback") == "debug_metadata_unsupported");
			}
			REQUIRE(std::filesystem::is_empty(cache.path()));
		}
	}
}

TEST_CASE("Persistent cache preserves explicit non-MLIR backend selection", "[cache][dispatch]") {
	auto* registry = compiler::CompilationBackendRegistry::getInstance();
	bool tested = false;
	for (const auto* backend : {"bc", "tbc", "asmjit", "cpp"}) {
		if (!registry->hasBackend(backend)) {
			continue;
		}
		tested = true;
		DYNAMIC_SECTION(backend) {
			TemporaryCacheDirectory cache;
			Options options;
			options.setOption("engine.backend", std::string(backend));
			options.setOption("engine.tier0.backend", std::string("interpreter"));
			options.setOption("engine.tier1.backend", std::string("mlir"));
			options.setOption("engine.tiered.backgroundPromotion", true);
			options.setOption("engine.Blob.CacheDir", cache.path().string());
			options.setOption("engine.Blob.CacheKey", std::string("explicit-backend"));
			NautilusEngine engine(options);
			REQUIRE(engine.getNameOfBackend() == backend);
			int traces = 0;
			for (int iteration = 0; iteration < 2; ++iteration) {
				const auto before = traces;
				auto module = engine.createModule();
				module.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&](val<int32_t> value) {
					++traces;
					return value + 1;
				});
				auto compiled = module.compile();
				REQUIRE(compiled.getFunction<int32_t(int32_t)>("execute")(41) == 42);
				REQUIRE(traces > before);
				REQUIRE(cacheStat(compiled, "backend.name") == backend);
				REQUIRE(cacheStat(compiled, "tier") == "tier1");
				REQUIRE(cacheIntStat(compiled, "cache.eligible") == 0);
				REQUIRE(cacheIntStat(compiled, "cache.tracingRan") == 1);
				REQUIRE(cacheStat(compiled, "cache.object") == "not_checked");
				REQUIRE(cacheStat(compiled, "cache.mlir") == "not_checked");
				REQUIRE(cacheStat(compiled, "cache.fallback") == "backend_not_mlir");
				REQUIRE(compiled.getStatistics()->contains("compilation.totalMs"));
				REQUIRE(compiled.getStatistics()->contains("tracing.ms"));
			}
			REQUIRE(std::filesystem::is_empty(cache.path()));
		}
	}
	if (!tested) {
		SKIP("No non-MLIR compilation backend available");
	}
}

TEST_CASE("Persistent cache supports single-tier MLIR configurations without tracing warm hits", "[cache][dispatch]") {
#ifndef __linux__
	SKIP("Persistent module caching requires Linux ELF build IDs");
#endif
	for (const bool explicitBackend : {false, true}) {
		DYNAMIC_SECTION("explicit backend=" << explicitBackend) {
			TemporaryCacheDirectory cache;
			Options options;
			options.setOption("engine.tier0.backend", std::string("interpreter"));
			options.setOption("engine.tier1.backend", std::string("mlir"));
			options.setOption("engine.tiered.backgroundPromotion", explicitBackend);
			if (explicitBackend) {
				options.setOption("engine.backend", std::string("mlir"));
			}
			options.setOption("engine.Blob.CacheDir", cache.path().string());
			options.setOption("engine.Blob.CacheKey", std::string("single-tier-dispatch"));
			options.setOption("mlir.enableMultithreading", false);
			int traces = 0;
			for (int iteration = 0; iteration < 2; ++iteration) {
				const auto before = traces;
				NautilusEngine engine(options);
				REQUIRE(engine.getNameOfBackend() == "mlir");
				auto module = engine.createModule();
				module.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&](val<int32_t> value) {
					++traces;
					return value + 1;
				});
				auto compiled = module.compile();
				REQUIRE(compiled.getFunction<int32_t(int32_t)>("execute")(41) == 42);
				REQUIRE((iteration == 0 ? traces > before : traces == before));
				REQUIRE(cacheStat(compiled, "backend.name") == "mlir");
				REQUIRE(cacheStat(compiled, "tier") == "tier1");
				REQUIRE(cacheIntStat(compiled, "cache.eligible") == 1);
				REQUIRE(cacheIntStat(compiled, "cache.tracingRan") == (iteration == 0 ? 1 : 0));
				REQUIRE(cacheStat(compiled, "cache.object") == (iteration == 0 ? "written" : "hit"));
				REQUIRE(cacheStat(compiled, "cache.fallback") == "none");
				REQUIRE_FALSE(cacheStat(compiled, "compilation.unitId").empty());
				REQUIRE(compiled.getStatistics()->contains("compilation.totalMs"));
				REQUIRE(compiled.getStatistics()->contains("tracing.ms") == (iteration == 0));
			}
		}
	}
}

TEST_CASE("Persistent cache preserves configured and default background promotion", "[cache][dispatch]") {
	auto* registry = compiler::CompilationBackendRegistry::getInstance();
	for (const auto* tier0 : {"default", "interpreter", "bc", "tbc", "asmjit", "cpp", "mlir"}) {
		const std::string name(tier0);
		if (name != "default" && name != "interpreter" && !registry->hasBackend(name)) {
			continue;
		}
		DYNAMIC_SECTION(tier0) {
			TemporaryCacheDirectory cache;
			Options options;
			if (name != "default") {
				options.setOption("engine.tier0.backend", name);
				options.setOption("engine.tier1.backend", std::string("mlir"));
				options.setOption("engine.tiered.backgroundPromotion", true);
			}
			options.setOption("engine.Blob.CacheDir", cache.path().string());
			options.setOption("engine.Blob.CacheKey", std::string("two-tier-dispatch"));
			options.setOption("mlir.enableMultithreading", false);
			common::ArenaPool traceArenaPool;
			common::ArenaPool irArenaPool;
			auto jit = std::make_unique<compiler::TieredJITCompiler>(options, traceArenaPool, irArenaPool);
			auto* compiler = jit.get();
			NautilusEngine engine(std::move(jit), options);
			const auto expectedTier0 = name != "default"                ? name
			                           : registry->hasBackend("asmjit") ? "asmjit"
			                           : registry->hasBackend("bc")     ? "bc"
			                                                            : INTERPRETER_BACKEND;
			REQUIRE(engine.getNameOfBackend() == "tiered(" + expectedTier0 + ",mlir)");
			int traces = 0;
			for (int iteration = 0; iteration < 2; ++iteration) {
				const auto before = traces;
				auto module = engine.createModule();
				REQUIRE(module.getOptions().getOptionOrDefault<std::string>("engine.Blob.CacheDir", "") ==
				        cache.path().string());
				REQUIRE(module.getOptions().getOptionOrDefault<std::string>("engine.Blob.CacheKey", "") ==
				        "two-tier-dispatch");
				module.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&](val<int32_t> value) {
					if (tracing::inTracer()) {
						++traces;
					}
					return value + 1;
				});
				auto compiled = module.compile();
				REQUIRE(traces > before);
				const auto afterTracing = traces;
				auto execute = compiled.getFunction<int32_t(int32_t)>("execute");
				REQUIRE(execute(41) == 42);
				compiler->waitForPendingPromotions();
				REQUIRE(compiler->allPromotionsComplete());
				REQUIRE(compiled.getState()->version.load() == 1);
				REQUIRE(execute(-7) == -6);
				REQUIRE(traces == afterTracing);
				REQUIRE(cacheStat(compiled, "backend.name") == "mlir");
				REQUIRE(cacheStat(compiled, "tier") == "tier1");
				REQUIRE(cacheIntStat(compiled, "cache.eligible") == 0);
				REQUIRE(cacheIntStat(compiled, "cache.tracingRan") == 0);
				REQUIRE(cacheStat(compiled, "cache.object") == "not_checked");
				REQUIRE(cacheStat(compiled, "cache.mlir") == "not_checked");
				REQUIRE(cacheStat(compiled, "cache.fallback") == "tiered_compilation");
				REQUIRE(compiled.getStatistics()->contains("compilation.totalMs"));
				REQUIRE_FALSE(compiled.getStatistics()->contains("tracing.ms"));
			}
			REQUIRE(std::filesystem::is_empty(cache.path()));
		}
	}
}

TEST_CASE("MLIR persistent module cache prefers native objects and retains bytecode fallback") {
#ifndef __linux__
	SKIP("Persistent module caching requires Linux ELF build IDs");
#endif
	TemporaryCacheDirectory cache;
	std::atomic<int> traces {0};

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.Blob.CacheDir", cache.path().string());
	options.setOption("engine.Blob.CacheKey", std::string("multi-function-module-v1"));
	options.setOption("mlir.enableMultithreading", false);

	auto compile = [&](bool reverseOrder) {
		NautilusEngine engine(options);
		auto module = engine.createModule();
		auto increment = [&](val<int32_t> value) {
			traces.fetch_add(1);
			return value + 1;
		};
		auto add = [&](val<int64_t> left, val<int64_t> right) {
			traces.fetch_add(1);
			return left + right;
		};
		if (reverseOrder) {
			module.registerFunction<val<int64_t>(val<int64_t>, val<int64_t>)>("add", add);
			module.registerFunction<val<int32_t>(val<int32_t>)>("increment", increment);
		} else {
			module.registerFunction<val<int32_t>(val<int32_t>)>("increment", increment);
			module.registerFunction<val<int64_t>(val<int64_t>, val<int64_t>)>("add", add);
		}
		return module.compile();
	};

	{
		auto module = compile(false);
		REQUIRE(module.getFunction<int32_t(int32_t)>("increment")(4) == 5);
		REQUIRE(module.getFunction<int64_t(int64_t, int64_t)>("add")(20, 22) == 42);
		REQUIRE(cacheStat(module, "cache.object") == "written");
		REQUIRE(cacheStat(module, "cache.mlir") == "written");
	}
	const auto coldTraceCount = traces.load();
	REQUIRE(coldTraceCount >= 2);

	const auto objectPath = findArtifact(cache.path(), ".o");
	const auto bytecodePath = findArtifact(cache.path(), ".mlirbc");
	const auto manifestPath = findArtifact(cache.path(), ".manifest");
	REQUIRE(!objectPath.empty());
	REQUIRE(!bytecodePath.empty());
	REQUIRE(!manifestPath.empty());

	{
		auto module = compile(true);
		REQUIRE(module.getFunction<int32_t(int32_t)>("increment")(8) == 9);
		REQUIRE(module.getFunction<int64_t(int64_t, int64_t)>("add")(1, 2) == 3);
		REQUIRE(cacheStat(module, "cache.object") == "hit");
		REQUIRE(traces.load() == coldTraceCount);
	}

	{
		std::ofstream corrupt(objectPath, std::ios::binary | std::ios::trunc);
		corrupt << "not an object";
	}
	{
		auto module = compile(false);
		REQUIRE(module.getFunction<int32_t(int32_t)>("increment")(10) == 11);
		REQUIRE(cacheStat(module, "cache.mlir") == "hit");
		REQUIRE(traces.load() == coldTraceCount);
	}
	{
		auto module = compile(false);
		REQUIRE(module.getFunction<int64_t(int64_t, int64_t)>("add")(40, 2) == 42);
		REQUIRE(cacheStat(module, "cache.object") == "hit");
		REQUIRE(traces.load() == coldTraceCount);
	}
}

TEST_CASE("MLIR persistent module cache rebinds proxy functions without persisting their addresses") {
#ifndef __linux__
	SKIP("Persistent executable imports require Linux ELF build IDs");
#endif
	TemporaryCacheDirectory cache;
	std::atomic<int> traces {0};
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.Blob.CacheDir", cache.path().string());
	options.setOption("engine.Blob.CacheKey", std::string("proxy-module-v1"));
	options.setOption("mlir.enableMultithreading", false);

	auto compile = [&]() {
		NautilusEngine engine(options);
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>(val<int32_t>)>("call_proxy", [&](val<int32_t> value) {
			traces.fetch_add(1);
			auto* absoluteValue = static_cast<int (*)(int)>(&std::abs);
			return invoke(absoluteValue, value);
		});
		return module.compile();
	};

	{
		auto module = compile();
		REQUIRE(module.getFunction<int32_t(int32_t)>("call_proxy")(-42) == 42);
	}
	const auto coldTraceCount = traces.load();
	{
		auto module = compile();
		REQUIRE(module.getFunction<int32_t(int32_t)>("call_proxy")(-22) == 22);
		REQUIRE(cacheStat(module, "cache.object") == "hit");
		REQUIRE(traces.load() == coldTraceCount);
	}
}

#ifdef __linux__
TEST_CASE("MLIR persistent module cache loads native objects in a fresh process") {
	static constexpr auto CHILD_CACHE_DIRECTORY = "NAUTILUS_CACHE_CHILD_DIRECTORY";
	static constexpr auto CHILD_MODE = "NAUTILUS_CACHE_CHILD_MODE";
	if (const auto* cacheDirectory = std::getenv(CHILD_CACHE_DIRECTORY)) {
		const auto* mode = std::getenv(CHILD_MODE);
		REQUIRE(mode != nullptr);
		const bool warm = std::string_view(mode) == "warm";
		REQUIRE((warm || std::string_view(mode) == "cold"));
		CAPTURE(mode);
		int traces = 0;
		auto module = compileAbsoluteValueModule(cacheDirectory, traces);
		INFO(module.getStatistics()->toString());
		auto absoluteValue = module.getFunction<int32_t(int32_t)>("absolute_value");
		REQUIRE(absoluteValue(-42) == 42);
		REQUIRE(absoluteValue(21) == 21);
		REQUIRE(cacheIntStat(module, "cache.eligible") == 1);
		REQUIRE(cacheStat(module, "cache.object") == (warm ? "hit" : "written"));
		REQUIRE(cacheStat(module, "cache.mlir") == (warm ? "not_checked" : "written"));
		REQUIRE(cacheStat(module, "cache.fallback") == "none");
		REQUIRE(cacheIntStat(module, "cache.tracingRan") == (warm ? 0 : 1));
		REQUIRE((warm ? traces == 0 : traces > 0));
		std::ofstream report(std::filesystem::path(cacheDirectory) / (std::string(mode) + ".statistics"));
		report << cacheStat(module, "cache.key") << '\n' << traces << '\n';
		report << module.getStatistics()->toString();
		report.close();
		REQUIRE(report.good());
		return;
	}

	TemporaryCacheDirectory cache;
	const auto executable = std::filesystem::read_symlink("/proc/self/exe");
	for (const auto* mode : {"cold", "warm"}) {
		CAPTURE(mode);
		const auto child = ::fork();
		REQUIRE(child >= 0);
		if (child == 0) {
			if (::setenv(CHILD_CACHE_DIRECTORY, cache.path().c_str(), 1) != 0 || ::setenv(CHILD_MODE, mode, 1) != 0) {
				::_exit(125);
			}
			::execl(executable.c_str(), executable.c_str(),
			        "MLIR persistent module cache loads native objects in a fresh process", "--reporter", "compact",
			        static_cast<char*>(nullptr));
			::_exit(126);
		}
		int status = 0;
		pid_t waited;
		do {
			waited = ::waitpid(child, &status, 0);
		} while (waited < 0 && errno == EINTR);
		REQUIRE(waited == child);
		REQUIRE(WIFEXITED(status));
		REQUIRE(WEXITSTATUS(status) == 0);
		for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
			REQUIRE_FALSE(findArtifact(cache.path(), extension).empty());
		}
	}
	std::string coldKey, warmKey;
	int coldTraces = 0, warmTraces = -1;
	std::ifstream coldReport(cache.path() / "cold.statistics");
	std::ifstream warmReport(cache.path() / "warm.statistics");
	REQUIRE(static_cast<bool>(coldReport >> coldKey >> coldTraces));
	REQUIRE(static_cast<bool>(warmReport >> warmKey >> warmTraces));
	REQUIRE(coldKey == warmKey);
	REQUIRE(coldTraces > 0);
	REQUIRE(warmTraces == 0);
}
#endif

TEST_CASE("MLIR guarded cache preserves cleanup and isolates rebound state on object and MLIR hits", "[cache][guard]") {
#ifndef __linux__
	SKIP("Persistent executable imports require Linux ELF build IDs");
#endif
	TemporaryCacheDirectory cache;
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.Blob.CacheDir", cache.path().string());
	options.setOption("engine.Blob.CacheKey", std::string("guarded-rebinding-v1"));
	options.setOption("mlir.enableMultithreading", false);
	int traces = 0;
	auto compile = [&](GuardedCacheState& state) {
		RuntimeBindings bindings;
		auto binding = bindings.bind<GuardedCacheState>("guarded/state", &state);
		NautilusEngine engine(options);
		auto module = engine.createModule();
		module.setRuntimeBindings(bindings);
		module.registerFunction<val<int64_t>(val<int64_t>)>("execute", [binding, &traces](val<int64_t> value) {
			++traces;
			auto address = binding.get();
			val<GuardedCacheResource<1>> outer(address);
			val<GuardedCacheResource<2>> inner(address);
			return invoke(guardedCacheProxy, address, value);
		});
		return module.compile();
	};
	auto check = [](CompiledModule& module, GuardedCacheState& state) {
		REQUIRE(state.live == 0);
		REQUIRE(state.calls == 0);
		REQUIRE(state.cleanupOrder == 0);
		const auto before = state.total;
		auto execute = module.getFunction<int64_t(int64_t)>("execute");
		REQUIRE(execute(7) == before + 7);
		REQUIRE(state.live == 0);
		REQUIRE(state.cleanupOrder == 21);
		REQUIRE_THROWS_WITH(execute(-1), "guarded cache proxy");
		REQUIRE(state.total == before + 7);
		REQUIRE(state.live == 0);
		REQUIRE(state.cleanupOrder == 2121);
		REQUIRE(execute(3) == before + 10);
		REQUIRE(state.live == 0);
		REQUIRE(state.cleanupOrder == 212121);
		REQUIRE(state.calls == 3);
	};

	GuardedCacheState first {10}, second {100}, third {1000}, fourth {10000};
	{
		auto cold = compile(first);
		INFO(cold.getStatistics()->toString());
		REQUIRE(cacheStat(cold, "cache.object") == "written");
		REQUIRE(cacheStat(cold, "cache.mlir") == "written");
		REQUIRE(cacheStat(cold, "cache.fallback") == "none");
		REQUIRE(cacheIntStat(cold, "cache.tracingRan") == 1);
		const auto coldTraces = traces;
		REQUIRE(coldTraces > 0);
		check(cold, first);

		auto warm = compile(second);
		REQUIRE(cacheStat(warm, "cache.object") == "hit");
		REQUIRE(cacheStat(warm, "cache.fallback") == "none");
		REQUIRE(cacheIntStat(warm, "cache.tracingRan") == 0);
		check(warm, second);

		const auto object = findArtifact(cache.path(), ".o");
		REQUIRE_FALSE(object.empty());
		{
			std::ofstream corrupt(object, std::ios::binary | std::ios::trunc);
			corrupt << "corrupt guarded object";
		}
		auto repaired = compile(third);
		REQUIRE(cacheStat(repaired, "cache.object") == "invalid_rewritten");
		REQUIRE(cacheStat(repaired, "cache.mlir") == "hit");
		REQUIRE(cacheIntStat(repaired, "cache.tracingRan") == 0);
		check(repaired, third);
		auto warmRepaired = compile(fourth);
		REQUIRE(cacheStat(warmRepaired, "cache.object") == "hit");
		REQUIRE(cacheIntStat(warmRepaired, "cache.tracingRan") == 0);
		check(warmRepaired, fourth);
		REQUIRE(traces == coldTraces);
		REQUIRE(first.total == 20);
		REQUIRE(second.total == 110);
		REQUIRE(third.total == 1010);
		REQUIRE(first.calls == 3);
		REQUIRE(second.calls == 3);
		REQUIRE(third.calls == 3);
		REQUIRE(cold.getFunction<int64_t(int64_t)>("execute")(1) == 21);
		REQUIRE(first.live == 0);
		REQUIRE(first.cleanupOrder == 21212121);
		REQUIRE(second.total == 110);
	}
	std::ifstream manifest(findArtifact(cache.path(), ".manifest"), std::ios::binary);
	const std::string bytes(std::istreambuf_iterator<char> {manifest}, std::istreambuf_iterator<char> {});
	REQUIRE(bytes.starts_with("NMCACHE"));
	REQUIRE(bytes.find("nautilus.persistent.module-cache") != std::string::npos);
	REQUIRE(bytes.find("nautilus.persistent.module-cache.v") == std::string::npos);
	REQUIRE(bytes.find("nautilus.runtime-bindings:") != std::string::npos);
	REQUIRE(bytes.find("nautilus.runtime-bindings.v") == std::string::npos);
	REQUIRE(bytes.find("nautilus.mlir.module.v") == std::string::npos);
	REQUIRE(bytes.find("__gxx_personality_v0") != std::string::npos);
}

#ifdef __linux__
TEST_CASE("MLIR guarded cache forwards personality across ASLR fresh exec processes", "[cache][guard]") {
#if !__has_include(<unwind.h>) || defined(__arm__) || defined(__USING_SJLJ_EXCEPTIONS__)
	SKIP("The native personality bridge requires Linux DWARF unwinding headers");
#endif
	static constexpr auto CHILD_DIRECTORY = "NAUTILUS_GUARDED_CHILD_DIRECTORY";
	static constexpr auto CHILD_MODE = "NAUTILUS_GUARDED_CHILD_MODE";
	if (const auto* directory = std::getenv(CHILD_DIRECTORY)) {
		const auto personality = ::personality(0xffffffffUL);
		REQUIRE(personality != -1);
		REQUIRE((personality & ADDR_NO_RANDOMIZE) == 0);
		const auto* mode = std::getenv(CHILD_MODE);
		REQUIRE(mode != nullptr);
		const bool warm = std::string_view(mode) == "warm";
		REQUIRE((warm || std::string_view(mode) == "cold"));
		auto storage = std::make_unique<std::array<GuardedCacheState, 2>>();
		auto* state = &(*storage)[warm ? 1 : 0];
		state->total = warm ? 100 : 10;
		RuntimeBindings bindings;
		auto binding = bindings.bind<GuardedCacheState>("guarded/state", state);
		Options options;
		options.setOption("engine.backend", std::string("mlir"));
		options.setOption("engine.Blob.CacheDir", std::string(directory));
		options.setOption("engine.Blob.CacheKey", std::string("fresh-exec-guarded-personality-v1"));
		options.setOption("mlir.enableMultithreading", false);
		options.setRuntimeBindings(bindings);
		int traces = 0;
		NautilusEngine engine(options);
		auto module = engine.createModule();
		module.registerFunction<val<int64_t>(val<int64_t>)>("execute", [binding, &traces](val<int64_t> value) {
			++traces;
			auto address = binding.get();
			val<GuardedCacheResource<1>> outer(address);
			val<GuardedCacheResource<2>> inner(address);
			return invoke(guardedCacheProxy, address, value);
		});
		auto compiled = module.compile();
		REQUIRE(cacheStat(compiled, "cache.object") == (warm ? "hit" : "written"));
		REQUIRE(cacheStat(compiled, "cache.fallback") == "none");
		REQUIRE(cacheIntStat(compiled, "cache.tracingRan") == (warm ? 0 : 1));
		REQUIRE((warm ? traces == 0 : traces > 0));
		auto execute = compiled.getFunction<int64_t(int64_t)>("execute");
		REQUIRE(execute(7) == (warm ? 107 : 17));
		REQUIRE(state->live == 0);
		REQUIRE(state->cleanupOrder == 21);
		REQUIRE_THROWS_WITH(execute(-1), "guarded cache proxy");
		REQUIRE(state->live == 0);
		REQUIRE(state->cleanupOrder == 2121);
		REQUIRE(execute(3) == (warm ? 110 : 20));
		REQUIRE(state->live == 0);
		REQUIRE(state->cleanupOrder == 212121);
		REQUIRE(state->calls == 3);
		auto* bridge = compiler::mlir::getExceptionPersonalityAddress();
		auto* runtime = ::dlsym(RTLD_DEFAULT, "__gxx_personality_v0");
		REQUIRE(runtime != nullptr);
		REQUIRE(bridge != runtime);
		const auto image = common::locateExecutableAddress(bridge);
		REQUIRE(image.has_value());
		REQUIRE_FALSE(image->buildId.empty());
		REQUIRE(common::resolveExecutableAddress(*image) == bridge);
		std::ofstream report(std::filesystem::path(directory) / (std::string(mode) + ".addresses"));
		report << reinterpret_cast<uintptr_t>(bridge) << ' ' << reinterpret_cast<uintptr_t>(runtime) << ' '
		       << reinterpret_cast<uintptr_t>(state) << '\n';
		report << image->buildId << ' ' << image->loadOffset << ' ' << cacheStat(compiled, "cache.key") << '\n';
		report << compiled.getStatistics()->toString();
		report.close();
		REQUIRE(report.good());
		return;
	}

	TemporaryCacheDirectory cache;
	const auto executable = std::filesystem::read_symlink("/proc/self/exe");
	for (const auto* mode : {"cold", "warm"}) {
		const auto child = ::fork();
		REQUIRE(child >= 0);
		if (child == 0) {
			const auto personality = ::personality(0xffffffffUL);
			if (personality == -1 || ::personality(personality & ~ADDR_NO_RANDOMIZE) == -1) {
				::_exit(124);
			}
			if (::setenv(CHILD_DIRECTORY, cache.path().c_str(), 1) != 0 || ::setenv(CHILD_MODE, mode, 1) != 0) {
				::_exit(125);
			}
			::execl(executable.c_str(), executable.c_str(),
			        "MLIR guarded cache forwards personality across ASLR fresh exec processes", "--reporter", "compact",
			        static_cast<char*>(nullptr));
			::_exit(126);
		}
		int status = 0;
		pid_t waited;
		do {
			waited = ::waitpid(child, &status, 0);
		} while (waited < 0 && errno == EINTR);
		REQUIRE(waited == child);
		REQUIRE(WIFEXITED(status));
		REQUIRE(WEXITSTATUS(status) == 0);
	}
	std::array<uintptr_t, 3> cold {}, warm {};
	std::string coldBuildId, warmBuildId, coldKey, warmKey;
	uint64_t coldOffset = 0, warmOffset = 0;
	std::ifstream coldReport(cache.path() / "cold.addresses");
	std::ifstream warmReport(cache.path() / "warm.addresses");
	REQUIRE(static_cast<bool>(coldReport >> cold[0] >> cold[1] >> cold[2] >> coldBuildId >> coldOffset >> coldKey));
	REQUIRE(static_cast<bool>(warmReport >> warm[0] >> warm[1] >> warm[2] >> warmBuildId >> warmOffset >> warmKey));
	REQUIRE(coldBuildId == warmBuildId);
	REQUIRE(coldOffset == warmOffset);
	REQUIRE(coldKey == warmKey);
	for (std::size_t index = 0; index < cold.size(); ++index) {
		CAPTURE(index, cold[index], warm[index]);
		REQUIRE(cold[index] != warm[index]);
		for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
			std::ifstream input(findArtifact(cache.path(), extension), std::ios::binary);
			REQUIRE(input.good());
			const std::string bytes(std::istreambuf_iterator<char> {input}, std::istreambuf_iterator<char> {});
			for (const auto address : {cold[index], warm[index]}) {
				const std::string raw(reinterpret_cast<const char*>(&address), sizeof(address));
				REQUIRE(bytes.find(raw) == std::string::npos);
				REQUIRE(bytes.find(std::to_string(address)) == std::string::npos);
			}
		}
	}
}

TEST_CASE("MLIR guarded cache rejects callbacks in an unidentified executable image", "[cache][guard]") {
	std::unique_ptr<void, decltype(&::dlclose)> library(
	    ::dlopen(NAUTILUS_UNIDENTIFIED_CACHE_PROXY, RTLD_NOW | RTLD_LOCAL), &::dlclose);
	REQUIRE(library != nullptr);
	auto* address = ::dlsym(library.get(), "unidentifiedCacheProxy");
	REQUIRE(address != nullptr);
	REQUIRE_FALSE(common::locateExecutableAddress(address).has_value());
	auto callback = reinterpret_cast<int32_t (*)(int32_t)>(address);
	REQUIRE(callback(4) == 5);
	TemporaryCacheDirectory cache;
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.Blob.CacheDir", cache.path().string());
	options.setOption("engine.Blob.CacheKey", std::string("unidentified-callback-v1"));
	options.setOption("mlir.enableMultithreading", false);
	for (int iteration = 0; iteration < 2; ++iteration) {
		NautilusEngine engine(options);
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>(val<int32_t>)>(
		    "execute", [callback](val<int32_t> value) { return invoke(callback, value); });
		auto compiled = module.compile();
		REQUIRE(compiled.getFunction<int32_t(int32_t)>("execute")(19) == 20);
		REQUIRE_THAT(cacheStat(compiled, "cache.fallback"),
		             Catch::Matchers::StartsWith("artifact_publication_failed:external symbol is not in a relocatable "
		                                         "executable image:"));
		REQUIRE(cacheIntStat(compiled, "cache.tracingRan") == 1);
		for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
			REQUIRE(findArtifact(cache.path(), extension).empty());
		}
	}
}
#endif

TEST_CASE("MLIR guarded cache rejects raw heap cleanup addresses", "[cache][guard]") {
#ifndef __linux__
	SKIP("Persistent module caching requires Linux ELF build IDs");
#endif
	for (const bool encoded : {false, true}) {
		CAPTURE(encoded);
		TemporaryCacheDirectory cache;
		auto state = std::make_unique<int32_t>(0);
		Options options;
		options.setOption("engine.backend", std::string("mlir"));
		options.setOption("engine.Blob.CacheDir", cache.path().string());
		options.setOption("engine.Blob.CacheKey", std::string("raw-cleanup-v1"));
		options.setOption("mlir.enableMultithreading", false);
		for (int iteration = 0; iteration < 2; ++iteration) {
			NautilusEngine engine(options);
			auto module = engine.createModule();
			module.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&](val<int32_t> value) {
				val<int32_t*> address = encoded
				                            ? val<int32_t*>(val<uintptr_t>(reinterpret_cast<uintptr_t>(state.get())))
				                            : val<int32_t*>(state.get());
				tracing::registerDestructor(address.getState(), reinterpret_cast<void*>(rawCleanup));
				auto result = invoke(rawCleanupProxy, value);
				tracing::unregisterDestructor(address.getState());
				return result;
			});
			auto compiled = module.compile();
			REQUIRE(cacheStat(compiled, "cache.fallback") == "non_relocatable_pointer");
			REQUIRE(cacheIntStat(compiled, "cache.tracingRan") == 1);
			auto execute = compiled.getFunction<int32_t(int32_t)>("execute");
			REQUIRE(execute(7) == 7);
			REQUIRE(*state == iteration);
			REQUIRE_THROWS_WITH(execute(-1), "raw cleanup proxy");
			REQUIRE(*state == iteration + 1);
			for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
				REQUIRE(findArtifact(cache.path(), extension).empty());
			}
		}
	}
}

TEST_CASE("MLIR guarded cache rejects heap member-function wrapper state", "[cache][guard]") {
#ifndef __linux__
	SKIP("Persistent module caching requires Linux ELF build IDs");
#endif
	TemporaryCacheDirectory cache;
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.Blob.CacheDir", cache.path().string());
	options.setOption("engine.Blob.CacheKey", std::string("member-wrapper-state-v1"));
	options.setOption("mlir.enableMultithreading", false);
	GuardedCacheState state {42};
	RuntimeBindings bindings;
	auto binding = bindings.bind<GuardedCacheState>("state", &state);
	for (int iteration = 0; iteration < 2; ++iteration) {
		NautilusEngine engine(options);
		auto module = engine.createModule();
		module.setRuntimeBindings(bindings);
		module.registerFunction<val<int64_t>()>(
		    "execute", [binding] { return memberFunc<&GuardedCacheState::readTotal>()(binding.get()); });
		auto compiled = module.compile();
		REQUIRE(compiled.getFunction<int64_t()>("execute")() == 42);
		REQUIRE(cacheStat(compiled, "cache.fallback") == "non_relocatable_pointer");
		REQUIRE(cacheIntStat(compiled, "cache.tracingRan") == 1);
		for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
			REQUIRE(findArtifact(cache.path(), extension).empty());
		}
	}
}

TEST_CASE("MLIR persistent module cache rejects direct pointer constants") {
#ifndef __linux__
	SKIP("Persistent module caching requires Linux ELF build IDs");
#endif
	bool integerEncoded = false;
	SECTION("pointer literal") {
	}
	SECTION("integer-encoded pointer literal") {
		integerEncoded = true;
	}
	TemporaryCacheDirectory cache;
	std::atomic<int> traces {0};
	int value = 42;
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.Blob.CacheDir", cache.path().string());
	options.setOption("engine.Blob.CacheKey", std::string("direct-pointer-module-v1"));
	options.setOption("mlir.enableMultithreading", false);

	for (int iteration = 0; iteration < 2; ++iteration) {
		NautilusEngine engine(options);
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>()>("load_pointer", [&]() {
			traces.fetch_add(1);
			if (integerEncoded) {
				val<uintptr_t> address = reinterpret_cast<uintptr_t>(&value);
				val<int32_t*> pointer = address;
				return *pointer;
			}
			return *val<int32_t*>(&value);
		});
		auto compiled = module.compile();
		REQUIRE(compiled.getFunction<int32_t()>("load_pointer")() == 42);
		REQUIRE(cacheStat(compiled, "cache.fallback") == "non_relocatable_pointer");
	}
	REQUIRE(traces.load() >= 2);
	for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
		REQUIRE(findArtifact(cache.path(), extension).empty());
	}
}

} // namespace nautilus::engine
