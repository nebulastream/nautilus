#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include "nautilus/Artifact.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/RuntimeBinding.hpp"
#include "nautilus/cache/plugin.hpp"
#include "nautilus/common/ExecutableImage.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/compiler/artifact/ArtifactCodec.hpp"
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
#include <list>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
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
		std::filesystem::permissions(path_, std::filesystem::perms::owner_all);
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

Options cacheOptions(const std::filesystem::path& directory, const std::string& key) {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.cache.directory", directory.string());
	options.setOption("engine.cache.key", key);
	options.setOption("mlir.enableMultithreading", false);
	return options;
}

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

void requireNativeWarmStatistics(const CompiledModule& module) {
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

struct GuardedCacheState {
	int64_t total = 0;
	int64_t live = 0;
	int64_t calls = 0;
	int64_t cleanupOrder = 0;

	int64_t readTotal() {
		return total;
	}
};

void startGuardedBindingResource(GuardedCacheState* state) noexcept {
	++state->live;
}

template <int32_t Marker>
void finishGuardedBindingResource(GuardedCacheState* state) noexcept {
	--state->live;
	state->cleanupOrder = state->cleanupOrder * 10 + Marker;
}

template <int32_t Marker>
class GuardedBindingResource {
public:
	explicit GuardedBindingResource(val<GuardedCacheState*> state) : state_(std::move(state)) {
		invoke(startGuardedBindingResource, state_);
		if (tracing::inTracer()) {
			tracing::registerDestructor(state_.getState(),
			                            reinterpret_cast<void*>(finishGuardedBindingResource<Marker>));
		}
	}

	~GuardedBindingResource() noexcept {
		if (tracing::inTracer()) {
			tracing::unregisterDestructor(state_.getState());
		}
		invoke(finishGuardedBindingResource<Marker>, state_);
	}

	GuardedBindingResource(const GuardedBindingResource&) = delete;
	GuardedBindingResource& operator=(const GuardedBindingResource&) = delete;

private:
	val<GuardedCacheState*> state_;
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

} // namespace

TEST_CASE("MLIR guarded cache preserves cleanup and isolates rebound state on object and MLIR hits", "[cache][guard]") {
#if !defined(__linux__) || !defined(__x86_64__)
	SKIP("Persistent native caching requires the supported Linux x86-64 ELF producer");
#endif
	TemporaryCacheDirectory cache;
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.cache.directory", cache.path().string());
	options.setOption("engine.cache.key", std::string("guarded-rebinding-v1"));
	options.setOption("mlir.enableMultithreading", false);
	std::array<int, 4> wrappers {};
	auto compile = [&](GuardedCacheState& state, int& traces) {
		RuntimeBindings bindings;
		auto binding = bindings.bind<GuardedCacheState>("guarded/state", &state);
		auto engine = NautilusEngine(cache::createCompiler(options), options);
		auto module = engine.createModule();
		module.setRuntimeBindings(bindings);
		module.registerFunction<val<int64_t>(val<int64_t>)>("execute", [binding, &traces](val<int64_t> value) {
			++traces;
			auto address = binding.get();
			GuardedBindingResource<1> outer(address);
			GuardedBindingResource<2> inner(address);
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
		auto cold = compile(first, wrappers[0]);
		INFO(cold.getStatistics()->toString());
		REQUIRE(cacheStat(cold, "cache.object") == "written");
		REQUIRE(cacheStat(cold, "cache.mlir") == "written");
		REQUIRE(cacheStat(cold, "cache.fallback") == "none");
		REQUIRE(cacheIntStat(cold, "cache.tracingRan") == 1);
		const auto coldTraces = wrappers[0];
		REQUIRE(coldTraces > 0);
		check(cold, first);

		auto warm = compile(second, wrappers[1]);
		REQUIRE(cacheStat(warm, "cache.object") == "hit");
		REQUIRE(cacheStat(warm, "cache.fallback") == "none");
		REQUIRE(cacheIntStat(warm, "cache.tracingRan") == 0);
		requireNativeWarmStatistics(warm);
		check(warm, second);

		const auto object = findArtifact(cache.path(), ".o");
		REQUIRE_FALSE(object.empty());
		{
			std::ofstream corrupt(object, std::ios::binary | std::ios::trunc);
			corrupt << "corrupt guarded object";
		}
		auto repaired = compile(third, wrappers[2]);
		REQUIRE(cacheStat(repaired, "cache.object") == "invalid_rewritten");
		REQUIRE(cacheStat(repaired, "cache.mlir") == "hit");
		REQUIRE(cacheIntStat(repaired, "cache.tracingRan") == 0);
		requireNoFrontendStatistics(repaired);
		check(repaired, third);
		auto warmRepaired = compile(fourth, wrappers[3]);
		REQUIRE(cacheStat(warmRepaired, "cache.object") == "hit");
		REQUIRE(cacheIntStat(warmRepaired, "cache.tracingRan") == 0);
		requireNativeWarmStatistics(warmRepaired);
		check(warmRepaired, fourth);
		REQUIRE(wrappers[0] == coldTraces);
		REQUIRE(wrappers[1] == 0);
		REQUIRE(wrappers[2] == 0);
		REQUIRE(wrappers[3] == 0);
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
	artifact::detail::Reader envelope(bytes);
	REQUIRE(envelope.string(128).starts_with("NAUTILUS-MODULE-CACHE-"));
	const auto payload = envelope.string(artifact::detail::MAX_DESCRIPTOR_SIZE * 4);
	REQUIRE(envelope.string(64) == artifact::detail::digest(payload));
	envelope.finish();
	artifact::detail::Reader reader(payload);
	REQUIRE_FALSE(reader.string().empty());
	const auto descriptorBytes = reader.string();
	REQUIRE(reader.string(64) == artifact::detail::digest(descriptorBytes));
	reader.finish();
	const auto descriptor = artifact::detail::decodeDescriptor(descriptorBytes);
	REQUIRE(descriptor.version == 2);
	REQUIRE(descriptor.bindingSchema.size() == 1);
	REQUIRE(descriptor.moduleManifest.find("nautilus.runtime_binding.schema") != std::string::npos);
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
		options.setOption("engine.cache.directory", std::string(directory));
		options.setOption("engine.cache.key", std::string("fresh-exec-guarded-personality-v1"));
		options.setOption("mlir.enableMultithreading", false);
		options.setRuntimeBindings(bindings);
		int traces = 0;
		auto engine = NautilusEngine(cache::createCompiler(options), options);
		auto module = engine.createModule();
		module.registerFunction<val<int64_t>(val<int64_t>)>("execute", [binding, &traces](val<int64_t> value) {
			++traces;
			auto address = binding.get();
			GuardedBindingResource<1> outer(address);
			GuardedBindingResource<2> inner(address);
			return invoke(guardedCacheProxy, address, value);
		});
		auto compiled = module.compile();
		REQUIRE(cacheStat(compiled, "cache.object") == (warm ? "hit" : "written"));
		REQUIRE(cacheStat(compiled, "cache.fallback") == "none");
		REQUIRE(cacheIntStat(compiled, "cache.tracingRan") == (warm ? 0 : 1));
		REQUIRE((warm ? traces == 0 : traces > 0));
		if (warm) {
			requireNativeWarmStatistics(compiled);
		}
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
#endif

TEST_CASE("MLIR guarded cache rejects heap member-function wrapper state", "[cache][guard]") {
#if !defined(__linux__) || !defined(__x86_64__)
	SKIP("Persistent native caching requires the supported Linux x86-64 ELF producer");
#endif
	TemporaryCacheDirectory cache;
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.cache.directory", cache.path().string());
	options.setOption("engine.cache.key", std::string("member-wrapper-state-v1"));
	options.setOption("mlir.enableMultithreading", false);
	GuardedCacheState state {42};
	RuntimeBindings bindings;
	auto binding = bindings.bind<GuardedCacheState>("state", &state);
	for (int iteration = 0; iteration < 2; ++iteration) {
		auto engine = NautilusEngine(cache::createCompiler(options), options);
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

TEST_CASE("RuntimeBindings retains the raw native-resource allocation metadata guard",
          "[runtime-bindings][cache][guard]") {
	TemporaryCacheDirectory cache;
	const auto options = cacheOptions(cache.path(), "bound-native-resource-allocation-v1");
#if !defined(__linux__) || !defined(__x86_64__)
	SKIP("Persistent native caching requires the supported Linux x86-64 ELF producer");
#endif
	std::array<GuardedCacheState, 2> states {{{10}, {100}}};
	std::array<int, 2> wrappers {};
	for (std::size_t index = 0; index < states.size(); ++index) {
		CAPTURE(index);
		RuntimeBindings bindings;
		auto binding = bindings.bind<GuardedCacheState>("guarded/state", &states[index]);
		NautilusEngine engine(cache::createCompiler(options), options);
		auto builder = engine.createModule();
		builder.setRuntimeBindings(bindings);
		builder.registerFunction<val<int64_t>(val<int64_t>)>("execute",
		                                                     [binding, &wrappers, index](val<int64_t> value) {
			                                                     ++wrappers[index];
			                                                     auto address = binding.get();
			                                                     val<GuardedCacheResource<1>> outer(address);
			                                                     val<GuardedCacheResource<2>> inner(address);
			                                                     return invoke(guardedCacheProxy, address, value);
		                                                     });
		auto compiled = builder.compile();
		REQUIRE(states[index].total == (index == 0 ? 10 : 100));
		REQUIRE(states[index].live == 0);
		REQUIRE(states[index].calls == 0);
		REQUIRE(states[index].cleanupOrder == 0);
		REQUIRE(wrappers[index] > 0);
		REQUIRE(cacheIntStat(compiled, "cache.tracingRan") == 1);
		REQUIRE(cacheStat(compiled, "cache.fallback") == "unsupported_allocation_metadata");
		REQUIRE(cacheStat(compiled, "cache.rejection") == "allocation_metadata_origins_unavailable");
		for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
			REQUIRE(findArtifact(cache.path(), extension).empty());
		}
		const auto before = states[index].total;
		auto execute = compiled.getFunction<int64_t(int64_t)>("execute");
		REQUIRE(execute(7) == before + 7);
		REQUIRE(states[index].live == 0);
		REQUIRE(states[index].cleanupOrder == 21);
		REQUIRE_THROWS_WITH(execute(-1), "guarded cache proxy");
		REQUIRE(states[index].live == 0);
		REQUIRE(states[index].cleanupOrder == 2121);
		REQUIRE(states[index].total == before + 7);
	}
}

} // namespace nautilus::engine
