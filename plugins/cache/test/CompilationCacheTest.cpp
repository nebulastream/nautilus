#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/cache/plugin.hpp"
#include "nautilus/common/ExecutableImage.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/compiler/backends/mlir/ExceptionPersonality.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/select.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_std.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <barrier>
#include <bit>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>
#ifdef ENABLE_LOGGING
#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>
#endif
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
		        ("nautilus-plugin-cache-test-" +
		         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + "-" +
		         std::to_string(sequence.fetch_add(1)));
		std::filesystem::create_directories(path_);
		std::filesystem::permissions(path_, std::filesystem::perms::owner_all);
	}

	~TemporaryCacheDirectory() {
		std::error_code ignored;
		std::filesystem::permissions(path_, std::filesystem::perms::owner_all, std::filesystem::perm_options::add,
		                             ignored);
		std::filesystem::remove_all(path_, ignored);
	}

	const std::filesystem::path& path() const {
		return path_;
	}

private:
	std::filesystem::path path_;
};

Options cacheOptions(const std::filesystem::path& directory, const std::string& key) {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.cache.directory", directory.string());
	options.setOption("engine.cache.key", key);
	options.setOption("mlir.enableMultithreading", false);
	return options;
}

void requireCachePlatform() {
#if !defined(__linux__) || !defined(__x86_64__)
	SKIP("Persistent native caching requires the supported Linux x86-64 ELF producer");
#endif
}

template <typename T>
T cacheStat(const CompiledModule& module, const std::string& name) {
	const auto statistics = module.getStatistics();
	REQUIRE(statistics != nullptr);
	INFO(statistics->toString());
	const auto* value = statistics->find(name);
	REQUIRE(value != nullptr);
	REQUIRE(std::holds_alternative<T>(*value));
	return std::get<T>(*value);
}

void requireNoCacheStatistics(const CompiledModule& module) {
	const auto statistics = module.getStatistics();
	REQUIRE(statistics != nullptr);
	for (const auto& [name, value] : *statistics) {
		CAPTURE(name);
		REQUIRE_FALSE(name.starts_with("cache."));
	}
}

void requireNoFrontendStatistics(const CompiledModule& module) {
	const auto statistics = module.getStatistics();
	REQUIRE(statistics != nullptr);
	for (const auto* name : {"tracing.ms", "ssaCreation.ms", "irGeneration.ms", "frontend.totalMs"}) {
		REQUIRE_FALSE(statistics->contains(name));
	}
	for (const auto& [name, value] : *statistics) {
		CAPTURE(name);
		REQUIRE_FALSE(name.starts_with("ir."));
		REQUIRE_FALSE(name.starts_with("irPasses."));
	}
	REQUIRE(statistics->contains("compilation.totalMs"));
	REQUIRE(statistics->contains("backend.totalMs"));
}

void requireNativeHit(const CompiledModule& module) {
	REQUIRE(cacheStat<std::string>(module, "cache.object") == "hit");
	REQUIRE(cacheStat<std::string>(module, "cache.mlir") == "not_checked");
	REQUIRE(cacheStat<int64_t>(module, "cache.tracingRan") == 0);
	REQUIRE(cacheStat<std::string>(module, "cache.fallback") == "none");
	requireNoFrontendStatistics(module);
	const auto statistics = module.getStatistics();
	for (const auto& [name, value] : *statistics) {
		CAPTURE(name);
		REQUIRE_FALSE(name.starts_with("mlir."));
		REQUIRE_FALSE(name.starts_with("llvm."));
	}
	REQUIRE_FALSE(statistics->contains("jit.compile.ms"));
	REQUIRE(statistics->contains("jit.objectLoad.ms"));
}

void requireWritten(const CompiledModule& module) {
	REQUIRE(cacheStat<std::string>(module, "cache.object") == "written");
	REQUIRE(cacheStat<std::string>(module, "cache.mlir") == "written");
	REQUIRE(cacheStat<int64_t>(module, "cache.tracingRan") == 1);
	REQUIRE(cacheStat<std::string>(module, "cache.fallback") == "none");
	REQUIRE(module.getStatistics()->contains("frontend.totalMs"));
	REQUIRE(module.getStatistics()->contains("tracing.ms"));
	const auto certificate = cacheStat<int64_t>(module, "cache.scalarCertificate");
	REQUIRE((certificate == 0 || certificate == 1));
	REQUIRE(cacheStat<std::string>(module, "cache.scalarRejection").empty() == (certificate == 1));
}

void requireFallback(const CompiledModule& module, int64_t tracingRan = 1) {
	REQUIRE(cacheStat<std::string>(module, "cache.object") != "hit");
	REQUIRE(cacheStat<std::string>(module, "cache.mlir") != "hit");
	REQUIRE(cacheStat<int64_t>(module, "cache.tracingRan") == tracingRan);
	const auto reason = cacheStat<std::string>(module, "cache.fallback");
	REQUIRE_FALSE(reason.empty());
	REQUIRE(reason != "none");
	REQUIRE(module.getStatistics()->contains("compilation.totalMs"));
}

void writeFile(const std::filesystem::path& path, std::string_view contents) {
	std::ofstream output(path, std::ios::binary | std::ios::trunc);
	REQUIRE(output.is_open());
	output.write(contents.data(), static_cast<std::streamsize>(contents.size()));
	output.close();
	REQUIRE_FALSE(output.fail());
}

std::string readFile(const std::filesystem::path& path) {
	std::ifstream input(path, std::ios::binary);
	REQUIRE(input.is_open());
	std::string contents {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
	REQUIRE_FALSE(input.bad());
	return contents;
}

std::set<std::filesystem::path> debugSourceFiles() {
	std::set<std::filesystem::path> files;
#ifdef __linux__
	const auto prefix = "nautilus_debug_" + std::to_string(::getpid()) + "_";
#else
	const std::string prefix = "nautilus_debug_";
#endif
	for (const auto& directory : {std::filesystem::temp_directory_path(), std::filesystem::current_path()}) {
		for (const auto& entry : std::filesystem::directory_iterator(directory)) {
			if (entry.is_regular_file() && entry.path().extension() == ".ir" &&
			    entry.path().filename().string().starts_with(prefix)) {
				files.insert(entry.path());
			}
		}
	}
	return files;
}

std::map<std::string, std::string> readArtifacts(const std::filesystem::path& directory) {
	std::map<std::string, std::string> result;
	for (const auto& entry : std::filesystem::directory_iterator(directory)) {
		const auto extension = entry.path().extension();
		if (entry.is_regular_file() && (extension == ".o" || extension == ".mlirbc" || extension == ".manifest")) {
			result.emplace(entry.path().filename().string(), readFile(entry.path()));
		}
	}
	return result;
}

std::filesystem::path artifactPath(const CompiledModule& module, const std::filesystem::path& directory,
                                   const std::string& extension) {
	const auto key = cacheStat<std::string>(module, "cache.key");
	REQUIRE_FALSE(key.empty());
	return directory / (key + extension);
}

void requireArtifacts(const CompiledModule& module, const std::filesystem::path& directory) {
	for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
		const auto path = artifactPath(module, directory, extension);
		REQUIRE(std::filesystem::is_regular_file(path));
		REQUIRE(std::filesystem::file_size(path) > 0);
	}
}

CompiledModule compileIncrement(const NautilusEngine& engine, int& traces, int32_t increment = 1,
                                ModuleOptions overrides = {}) {
	auto module = engine.createModule(std::move(overrides));
	module.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&traces, increment](val<int32_t> value) {
		++traces;
		return value + cacheInvariant(increment);
	});
	return module.compile();
}

CompiledModule compileIncrement(const Options& options, int& traces, int32_t increment = 1) {
	NautilusEngine engine(cache::createCompiler(options), options);
	return compileIncrement(engine, traces, increment);
}

std::vector<std::pair<std::string, ModuleOptions>> validationModes() {
	std::vector<std::pair<std::string, ModuleOptions>> result;
	for (const auto* mode : {"passes disabled", "optimization disabled", "one iteration", "fixed point"}) {
		ModuleOptions options;
		options.setOption("ir.runPasses", std::string_view(mode) != "passes disabled");
		options.setOption("ir.runOptimizationPasses", std::string_view(mode) != "optimization disabled");
		options.setOption("ir.maxPipelineIterations", std::string_view(mode) == "fixed point" ? 4 : 1);
		result.emplace_back(mode, std::move(options));
	}
	return result;
}

int64_t cacheProxy(int64_t value) noexcept {
	return value * 3 - 4;
}

double cacheConvertedProxy(double value) noexcept {
	return value + 1.0;
}

uintptr_t cacheProxyAddress() noexcept {
	return reinterpret_cast<uintptr_t>(&cacheProxy);
}

struct CacheFailure : std::runtime_error {
	explicit CacheFailure(int32_t call) : std::runtime_error("cache guarded native failure"), call(call) {
	}
	int32_t call;
};

int32_t cacheMaybeThrow(int32_t* state, bool fail) {
	++state[2];
	if (fail) {
		throw CacheFailure(state[2]);
	}
	return state[2];
}

void firstCleanup(int32_t* state) noexcept {
	state[0] = state[0] * 10 + 1;
	++state[1];
}

void secondCleanup(int32_t* state) noexcept {
	state[0] = state[0] * 10 + 2;
	++state[1];
}

class NativeGuard {
public:
	NativeGuard(val<int32_t*> state, void (*cleanup)(int32_t*) noexcept) : state_(std::move(state)), cleanup_(cleanup) {
		if (tracing::inTracer()) {
			tracing::registerDestructor(state_.state, reinterpret_cast<void*>(cleanup_));
		}
	}

	~NativeGuard() noexcept {
		if (tracing::inTracer()) {
			tracing::unregisterDestructor(state_.state);
		}
		invoke(cleanup_, state_);
	}

	NativeGuard(const NativeGuard&) = delete;
	NativeGuard& operator=(const NativeGuard&) = delete;

private:
	val<int32_t*> state_;
	void (*cleanup_)(int32_t*) noexcept;
};

struct alignas(64) CacheOwnedBuffer {
	int32_t* state = nullptr;
	int32_t id = 0;

	CacheOwnedBuffer() noexcept = default;
	CacheOwnedBuffer(int32_t* state, int32_t id) noexcept : state(state), id(id) {
		++state[2];
	}
	CacheOwnedBuffer(const CacheOwnedBuffer& other) noexcept : CacheOwnedBuffer(other.state, other.id + 1) {
	}
	~CacheOwnedBuffer() noexcept {
		if (state) {
			state[0] = state[0] * 10 + id;
			++state[1];
		}
	}
};

int32_t useCacheOwnedBuffer(CacheOwnedBuffer* buffer, bool fail) {
	if (reinterpret_cast<uintptr_t>(buffer) % alignof(CacheOwnedBuffer) != 0) {
		throw std::runtime_error("misaligned cached owned buffer");
	}
	++buffer->state[3];
	if (fail) {
		throw CacheFailure(buffer->state[3]);
	}
	return buffer->id;
}

void useCacheOwnedBufferVoid(CacheOwnedBuffer* buffer, bool fail) {
	useCacheOwnedBuffer(buffer, fail);
}

using RuntimeWrappers = std::array<int, 7>;

CompiledModule compileRuntimeModule(const Options& options, RuntimeWrappers& wrappers) {
	NautilusEngine engine(cache::createCompiler(options), options);
	auto module = engine.createModule();
	module.registerFunction<val<int64_t>(val<int64_t>)>("proxy", [&wrappers](val<int64_t> value) {
		++wrappers[0];
		return invoke(cacheProxy, value);
	});
	module.registerFunction<val<uintptr_t>()>("proxy_address", [&wrappers] {
		++wrappers[1];
		return invoke(cacheProxyAddress);
	});
	module.registerFunction<val<int64_t>(val<int64_t*>, val<int64_t>)>(
	    "state", [&wrappers](val<int64_t*> state, val<int64_t> delta) {
		    ++wrappers[2];
		    *state = val<int64_t>(*state) + delta;
		    return val<int64_t>(*state);
	    });
	module.registerFunction<val<int32_t>(val<int32_t*>, val<bool>)>("guarded",
	                                                                [&wrappers](val<int32_t*> state, val<bool> fail) {
		                                                                ++wrappers[3];
		                                                                NativeGuard first(state, firstCleanup);
		                                                                NativeGuard second(state, secondCleanup);
		                                                                return invoke(cacheMaybeThrow, state, fail);
	                                                                });
	module.registerFunction<val<int32_t>(val<int32_t*>, val<bool>)>(
	    "typed_owned", [&wrappers](val<int32_t*> state, val<bool> fail) {
		    ++wrappers[4];
		    val<CacheOwnedBuffer> empty;
		    val<CacheOwnedBuffer> first(state, cacheLiteral<int32_t {1}>());
		    val<CacheOwnedBuffer> copy(first);
		    val<CacheOwnedBuffer> moved(std::move(copy));
		    return invoke(useCacheOwnedBuffer, &moved, fail);
	    });
	module.registerFunction<val<int32_t>(val<int32_t*>, val<bool>, val<int32_t (*)(CacheOwnedBuffer*, bool)>)>(
	    "typed_callback",
	    [&wrappers](val<int32_t*> state, val<bool> fail, val<int32_t (*)(CacheOwnedBuffer*, bool)> callback) {
		    ++wrappers[5];
		    val<CacheOwnedBuffer> first(state, cacheLiteral<int32_t {1}>());
		    val<CacheOwnedBuffer> copy(first);
		    return callback(&copy, fail);
	    });
	module.registerFunction<void(val<int32_t*>, val<bool>, val<void (*)(CacheOwnedBuffer*, bool)>)>(
	    "typed_callback_void",
	    [&wrappers](val<int32_t*> state, val<bool> fail, val<void (*)(CacheOwnedBuffer*, bool)> callback) {
		    ++wrappers[6];
		    val<CacheOwnedBuffer> first(state, cacheLiteral<int32_t {1}>());
		    val<CacheOwnedBuffer> copy(first);
		    callback(&copy, fail);
	    });
	return module.compile();
}

void checkRuntimeModule(CompiledModule& module) {
	const auto proxy = module.getFunction<int64_t(int64_t)>("proxy");
	REQUIRE(proxy(-9) == cacheProxy(-9));
	REQUIRE(proxy(17) == cacheProxy(17));
	REQUIRE(module.getFunction<uintptr_t()>("proxy_address")() == cacheProxyAddress());
	const auto state = module.getFunction<int64_t(int64_t*, int64_t)>("state");
	int64_t first = 10, second = -40;
	REQUIRE(state(&first, 5) == 15);
	REQUIRE(state(&second, 7) == -33);
	REQUIRE(state(&first, -2) == 13);
	REQUIRE(first == 13);
	REQUIRE(second == -33);
	const auto guarded = module.getFunction<int32_t(int32_t*, bool)>("guarded");
	int32_t cleanups[3] = {};
	REQUIRE(guarded(cleanups, false) == 1);
	REQUIRE(cleanups[0] == 21);
	REQUIRE(cleanups[1] == 2);
	REQUIRE(cleanups[2] == 1);
	cleanups[0] = cleanups[1] = 0;
	try {
		guarded(cleanups, true);
		FAIL("The cached native module swallowed its exception");
	} catch (const CacheFailure& failure) {
		REQUIRE(failure.call == 2);
		REQUIRE(std::string(failure.what()) == "cache guarded native failure");
	}
	REQUIRE(cleanups[0] == 21);
	REQUIRE(cleanups[1] == 2);
	REQUIRE(cleanups[2] == 2);
	cleanups[0] = cleanups[1] = 0;
	REQUIRE(guarded(cleanups, false) == 3);
	REQUIRE(cleanups[0] == 21);
	REQUIRE(cleanups[1] == 2);
	REQUIRE(cleanups[2] == 3);
	const auto owned = module.getFunction<int32_t(int32_t*, bool)>("typed_owned");
	const auto callback =
	    module.getFunction<int32_t(int32_t*, bool, int32_t (*)(CacheOwnedBuffer*, bool))>("typed_callback");
	const auto callbackVoid =
	    module.getFunction<void(int32_t*, bool, void (*)(CacheOwnedBuffer*, bool))>("typed_callback_void");
	for (const std::string_view call : {"direct", "callback", "void callback"}) {
		int32_t counts[4] = {};
		for (const bool fail : {false, true, false}) {
			CAPTURE(call, fail);
			counts[0] = counts[1] = 0;
			const auto execute = [&] {
				if (call == "direct") {
					return owned(counts, fail);
				}
				if (call == "callback") {
					return callback(counts, fail, useCacheOwnedBuffer);
				}
				callbackVoid(counts, fail, useCacheOwnedBufferVoid);
				return int32_t {2};
			};
			if (fail) {
				try {
					execute();
					FAIL("Cached owned cleanup did not propagate its native exception");
				} catch (const CacheFailure& failure) {
					REQUIRE(failure.call == 2);
				}
			} else {
				REQUIRE(execute() == 2);
			}
			REQUIRE(counts[0] == 21);
			REQUIRE(counts[1] == 2);
			REQUIRE(counts[2] == 2 * counts[3]);
		}
		REQUIRE(counts[3] == 3);
	}
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

#ifdef ENABLE_LOGGING
class ReportSink final : public spdlog::sinks::base_sink<std::mutex> {
public:
	std::vector<std::string> reports() {
		std::lock_guard lock(mutex_);
		return reports_;
	}

private:
	void sink_it_(const spdlog::details::log_msg& message) override {
		const std::string text(message.payload.data(), message.payload.size());
		if (text.find("nautilus compilation statistics") != std::string::npos) {
			reports_.push_back(text);
		}
	}

	void flush_() override {
	}

	std::vector<std::string> reports_;
};

class ReportCapture {
public:
	ReportCapture() : previous_(spdlog::default_logger()), sink_(std::make_shared<ReportSink>()) {
		auto logger = std::make_shared<spdlog::logger>("cache-test-statistics", sink_);
		logger->set_level(spdlog::level::info);
		spdlog::set_default_logger(std::move(logger));
	}

	~ReportCapture() {
		spdlog::set_default_logger(std::move(previous_));
	}

	std::vector<std::string> reports() const {
		return sink_->reports();
	}

private:
	std::shared_ptr<spdlog::logger> previous_;
	std::shared_ptr<ReportSink> sink_;
};
#endif

} // namespace

TEST_CASE("Core engines ignore cache options and never produce cache policy statistics", "[cache][plugin][core]") {
	TemporaryCacheDirectory directory;
	for (const bool aliases : {false, true}) {
		CAPTURE(aliases);
		auto options = cacheOptions(directory.path(), "core-cache-is-opt-in");
		if (aliases) {
			options.setOption("engine.Blob.CacheDir", directory.path().string());
			options.setOption("engine.Blob.CacheKey", std::string("core-cache-is-opt-in"));
		}
		NautilusEngine engine(options);
		std::array<int, 2> wrappers {};
		for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
			auto module = compileIncrement(engine, wrappers[iteration]);
			REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(41) == 42);
			REQUIRE(wrappers[iteration] > 0);
			requireNoCacheStatistics(module);
			REQUIRE(module.getStatistics()->contains("tracing.ms"));
			REQUIRE(module.getStatistics()->contains("frontend.totalMs"));
		}
	}
	REQUIRE(std::filesystem::is_empty(directory.path()));
}

TEST_CASE("Cache factory missing directory or semantic key executes real fallback twice", "[cache][dispatch]") {
	for (const auto* setting : {"neither", "directory", "key", "empty directory", "empty key"}) {
		DYNAMIC_SECTION(setting) {
			TemporaryCacheDirectory directory;
			Options options;
			options.setOption("engine.backend", std::string("mlir"));
			options.setOption("mlir.enableMultithreading", false);
			if (std::string_view(setting) == "directory" || std::string_view(setting) == "empty key") {
				options.setOption("engine.cache.directory", directory.path().string());
			}
			if (std::string_view(setting) == "key" || std::string_view(setting) == "empty directory") {
				options.setOption("engine.cache.key", std::string("missing-cache-option"));
			}
			if (std::string_view(setting) == "empty directory") {
				options.setOption("engine.cache.directory", std::string {});
			} else if (std::string_view(setting) == "empty key") {
				options.setOption("engine.cache.key", std::string {});
			}
			NautilusEngine engine(cache::createCompiler(options), options);
			std::array<int, 2> wrappers {};
			for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
				auto module = compileIncrement(engine, wrappers[iteration]);
				REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(41) == 42);
				REQUIRE(wrappers[iteration] > 0);
				requireFallback(module);
				REQUIRE_FALSE(module.getStatistics()->contains("cache.key"));
				REQUIRE(module.getStatistics()->contains("frontend.totalMs"));
			}
			REQUIRE(std::filesystem::is_empty(directory.path()));
		}
	}
}

TEST_CASE("Canonical cache options and equal legacy aliases reuse the same native entry", "[cache][options]") {
	requireCachePlatform();
	TemporaryCacheDirectory directory;
	const std::string semanticKey = "canonical-alias-equivalence";
	const auto canonical = cacheOptions(directory.path(), semanticKey);
	int coldWrappers = 0;
	auto cold = compileIncrement(canonical, coldWrappers);
	requireWritten(cold);
	REQUIRE(coldWrappers > 0);
	const auto original = readArtifacts(directory.path());
	REQUIRE(original.size() == 3);
	const auto key = cacheStat<std::string>(cold, "cache.key");
	for (const auto* spelling :
	     {"legacy only", "both equal", "canonical directory legacy key", "legacy directory canonical key"}) {
		CAPTURE(spelling);
		Options options;
		options.setOption("engine.backend", std::string("mlir"));
		options.setOption("mlir.enableMultithreading", false);
		const std::string_view kind(spelling);
		if (kind == "both equal" || kind == "canonical directory legacy key") {
			options.setOption("engine.cache.directory", directory.path().string());
		}
		if (kind == "both equal" || kind == "legacy directory canonical key") {
			options.setOption("engine.cache.key", semanticKey);
		}
		if (kind != "canonical directory legacy key") {
			options.setOption("engine.Blob.CacheDir", directory.path().string());
		}
		if (kind != "legacy directory canonical key") {
			options.setOption("engine.Blob.CacheKey", semanticKey);
		}
		auto jit = cache::createCompiler(options);
		REQUIRE(jit->getOptions().getOptionOrDefault<std::string>("engine.cache.directory", "") ==
		        directory.path().string());
		REQUIRE(jit->getOptions().getOptionOrDefault<std::string>("engine.cache.key", "") == semanticKey);
		REQUIRE_FALSE(jit->getOptions().hasOption("engine.Blob.CacheDir"));
		REQUIRE_FALSE(jit->getOptions().hasOption("engine.Blob.CacheKey"));
		NautilusEngine engine(std::move(jit), options);
		int wrappers = 0;
		auto warm = compileIncrement(engine, wrappers);
		REQUIRE(warm.getFunction<int32_t(int32_t)>("execute")(-7) == -6);
		REQUIRE(wrappers == 0);
		requireNativeHit(warm);
		REQUIRE(cacheStat<std::string>(warm, "cache.key") == key);
		REQUIRE(readArtifacts(directory.path()) == original);
	}
	TemporaryCacheDirectory otherDirectory;
	int otherWrappers = 0;
	auto other = compileIncrement(cacheOptions(otherDirectory.path(), semanticKey), otherWrappers);
	requireWritten(other);
	REQUIRE(cacheStat<std::string>(other, "cache.key") == key);
	REQUIRE(otherWrappers > 0);
}

TEST_CASE("Conflicting and mistyped cache options cannot reuse or publish artifacts", "[cache][options]") {
	requireCachePlatform();
	for (const auto* invalid : {"directory conflict", "key conflict", "canonical directory int", "canonical key bool",
	                            "legacy directory double", "legacy key int"}) {
		DYNAMIC_SECTION(invalid) {
			TemporaryCacheDirectory directory;
			TemporaryCacheDirectory other;
			auto options = cacheOptions(directory.path(), "invalid-option-rejection");
			int coldWrappers = 0;
			auto cold = compileIncrement(options, coldWrappers);
			requireWritten(cold);
			const auto original = readArtifacts(directory.path());
			const std::string_view kind(invalid);
			if (kind == "directory conflict") {
				options.setOption("engine.Blob.CacheDir", other.path().string());
			} else if (kind == "key conflict") {
				options.setOption("engine.Blob.CacheKey", std::string("different-semantics"));
			} else if (kind == "canonical directory int") {
				options.setOption("engine.cache.directory", 7);
			} else if (kind == "canonical key bool") {
				options.setOption("engine.cache.key", true);
			} else if (kind == "legacy directory double") {
				options.setOption("engine.Blob.CacheDir", 1.5);
			} else {
				options.setOption("engine.Blob.CacheKey", 7);
			}
			REQUIRE_THROWS_AS(cache::createCompiler(options), RuntimeException);
			const auto valid = cacheOptions(directory.path(), "invalid-option-rejection");
			NautilusEngine engine(cache::createCompiler(valid), valid);
			std::array<int, 2> wrappers {};
			for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
				REQUIRE_THROWS_AS(compileIncrement(engine, wrappers[iteration], 1, options.deriveModuleOptions()),
				                  RuntimeException);
				REQUIRE(wrappers[iteration] == 0);
				REQUIRE(readArtifacts(directory.path()) == original);
				REQUIRE(std::filesystem::is_empty(other.path()));
			}
		}
	}
}

TEST_CASE("Native cache hits skip every wrapper and ignore missing or corrupt bytecode", "[cache][native]") {
	requireCachePlatform();
	TemporaryCacheDirectory directory;
	const auto options = cacheOptions(directory.path(), "multi-export-native-first");
	auto compile = [&](bool reverse, std::array<int, 2>& wrappers) {
		NautilusEngine engine(cache::createCompiler(options), options);
		auto builder = engine.createModule();
		auto increment = [&wrappers](val<int32_t> value) {
			++wrappers[0];
			return value + cacheLiteral<int32_t {1}>();
		};
		auto sum = [&wrappers](val<int64_t> left, val<int64_t> right) {
			++wrappers[1];
			return left + right;
		};
		if (reverse) {
			builder.registerFunction<val<int64_t>(val<int64_t>, val<int64_t>)>("sum", sum);
			builder.registerFunction<val<int32_t>(val<int32_t>)>("increment", increment);
		} else {
			builder.registerFunction<val<int32_t>(val<int32_t>)>("increment", increment);
			builder.registerFunction<val<int64_t>(val<int64_t>, val<int64_t>)>("sum", sum);
		}
		return builder.compile();
	};
	std::array<int, 2> coldWrappers {};
	auto cold = compile(false, coldWrappers);
	requireWritten(cold);
	requireArtifacts(cold, directory.path());
	for (const auto calls : coldWrappers) {
		REQUIRE(calls > 0);
	}
	const auto recordedCold = coldWrappers;
	const auto bytecode = artifactPath(cold, directory.path(), ".mlirbc");
	for (const auto* damage : {"none", "corrupt", "missing"}) {
		CAPTURE(damage);
		if (std::string_view(damage) == "corrupt") {
			writeFile(bytecode, "not MLIR bytecode");
		} else if (std::string_view(damage) == "missing") {
			REQUIRE(std::filesystem::remove(bytecode));
		}
		std::array<int, 2> warmWrappers {};
		auto warm = compile(true, warmWrappers);
		REQUIRE(warm.getFunction<int32_t(int32_t)>("increment")(41) == 42);
		REQUIRE(warm.getFunction<int64_t(int64_t, int64_t)>("sum")(int64_t {1} << 40, 7) == (int64_t {1} << 40) + 7);
		requireNativeHit(warm);
		REQUIRE(cacheStat<std::string>(warm, "cache.key") == cacheStat<std::string>(cold, "cache.key"));
		REQUIRE((warmWrappers == std::array<int, 2> {}));
		REQUIRE(coldWrappers == recordedCold);
	}
}

TEST_CASE("Bytecode repairs are non-native hits and corrupting both payloads retraces", "[cache][repair]") {
	requireCachePlatform();
	TemporaryCacheDirectory directory;
	const auto options = cacheOptions(directory.path(), "bytecode-repair-and-retrace");
	int coldWrappers = 0;
	auto cold = compileIncrement(options, coldWrappers);
	requireWritten(cold);
	const auto object = artifactPath(cold, directory.path(), ".o");
	const auto bytecode = artifactPath(cold, directory.path(), ".mlirbc");
	const auto key = cacheStat<std::string>(cold, "cache.key");
	for (const bool missing : {false, true}) {
		CAPTURE(missing);
		if (missing) {
			REQUIRE(std::filesystem::remove(object));
		} else {
			writeFile(object, "not a native object");
		}
		int wrappers = 0;
		auto repaired = compileIncrement(options, wrappers);
		REQUIRE(repaired.getFunction<int32_t(int32_t)>("execute")(19) == 20);
		REQUIRE(wrappers == 0);
		REQUIRE(cacheStat<std::string>(repaired, "cache.object") != "hit");
		REQUIRE(cacheStat<std::string>(repaired, "cache.mlir") == "hit");
		REQUIRE(cacheStat<int64_t>(repaired, "cache.tracingRan") == 0);
		REQUIRE(cacheStat<std::string>(repaired, "cache.key") == key);
		requireNoFrontendStatistics(repaired);
		REQUIRE(repaired.getStatistics()->contains("mlir.bytecodeLoad.ms"));
		REQUIRE(repaired.getStatistics()->contains("jit.compile.ms"));
		REQUIRE_FALSE(repaired.getStatistics()->contains("jit.objectLoad.ms"));
		REQUIRE(readFile(object) != "not a native object");
		int warmWrappers = 0;
		auto warm = compileIncrement(options, warmWrappers);
		requireNativeHit(warm);
		REQUIRE(warmWrappers == 0);
	}
	writeFile(object, "corrupt object");
	writeFile(bytecode, "corrupt bytecode");
	int retracedWrappers = 0;
	auto retraced = compileIncrement(options, retracedWrappers);
	REQUIRE(retraced.getFunction<int32_t(int32_t)>("execute")(-7) == -6);
	REQUIRE(retracedWrappers > 0);
	REQUIRE(cacheStat<int64_t>(retraced, "cache.tracingRan") == 1);
	REQUIRE(cacheStat<std::string>(retraced, "cache.object") != "hit");
	REQUIRE(cacheStat<std::string>(retraced, "cache.mlir") != "hit");
	REQUIRE(cacheStat<std::string>(retraced, "cache.key") == key);
	REQUIRE(retraced.getStatistics()->contains("frontend.totalMs"));
	requireArtifacts(retraced, directory.path());
	int finalWrappers = 0;
	auto final = compileIncrement(options, finalWrappers);
	requireNativeHit(final);
	REQUIRE(finalWrappers == 0);
	REQUIRE(coldWrappers > 0);
}

TEST_CASE("Corrupt manifests never authorize native or bytecode cache hits", "[cache][repair]") {
	requireCachePlatform();
	for (const auto* corruption : {"truncated", "invalid header", "changed bytes"}) {
		DYNAMIC_SECTION(corruption) {
			TemporaryCacheDirectory directory;
			const auto options = cacheOptions(directory.path(), "corrupt-manifest");
			int coldWrappers = 0;
			auto cold = compileIncrement(options, coldWrappers);
			requireWritten(cold);
			const auto path = artifactPath(cold, directory.path(), ".manifest");
			auto contents = readFile(path);
			if (std::string_view(corruption) == "truncated") {
				contents.resize(contents.size() / 2);
			} else if (std::string_view(corruption) == "invalid header") {
				contents[0] ^= 0x40;
			} else {
				contents[contents.size() / 2] ^= 0x01;
			}
			writeFile(path, contents);
			int wrappers = 0;
			auto recovered = compileIncrement(options, wrappers);
			REQUIRE(recovered.getFunction<int32_t(int32_t)>("execute")(41) == 42);
			REQUIRE(wrappers > 0);
			REQUIRE(cacheStat<int64_t>(recovered, "cache.tracingRan") == 1);
			REQUIRE(cacheStat<std::string>(recovered, "cache.object") != "hit");
			REQUIRE(cacheStat<std::string>(recovered, "cache.mlir") != "hit");
			int warmWrappers = 0;
			auto warm = compileIncrement(options, warmWrappers);
			requireNativeHit(warm);
			REQUIRE(warmWrappers == 0);
		}
	}
}

TEST_CASE("Runtime pointer arguments proxy imports and exception cleanup survive both cache paths", "[cache][guard]") {
	requireCachePlatform();
	for (const bool passes : {false, true}) {
		CAPTURE(passes);
		TemporaryCacheDirectory directory;
		auto options = cacheOptions(directory.path(), "runtime-pointer-guarded-module");
		options.setOption("ir.runPasses", passes);
		RuntimeWrappers coldWrappers {};
		auto cold = compileRuntimeModule(options, coldWrappers);
		requireWritten(cold);
		checkRuntimeModule(cold);
		for (const auto calls : coldWrappers) {
			REQUIRE(calls > 0);
		}
		const auto originalCalls = coldWrappers;
		RuntimeWrappers warmWrappers {};
		auto warm = compileRuntimeModule(options, warmWrappers);
		requireNativeHit(warm);
		checkRuntimeModule(warm);
		REQUIRE((warmWrappers == RuntimeWrappers {}));
		writeFile(artifactPath(cold, directory.path(), ".o"), "corrupt guarded object");
		RuntimeWrappers repairWrappers {};
		auto repaired = compileRuntimeModule(options, repairWrappers);
		REQUIRE(cacheStat<std::string>(repaired, "cache.object") != "hit");
		REQUIRE(cacheStat<std::string>(repaired, "cache.mlir") == "hit");
		REQUIRE(cacheStat<int64_t>(repaired, "cache.tracingRan") == 0);
		checkRuntimeModule(repaired);
		REQUIRE((repairWrappers == RuntimeWrappers {}));
		REQUIRE(coldWrappers == originalCalls);
		checkRuntimeModule(cold);
		checkRuntimeModule(warm);
	}
}

TEST_CASE("Per-module semantic keys and arbitrary typed worker options partition cache entries", "[cache][options]") {
	requireCachePlatform();
	TemporaryCacheDirectory directory;
	const auto options = cacheOptions(directory.path(), "engine-default-key");
	NautilusEngine engine(cache::createCompiler(options), options);
	std::set<std::string> keys;
	for (const auto* semanticKey : {"plan/increment=7", "plan/increment=11"}) {
		for (const bool asString : {false, true}) {
			for (const int workers : {1, 4}) {
				CAPTURE(semanticKey, asString, workers);
				const int32_t increment = std::string_view(semanticKey) == "plan/increment=7" ? 7 : 11;
				ModuleOptions overrides;
				overrides.setOption("engine.cache.key", std::string(semanticKey));
				if (asString) {
					overrides.setOption("nes.workerThreads", std::to_string(workers));
				} else {
					overrides.setOption("nes.workerThreads", workers);
				}
				std::array<int, 2> wrappers {};
				std::string coldKey;
				for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
					auto compiled = compileIncrement(engine, wrappers[iteration], increment * workers, overrides);
					REQUIRE(compiled.getFunction<int32_t(int32_t)>("execute")(3) == 3 + increment * workers);
					const auto key = cacheStat<std::string>(compiled, "cache.key");
					if (iteration == 0) {
						requireWritten(compiled);
						REQUIRE(wrappers[iteration] > 0);
						REQUIRE(keys.insert(key).second);
						coldKey = key;
					} else {
						requireNativeHit(compiled);
						REQUIRE(wrappers[iteration] == 0);
						REQUIRE(key == coldKey);
					}
				}
			}
		}
	}
	REQUIRE(keys.size() == 8);
	REQUIRE(engine.createModule().getOptions().getOptionOrDefault<std::string>("engine.cache.key", "") ==
	        "engine-default-key");
}

TEST_CASE("Cache keys include export names signatures and argument counts", "[cache][exports]") {
	requireCachePlatform();
	TemporaryCacheDirectory directory;
	const auto options = cacheOptions(directory.path(), "export-contract");
	std::set<std::string> keys;
	for (const auto* kind : {"i32", "renamed", "i64", "two arguments", "extra export"}) {
		CAPTURE(kind);
		std::string coldKey;
		std::array<int, 2> wrappers {};
		for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
			NautilusEngine engine(cache::createCompiler(options), options);
			auto builder = engine.createModule();
			const std::string_view shape(kind);
			if (shape == "i64") {
				builder.registerFunction<val<int64_t>(val<int64_t>)>("execute", [&, iteration](val<int64_t> value) {
					++wrappers[iteration];
					return value;
				});
			} else if (shape == "two arguments") {
				builder.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>(
				    "execute", [&, iteration](val<int32_t> left, val<int32_t> right) {
					    ++wrappers[iteration];
					    return left + right;
				    });
			} else {
				builder.registerFunction<val<int32_t>(val<int32_t>)>(shape == "renamed" ? "renamed" : "execute",
				                                                     [&, iteration](val<int32_t> value) {
					                                                     ++wrappers[iteration];
					                                                     return value;
				                                                     });
				if (shape == "extra export") {
					builder.registerFunction<val<int32_t>(val<int32_t>)>("extra", [&, iteration](val<int32_t> value) {
						++wrappers[iteration];
						return value + cacheLiteral<int32_t {7}>();
					});
				}
			}
			auto compiled = builder.compile();
			if (shape == "i64") {
				REQUIRE(compiled.getFunction<int64_t(int64_t)>("execute")(int64_t {1} << 40) == (int64_t {1} << 40));
			} else if (shape == "two arguments") {
				REQUIRE(compiled.getFunction<int32_t(int32_t, int32_t)>("execute")(19, 23) == 42);
			} else {
				REQUIRE(compiled.getFunction<int32_t(int32_t)>(shape == "renamed" ? "renamed" : "execute")(42) == 42);
				if (shape == "extra export") {
					REQUIRE(compiled.getFunction<int32_t(int32_t)>("extra")(35) == 42);
				}
			}
			const auto key = cacheStat<std::string>(compiled, "cache.key");
			if (iteration == 0) {
				requireWritten(compiled);
				REQUIRE(wrappers[iteration] > 0);
				REQUIRE(keys.insert(key).second);
				coldKey = key;
			} else {
				requireNativeHit(compiled);
				REQUIRE(wrappers[iteration] == 0);
				REQUIRE(key == coldKey);
			}
		}
	}
	REQUIRE(keys.size() == 5);
}

TEST_CASE("Typed cache roots validate signatures before any optional optimization", "[cache][preflight]") {
	requireCachePlatform();
	for (const auto& [mode, overrides] : validationModes()) {
		for (const auto* mismatch : {"matching", "return type", "argument type", "argument count"}) {
			DYNAMIC_SECTION(mode << ": " << mismatch) {
				TemporaryCacheDirectory directory;
				auto options = cacheOptions(directory.path(), "root-signature-preflight");
				options.applyOverrides(overrides);
				auto jit = cache::createCompiler(options);
				compiler::CompilableFunction::Signature signature {Type::i32, {Type::i32}};
				const std::string_view kind(mismatch);
				if (kind == "return type") {
					signature.returnType = Type::i64;
				} else if (kind == "argument type") {
					signature.argumentTypes[0] = Type::i64;
				} else if (kind == "argument count") {
					signature.argumentTypes.push_back(Type::i32);
				}
				std::array<int, 2> wrappers {};
				for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
					std::list<compiler::CompilableFunction> functions;
					functions.emplace_back("execute",
					                       details::createFunctionWrapper([&, iteration](val<int32_t> value) {
						                       ++wrappers[iteration];
						                       return value;
					                       }),
					                       std::unordered_map<std::string, std::string> {}, std::optional {signature});
					if (kind != "matching") {
						CompiledModule rejected(jit->compile(functions, options.deriveModuleOptions()), {});
						REQUIRE(rejected.getFunction<int32_t(int32_t)>("execute")(42) == 42);
						REQUIRE(wrappers[iteration] > 0);
						requireFallback(rejected);
						REQUIRE(cacheStat<std::string>(rejected, "cache.rejection").find("Traced root signature") !=
						        std::string::npos);
						REQUIRE(readArtifacts(directory.path()).empty());
						REQUIRE_FALSE(tracing::inTracer());
					} else {
						CompiledModule compiled(jit->compile(functions, options.deriveModuleOptions()), {});
						REQUIRE(compiled.getFunction<int32_t(int32_t)>("execute")(42) == 42);
						if (iteration == 0) {
							requireWritten(compiled);
							REQUIRE(wrappers[iteration] > 0);
						} else {
							requireNativeHit(compiled);
							REQUIRE(wrappers[iteration] == 0);
						}
					}
				}
			}
		}
	}
}

TEST_CASE("Direct wrappers and signature-less lists cannot invent cache export metadata", "[cache][dispatch]") {
	auto* registry = compiler::CompilationBackendRegistry::getInstance();
	std::vector<std::string> backends {"mlir"};
	for (const auto* backend : {"bc", "tbc", "asmjit", "cpp"}) {
		if (registry->hasBackend(backend)) {
			backends.emplace_back(backend);
			break;
		}
	}
	for (const auto& backend : backends) {
		for (const bool wrapperRoute : {false, true}) {
			DYNAMIC_SECTION(backend << ": wrapper=" << wrapperRoute) {
				TemporaryCacheDirectory directory;
				auto options = cacheOptions(directory.path(), "signature-less-fallback");
				options.setOption("engine.backend", backend);
				auto jit = cache::createCompiler(options);
				REQUIRE(jit->getName() == backend);
				std::array<int, 2> wrappers {};
				for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
					auto wrapper = details::createFunctionWrapper([&, iteration](val<int32_t> value) {
						++wrappers[iteration];
						return value + cacheLiteral<int32_t {1}>();
					});
					std::list<compiler::CompilableFunction> functions;
					functions.emplace_back("execute", wrapper);
					CompiledModule compiled(wrapperRoute ? jit->compile(wrapper, options.deriveModuleOptions())
					                                     : jit->compile(functions, options.deriveModuleOptions()),
					                        {});
					REQUIRE(compiled.getFunction<int32_t(int32_t)>("execute")(41) == 42);
					REQUIRE(compiled.getFunction<int32_t(int32_t)>("execute")(-7) == -6);
					REQUIRE(wrappers[iteration] > 0);
					requireFallback(compiled);
					REQUIRE(cacheStat<std::string>(compiled, "backend.name") == backend);
					const auto reason = cacheStat<std::string>(compiled, "cache.fallback");
					if (backend != "mlir") {
						REQUIRE(reason == "backend_not_mlir");
					} else if (wrapperRoute) {
						REQUIRE(reason == "missing_export_signature");
					} else {
						REQUIRE(reason.starts_with("unsupported_export_metadata:"));
					}
					REQUIRE(compiled.getStatistics()->contains("tracing.ms"));
					REQUIRE(compiled.getStatistics()->contains("frontend.totalMs"));
					REQUIRE_FALSE(compiled.getStatistics()->contains("cache.key"));
				}
				REQUIRE(std::filesystem::is_empty(directory.path()));
			}
		}
	}
}

TEST_CASE("Cache injection preserves debug perf and explicitly selected non-MLIR backends", "[cache][dispatch]") {
	auto* registry = compiler::CompilationBackendRegistry::getInstance();
	std::vector<std::pair<std::string, Options>> configurations;
	for (const auto* setting : {"debug", "perf", "perf.sample"}) {
		Options options;
		options.setOption("engine.backend", std::string("mlir"));
		options.setOption(setting, true);
		configurations.emplace_back(setting, std::move(options));
	}
	for (const auto* backend : {"bc", "tbc", "asmjit", "cpp"}) {
		if (registry->hasBackend(backend)) {
			Options options;
			options.setOption("engine.backend", std::string(backend));
			options.setOption("engine.tier0.backend", std::string("interpreter"));
			options.setOption("engine.tier1.backend", std::string("mlir"));
			options.setOption("engine.tiered.backgroundPromotion", true);
			configurations.emplace_back(backend, std::move(options));
		}
	}
	for (const auto& [name, overrides] : configurations) {
		DYNAMIC_SECTION(name) {
			TemporaryCacheDirectory directory;
			auto options = cacheOptions(directory.path(), "debug-and-backend-delegation");
			options.applyOverrides(overrides);
			NautilusEngine engine(cache::createCompiler(options), options);
			const auto expected = options.getOptionOrDefault<std::string>("engine.backend", "mlir");
			REQUIRE(engine.getNameOfBackend() == expected);
			std::array<int, 2> wrappers {};
			for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
				const auto beforeSources = expected == "mlir" ? debugSourceFiles() : std::set<std::filesystem::path> {};
				auto module = compileIncrement(engine, wrappers[iteration]);
				REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(41) == 42);
				REQUIRE(wrappers[iteration] > 0);
				requireFallback(module);
				REQUIRE(cacheStat<std::string>(module, "backend.name") == expected);
				REQUIRE(cacheStat<std::string>(module, "cache.fallback") ==
				        (expected == "mlir" ? "debug_metadata_unsupported" : "backend_not_mlir"));
				REQUIRE(module.getStatistics()->contains("tracing.ms"));
				REQUIRE_FALSE(module.getStatistics()->contains("cache.key"));
				if (expected == "mlir") {
					REQUIRE(module.getStatistics()->contains("mlir.loweringFromIR.ms"));
					std::size_t newSources = 0;
					for (const auto& source : debugSourceFiles()) {
						if (!beforeSources.contains(source)) {
							++newSources;
							REQUIRE(readFile(source).find("execute") != std::string::npos);
						}
					}
					REQUIRE(newSources > 0);
				}
			}
			REQUIRE(std::filesystem::is_empty(directory.path()));
		}
	}
}

TEST_CASE("Unsupported target and inlining options bypass populated caches without changing entries",
          "[cache][options][guard]") {
	requireCachePlatform();
	TemporaryCacheDirectory directory;
	const auto baseline = cacheOptions(directory.path(), "unsupported-codegen-options");
	int coldWrappers = 0;
	auto cold = compileIncrement(baseline, coldWrappers);
	requireWritten(cold);
	const auto original = readArtifacts(directory.path());
	for (const bool pinnedCPU : {false, true}) {
		CAPTURE(pinnedCPU);
		auto options = baseline;
		if (pinnedCPU) {
			options.setOption("mlir.targetCpu", std::string("generic"));
		} else {
			options.setOption("mlir.inline_invoke_calls", true);
		}
		for (int attempt = 0; attempt < 2; ++attempt) {
			int wrappers = 0;
			auto module = compileIncrement(options, wrappers);
			REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(41) == 42);
			REQUIRE(wrappers > 0);
			requireFallback(module);
			REQUIRE(cacheStat<std::string>(module, "cache.fallback") ==
			        (pinnedCPU ? "pinned_target_cpu_unsupported" : "inline_invoke_calls_unsupported"));
			REQUIRE_FALSE(module.getStatistics()->contains("cache.key"));
			REQUIRE(readArtifacts(directory.path()) == original);
		}
	}
	int warmWrappers = 0;
	auto warm = compileIncrement(baseline, warmWrappers);
	requireNativeHit(warm);
	REQUIRE(warmWrappers == 0);
}

TEST_CASE("Single-tier default and explicit MLIR dispatch both support native warm hits", "[cache][dispatch]") {
	requireCachePlatform();
	for (const bool explicitBackend : {false, true}) {
		DYNAMIC_SECTION("explicit=" << explicitBackend) {
			TemporaryCacheDirectory directory;
			Options options;
			options.setOption("engine.tier0.backend", std::string("interpreter"));
			options.setOption("engine.tier1.backend", std::string("mlir"));
			options.setOption("engine.tiered.backgroundPromotion", explicitBackend);
			options.setOption("engine.cache.directory", directory.path().string());
			options.setOption("engine.cache.key", std::string("single-tier-cache-dispatch"));
			options.setOption("mlir.enableMultithreading", false);
			if (explicitBackend) {
				options.setOption("engine.backend", std::string("mlir"));
			}
			NautilusEngine engine(cache::createCompiler(options), options);
			REQUIRE(engine.getNameOfBackend() == "mlir");
			std::array<int, 2> wrappers {};
			for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
				auto module = compileIncrement(engine, wrappers[iteration]);
				REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(41) == 42);
				if (iteration == 0) {
					requireWritten(module);
					REQUIRE(wrappers[iteration] > 0);
				} else {
					requireNativeHit(module);
					REQUIRE(wrappers[iteration] == 0);
				}
			}
		}
	}
}

TEST_CASE("Cache compiler destruction joins default and explicit background promotions", "[cache][tiering]") {
	auto* registry = compiler::CompilationBackendRegistry::getInstance();
	std::vector<std::string> tiers {"default", INTERPRETER_BACKEND};
	for (const auto* backend : {"bc", "tbc", "asmjit", "cpp"}) {
		if (registry->hasBackend(backend)) {
			tiers.emplace_back(backend);
			break;
		}
	}
	for (const auto& tier0 : tiers) {
		DYNAMIC_SECTION(tier0) {
			TemporaryCacheDirectory directory;
			Options options;
			options.setOption("engine.cache.directory", directory.path().string());
			options.setOption("engine.cache.key", std::string("background-ownership"));
			options.setOption("mlir.enableMultithreading", false);
			if (tier0 != "default") {
				options.setOption("engine.tier0.backend", tier0);
				options.setOption("engine.tier1.backend", std::string("mlir"));
				options.setOption("engine.tiered.backgroundPromotion", true);
			}
			const auto expectedTier0 = tier0 != "default"               ? tier0
			                           : registry->hasBackend("asmjit") ? std::string("asmjit")
			                           : registry->hasBackend("bc")     ? std::string("bc")
			                                                            : std::string(INTERPRETER_BACKEND);
			std::array<std::atomic<int>, 2> wrappers {};
			std::vector<CompiledModule> compiled;
			{
				NautilusEngine engine(cache::createCompiler(options), options);
				REQUIRE(engine.getNameOfBackend() == "tiered(" + expectedTier0 + ",mlir)");
				for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
					auto builder = engine.createModule();
					builder.setOption("nes.workerThreads", static_cast<int>(iteration + 1));
					builder.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&, iteration](val<int32_t> value) {
						if (tracing::inTracer()) {
							wrappers[iteration].fetch_add(1);
						}
						return value + cacheInvariant(static_cast<int32_t>(iteration + 1));
					});
					compiled.push_back(builder.compile());
					REQUIRE(wrappers[iteration].load() > 0);
					REQUIRE(compiled.back().getFunction<int32_t(int32_t)>("execute")(40) ==
					        40 + static_cast<int32_t>(iteration + 1));
				}
				auto discarded = engine.createModule();
				discarded.registerFunction<val<int32_t>(val<int32_t>)>(
				    "discarded", [](val<int32_t> value) { return value + cacheLiteral<int32_t {7}>(); });
				(void) discarded.compile();
			}
			for (std::size_t iteration = 0; iteration < compiled.size(); ++iteration) {
				auto& module = compiled[iteration];
				const auto traced = wrappers[iteration].load();
				REQUIRE(module.getState()->version.load() == 1);
				REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(-7) ==
				        -7 + static_cast<int32_t>(iteration + 1));
				REQUIRE(wrappers[iteration].load() == traced);
				REQUIRE(cacheStat<std::string>(module, "backend.name") == "mlir");
				REQUIRE(cacheStat<std::string>(module, "tier") == "tier1");
				requireFallback(module);
				REQUIRE(cacheStat<std::string>(module, "cache.fallback") == "tiered_compilation");
				REQUIRE_FALSE(module.getStatistics()->contains("tracing.ms"));
			}
			REQUIRE(std::filesystem::is_empty(directory.path()));
		}
	}
}

TEST_CASE("Cache injection leaves explicitly interpreted modules uncompiled", "[cache][dispatch]") {
	TemporaryCacheDirectory directory;
	auto options = cacheOptions(directory.path(), "interpreted-delegation");
	options.setOption("engine.Compilation", false);
	NautilusEngine engine(cache::createCompiler(options), options);
	REQUIRE_FALSE(engine.isCompiled());
	int wrappers = 0, runtimeCalls = 0;
	for (int iteration = 0; iteration < 2; ++iteration) {
		auto builder = engine.createModule();
		builder.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&](val<int32_t> value) {
			if (tracing::inTracer()) {
				++wrappers;
			} else {
				++runtimeCalls;
			}
			return value + 1;
		});
		auto module = builder.compile();
		REQUIRE(module.getExecutable() == nullptr);
		REQUIRE(module.getStatistics() == nullptr);
		REQUIRE(module.getState()->version.load() == 0);
		REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(41) == 42);
		REQUIRE(runtimeCalls == iteration + 1);
		REQUIRE(wrappers == 0);
	}
	REQUIRE(std::filesystem::is_empty(directory.path()));
}

TEST_CASE("Storage lookup and publication failures preserve real compilation", "[cache][storage]") {
	requireCachePlatform();
	for (const auto* failure : {"directory is a file", "parent is a file", "directory replaced during trace"}) {
		DYNAMIC_SECTION(failure) {
			TemporaryCacheDirectory directory;
			const auto blocker = directory.path() / "blocked";
			const std::string_view kind(failure);
			std::filesystem::path cacheDirectory = blocker;
			if (kind != "directory replaced during trace") {
				writeFile(blocker, "not a directory");
				if (kind == "parent is a file") {
					cacheDirectory /= "cache";
				}
			} else {
				std::filesystem::create_directory(blocker);
				std::filesystem::permissions(blocker, std::filesystem::perms::owner_all);
			}
			const auto options = cacheOptions(cacheDirectory, "storage-failure");
			std::array<int, 2> wrappers {};
			for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
				NautilusEngine engine(cache::createCompiler(options), options);
				auto builder = engine.createModule();
				builder.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&, iteration](val<int32_t> value) {
					++wrappers[iteration];
					if (kind == "directory replaced during trace" && std::filesystem::is_directory(blocker)) {
						std::filesystem::remove_all(blocker);
						std::ofstream output(blocker, std::ios::binary);
						output << "publication blocked";
						if (!output.good()) {
							throw std::runtime_error("Could not arrange publication failure");
						}
					}
					return value + cacheLiteral<int32_t {1}>();
				});
				auto module = builder.compile();
				REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(41) == 42);
				REQUIRE(wrappers[iteration] > 0);
				requireFallback(module);
				REQUIRE(readArtifacts(directory.path()).empty());
				REQUIRE(std::filesystem::is_regular_file(blocker));
			}
			REQUIRE(std::filesystem::remove(blocker));
			std::filesystem::create_directories(cacheDirectory);
			std::filesystem::permissions(cacheDirectory, std::filesystem::perms::owner_all);
			int recoveryWrappers = 0;
			auto recovered = compileIncrement(options, recoveryWrappers);
			requireWritten(recovered);
			REQUIRE(recoveryWrappers > 0);
			int warmWrappers = 0;
			auto warm = compileIncrement(options, warmWrappers);
			requireNativeHit(warm);
			REQUIRE(warmWrappers == 0);
		}
	}
}

TEST_CASE("Concurrent shared-engine requests serialize same-key publication without retracing hits",
          "[cache][concurrency]") {
	requireCachePlatform();
	TemporaryCacheDirectory directory;
	const auto options = cacheOptions(directory.path(), "concurrent-same-key");
	constexpr std::size_t count = 6;
	std::array<std::atomic<int>, count> wrappers {};
	std::array<std::optional<CompiledModule>, count> modules;
	std::array<std::exception_ptr, count> failures {};
	std::barrier start(static_cast<std::ptrdiff_t>(count));
	{
		NautilusEngine engine(cache::createCompiler(options), options);
		std::vector<std::thread> threads;
		for (std::size_t index = 0; index < count; ++index) {
			threads.emplace_back([&, index] {
				start.arrive_and_wait();
				try {
					auto builder = engine.createModule();
					builder.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&, index](val<int32_t> value) {
						wrappers[index].fetch_add(1);
						return value + cacheLiteral<int32_t {1}>();
					});
					modules[index].emplace(builder.compile());
				} catch (...) {
					failures[index] = std::current_exception();
				}
			});
		}
		for (auto& thread : threads) {
			thread.join();
		}
	}
	std::size_t coldRequests = 0;
	std::set<const compiler::CompilationStatistics*> statistics;
	std::set<std::string> keys;
	for (std::size_t index = 0; index < count; ++index) {
		CAPTURE(index);
		if (failures[index]) {
			std::rethrow_exception(failures[index]);
		}
		REQUIRE(modules[index].has_value());
		auto& module = *modules[index];
		REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(static_cast<int32_t>(index)) ==
		        static_cast<int32_t>(index + 1));
		REQUIRE(statistics.insert(module.getStatistics().get()).second);
		keys.insert(cacheStat<std::string>(module, "cache.key"));
		if (wrappers[index].load() > 0) {
			++coldRequests;
			requireWritten(module);
		} else {
			requireNativeHit(module);
		}
	}
	REQUIRE(coldRequests == 1);
	REQUIRE(keys.size() == 1);
	REQUIRE(readArtifacts(directory.path()).size() == 3);
}

TEST_CASE("Cache compilation emits exactly one complete report for each synchronous request", "[cache][statistics]") {
#ifdef ENABLE_LOGGING
	requireCachePlatform();
	std::vector<std::string> modes {"cold and warm", "debug fallback", "keyless fallback"};
	std::string nonMLIR;
	for (const auto* backend : {"bc", "tbc", "asmjit", "cpp"}) {
		if (compiler::CompilationBackendRegistry::getInstance()->hasBackend(backend)) {
			nonMLIR = backend;
			modes.emplace_back("non-MLIR fallback");
			break;
		}
	}
	for (const auto& mode : modes) {
		DYNAMIC_SECTION(mode) {
			TemporaryCacheDirectory directory;
			auto options = cacheOptions(directory.path(), mode == "keyless fallback" ? "" : "report");
			options.setOption("engine.logStatistics", true);
			if (mode == "debug fallback") {
				options.setOption("debug", true);
			} else if (mode == "non-MLIR fallback") {
				options.setOption("engine.backend", nonMLIR);
			}
			ReportCapture capture;
			std::array<int, 2> wrappers {};
			for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
				auto module = compileIncrement(options, wrappers[iteration]);
				REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(41) == 42);
				const auto reports = capture.reports();
				REQUIRE(reports.size() == iteration + 1);
				const auto& report = reports.back();
				const auto expectedReport =
				    module.getStatistics()->formatReport(cacheStat<std::string>(module, "compilation.unitId"),
				                                         cacheStat<std::string>(module, "backend.name"));
				REQUIRE(report == "\n" + expectedReport);
				if (std::string_view(mode) == "cold and warm") {
					if (iteration == 0) {
						requireWritten(module);
					} else {
						requireNativeHit(module);
						REQUIRE(wrappers[iteration] == 0);
					}
				} else {
					requireFallback(module);
					REQUIRE(wrappers[iteration] > 0);
					REQUIRE(report.find(cacheStat<std::string>(module, "cache.fallback")) != std::string::npos);
				}
			}
		}
	}
#else
	SKIP("Compilation report capture requires logging support");
#endif
}

TEST_CASE("Changing root attributes never reuses an incompatible cached export", "[cache][exports]") {
	requireCachePlatform();
	TemporaryCacheDirectory directory;
	const auto options = cacheOptions(directory.path(), "root-attribute-contract");
	int coldWrappers = 0;
	auto cold = compileIncrement(options, coldWrappers, 7);
	requireWritten(cold);
	const auto original = readArtifacts(directory.path());
	for (const auto* attributeValue : {"false", "true"}) {
		CAPTURE(attributeValue);
		auto jit = cache::createCompiler(options);
		int wrappers = 0;
		std::list<compiler::CompilableFunction> functions;
		functions.emplace_back("execute", details::createFunctionWrapper([&](val<int32_t> value) {
			                       ++wrappers;
			                       return value + cacheLiteral<int32_t {11}>();
		                       }),
		                       std::unordered_map<std::string, std::string> {{"entry", attributeValue}},
		                       std::optional {compiler::CompilableFunction::Signature {Type::i32, {Type::i32}}});
		CompiledModule changed(jit->compile(functions, options.deriveModuleOptions()), {});
		REQUIRE(changed.getFunction<int32_t(int32_t)>("execute")(3) == 14);
		REQUIRE(wrappers > 0);
		requireFallback(changed);
		REQUIRE_FALSE(changed.getStatistics()->contains("cache.key"));
		for (const auto& [name, contents] : original) {
			REQUIRE(readFile(directory.path() / name) == contents);
		}
		int warmWrappers = 0;
		auto warm = compileIncrement(options, warmWrappers, 7);
		requireNativeHit(warm);
		REQUIRE(warm.getFunction<int32_t(int32_t)>("execute")(3) == 10);
		REQUIRE(warmWrappers == 0);
	}
}

TEST_CASE("Legacy scalar eligibility retains the failed preoptimization certificate", "[cache][legacy]") {
	requireCachePlatform();
	for (const auto& [mode, overrides] : validationModes()) {
		DYNAMIC_SECTION(mode) {
			TemporaryCacheDirectory directory;
			auto options = cacheOptions(directory.path(), "legacy-scalar-certificate");
			options.applyOverrides(overrides);
			std::array<int, 2> wrappers {};
			for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
				NautilusEngine engine(cache::createCompiler(options), options);
				auto builder = engine.createModule();
				builder.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&, iteration](val<int32_t> value) {
					++wrappers[iteration];
					val<int32_t> dead = 37;
					(void) dead;
					return value + cacheLiteral<int32_t {7}>();
				});
				auto module = builder.compile();
				REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(35) == 42);
				if (iteration == 0) {
					requireWritten(module);
					REQUIRE(wrappers[iteration] > 0);
					REQUIRE(cacheStat<int64_t>(module, "cache.scalarCertificate") == 0);
					REQUIRE(cacheStat<std::string>(module, "cache.scalarRejection").find("uncertified_scalar") !=
					        std::string::npos);
				} else {
					requireNativeHit(module);
					REQUIRE(wrappers[iteration] == 0);
				}
			}
		}
	}
}

namespace {

void requireUnsafeFallback(const CompiledModule& module, const std::filesystem::path& directory) {
	requireFallback(module);
	REQUIRE(cacheStat<std::string>(module, "cache.fallback") == "non_relocatable_pointer");
	REQUIRE(cacheStat<int64_t>(module, "cache.scalarCertificate") == 0);
	REQUIRE_FALSE(cacheStat<std::string>(module, "cache.scalarRejection").empty());
	REQUIRE_FALSE(cacheStat<std::string>(module, "cache.rejection").empty());
	REQUIRE(readArtifacts(directory).empty());
}

struct MemberState {
	int64_t total;
	int64_t readTotal() {
		return total;
	}
};

} // namespace

TEST_CASE("Captured pointers raw cleanup operands and heap member wrappers remain ineligible", "[cache][legacy]") {
	requireCachePlatform();
	for (const auto* kind :
	     {"pointer literal", "encoded pointer", "raw cleanup", "encoded cleanup", "member wrapper"}) {
		DYNAMIC_SECTION(kind) {
			TemporaryCacheDirectory directory;
			const auto options = cacheOptions(directory.path(), "raw-pointer-and-cleanup");
			auto state = std::make_unique<int32_t>(42);
			MemberState member {73};
			std::array<int, 2> wrappers {};
			const std::string_view expression(kind);
			for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
				NautilusEngine engine(cache::createCompiler(options), options);
				auto builder = engine.createModule();
				if (expression == "member wrapper") {
					builder.registerFunction<val<int64_t>(val<MemberState*>)>(
					    "execute", [&, iteration](val<MemberState*> runtime) {
						    ++wrappers[iteration];
						    return memberFunc<&MemberState::readTotal>()(runtime);
					    });
				} else {
					builder.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&, iteration](val<int32_t> value) {
						++wrappers[iteration];
						val<int32_t*> address =
						    expression == "encoded pointer" || expression == "encoded cleanup"
						        ? val<int32_t*>(val<uintptr_t>(reinterpret_cast<uintptr_t>(state.get())))
						        : val<int32_t*>(state.get());
						if (expression == "raw cleanup" || expression == "encoded cleanup") {
							tracing::registerDestructor(address.state, reinterpret_cast<void*>(rawCleanup));
							auto result = invoke(rawCleanupProxy, value);
							tracing::unregisterDestructor(address.state);
							return result;
						}
						return val<int32_t>(*address);
					});
				}
				auto module = builder.compile();
				REQUIRE(wrappers[iteration] > 0);
				requireUnsafeFallback(module, directory.path());
				if (expression == "member wrapper") {
					REQUIRE(module.getFunction<int64_t(MemberState*)>("execute")(&member) == 73);
					member.total += 7;
					REQUIRE(module.getFunction<int64_t(MemberState*)>("execute")(&member) == 80);
					member.total = 73;
				} else if (expression == "raw cleanup" || expression == "encoded cleanup") {
					const auto execute = module.getFunction<int32_t(int32_t)>("execute");
					const auto before = *state;
					REQUIRE(execute(7) == 7);
					REQUIRE(*state == before);
					REQUIRE_THROWS_WITH(execute(-1), "raw cleanup proxy");
					REQUIRE(*state == before + 1);
				} else {
					REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(0) == *state);
					++*state;
					REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(0) == *state);
				}
			}
		}
	}
}

TEST_CASE("Encoded heap expressions cannot become relocatable through control flow or callees", "[cache][legacy]") {
	requireCachePlatform();
	for (const bool foldConstants : {false, true}) {
		for (const auto* expression :
		     {"direct", "arithmetic", "runtime offset", "branch", "loop", "null base", "null pointer offset",
		      "pointer difference", "callee result", "callee argument", "indirect result", "indirect argument",
		      "selected result", "nonnull capture", "nonnull difference", "null branch", "unrelated null guard"}) {
			DYNAMIC_SECTION(foldConstants << ": " << expression) {
				TemporaryCacheDirectory directory;
				auto options = cacheOptions(directory.path(), "encoded-heap-expressions");
				options.setOption("ir.runOptimizationPasses", true);
				options.setOption("ir.disableConstantFolding", !foldConstants);
				std::array<std::unique_ptr<std::array<int64_t, 4>>, 2> storage;
				std::array<int, 2> wrappers {};
				for (std::size_t iteration = 0; iteration < storage.size(); ++iteration) {
					storage[iteration] = std::make_unique<std::array<int64_t, 4>>();
					auto& values = *storage[iteration];
					values = {11 + static_cast<int64_t>(iteration), 22, 33, 44};
					const auto encoded = reinterpret_cast<uintptr_t>(values.data());
					NautilusFunction encodedAddress {"encoded_address", [encoded] { return val<uintptr_t>(encoded); }};
					NautilusFunction offsetEncoded {
					    "offset_encoded", [encoded](val<uintptr_t> delta) { return val<uintptr_t>(encoded) + delta; }};
					NautilusFunction shiftedEncoded {"shifted_encoded", [encoded](val<uintptr_t> delta) {
						                                 return val<uintptr_t>(encoded + 3 * sizeof(int64_t)) - delta;
					                                 }};
					NautilusFunction dereference {"dereference", [](val<uintptr_t> address) -> val<int64_t> {
						                              val<int64_t*> pointer = address;
						                              return *pointer;
					                              }};
					NautilusEngine engine(cache::createCompiler(options), options);
					auto builder = engine.createModule();
					builder.registerFunction<val<int64_t>(val<uintptr_t>, val<int64_t*>, val<int64_t*>)>(
					    "execute",
					    [=, &wrappers, &encodedAddress, &dereference, &offsetEncoded, &shiftedEncoded](
					        val<uintptr_t> offset, val<int64_t*> runtime, val<int64_t*> state) -> val<int64_t> {
						    ++wrappers[iteration];
						    val<uintptr_t> address = encoded;
						    const std::string_view kind(expression);
						    if (kind == "arithmetic") {
							    address = (address - sizeof(int64_t)) + sizeof(int64_t);
						    } else if (kind == "runtime offset") {
							    address = address + offset * sizeof(int64_t);
						    } else if (kind == "branch") {
							    address = static_cast<val<uintptr_t>>(runtime);
							    if (offset > 0) {
								    address = encoded;
							    }
						    } else if (kind == "loop") {
							    for (val<uintptr_t> index = 0; index < offset; ++index) {
								    address = address + sizeof(int64_t);
							    }
						    } else if (kind == "null base") {
							    val<int64_t*> null = nullptr;
							    address = static_cast<val<uintptr_t>>(null) + address;
						    } else if (kind == "null pointer offset") {
							    val<int8_t*> null = nullptr;
							    auto pointer = static_cast<val<int64_t*>>(null + address);
							    return *pointer + *state;
						    } else if (kind == "pointer difference" || kind == "nonnull difference") {
							    const auto base = static_cast<val<uintptr_t>>(runtime);
							    address = (base + address) - base;
						    } else if (kind == "callee result") {
							    address = encodedAddress();
						    } else if (kind == "callee argument") {
							    return dereference(address) + *state;
						    } else if (kind == "indirect result") {
							    address = encodedAddress.getFuncPtr()();
						    } else if (kind == "indirect argument") {
							    return dereference.getFuncPtr()(address) + *state;
						    } else if (kind == "selected result") {
							    auto callback = offsetEncoded.getFuncPtr();
							    if (offset > 0) {
								    callback = shiftedEncoded.getFuncPtr();
							    }
							    address = callback(offset * cacheLiteral<uintptr_t {sizeof(int64_t)}>());
						    }
						    if (kind == "null branch" || kind == "unrelated null guard") {
							    auto base =
							        select(offset > 0, static_cast<val<int8_t*>>(runtime), val<int8_t*>(nullptr));
							    if (kind == "null branch") {
								    if (base == nullptr) {
									    auto pointer = static_cast<val<int64_t*>>(base + address);
									    return *pointer + *state;
								    }
							    } else if (runtime != nullptr) {
								    auto delta = select(offset > 0, val<uintptr_t>(0), address);
								    val<int64_t*> pointer = static_cast<val<uintptr_t>>(base) + delta;
								    return *pointer + *state;
							    }
							    return *runtime + *state;
						    }
						    val<int64_t*> pointer = address;
						    if (kind == "nonnull capture" || kind == "nonnull difference") {
							    if (pointer != nullptr) {
								    return pointer[1] + *state;
							    }
							    return *state;
						    }
						    return *pointer + *state;
					    });
					auto module = builder.compile();
					REQUIRE(wrappers[iteration] > 0);
					requireUnsafeFallback(module, directory.path());
					const auto execute = module.getFunction<int64_t(uintptr_t, int64_t*, int64_t*)>("execute");
					const std::string_view kind(expression);
					const bool guarded = kind == "nonnull capture" || kind == "nonnull difference";
					const bool advances = kind == "runtime offset" || kind == "loop";
					std::array<int64_t, 4> runtimeValues {1011 + static_cast<int64_t>(iteration), 1022, 1033, 1044};
					const auto first = kind == "branch" ? runtimeValues[0] : values[guarded ? 1 : 0];
					const auto second = kind == "null branch" || kind == "unrelated null guard" ? runtimeValues[0]
					                    : kind == "selected result" ? values[1]
					                                                : values[guarded    ? 1
					                                                         : advances ? 2
					                                                                    : 0];
					REQUIRE(execute(0, runtimeValues.data(), &values[3]) == first + values[3]);
					REQUIRE(execute(2, runtimeValues.data(), &values[3]) == second + values[3]);
				}
				REQUIRE(storage[0].get() != storage[1].get());
			}
		}
	}
}

TEST_CASE("Legacy cache rejects memory and native callback laundering with runtime pointer arguments",
          "[cache][legacy]") {
	requireCachePlatform();
	for (const bool foldConstants : {false, true}) {
		for (const std::string_view kind : {"runtime slot",
		                                    "runtime alias",
		                                    "argument alias",
		                                    "alloca",
		                                    "pointer load",
		                                    "indirect load",
		                                    "callee store",
		                                    "callee load",
		                                    "indirect store",
		                                    "native store",
		                                    "native load",
		                                    "native result",
		                                    "native pointer result",
		                                    "indirect result",
		                                    "partial store",
		                                    "cross export",
		                                    "pointer cancellation",
		                                    "native difference",
		                                    "native memory difference",
		                                    "native consumer",
		                                    "native integer consumer",
		                                    "native void consumer",
		                                    "native callback",
		                                    "native callback offset",
		                                    "native returned callback"}) {
			DYNAMIC_SECTION(foldConstants << ": " << kind) {
				TemporaryCacheDirectory directory;
				auto options = cacheOptions(directory.path(), "memory-and-call-laundering");
				options.setOption("ir.runOptimizationPasses", true);
				options.setOption("ir.disableConstantFolding", !foldConstants);
				std::array<std::unique_ptr<int64_t>, 2> values;
				std::array<int, 2> wrappers {};
				for (std::size_t iteration = 0; iteration < values.size(); ++iteration) {
					values[iteration] = std::make_unique<int64_t>(101 + iteration * 17);
					const auto encoded = reinterpret_cast<uintptr_t>(values[iteration].get());
					std::array<uintptr_t, 2> scratch {};
					auto* scratchAddress = &scratch[1];
					int64_t observed = 0;
					NautilusFunction storeAddress {"store_address",
					                               [encoded](val<uintptr_t*> slot) { *slot = encoded; }};
					NautilusFunction loadAddress {"load_address",
					                              [](val<uintptr_t*> slot) -> val<uintptr_t> { return *slot; }};
					NautilusFunction encodedCallback {"encoded_callback",
					                                  [encoded] { return val<uintptr_t>(encoded); }};
					NautilusFunction offsetCallback {"offset_callback",
					                                 [encoded](val<uintptr_t> base) { return base - encoded; }};
					NautilusFunction callbackFactory {"callback_factory",
					                                  [&offsetCallback] { return offsetCallback.getFuncPtr(); }};
					auto nativeStore = +[](uintptr_t* slot, uintptr_t value) noexcept {
						*slot = value;
					};
					auto nativeLoad = +[](uintptr_t* slot) noexcept {
						return *slot;
					};
					auto nativeIdentity = +[](uintptr_t address) {
						return address;
					};
					auto nativePointer = +[](uintptr_t address) noexcept {
						return reinterpret_cast<int64_t*>(address);
					};
					auto nativeDifference = +[](uintptr_t left, uintptr_t right) noexcept {
						return left - right;
					};
					auto nativeMemoryDifference = +[](uintptr_t* slot) noexcept {
						return reinterpret_cast<uintptr_t>(slot) - *slot;
					};
					auto nativeConsumer = +[](uintptr_t* slot) noexcept {
						return *reinterpret_cast<int64_t*>(*slot);
					};
					auto nativeIntegerConsumer = +[](uintptr_t address) noexcept {
						return *reinterpret_cast<int64_t*>(address);
					};
					auto nativeVoidConsumer = +[](uintptr_t* slot, int64_t* result) noexcept {
						*result = *reinterpret_cast<int64_t*>(*slot);
					};
					auto nativeCallback = +[](uintptr_t (*callback)()) {
						return *reinterpret_cast<int64_t*>(callback());
					};
					auto nativeOffsetCallback = +[](uintptr_t (*callback)(uintptr_t), uintptr_t base) {
						return *reinterpret_cast<int64_t*>(base - callback(base));
					};
					NautilusEngine engine(cache::createCompiler(options), options);
					auto builder = engine.createModule();
					if (kind == "cross export") {
						builder.registerFunction<val<uintptr_t>(val<uintptr_t*>)>("initialize",
						                                                          [=](val<uintptr_t*> slot) {
							                                                          slot[1] = encoded;
							                                                          return val<uintptr_t>(encoded);
						                                                          });
					}
					builder.registerFunction<val<int64_t>(val<uintptr_t*>, val<uintptr_t*>, val<uintptr_t**>,
					                                      val<int64_t*>, val<uintptr_t (*)(uintptr_t)>)>(
					    "execute",
					    [=, &storeAddress, &loadAddress, &encodedCallback, &offsetCallback, &callbackFactory,
					     &wrappers](val<uintptr_t*> scratch, val<uintptr_t*> argument, val<uintptr_t**> table,
					                val<int64_t*> observed, val<uintptr_t (*)(uintptr_t)> callback) -> val<int64_t> {
						    ++wrappers[iteration];
						    if (kind == "native callback") {
							    return invoke(nativeCallback, encodedCallback.getFuncPtr());
						    }
						    if (kind == "native returned callback") {
							    return invoke(nativeOffsetCallback, callbackFactory(),
							                  static_cast<val<uintptr_t>>(argument));
						    }
						    if (kind == "native callback offset") {
							    return invoke(nativeOffsetCallback, offsetCallback.getFuncPtr(),
							                  static_cast<val<uintptr_t>>(argument));
						    }
						    auto slot = kind == "native memory difference" || kind == "native consumer" ||
						                        kind == "native void consumer"
						                    ? argument
						                    : scratch + 1;
						    if (kind == "alloca") {
							    slot = nautilus::details::nautilus_alloca<uintptr_t>();
						    }
						    if (kind == "pointer cancellation" || kind == "native memory difference") {
							    *slot = static_cast<val<uintptr_t>>(slot) - encoded;
						    } else if (kind == "callee store") {
							    storeAddress(slot);
						    } else if (kind == "indirect store") {
							    storeAddress.getFuncPtr()(slot);
						    } else if (kind == "native store") {
							    invoke(nativeStore, slot, val<uintptr_t>(encoded));
						    } else if (kind == "partial store") {
							    auto bytes = static_cast<val<uint8_t*>>(slot);
							    const auto encodedBytes =
							        std::bit_cast<std::array<uint8_t, sizeof(uintptr_t)>>(encoded);
							    for (static_val<std::size_t> index = 0; index < encodedBytes.size(); ++index) {
								    bytes[index] = encodedBytes[index];
							    }
						    } else if (kind != "cross export" && kind != "native result" &&
						               kind != "native pointer result" && kind != "indirect result" &&
						               kind != "native difference" && kind != "native integer consumer") {
							    *slot = encoded;
						    }
						    if (kind == "runtime alias" || kind == "argument alias") {
							    slot = argument;
						    } else if (kind == "indirect load") {
							    slot = *table;
						    }
						    if (kind == "native consumer") {
							    return invoke(nativeConsumer, slot);
						    }
						    if (kind == "native integer consumer") {
							    return invoke(nativeIntegerConsumer, val<uintptr_t>(encoded));
						    }
						    if (kind == "native void consumer") {
							    invoke(nativeVoidConsumer, slot, observed);
							    return *observed;
						    }
						    if (kind == "pointer load") {
							    val<int64_t*> pointer = *static_cast<val<int64_t**>>(slot);
							    return *pointer;
						    }
						    if (kind == "native pointer result") {
							    return *invoke(nativePointer, val<uintptr_t>(encoded));
						    }
						    val<uintptr_t> address;
						    if (kind == "pointer cancellation") {
							    val<uintptr_t> loaded = *slot;
							    address = static_cast<val<uintptr_t>>(slot) - loaded;
						    } else if (kind == "native difference") {
							    auto base = static_cast<val<uintptr_t>>(scratch);
							    address = invoke(nativeDifference, base + encoded, base);
						    } else if (kind == "native memory difference") {
							    address = invoke(nativeMemoryDifference, slot);
						    } else if (kind == "callee load") {
							    address = loadAddress(slot);
						    } else if (kind == "native load") {
							    address = invoke(nativeLoad, slot);
						    } else if (kind == "native result") {
							    address = invoke(nativeIdentity, val<uintptr_t>(encoded));
						    } else if (kind == "indirect result") {
							    address = callback(val<uintptr_t>(encoded));
						    } else {
							    address = *slot;
						    }
						    val<int64_t*> pointer = address;
						    return *pointer;
					    });
					auto module = builder.compile();
					REQUIRE(wrappers[iteration] > 0);
					requireUnsafeFallback(module, directory.path());
					REQUIRE(scratch[0] == 0);
					REQUIRE(scratch[1] == 0);
					REQUIRE(observed == 0);
					if (kind == "cross export") {
						REQUIRE(module.getFunction<uintptr_t(uintptr_t*)>("initialize")(scratch.data()) == encoded);
					}
					const auto execute = module.getFunction<int64_t(uintptr_t*, uintptr_t*, uintptr_t**, int64_t*,
					                                                uintptr_t (*)(uintptr_t))>("execute");
					REQUIRE(execute(scratch.data(), scratchAddress, &scratchAddress, &observed, nativeIdentity) ==
					        *values[iteration]);
				}
				REQUIRE(values[0].get() != values[1].get());
			}
		}
	}
}

TEST_CASE("Certified data does not launder partial encoded bytes bits callbacks or cleanup", "[cache][legacy]") {
	requireCachePlatform();
	for (const bool foldConstants : {false, true}) {
		for (const std::string_view kind : {"integer", "partial bytes", "boolean bits", "callback", "cleanup"}) {
			DYNAMIC_SECTION(foldConstants << ": " << kind) {
				TemporaryCacheDirectory directory;
				auto options = cacheOptions(directory.path(), "mixed-certified-encoded-fragments");
				options.setOption("ir.runOptimizationPasses", true);
				options.setOption("ir.disableConstantFolding", !foldConstants);
				std::array<std::unique_ptr<int64_t>, 2> values;
				std::array<int, 2> wrappers {};
				for (std::size_t iteration = 0; iteration < values.size(); ++iteration) {
					values[iteration] = std::make_unique<int64_t>(41 + iteration);
					const auto encoded = reinterpret_cast<uintptr_t>(values[iteration].get());
					uintptr_t scratch = encoded;
					if (kind == "partial bytes") {
						auto bytes = std::bit_cast<std::array<uint8_t, sizeof(uintptr_t)>>(encoded);
						for (std::size_t index = 0; index < sizeof(uintptr_t) / 2; ++index) {
							bytes[index] ^= 0xff;
						}
						scratch = std::bit_cast<uintptr_t>(bytes);
						REQUIRE(scratch != encoded);
					}
					std::array<bool, sizeof(uintptr_t) * 8> bits {};
					int64_t marker = 0;
					auto consumeInteger = +[](uintptr_t address, int64_t* marker) noexcept {
						return *reinterpret_cast<int64_t*>(address) + ++*marker;
					};
					auto consumeBytes = +[](uintptr_t* address, int64_t* marker) noexcept {
						return *reinterpret_cast<int64_t*>(*address) + ++*marker;
					};
					auto consumeBits = +[](bool* bits, int64_t* marker) noexcept {
						uintptr_t address = 0;
						for (std::size_t index = 0; index < sizeof(uintptr_t) * 8; ++index) {
							address |= static_cast<uintptr_t>(bits[index]) << index;
						}
						return *reinterpret_cast<int64_t*>(address) + ++*marker;
					};
					auto consumeCallback = +[](uintptr_t (*callback)(), int64_t* marker) {
						return *reinterpret_cast<int64_t*>(callback()) + ++*marker;
					};
					auto cleanup = +[](int64_t* value) noexcept {
						++*value;
					};
					auto throwing = +[](int64_t* marker, bool fail) {
						++*marker;
						if (fail) {
							throw std::runtime_error("mixed certified cleanup");
						}
						return *marker;
					};
					NautilusFunction callback {"mixed_encoded_callback", [encoded] {
						                           return val<uintptr_t>(encoded) + cacheLiteral<uintptr_t {0}>();
					                           }};
					NautilusEngine engine(cache::createCompiler(options), options);
					auto builder = engine.createModule();
					builder.registerFunction<val<int64_t>(val<uintptr_t*>, val<bool*>, val<int64_t*>, val<bool>)>(
					    "execute", [=, &callback, &wrappers](val<uintptr_t*> slot, val<bool*> bitStorage,
					                                         val<int64_t*> marked, val<bool> fail) {
						    ++wrappers[iteration];
						    *marked = cacheLiteral<int64_t {9}>();
						    if (kind == "partial bytes") {
							    auto bytes = static_cast<val<uint8_t*>>(slot);
							    const auto encodedBytes =
							        std::bit_cast<std::array<uint8_t, sizeof(uintptr_t)>>(encoded);
							    for (static_val<std::size_t> index = 0; index < sizeof(uintptr_t) / 2; ++index) {
								    bytes[cacheInvariant(static_cast<std::size_t>(index))] = encodedBytes[index];
							    }
							    return invoke(consumeBytes, slot, marked);
						    }
						    if (kind == "boolean bits") {
							    for (static_val<std::size_t> index = 0; index < sizeof(uintptr_t) * 8; ++index) {
								    bitStorage[cacheInvariant(static_cast<std::size_t>(index))] =
								        bool((encoded >> static_cast<std::size_t>(index)) & 1);
							    }
							    return invoke(consumeBits, bitStorage, marked);
						    }
						    if (kind == "callback") {
							    return invoke(consumeCallback, callback.getFuncPtr(), marked);
						    }
						    if (kind == "cleanup") {
							    val<int64_t*> address = val<uintptr_t>(encoded) + cacheLiteral<uintptr_t {0}>();
							    tracing::registerDestructor(address.state, reinterpret_cast<void*>(cleanup));
							    auto result = invoke(throwing, marked, fail);
							    tracing::unregisterDestructor(address.state);
							    return result;
						    }
						    return invoke(consumeInteger, val<uintptr_t>(encoded) + cacheLiteral<uintptr_t {0}>(),
						                  marked);
					    });
					auto module = builder.compile();
					REQUIRE(wrappers[iteration] > 0);
					requireUnsafeFallback(module, directory.path());
					REQUIRE(cacheStat<std::string>(module, "cache.scalarRejection").find("uncertified_scalar") !=
					        std::string::npos);
					REQUIRE(marker == 0);
					const auto execute = module.getFunction<int64_t(uintptr_t*, bool*, int64_t*, bool)>("execute");
					REQUIRE(execute(&scratch, bits.data(), &marker, false) ==
					        (kind == "cleanup" ? 10 : *values[iteration] + 10));
					REQUIRE(marker == 10);
					if (kind == "partial bytes") {
						REQUIRE(scratch == encoded);
					}
					if (kind == "cleanup") {
						const auto before = *values[iteration];
						REQUIRE_THROWS_WITH(execute(&scratch, bits.data(), &marker, true), "mixed certified cleanup");
						REQUIRE(*values[iteration] == before + 1);
						REQUIRE(marker == 10);
					}
				}
				REQUIRE(values[0].get() != values[1].get());
			}
		}
	}
}

TEST_CASE("Encoded addresses consumed only by cleanup never publish artifacts", "[cache][legacy][guard]") {
	requireCachePlatform();
	TemporaryCacheDirectory directory;
	const auto options = cacheOptions(directory.path(), "encoded-destructor-only");
	std::array<std::unique_ptr<int64_t>, 2> values;
	std::array<int, 2> wrappers {};
	for (std::size_t iteration = 0; iteration < values.size(); ++iteration) {
		values[iteration] = std::make_unique<int64_t>(41 + iteration);
		const auto encoded = reinterpret_cast<uintptr_t>(values[iteration].get());
		auto cleanup = +[](int64_t* pointer) noexcept {
			++*pointer;
		};
		auto nativeCall = +[](int64_t value) {
			if (value < 0) {
				throw std::runtime_error("encoded cleanup");
			}
			return value;
		};
		NautilusEngine engine(cache::createCompiler(options), options);
		auto builder = engine.createModule();
		builder.registerFunction<val<int64_t>(val<int64_t*>, val<int64_t>)>(
		    "execute", [=, &wrappers](val<int64_t*> base, val<int64_t> value) {
			    ++wrappers[iteration];
			    val<int64_t*> address = static_cast<val<uintptr_t>>(base) + encoded;
			    tracing::registerDestructor(address.state, reinterpret_cast<void*>(cleanup));
			    auto result = invoke(nativeCall, value);
			    tracing::unregisterDestructor(address.state);
			    return result;
		    });
		auto module = builder.compile();
		REQUIRE(wrappers[iteration] > 0);
		requireUnsafeFallback(module, directory.path());
		const auto execute = module.getFunction<int64_t(int64_t*, int64_t)>("execute");
		const auto before = *values[iteration];
		REQUIRE(execute(nullptr, 7) == 7);
		REQUIRE(*values[iteration] == before);
		REQUIRE_THROWS_WITH(execute(nullptr, -1), "encoded cleanup");
		REQUIRE(*values[iteration] == before + 1);
	}
}

#ifdef __linux__
extern "C" int32_t nautilusCacheUnidentifiedProxy(int32_t value);

TEST_CASE("Unidentified native callbacks compile normally without cache publication", "[cache][imports]") {
	requireCachePlatform();
	REQUIRE_FALSE(common::locateExecutableAddress(reinterpret_cast<const void*>(&nautilusCacheUnidentifiedProxy)));
	TemporaryCacheDirectory directory;
	const auto options = cacheOptions(directory.path(), "unidentified-native-callback");
	std::array<int, 2> wrappers {};
	for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
		NautilusEngine engine(cache::createCompiler(options), options);
		auto builder = engine.createModule();
		builder.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&, iteration](val<int32_t> value) {
			++wrappers[iteration];
			return invoke(nautilusCacheUnidentifiedProxy, value);
		});
		auto module = builder.compile();
		REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(41) == 42);
		REQUIRE(wrappers[iteration] > 0);
		requireFallback(module);
		REQUIRE(readArtifacts(directory.path()).empty());
	}
}

TEST_CASE("Native and bytecode cache loads relocate proxies and exceptions across a fresh ASLR exec",
          "[cache][guard][exec]") {
	requireCachePlatform();
	constexpr auto TEST_NAME =
	    "Native and bytecode cache loads relocate proxies and exceptions across a fresh ASLR exec";
	constexpr auto CHILD_DIRECTORY = "NAUTILUS_CACHE_RUNTIME_CHILD_DIRECTORY";
	constexpr auto CHILD_MODE = "NAUTILUS_CACHE_RUNTIME_CHILD_MODE";
	if (const auto* directory = std::getenv(CHILD_DIRECTORY)) {
		const auto personality = ::personality(0xffffffffUL);
		REQUIRE(personality != -1);
		REQUIRE((personality & ADDR_NO_RANDOMIZE) == 0);
		const auto* mode = std::getenv(CHILD_MODE);
		REQUIRE(mode != nullptr);
		const bool repair = std::string_view(mode) == "repair";
		REQUIRE((repair || std::string_view(mode) == "native" || std::string_view(mode) == "repaired native"));
		const auto options = cacheOptions(directory, "fresh-exec-runtime-guarded");
		RuntimeWrappers wrappers {};
		auto module = compileRuntimeModule(options, wrappers);
		checkRuntimeModule(module);
		REQUIRE((wrappers == RuntimeWrappers {}));
		REQUIRE(cacheStat<int64_t>(module, "cache.tracingRan") == 0);
		if (repair) {
			REQUIRE(cacheStat<std::string>(module, "cache.object") != "hit");
			REQUIRE(cacheStat<std::string>(module, "cache.mlir") == "hit");
			REQUIRE(module.getStatistics()->contains("jit.compile.ms"));
		} else {
			requireNativeHit(module);
		}
		auto* bridge = compiler::mlir::getExceptionPersonalityAddress();
		auto* runtime = ::dlsym(RTLD_DEFAULT, "__gxx_personality_v0");
		REQUIRE(runtime != nullptr);
		REQUIRE(bridge != runtime);
		const auto image = common::locateExecutableAddress(bridge);
		REQUIRE(image.has_value());
		REQUIRE(common::resolveExecutableAddress(*image) == bridge);
		std::ofstream report(std::filesystem::path(directory) / (std::string(mode) + ".report"));
		report << cacheProxyAddress() << ' ' << reinterpret_cast<uintptr_t>(bridge) << ' '
		       << reinterpret_cast<uintptr_t>(runtime) << '\n';
		report << image->buildId << ' ' << image->loadOffset << ' ' << cacheStat<std::string>(module, "cache.key")
		       << '\n';
		report << module.getStatistics()->toString();
		report.close();
		REQUIRE_FALSE(report.fail());
		return;
	}
	TemporaryCacheDirectory directory;
	const auto options = cacheOptions(directory.path(), "fresh-exec-runtime-guarded");
	RuntimeWrappers coldWrappers {};
	auto cold = compileRuntimeModule(options, coldWrappers);
	requireWritten(cold);
	checkRuntimeModule(cold);
	const auto recordedWrappers = coldWrappers;
	for (const auto calls : coldWrappers) {
		REQUIRE(calls > 0);
	}
	const auto bytecodePath = artifactPath(cold, directory.path(), ".mlirbc");
	const auto originalBytecode = readFile(bytecodePath);
	const auto* bridge = compiler::mlir::getExceptionPersonalityAddress();
	const auto image = common::locateExecutableAddress(bridge);
	REQUIRE(image.has_value());
	const std::array<uintptr_t, 3> parentAddresses {
	    cacheProxyAddress(), reinterpret_cast<uintptr_t>(bridge),
	    reinterpret_cast<uintptr_t>(::dlsym(RTLD_DEFAULT, "__gxx_personality_v0"))};
	const auto executable = std::filesystem::read_symlink("/proc/self/exe");
	for (const auto* mode : {"native", "repair", "repaired native"}) {
		CAPTURE(mode);
		if (std::string_view(mode) == "native") {
			writeFile(bytecodePath, "native loading must not parse this bytecode");
		} else if (std::string_view(mode) == "repair") {
			writeFile(bytecodePath, originalBytecode);
			writeFile(artifactPath(cold, directory.path(), ".o"), "force bytecode repair in a new address space");
		}
		const auto child = ::fork();
		REQUIRE(child >= 0);
		if (child == 0) {
			if (::setenv(CHILD_DIRECTORY, directory.path().c_str(), 1) != 0 || ::setenv(CHILD_MODE, mode, 1) != 0 ||
			    ::setenv("TMPDIR", directory.path().c_str(), 1) != 0 || ::chdir(directory.path().c_str()) != 0) {
				::_exit(125);
			}
			::execl(executable.c_str(), executable.c_str(), TEST_NAME, "--reporter", "compact",
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
		std::array<uintptr_t, 3> childAddresses {};
		std::string childBuildId, childKey;
		uint64_t childOffset = 0;
		std::ifstream report(directory.path() / (std::string(mode) + ".report"));
		REQUIRE(static_cast<bool>(report >> childAddresses[0] >> childAddresses[1] >> childAddresses[2] >>
		                          childBuildId >> childOffset >> childKey));
		REQUIRE(childBuildId == image->buildId);
		REQUIRE(childOffset == image->loadOffset);
		REQUIRE(childKey == cacheStat<std::string>(cold, "cache.key"));
		for (std::size_t index = 0; index < parentAddresses.size(); ++index) {
			CAPTURE(index, parentAddresses[index], childAddresses[index]);
			REQUIRE(childAddresses[index] != 0);
			REQUIRE(childAddresses[index] != parentAddresses[index]);
			for (const auto& [name, contents] : readArtifacts(directory.path())) {
				CAPTURE(name);
				for (const auto address : {parentAddresses[index], childAddresses[index]}) {
					const std::string raw(reinterpret_cast<const char*>(&address), sizeof(address));
					REQUIRE(contents.find(raw) == std::string::npos);
					REQUIRE(contents.find(std::to_string(address)) == std::string::npos);
				}
			}
		}
		std::cout << "cache ASLR " << mode << " parent=" << parentAddresses[0] << " child=" << childAddresses[0]
		          << " child wrappers=0\n";
		REQUIRE(coldWrappers == recordedWrappers);
	}
}
#endif

TEST_CASE("Core background promotion remains free of cache policy after compiler destruction",
          "[cache][core][tiering]") {
	TemporaryCacheDirectory directory;
	Options options;
	options.setOption("engine.cache.directory", directory.path().string());
	options.setOption("engine.cache.key", std::string("core-background-is-not-a-cache"));
	options.setOption("engine.tier0.backend", std::string(INTERPRETER_BACKEND));
	options.setOption("engine.tier1.backend", std::string("mlir"));
	options.setOption("engine.tiered.backgroundPromotion", true);
	options.setOption("mlir.enableMultithreading", false);
	std::array<std::atomic<int>, 2> wrappers {};
	std::vector<CompiledModule> compiled;
	{
		NautilusEngine engine(options);
		for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
			auto builder = engine.createModule();
			builder.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&, iteration](val<int32_t> value) {
				if (tracing::inTracer()) {
					wrappers[iteration].fetch_add(1);
				}
				return value + cacheLiteral<int32_t {1}>();
			});
			compiled.push_back(builder.compile());
			REQUIRE(wrappers[iteration].load() > 0);
		}
	}
	for (auto& module : compiled) {
		REQUIRE(module.getState()->version.load() == 1);
		REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(41) == 42);
		requireNoCacheStatistics(module);
	}
	REQUIRE(std::filesystem::is_empty(directory.path()));
}

TEST_CASE("Concurrent distinct-key modules retain independent IR options statistics and executables",
          "[cache][concurrency]") {
	requireCachePlatform();
	TemporaryCacheDirectory directory;
	const auto options = cacheOptions(directory.path(), "unused-engine-key");
	constexpr std::size_t count = 4;
	std::array<std::atomic<int>, count> wrappers {};
	std::array<std::optional<CompiledModule>, count> modules;
	std::array<std::exception_ptr, count> failures {};
	std::barrier start(static_cast<std::ptrdiff_t>(count));
	std::set<std::string> keys;
	{
		NautilusEngine engine(cache::createCompiler(options), options);
		std::vector<std::thread> threads;
		for (std::size_t index = 0; index < count; ++index) {
			threads.emplace_back([&, index] {
				start.arrive_and_wait();
				try {
					auto builder = engine.createModule();
					builder.setOption("engine.cache.key", "concurrent-plan/" + std::to_string(index));
					builder.setOption("nes.workerThreads", static_cast<int>(index + 1));
					builder.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&, index](val<int32_t> value) {
						wrappers[index].fetch_add(1);
						return value + cacheInvariant(static_cast<int32_t>(index + 1));
					});
					modules[index].emplace(builder.compile());
				} catch (...) {
					failures[index] = std::current_exception();
				}
			});
		}
		for (auto& thread : threads) {
			thread.join();
		}
		for (std::size_t index = 0; index < count; ++index) {
			CAPTURE(index);
			if (failures[index]) {
				std::rethrow_exception(failures[index]);
			}
			REQUIRE(modules[index].has_value());
			requireWritten(*modules[index]);
			REQUIRE(wrappers[index].load() > 0);
			REQUIRE(keys.insert(cacheStat<std::string>(*modules[index], "cache.key")).second);
			ModuleOptions overrides;
			overrides.setOption("engine.cache.key", "concurrent-plan/" + std::to_string(index));
			overrides.setOption("nes.workerThreads", static_cast<int>(index + 1));
			int warmWrappers = 0;
			auto warm = compileIncrement(engine, warmWrappers, static_cast<int32_t>(index + 1), overrides);
			requireNativeHit(warm);
			REQUIRE(warmWrappers == 0);
			REQUIRE(warm.getFunction<int32_t(int32_t)>("execute")(7) == static_cast<int32_t>(8 + index));
		}
	}
	for (std::size_t index = 0; index < count; ++index) {
		REQUIRE(modules[index]->getFunction<int32_t(int32_t)>("execute")(7) == static_cast<int32_t>(8 + index));
	}
	REQUIRE(keys.size() == count);
	REQUIRE(readArtifacts(directory.path()).size() == count * 3);
}

#ifdef __linux__
TEST_CASE("Untrusted file metadata cannot authorize native hits or overwrite external targets", "[cache][storage]") {
	requireCachePlatform();
	for (const auto* kind : {"symlink object", "hardlinked object", "public object", "symlink manifest"}) {
		DYNAMIC_SECTION(kind) {
			TemporaryCacheDirectory directory;
			TemporaryCacheDirectory outside;
			const auto options = cacheOptions(directory.path(), "untrusted-file-metadata");
			int coldWrappers = 0;
			auto cold = compileIncrement(options, coldWrappers);
			requireWritten(cold);
			const std::string_view failure(kind);
			const auto path = artifactPath(cold, directory.path(), failure == "symlink manifest" ? ".manifest" : ".o");
			const auto contents = readFile(path);
			const auto target = outside.path() / "target";
			writeFile(target, contents);
			std::filesystem::permissions(target,
			                             std::filesystem::perms::owner_read | std::filesystem::perms::owner_write);
			if (failure == "public object") {
				std::filesystem::permissions(path, std::filesystem::perms::group_read,
				                             std::filesystem::perm_options::add);
			} else {
				REQUIRE(std::filesystem::remove(path));
				if (failure == "hardlinked object") {
					std::filesystem::create_hard_link(target, path);
				} else {
					std::filesystem::create_symlink(target, path);
				}
			}
			int wrappers = 0;
			auto rejected = compileIncrement(options, wrappers);
			REQUIRE(rejected.getFunction<int32_t(int32_t)>("execute")(41) == 42);
			REQUIRE(cacheStat<std::string>(rejected, "cache.object") != "hit");
			if (failure == "symlink manifest") {
				REQUIRE(wrappers > 0);
				REQUIRE(cacheStat<int64_t>(rejected, "cache.tracingRan") == 1);
			} else {
				REQUIRE(wrappers == 0);
				REQUIRE(cacheStat<std::string>(rejected, "cache.mlir") == "hit");
				REQUIRE(cacheStat<int64_t>(rejected, "cache.tracingRan") == 0);
			}
			REQUIRE(readFile(target) == contents);
			REQUIRE(readFile(path) == contents);
		}
	}
}
TEST_CASE("Untrusted directories traversal and lock files decline before storage or frontend reuse",
          "[cache][storage]") {
	requireCachePlatform();
	for (const std::string_view kind : {"public directory", "symlink directory", "parent traversal", "symlink lock"}) {
		DYNAMIC_SECTION(kind) {
			TemporaryCacheDirectory directory;
			TemporaryCacheDirectory outside;
			std::filesystem::path cacheDirectory = directory.path();
			std::filesystem::path entry;
			if (kind == "public directory") {
				std::filesystem::permissions(cacheDirectory, std::filesystem::perms::group_read,
				                             std::filesystem::perm_options::add);
			} else if (kind == "symlink directory") {
				cacheDirectory /= "cache";
				std::filesystem::create_directory_symlink(outside.path(), cacheDirectory);
			} else if (kind == "parent traversal") {
				cacheDirectory /= "..";
				cacheDirectory /= directory.path().filename();
			}
			const auto options = cacheOptions(cacheDirectory, "untrusted-directory-and-lock");
			if (kind == "symlink lock") {
				int coldWrappers = 0;
				auto cold = compileIncrement(options, coldWrappers);
				requireWritten(cold);
				entry = artifactPath(cold, directory.path(), ".lock");
				REQUIRE(std::filesystem::remove(entry));
				writeFile(outside.path() / "lock", "external lock");
				std::filesystem::create_symlink(outside.path() / "lock", entry);
			}
			const auto original = readArtifacts(directory.path());
			for (int attempt = 0; attempt < 2; ++attempt) {
				int wrappers = 0;
				auto module = compileIncrement(options, wrappers);
				REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(41) == 42);
				REQUIRE(wrappers > 0);
				requireFallback(module);
				REQUIRE(cacheStat<std::string>(module, "cache.fallback").starts_with("cache_storage_unavailable:"));
				REQUIRE(readArtifacts(directory.path()) == original);
			}
			if (kind == "symlink lock") {
				REQUIRE(readFile(outside.path() / "lock") == "external lock");
				REQUIRE(std::filesystem::is_symlink(entry));
			}
		}
	}
}

TEST_CASE("Cooperative fresh processes publish one same-key entry and bypass all other wrappers",
          "[cache][concurrency][exec]") {
	requireCachePlatform();
	constexpr auto CHILD_DIRECTORY = "NAUTILUS_CACHE_PROCESS_DIRECTORY";
	constexpr auto CHILD_REPORT = "NAUTILUS_CACHE_PROCESS_REPORT";
	constexpr auto TEST_NAME = "Cooperative fresh processes publish one same-key entry and bypass all other wrappers";
	if (const auto* directory = std::getenv(CHILD_DIRECTORY)) {
		const auto* report = std::getenv(CHILD_REPORT);
		REQUIRE(report != nullptr);
		int wrappers = 0;
		auto module = compileIncrement(cacheOptions(directory, "cooperative-process-publication"), wrappers);
		REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(41) == 42);
		if (wrappers > 0) {
			requireWritten(module);
		} else {
			requireNativeHit(module);
		}
		writeFile(report, std::to_string(wrappers) + " " + cacheStat<std::string>(module, "cache.object") + " " +
		                      cacheStat<std::string>(module, "cache.key"));
		return;
	}
	TemporaryCacheDirectory directory;
	const auto executable = std::filesystem::read_symlink("/proc/self/exe");
	constexpr std::size_t count = 4;
	std::array<pid_t, count> children {};
	for (std::size_t index = 0; index < count; ++index) {
		children[index] = ::fork();
		REQUIRE(children[index] >= 0);
		if (children[index] == 0) {
			const auto cache = directory.path() / "cache";
			const auto report = directory.path() / (std::to_string(index) + ".report");
			if (::setenv(CHILD_DIRECTORY, cache.c_str(), 1) != 0 || ::setenv(CHILD_REPORT, report.c_str(), 1) != 0) {
				::_exit(125);
			}
			::execl(executable.c_str(), executable.c_str(), TEST_NAME, "--reporter", "compact",
			        static_cast<char*>(nullptr));
			::_exit(126);
		}
	}
	std::size_t coldRequests = 0;
	std::set<std::string> keys;
	for (std::size_t index = 0; index < count; ++index) {
		int status = 0;
		pid_t waited;
		do {
			waited = ::waitpid(children[index], &status, 0);
		} while (waited < 0 && errno == EINTR);
		REQUIRE(waited == children[index]);
		REQUIRE(WIFEXITED(status));
		REQUIRE(WEXITSTATUS(status) == 0);
		std::ifstream report(directory.path() / (std::to_string(index) + ".report"));
		int wrappers = -1;
		std::string object, key;
		report >> wrappers >> object >> key;
		REQUIRE_FALSE(report.fail());
		REQUIRE(wrappers >= 0);
		REQUIRE(object == (wrappers > 0 ? "written" : "hit"));
		coldRequests += wrappers > 0;
		keys.insert(key);
	}
	REQUIRE(coldRequests == 1);
	REQUIRE(keys.size() == 1);
	REQUIRE(readArtifacts(directory.path() / "cache").size() == 3);
}

TEST_CASE("Inaccessible cache directories never prevent ordinary compiled execution", "[cache][storage]") {
	requireCachePlatform();
	const auto directory =
	    std::filesystem::path("/proc/self") / ("nautilus-plugin-cache-inaccessible-" + std::to_string(::getpid()));
	REQUIRE_FALSE(std::filesystem::exists(directory));
	const auto options = cacheOptions(directory, "inaccessible-cache-storage");
	std::array<int, 2> wrappers {};
	for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
		auto module = compileIncrement(options, wrappers[iteration]);
		REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(41) == 42);
		REQUIRE(module.getFunction<int32_t(int32_t)>("execute")(-7) == -6);
		REQUIRE(wrappers[iteration] > 0);
		requireFallback(module);
		REQUIRE(module.getStatistics()->contains("tracing.ms"));
		REQUIRE_FALSE(std::filesystem::exists(directory));
	}
}
#endif

TEST_CASE("Runtime pointer arithmetic cannot erase uncertified encoded offsets", "[cache][legacy][regression]") {
	requireCachePlatform();
	auto storage = std::make_unique<uint8_t>(37);
	for (const auto& [mode, overrides] : validationModes()) {
		for (const bool reverse : {false, true}) {
			DYNAMIC_SECTION(mode << ": reverse=" << reverse) {
				TemporaryCacheDirectory directory;
				auto options = cacheOptions(directory.path(), "runtime-plus-encoded-offset");
				options.applyOverrides(overrides);
				NautilusEngine engine(cache::createCompiler(options), options);
				for (int attempt = 0; attempt < 2; ++attempt) {
					int wrappers = 0;
					auto builder = engine.createModule();
					builder.registerFunction<val<uint8_t*>(val<uint8_t*>)>(
					    "execute", [&, encoded = reinterpret_cast<uintptr_t>(storage.get())](val<uint8_t*> base) {
						    ++wrappers;
						    const auto address = static_cast<val<uintptr_t>>(base);
						    val<uint8_t*> result =
						        reverse ? val<uintptr_t>(encoded) + address : address + val<uintptr_t>(encoded);
						    return result;
					    });
					auto compiled = builder.compile();
					REQUIRE(wrappers > 0);
					requireFallback(compiled);
					REQUIRE(cacheStat<int64_t>(compiled, "cache.scalarCertificate") == 0);
					REQUIRE(cacheStat<std::string>(compiled, "cache.fallback") == "non_relocatable_pointer");
					REQUIRE(cacheStat<std::string>(compiled, "cache.rejection").find("Constant") != std::string::npos);
					REQUIRE(compiled.getFunction<uint8_t*(uint8_t*)>("execute")(nullptr) == storage.get());
					REQUIRE(readArtifacts(directory.path()).empty());
				}
			}
		}
	}
}

TEST_CASE("Legacy pointer offsets require their own recorded invariant evidence", "[cache][legacy][regression]") {
	requireCachePlatform();
	TemporaryCacheDirectory directory;
	const auto options = cacheOptions(directory.path(), "legacy-certified-offset");
	NautilusEngine engine(cache::createCompiler(options), options);
	std::array<uint8_t, 8> storage {};
	for (int attempt = 0; attempt < 2; ++attempt) {
		int wrappers = 0;
		auto builder = engine.createModule();
		builder.registerFunction<val<uint8_t*>(val<uint8_t*>)>("execute", [&](val<uint8_t*> base) {
			++wrappers;
			val<int32_t> unrelated(37);
			(void) unrelated;
			return base + cacheLiteral<int32_t {2}>();
		});
		auto compiled = builder.compile();
		REQUIRE(compiled.getFunction<uint8_t*(uint8_t*)>("execute")(storage.data()) == storage.data() + 2);
		if (attempt == 0) {
			requireWritten(compiled);
			REQUIRE(cacheStat<int64_t>(compiled, "cache.scalarCertificate") == 0);
			REQUIRE(wrappers > 0);
		} else {
			requireNativeHit(compiled);
			REQUIRE(wrappers == 0);
		}
	}
}

TEST_CASE("Cache transport preserves native conversions root names and conservative call attributes",
          "[cache][imports][review3]") {
	requireCachePlatform();
	for (const bool reverse : {false, true}) {
		DYNAMIC_SECTION("reverse=" << reverse) {
			TemporaryCacheDirectory directory;
			const auto options = cacheOptions(directory.path(), "native-conversion-name-attribute-regressions");
			NautilusEngine engine(cache::createCompiler(options), options);
			for (const std::string_view stage : {"cold", "warm", "repair", "repaired warm"}) {
				CAPTURE(stage);
				std::array<int, 5> wrappers {};
				NautilusFunction helper("helper",
				                        [](val<int32_t> value) { return value + cacheLiteral<int32_t {7}>(); });
				auto builder = engine.createModule();
				builder.registerFunction<val<double>(val<int64_t>, val<const int64_t*>, val<double (*)(double)>)>(
				    "conversion",
				    [&](val<int64_t> value, val<const int64_t*> element, val<double (*)(double)> callback) {
					    ++wrappers[0];
					    return invoke(cacheConvertedProxy, value) + invoke(cacheConvertedProxy, *element) +
					           callback(value);
				    });
				builder.registerFunction<val<int64_t>(val<int64_t>)>("attributes", [&](val<int64_t> value) {
					++wrappers[1];
					FunctionAttributes precise {ModRefInfo::NoModRef, true, true};
					FunctionAttributes conservative {ModRefInfo::ModRef, false, false};
					int64_t (*pointer)(int64_t) = cacheProxy;
					return invoke(reverse ? conservative : precise, pointer, value) +
					       invoke(reverse ? precise : conservative, pointer, value);
				});
				const auto registerRoot = [&] {
					builder.registerFunction<val<int32_t>(val<int32_t>)>("helper", [&](val<int32_t> value) {
						++wrappers[3];
						return value + cacheLiteral<int32_t {11}>();
					});
				};
				if (reverse) {
					registerRoot();
				}
				builder.registerFunction<val<int32_t>(val<int32_t>)>("caller", [&](val<int32_t> value) {
					++wrappers[2];
					return helper(value);
				});
				if (!reverse) {
					registerRoot();
				}
				builder.registerFunction<val<int32_t>(val<int32_t>)>("helper_2", [&](val<int32_t> value) {
					++wrappers[4];
					return value + cacheLiteral<int32_t {13}>();
				});
				auto compiled = builder.compile();
				int64_t element = 42;
				REQUIRE(compiled.getFunction<double(int64_t, const int64_t*, double (*)(double))>("conversion")(
				            42, &element, cacheConvertedProxy) == 129.0);
				REQUIRE(compiled.getFunction<int64_t(int64_t)>("attributes")(42) == 2 * cacheProxy(42));
				REQUIRE(compiled.getFunction<int32_t(int32_t)>("caller")(5) == 12);
				REQUIRE(compiled.getFunction<int32_t(int32_t)>("helper")(5) == 16);
				REQUIRE(compiled.getFunction<int32_t(int32_t)>("helper_2")(5) == 18);
				if (stage == "cold") {
					requireWritten(compiled);
					for (const auto calls : wrappers) {
						REQUIRE(calls > 0);
					}
				} else {
					REQUIRE((wrappers == std::array<int, 5> {}));
					if (stage == "repair") {
						REQUIRE(cacheStat<std::string>(compiled, "cache.object") == "invalid_rewritten");
						REQUIRE(cacheStat<std::string>(compiled, "cache.mlir") == "hit");
						requireNoFrontendStatistics(compiled);
					} else {
						requireNativeHit(compiled);
					}
				}
				if (stage == "warm") {
					writeFile(artifactPath(compiled, directory.path(), ".o"), "force review regression repair");
				}
			}
		}
	}
}

TEST_CASE("Cache packed wrapper name collisions fail catchably without publishing entries",
          "[cache][exports][review3]") {
	requireCachePlatform();
	for (const bool reverse : {false, true}) {
		CAPTURE(reverse);
		TemporaryCacheDirectory directory;
		const auto options = cacheOptions(directory.path(), "packed-wrapper-name-collision");
		NautilusEngine engine(cache::createCompiler(options), options);
		for (int attempt = 0; attempt < 2; ++attempt) {
			int wrappers = 0;
			auto builder = engine.createModule();
			for (const auto* name : reverse ? std::array {"_mlir_foo", "foo"} : std::array {"foo", "_mlir_foo"}) {
				builder.registerFunction<val<int32_t>()>(name, [&] {
					++wrappers;
					return cacheLiteral<int32_t {7}>();
				});
			}
			REQUIRE_THROWS_WITH(builder.compile(),
			                    Catch::Matchers::ContainsSubstring("packed wrapper symbol conflicts"));
			REQUIRE(wrappers > 0);
			REQUIRE(readArtifacts(directory.path()).empty());
			REQUIRE_FALSE(tracing::inTracer());
		}
	}
}

TEST_CASE("Typed owned cache modules preserve cleanup with every optional pass setting", "[cache][allocation-origin]") {
	requireCachePlatform();
	for (const auto& [mode, overrides] : validationModes()) {
		DYNAMIC_SECTION(mode) {
			TemporaryCacheDirectory directory;
			auto options = cacheOptions(directory.path(), "typed-owned-pass-settings");
			options.applyOverrides(overrides);
			RuntimeWrappers coldWrappers {}, warmWrappers {}, repairWrappers {}, repairedWrappers {};
			auto cold = compileRuntimeModule(options, coldWrappers);
			requireWritten(cold);
			REQUIRE(cacheStat<int64_t>(cold, "cache.scalarCertificate") == 1);
			for (const auto calls : coldWrappers) {
				REQUIRE(calls > 0);
			}
			const auto traced = coldWrappers;
			checkRuntimeModule(cold);
			REQUIRE(coldWrappers == traced);
			const auto bytecode = artifactPath(cold, directory.path(), ".mlirbc");
			const auto originalBytecode = readFile(bytecode);
			std::filesystem::remove(bytecode);
			auto warm = compileRuntimeModule(options, warmWrappers);
			requireNativeHit(warm);
			checkRuntimeModule(warm);
			REQUIRE((warmWrappers == RuntimeWrappers {}));
			writeFile(bytecode, originalBytecode);
			std::filesystem::permissions(bytecode,
			                             std::filesystem::perms::owner_read | std::filesystem::perms::owner_write);
			writeFile(artifactPath(cold, directory.path(), ".o"), "force owned bytecode repair");
			auto repaired = compileRuntimeModule(options, repairWrappers);
			REQUIRE(cacheStat<std::string>(repaired, "cache.object") == "invalid_rewritten");
			REQUIRE(cacheStat<std::string>(repaired, "cache.mlir") == "hit");
			REQUIRE(cacheStat<int64_t>(repaired, "cache.tracingRan") == 0);
			requireNoFrontendStatistics(repaired);
			checkRuntimeModule(repaired);
			REQUIRE((repairWrappers == RuntimeWrappers {}));
			auto repairedWarm = compileRuntimeModule(options, repairedWrappers);
			requireNativeHit(repairedWarm);
			checkRuntimeModule(repairedWarm);
			REQUIRE((repairedWrappers == RuntimeWrappers {}));
			REQUIRE(coldWrappers == traced);
		}
	}
}

TEST_CASE("Allocation metadata independently gates strict and legacy cache success before optional passes",
          "[cache][allocation-origin][guard][replay]") {
	requireCachePlatform();
	for (const auto& [mode, overrides] : validationModes()) {
		for (const bool legacy : {false, true}) {
			for (const std::string_view kind : {"typed dead", "raw dead", "raw to typed", "typed to raw"}) {
				DYNAMIC_SECTION(mode << ": " << kind << ": legacy=" << legacy) {
					TemporaryCacheDirectory directory;
					auto options = cacheOptions(directory.path(), "allocation-table-policy");
					options.applyOverrides(overrides);
					NautilusEngine engine(cache::createCompiler(options), options);
					uint8_t storage = 7;
					for (int attempt = 0; attempt < 2; ++attempt) {
						int wrappers = 0;
						auto builder = engine.createModule();
						builder.registerFunction<val<uint8_t*>(val<uint8_t*>, val<bool>)>(
						    "execute", [&, kind, legacy](val<uint8_t*> base, val<bool> flag) {
							    ++wrappers;
							    nautilus::details::nautilus_alloca<int64_t>();
							    const bool typed =
							        kind == "typed dead" ||
							        (kind == "typed to raw" ? wrappers == 1 : kind == "raw to typed" && wrappers != 1);
							    const auto origin = TypedAllocation::forType<int64_t>();
							    auto& ref = typed ? tracing::traceTypedAlloca(origin)
							                      : tracing::traceAlloca(origin.getSize(), origin.getAlignment());
							    val<void*> unused(ref);
							    if (legacy) {
								    val<int32_t> unrelated(37);
								    (void) unrelated;
							    }
							    if (flag) {
								    return base;
							    }
							    return base;
						    });
						auto compiled = builder.compile();
						const auto execute = compiled.getFunction<uint8_t*(uint8_t*, bool)>("execute");
						REQUIRE(execute(&storage, false) == &storage);
						REQUIRE(execute(&storage, true) == &storage);
						if (kind == "typed dead") {
							if (attempt == 0) {
								requireWritten(compiled);
								REQUIRE(cacheStat<int64_t>(compiled, "cache.scalarCertificate") == !legacy);
								REQUIRE(wrappers > 0);
							} else {
								requireNativeHit(compiled);
								REQUIRE(wrappers == 0);
							}
						} else {
							requireFallback(compiled);
							REQUIRE(wrappers >= 2);
							REQUIRE(cacheStat<int64_t>(compiled, "cache.scalarCertificate") == !legacy);
							REQUIRE(cacheStat<std::string>(compiled, "cache.fallback") ==
							        "unsupported_allocation_metadata");
							REQUIRE(cacheStat<std::string>(compiled, "cache.rejection") ==
							        "allocation_metadata_origins_unavailable function=execute index=1");
							REQUIRE(readArtifacts(directory.path()).empty());
						}
					}
				}
			}
		}
	}
}

TEST_CASE("Typed owned allocations do not certify constructor scalars or encoded cleanup addresses",
          "[cache][allocation-origin][guard]") {
	requireCachePlatform();
	for (const auto& [mode, overrides] : validationModes()) {
		for (const std::string_view kind : {"raw constructor", "encoded constructor", "destructor only"}) {
			DYNAMIC_SECTION(mode << ": " << kind) {
				TemporaryCacheDirectory directory;
				auto options = cacheOptions(directory.path(), "typed-owned-uncertified-input");
				options.applyOverrides(overrides);
				NautilusEngine engine(cache::createCompiler(options), options);
				int32_t external[4] = {};
				for (int attempt = 0; attempt < 2; ++attempt) {
					int wrappers = 0;
					auto builder = engine.createModule();
					builder.registerFunction<val<int32_t>(val<int32_t*>, val<bool>)>(
					    "execute", [&, kind](val<int32_t*> runtime, val<bool> fail) {
						    ++wrappers;
						    val<CacheOwnedBuffer> empty;
						    if (kind == "raw constructor") {
							    val<CacheOwnedBuffer> object(runtime, int32_t {1});
							    return invoke(useCacheOwnedBuffer, &object, fail);
						    }
						    val<int32_t*> encoded = val<uintptr_t>(reinterpret_cast<uintptr_t>(external));
						    if (kind == "encoded constructor") {
							    val<CacheOwnedBuffer> object(encoded, cacheLiteral<int32_t {1}>());
							    return invoke(useCacheOwnedBuffer, &object, fail);
						    }
						    NativeGuard guard(encoded, rawCleanup);
						    return invoke(cacheMaybeThrow, runtime, fail);
					    });
					auto compiled = builder.compile();
					REQUIRE(wrappers > 0);
					requireUnsafeFallback(compiled, directory.path());
					int32_t runtime[4] = {};
					REQUIRE(compiled.getFunction<int32_t(int32_t*, bool)>("execute")(runtime, false) == 1);
				}
			}
		}
	}
}

} // namespace nautilus::engine
