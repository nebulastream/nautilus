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
#include <barrier>
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
#include <thread>
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
struct alignas(64) GuardedCacheResource {
	GuardedCacheState* state;
	int64_t marker;

	explicit GuardedCacheResource(GuardedCacheState* state) noexcept : GuardedCacheResource(state, Marker) {
	}

	GuardedCacheResource(GuardedCacheState* state, int64_t marker) noexcept : state(state), marker(marker) {
		++state->live;
	}

	GuardedCacheResource(const GuardedCacheResource& other) noexcept : GuardedCacheResource(other.state, other.marker) {
	}

	~GuardedCacheResource() noexcept {
		--state->live;
		state->cleanupOrder = state->cleanupOrder * 10 + marker;
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

int64_t guardedOwnedAliasProxy(GuardedCacheResource<1>* resource, GuardedCacheState* observed, int64_t value) {
	if (reinterpret_cast<uintptr_t>(resource) % alignof(GuardedCacheResource<1>) != 0) {
		throw std::runtime_error("misaligned bound owned resource");
	}
	guardedCacheProxy(resource->state, value);
	return observed->total;
}

CompiledModule compileOwnedBinding(const NautilusEngine& engine, GuardedCacheState* state, GuardedCacheState* observed,
                                   std::atomic<int>& traces) {
	RuntimeBindings bindings;
	auto owner = bindings.bind<GuardedCacheState>("guarded/state", state);
	auto observer = bindings.bind<GuardedCacheState>("guarded/observer", observed);
	auto module = engine.createModule();
	module.setRuntimeBindings(bindings);
	module.registerFunction<val<int64_t>(val<int64_t>)>("execute", [owner, observer, &traces](val<int64_t> value) {
		++traces;
		val<GuardedCacheResource<1>> outer(owner.get());
		val<GuardedCacheResource<1>> copied(outer);
		val<GuardedCacheResource<1>> moved(std::move(copied));
		val<GuardedCacheResource<2>> inner(observer.get());
		return invoke(guardedOwnedAliasProxy, &moved, observer.get(), value);
	});
	return module.compile();
}

void checkOwnedBinding(CompiledModule& module, GuardedCacheState& state, GuardedCacheState& observed) {
	auto execute = module.getFunction<int64_t(int64_t)>("execute");
	const bool aliases = &state == &observed;
	const auto before = state.total;
	const auto observedBefore = observed.total;
	for (int attempt = 0; attempt < 3; ++attempt) {
		state.cleanupOrder = observed.cleanupOrder = 0;
		if (attempt == 1) {
			REQUIRE_THROWS_WITH(execute(-1), "guarded cache proxy");
		} else {
			REQUIRE(execute(attempt == 0 ? 7 : 3) == (aliases ? before + (attempt == 0 ? 7 : 10) : observedBefore));
		}
		REQUIRE(state.total == before + (attempt == 2 ? 10 : 7));
		REQUIRE(state.live == 0);
		REQUIRE(observed.live == 0);
		REQUIRE(state.calls == attempt + 1);
		REQUIRE(state.cleanupOrder == (aliases ? 211 : 11));
		if (!aliases) {
			REQUIRE(observed.total == observedBefore);
			REQUIRE(observed.calls == 0);
			REQUIRE(observed.cleanupOrder == 2);
		}
		REQUIRE_FALSE(tracing::inTracer());
	}
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
#if !defined(__x86_64__)
	SKIP("Persistent native caching requires the supported Linux x86-64 ELF producer");
#endif
#if !__has_include(<unwind.h>) || defined(__arm__) || defined(__USING_SJLJ_EXCEPTIONS__)
	SKIP("The native personality bridge requires Linux DWARF unwinding headers");
#endif
	static constexpr auto CHILD_DIRECTORY = "NAUTILUS_GUARDED_CHILD_DIRECTORY";
	static constexpr auto CHILD_MODE = "NAUTILUS_GUARDED_CHILD_MODE";
	static constexpr auto CHILD_TYPED = "NAUTILUS_GUARDED_CHILD_TYPED";
	if (const auto* directory = std::getenv(CHILD_DIRECTORY)) {
		const auto personality = ::personality(0xffffffffUL);
		REQUIRE(personality != -1);
		REQUIRE((personality & ADDR_NO_RANDOMIZE) == 0);
		const auto* mode = std::getenv(CHILD_MODE);
		REQUIRE(mode != nullptr);
		const bool warm = std::string_view(mode) == "warm";
		REQUIRE((warm || std::string_view(mode) == "cold"));
		const auto* typedMode = std::getenv(CHILD_TYPED);
		REQUIRE(typedMode != nullptr);
		const bool typed = std::string_view(typedMode) == "1";
		auto storage = std::make_unique<std::array<GuardedCacheState, 2>>();
		auto* state = &(*storage)[warm ? 1 : 0];
		state->total = warm ? 100 : 10;
		RuntimeBindings bindings;
		auto binding = bindings.bind<GuardedCacheState>("guarded/state", state);
		Options options;
		options.setOption("engine.backend", std::string("mlir"));
		options.setOption("engine.cache.directory", std::string(directory));
		options.setOption("engine.cache.key", std::string("fresh-exec-guarded-personality-v2/typed=") + typedMode);
		options.setOption("mlir.enableMultithreading", false);
		options.setRuntimeBindings(bindings);
		int traces = 0;
		auto engine = NautilusEngine(cache::createCompiler(options), options);
		auto module = engine.createModule();
		module.registerFunction<val<int64_t>(val<int64_t>)>("execute", [binding, typed, &traces](val<int64_t> value) {
			++traces;
			auto address = binding.get();
			if (typed) {
				val<GuardedCacheResource<1>> outer(address);
				val<GuardedCacheResource<2>> inner(address);
				return invoke(guardedCacheProxy, address, value);
			}
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

	for (const bool typed : {false, true}) {
		CAPTURE(typed);
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
				if (::setenv(CHILD_DIRECTORY, cache.path().c_str(), 1) != 0 || ::setenv(CHILD_MODE, mode, 1) != 0 ||
				    ::setenv(CHILD_TYPED, typed ? "1" : "0", 1) != 0) {
					::_exit(125);
				}
				::execl(executable.c_str(), executable.c_str(),
				        "MLIR guarded cache forwards personality across ASLR fresh exec processes", "--reporter",
				        "compact", static_cast<char*>(nullptr));
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

TEST_CASE("RuntimeBindings caches typed owned resources with rebound aliases and exception recovery",
          "[runtime-bindings][cache][guard][allocation-origin]") {
#if !defined(__linux__) || !defined(__x86_64__)
	SKIP("Persistent native caching requires the supported Linux x86-64 ELF producer");
#endif
	for (const auto* mode : {"passes disabled", "optimization disabled", "one iteration", "fixed point"}) {
		CAPTURE(mode);
		TemporaryCacheDirectory cache;
		auto options = cacheOptions(cache.path(), "bound-native-resource-allocation-v2");
		options.setOption("ir.runPasses", std::string_view(mode) != "passes disabled");
		options.setOption("ir.runOptimizationPasses", std::string_view(mode) != "optimization disabled");
		options.setOption("ir.maxPipelineIterations", std::string_view(mode) == "fixed point" ? 4 : 1);
		std::array<std::array<GuardedCacheState, 2>, 4> states {};
		std::array<std::atomic<int>, 4> wrappers {};
		std::array<std::optional<CompiledModule>, 4> modules;
		for (std::size_t index = 0; index < modules.size(); ++index) {
			states[index][0].total = 10 + index * 100;
			states[index][1].total = 31 + index * 100;
			NautilusEngine engine(cache::createCompiler(options), options);
			modules[index].emplace(
			    compileOwnedBinding(engine, &states[index][0], &states[index][index % 2 == 0], wrappers[index]));
			CAPTURE(index);
			const auto& compiled = *modules[index];
			REQUIRE(cacheStat(compiled, "cache.object") == (index == 0   ? "written"
			                                                : index == 2 ? "invalid_rewritten"
			                                                             : "hit"));
			REQUIRE(cacheStat(compiled, "cache.mlir") == (index == 0 ? "written" : index == 2 ? "hit" : "not_checked"));
			REQUIRE(cacheStat(compiled, "cache.fallback") == (index == 2 ? "invalid_object" : "none"));
			REQUIRE(cacheIntStat(compiled, "cache.tracingRan") == (index == 0 ? 1 : 0));
			REQUIRE((index == 0 ? wrappers[index].load() > 0 : wrappers[index].load() == 0));
			if (index == 2) {
				requireNoFrontendStatistics(compiled);
			} else if (index != 0) {
				requireNativeWarmStatistics(compiled);
			}
			REQUIRE(states[index][0].live == 0);
			REQUIRE(states[index][0].calls == 0);
			if (index == 1) {
				std::ofstream corrupt(findArtifact(cache.path(), ".o"), std::ios::binary | std::ios::trunc);
				corrupt << "corrupt bound owned object";
				corrupt.close();
				REQUIRE(corrupt.good());
			}
		}
		const auto coldWrappers = wrappers[0].load();
		for (std::size_t index = 0; index < modules.size(); ++index) {
			checkOwnedBinding(*modules[index], states[index][0], states[index][index % 2 == 0]);
			REQUIRE(wrappers[index].load() == (index == 0 ? coldWrappers : 0));
			for (std::size_t other = index + 1; other < modules.size(); ++other) {
				REQUIRE(states[other][0].calls == 0);
				REQUIRE(states[other][0].live == 0);
			}
		}
	}
}

TEST_CASE("RuntimeBindings concurrently loads independent typed owned alias environments",
          "[runtime-bindings][cache][guard][allocation-origin][concurrent]") {
#if !defined(__linux__) || !defined(__x86_64__)
	SKIP("Persistent native caching requires the supported Linux x86-64 ELF producer");
#endif
	for (const bool passes : {false, true}) {
		TemporaryCacheDirectory cache;
		auto options = cacheOptions(cache.path(), "concurrent-bound-owned-v1");
		options.setOption("ir.runPasses", passes);
		std::array<std::array<GuardedCacheState, 2>, 5> states {};
		std::array<std::atomic<int>, 5> wrappers {};
		std::array<std::optional<CompiledModule>, 5> modules;
		std::array<std::exception_ptr, 4> errors;
		for (std::size_t index = 0; index < states.size(); ++index) {
			states[index][0].total = 10 + index * 100;
			states[index][1].total = 31 + index * 100;
		}
		{
			NautilusEngine engine(cache::createCompiler(options), options);
			modules[0].emplace(compileOwnedBinding(engine, &states[0][0], &states[0][1], wrappers[0]));
			REQUIRE(cacheStat(*modules[0], "cache.object") == "written");
			std::barrier synchronize(4);
			std::array<std::thread, 4> workers;
			for (std::size_t index = 0; index < workers.size(); ++index) {
				workers[index] = std::thread([&, index] {
					synchronize.arrive_and_wait();
					try {
						const auto next = index + 1;
						modules[next].emplace(compileOwnedBinding(engine, &states[next][0],
						                                          &states[next][next % 2 == 0], wrappers[next]));
					} catch (...) {
						errors[index] = std::current_exception();
					}
				});
			}
			for (auto& worker : workers) {
				worker.join();
			}
		}
		for (const auto& error : errors) {
			if (error) {
				std::rethrow_exception(error);
			}
		}
		const auto coldWrappers = wrappers[0].load();
		REQUIRE(coldWrappers > 0);
		for (std::size_t index = 0; index < modules.size(); ++index) {
			CAPTURE(passes, index);
			REQUIRE(modules[index].has_value());
			if (index != 0) {
				REQUIRE(cacheStat(*modules[index], "cache.object") == "hit");
				requireNativeWarmStatistics(*modules[index]);
				REQUIRE(wrappers[index].load() == 0);
			}
			checkOwnedBinding(*modules[index], states[index][0], states[index][index % 2 == 0]);
			REQUIRE(wrappers[index].load() == (index == 0 ? coldWrappers : 0));
		}
	}
}

TEST_CASE("RuntimeBindings cannot upgrade raw dead unused or replayed allocation metadata after DCE",
          "[runtime-bindings][cache][guard][allocation-origin][replay]") {
#if !defined(__linux__) || !defined(__x86_64__)
	SKIP("Persistent native caching requires the supported Linux x86-64 ELF producer");
#endif
	for (const bool passes : {false, true}) {
		for (const bool legacy : {false, true}) {
			for (const std::string_view kind : {"raw dead", "raw live", "raw to typed", "typed to raw"}) {
				CAPTURE(passes, legacy, kind);
				TemporaryCacheDirectory cache;
				auto options = cacheOptions(cache.path(), "bound-allocation-metadata-guard-v1");
				options.setOption("ir.runPasses", passes);
				NautilusEngine engine(cache::createCompiler(options), options);
				GuardedCacheState state {10};
				RuntimeBindings bindings;
				auto binding = bindings.bind<GuardedCacheState>("guarded/state", &state);
				for (int attempt = 0; attempt < 2; ++attempt) {
					int wrappers = 0;
					auto builder = engine.createModule();
					builder.setRuntimeBindings(bindings);
					builder.registerFunction<val<GuardedCacheState*>(val<bool>)>(
					    "execute", [&, kind, legacy](val<bool> flag) {
						    ++wrappers;
						    nautilus::details::nautilus_alloca<int64_t>();
						    const auto origin = TypedAllocation::forType<int64_t>();
						    const bool typed =
						        kind == "typed to raw" ? wrappers == 1 : kind == "raw to typed" && wrappers != 1;
						    auto& ref = typed ? tracing::traceTypedAlloca(origin)
						                      : tracing::traceAlloca(origin.getSize(), origin.getAlignment());
						    if (kind == "raw live") {
							    val<int64_t*> slot(ref);
							    *slot = cacheLiteral<int64_t {7}>();
						    }
						    if (legacy) {
							    val<int32_t> unrelated(37);
							    (void) unrelated;
						    }
						    if (flag) {
							    return binding.get();
						    }
						    return binding.get();
					    });
					auto compiled = builder.compile();
					REQUIRE(wrappers >= 2);
					REQUIRE(cacheIntStat(compiled, "cache.scalarCertificate") == !legacy);
					REQUIRE(cacheStat(compiled, "cache.fallback") == "unsupported_allocation_metadata");
					REQUIRE(cacheStat(compiled, "cache.rejection") ==
					        "allocation_metadata_origins_unavailable function=execute index=1");
					for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
						REQUIRE(findArtifact(cache.path(), extension).empty());
					}
					auto execute = compiled.getFunction<GuardedCacheState*(bool)>("execute");
					REQUIRE(execute(false) == &state);
					REQUIRE(execute(true) == &state);
				}
			}
		}
	}
}

} // namespace nautilus::engine
