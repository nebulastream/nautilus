#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/RuntimeBinding.hpp"
#include "nautilus/cache/plugin.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/region.hpp"
#include "nautilus/select.hpp"
#include "nautilus/static.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_std.hpp"
#ifdef ENABLE_TRACING
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/SSAVerifier.hpp"
#endif
#include <array>
#include <atomic>
#include <barrier>
#include <bit>
#include <cerrno>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <llvm/IR/Module.h>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
#ifdef __linux__
#include <sys/personality.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace nautilus::engine {
namespace {

Options bindingOptions(const std::string& backend) {
	Options options;
	if (backend == "interpreter") {
		options.setOption("engine.Compilation", false);
	} else {
		options.setOption("engine.backend", backend);
	}
	options.setOption("mlir.enableMultithreading", false);
	return options;
}

struct CertifiedBindingState {
	int64_t total = 0;
	int64_t calls = 0;
	int64_t cleanups = 0;
	int64_t live = 0;
	uint8_t byte = 0;
	bool bit = false;

	bool operator==(const CertifiedBindingState&) const = default;
};

void startCertifiedBindingCleanup(CertifiedBindingState* state) noexcept {
	++state->live;
}

void finishCertifiedBindingCleanup(CertifiedBindingState* state) noexcept {
	--state->live;
	++state->cleanups;
}

class CertifiedBindingCleanup {
public:
	explicit CertifiedBindingCleanup(val<CertifiedBindingState*> state) : state_(std::move(state)) {
		invoke(startCertifiedBindingCleanup, state_);
		if (tracing::inTracer()) {
			tracing::registerDestructor(state_.getState(), reinterpret_cast<void*>(finishCertifiedBindingCleanup));
		}
	}

	~CertifiedBindingCleanup() noexcept {
		if (tracing::inTracer()) {
			tracing::unregisterDestructor(state_.getState());
		}
		invoke(finishCertifiedBindingCleanup, state_);
	}

	CertifiedBindingCleanup(const CertifiedBindingCleanup&) = delete;
	CertifiedBindingCleanup& operator=(const CertifiedBindingCleanup&) = delete;

private:
	val<CertifiedBindingState*> state_;
};

bool certifiedBindingReady(CertifiedBindingState* state, int64_t threshold) noexcept {
	++state->calls;
	return state->total >= threshold;
}

int64_t certifiedBindingCount(CertifiedBindingState* state, int64_t delta) noexcept {
	++state->calls;
	state->total += delta;
	return state->total;
}

int64_t certifiedBindingThrow(CertifiedBindingState* state, int64_t value) {
	++state->calls;
	if (state->live != 1) {
		throw std::runtime_error("missing live certified cleanup");
	}
	if (value < 0) {
		throw std::runtime_error("certified scalar cleanup");
	}
	return state->total + value + state->byte + state->bit;
}

struct BindingAliasState {
	int64_t first;
	int64_t second;
};

static_assert(std::is_standard_layout_v<BindingAliasState>);

struct BindingSchemaFirst {
	int64_t fields[2];
};

struct BindingSchemaSecond {
	int64_t fields[2];
};

struct alignas(16) BindingSchemaAligned {
	int64_t fields[2];
};

static_assert(sizeof(BindingSchemaFirst) == sizeof(BindingSchemaSecond));
static_assert(alignof(BindingSchemaFirst) == alignof(BindingSchemaSecond));
static_assert(sizeof(BindingSchemaFirst) == sizeof(BindingSchemaAligned));
static_assert(alignof(BindingSchemaFirst) != alignof(BindingSchemaAligned));

class BindingCacheDirectory {
public:
	BindingCacheDirectory() {
		static std::atomic<uint64_t> sequence {0};
		const auto* output = std::getenv("NAUTILUS_BINDING_TEST_OUTPUT_DIR");
		preserve_ = output != nullptr && *output != '\0';
		const auto root = preserve_ ? std::filesystem::path(output) : std::filesystem::temp_directory_path();
		path_ =
		    root / ("nautilus-bindings-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
		            "-" + std::to_string(sequence.fetch_add(1)));
		std::filesystem::create_directories(path_);
		std::filesystem::permissions(path_, std::filesystem::perms::owner_all);
		if (preserve_) {
			std::cout << "RuntimeBindings test artifacts: " << path_ << '\n';
		}
	}

	~BindingCacheDirectory() {
		if (!preserve_) {
			std::error_code error;
			std::filesystem::remove_all(path_, error);
		}
	}

	const std::filesystem::path& path() const {
		return path_;
	}

private:
	std::filesystem::path path_;
	bool preserve_ = false;
};

Options bindingCacheOptions(const std::filesystem::path& directory, const std::string& key) {
#if !defined(__linux__) || !defined(__x86_64__)
	SKIP("Persistent native caching requires the supported Linux x86-64 ELF producer");
#endif
	auto options = bindingOptions("mlir");
	options.setOption("engine.cache.directory", directory.string());
	options.setOption("engine.cache.key", key);
	return options;
}

template <typename T>
T bindingStat(const CompiledModule& module, const std::string& key) {
	const auto stats = module.getStatistics();
	REQUIRE(stats != nullptr);
	INFO(stats->toString());
	if ((key == "cache.object" || key == "cache.mlir") && stats->contains(key) &&
	    std::get<std::string>(*stats->find(key)) == "hit") {
		REQUIRE(std::get<int64_t>(*stats->find("cache.tracingRan")) == 0);
		for (const auto* name : {"tracing.ms", "ssaCreation.ms", "irGeneration.ms", "frontend.totalMs"}) {
			REQUIRE_FALSE(stats->contains(name));
		}
		for (const auto& [name, entry] : *stats) {
			CAPTURE(name);
			REQUIRE_FALSE(name.starts_with("ir."));
			REQUIRE_FALSE(name.starts_with("irPasses."));
			if (key == "cache.object") {
				REQUIRE_FALSE(name.starts_with("mlir."));
				REQUIRE_FALSE(name.starts_with("llvm."));
			}
		}
		if (key == "cache.object") {
			REQUIRE(std::get<std::string>(*stats->find("cache.mlir")) == "not_checked");
			REQUIRE(std::get<std::string>(*stats->find("cache.fallback")) == "none");
			REQUIRE_FALSE(stats->contains("jit.compile.ms"));
			REQUIRE(stats->contains("jit.objectLoad.ms"));
		}
	}
	const auto* value = stats->find(key);
	REQUIRE(value != nullptr);
	REQUIRE(std::holds_alternative<T>(*value));
	return std::get<T>(*value);
}

std::filesystem::path bindingArtifact(const std::filesystem::path& directory, std::string_view extension) {
	for (const auto& entry : std::filesystem::directory_iterator(directory)) {
		if (entry.path().extension() == extension) {
			return entry.path();
		}
	}
	return {};
}

void corruptBindingArtifact(const std::filesystem::path& path) {
	REQUIRE(!path.empty());
	std::ofstream output(path, std::ios::binary | std::ios::trunc);
	output << "corrupt runtime binding artifact";
	output.close();
	REQUIRE(output.good());
}

void createBindingHistory() {
	int64_t unused = 0;
	for (int index = 0; index < 97; ++index) {
		RuntimeBindings discarded;
		(void) discarded.bind<int64_t>("discarded/" + std::to_string(index), &unused);
	}
}

CompiledModule compileBindingPair(const NautilusEngine& engine, int64_t* left, int64_t* right, bool reverseOrder,
                                  std::atomic<int>& traces) {
	RuntimeBindings bindings;
	RuntimeBinding<int64_t> leftBinding;
	RuntimeBinding<int64_t> rightBinding;
	if (reverseOrder) {
		rightBinding = bindings.bind<int64_t>("operator/17/right", right);
		leftBinding = bindings.bind<int64_t>("operator/17/left", left);
	} else {
		leftBinding = bindings.bind<int64_t>("operator/17/left", left);
		rightBinding = bindings.bind<int64_t>("operator/17/right", right);
	}

	auto module = engine.createModule();
	module.setRuntimeBindings(bindings);
	auto execute = [leftBinding, rightBinding, &traces](val<int64_t> delta) -> val<int64_t> {
		traces.fetch_add(1);
		auto leftAddress = leftBinding.get();
		*leftAddress += delta;
		return *rightBinding.get();
	};
	auto sameAddress = [leftBinding, rightBinding, &traces] {
		traces.fetch_add(1);
		return leftBinding.get() == rightBinding.get();
	};
	if (reverseOrder) {
		module.registerFunction<val<bool>()>("same_address", sameAddress);
		module.registerFunction<val<int64_t>(val<int64_t>)>("execute", execute);
	} else {
		module.registerFunction<val<int64_t>(val<int64_t>)>("execute", execute);
		module.registerFunction<val<bool>()>("same_address", sameAddress);
	}
	module.registerFunction<val<int64_t*>()>("left_address", [leftBinding, &traces] {
		traces.fetch_add(1);
		return leftBinding.get();
	});
	module.registerFunction<val<int64_t*>()>("right_address", [rightBinding, &traces] {
		traces.fetch_add(1);
		return rightBinding.get();
	});
	return module.compile();
}

CompiledModule compileBindingPair(const Options& options, int64_t* left, int64_t* right, bool reverseOrder,
                                  std::atomic<int>& traces) {
	NautilusEngine engine(cache::createCompiler(options), options);
	return compileBindingPair(engine, left, right, reverseOrder, traces);
}

void requireBindingAddresses(CompiledModule& module, int64_t* left, int64_t* right) {
	REQUIRE(module.getFunction<int64_t*()>("left_address")() == left);
	REQUIRE(module.getFunction<int64_t*()>("right_address")() == right);
	REQUIRE(module.getFunction<bool()>("same_address")() == (left == right));
}
} // namespace

TEST_CASE("RuntimeBindings caches certified scalars with ModRef calls stores and native cleanup",
          "[runtime-bindings][cache]") {
	BindingCacheDirectory cache;
	std::vector<std::string> keys;
	for (const auto configuration : {std::array<int64_t, 3> {2, 10, 7}, std::array<int64_t, 3> {3, 20, -4}}) {
		const auto [scale, threshold, bias] = configuration;
		CAPTURE(scale, threshold, bias);
		const auto options = bindingCacheOptions(cache.path(), "certified-modref-v1/scale=" + std::to_string(scale) +
		                                                           "/threshold=" + std::to_string(threshold) +
		                                                           "/bias=" + std::to_string(bias));
		int traces = 0;
		const auto compile = [&](CertifiedBindingState& storage) {
			RuntimeBindings bindings;
			auto state = bindings.bind<CertifiedBindingState>("state", &storage);
			auto total = bindings.bind<int64_t>("total", &storage.total);
			auto byte = bindings.bind<uint8_t>("byte", &storage.byte);
			auto bit = bindings.bind<bool>("bit", &storage.bit);
			auto engine = NautilusEngine(cache::createCompiler(options), options);
			auto module = engine.createModule();
			module.setRuntimeBindings(bindings);
			module.registerFunction<val<int64_t>(val<int64_t>)>("execute", [=, &traces](val<int64_t> delta) {
				++traces;
				CertifiedBindingCleanup cleanup(state.get());
				auto ready = invoke(certifiedBindingReady, state.get(), cacheInvariant(threshold));
				auto count = invoke(certifiedBindingCount, state.get(),
				                    delta * cacheInvariant(scale) + cacheLiteral<int64_t {2}>());
				auto result = count + select(ready, cacheLiteral<int64_t {1}>(), cacheLiteral<int64_t {0}>()) +
				              cacheInvariant(bias) +
				              static_cast<val<int64_t>>(cacheInvariant(1.5) * cacheInvariant(2.0));
				*total.get() = result;
				*byte.get() = static_cast<val<uint8_t>>(result & cacheLiteral<int64_t {255}>());
				*bit.get() = select(ready, cacheLiteral<true>(), cacheLiteral<false>());
				return invoke(certifiedBindingThrow, state.get(), delta) + result;
			});
			module.registerFunction<val<int64_t>()>("implicit_scalars", [total] {
				val<int64_t> value;
				val<bool> flag;
				val<double> floating;
				++value;
				auto previous = value--;
				--value;
				++value;
				auto pointer = total.get();
				++pointer;
				--pointer;
				auto distance = (pointer + cacheLiteral<int64_t {1}>()) - pointer;
				return select(!flag, -value + previous + distance + static_cast<val<int64_t>>(floating),
				              cacheLiteral<int64_t {0}>());
			});
			return module.compile();
		};
		const auto check = [&](CompiledModule& module, CertifiedBindingState& storage) {
			REQUIRE(module.getFunction<int64_t()>("implicit_scalars")() == 2);
			auto execute = module.getFunction<int64_t(int64_t)>("execute");
			for (const int64_t delta : {4, -2, 0}) {
				const auto before = storage;
				const bool ready = before.total >= threshold;
				const auto expected = before.total + delta * scale + 2 + ready + bias + 3;
				const auto expectedByte = static_cast<uint8_t>(expected & 255);
				if (delta < 0) {
					REQUIRE_THROWS_WITH(execute(delta), "certified scalar cleanup");
				} else {
					REQUIRE(execute(delta) == expected * 2 + delta + expectedByte + ready);
				}
				REQUIRE(storage.total == expected);
				REQUIRE(storage.byte == expectedByte);
				REQUIRE(storage.bit == ready);
				REQUIRE(storage.calls == before.calls + 3);
				REQUIRE(storage.cleanups == before.cleanups + 1);
				REQUIRE(storage.live == 0);
			}
		};
		auto first = std::make_unique<CertifiedBindingState>();
		auto second = std::make_unique<CertifiedBindingState>();
		auto third = std::make_unique<CertifiedBindingState>();
		first->total = 5;
		second->total = 101;
		third->total = 211;
		REQUIRE(first.get() != second.get());
		REQUIRE(second.get() != third.get());
		const auto before = *first;
		auto cold = compile(*first);
		REQUIRE(*first == before);
		REQUIRE(bindingStat<int64_t>(cold, "cache.scalarCertificate") == 1);
		REQUIRE(bindingStat<std::string>(cold, "cache.scalarRejection").empty());
		REQUIRE(bindingStat<std::string>(cold, "cache.fallback") == "none");
		REQUIRE(bindingStat<std::string>(cold, "cache.object") == "written");
		REQUIRE(bindingStat<std::string>(cold, "cache.mlir") == "written");
		REQUIRE(bindingStat<int64_t>(cold, "cache.tracingRan") == 1);
		keys.push_back(bindingStat<std::string>(cold, "cache.key"));
		const auto coldTraces = traces;
		REQUIRE(coldTraces > 0);
		check(cold, *first);
		const auto firstAfter = *first;
		auto warm = compile(*second);
		REQUIRE(second->total == 101);
		REQUIRE(second->calls == 0);
		REQUIRE(second->cleanups == 0);
		REQUIRE(bindingStat<std::string>(warm, "cache.object") == "hit");
		REQUIRE(bindingStat<int64_t>(warm, "cache.tracingRan") == 0);
		REQUIRE(bindingStat<std::string>(warm, "cache.key") == keys.back());
		REQUIRE(traces == coldTraces);
		check(warm, *second);
		REQUIRE(*first == firstAfter);
		const auto secondAfter = *second;
		REQUIRE(std::filesystem::remove(cache.path() / (keys.back() + ".o")));
		auto repaired = compile(*third);
		REQUIRE(third->total == 211);
		REQUIRE(third->calls == 0);
		REQUIRE(third->cleanups == 0);
		REQUIRE(bindingStat<std::string>(repaired, "cache.mlir") == "hit");
		REQUIRE(bindingStat<int64_t>(repaired, "cache.tracingRan") == 0);
		REQUIRE(bindingStat<std::string>(repaired, "cache.key") == keys.back());
		REQUIRE(std::filesystem::exists(cache.path() / (keys.back() + ".o")));
		check(repaired, *third);
		REQUIRE(*first == firstAfter);
		REQUIRE(*second == secondAfter);
		check(cold, *first);
		REQUIRE(*second == secondAfter);
		REQUIRE(traces == coldTraces);
	}
	REQUIRE(keys.size() == 2);
	REQUIRE(keys[0] != keys[1]);
}

TEST_CASE("RuntimeBindings caches certified pointer steps against rebound storage", "[runtime-bindings][cache]") {
	const bool fold = GENERATE(false, true);
	const bool optimize = GENERATE(false, true);
	CAPTURE(fold, optimize);
	BindingCacheDirectory cache;
	auto options = bindingCacheOptions(cache.path(), "certified-pointer-steps-v1");
	options.setOption("engine.foldStaticConstants", fold);
	options.setOption("ir.runOptimizationPasses", optimize);
	options.setOption("ir.maxPipelineIterations", 8);
	int traces = 0;
	const auto compile = [&](std::array<int64_t, 4>& values) {
		RuntimeBindings bindings;
		auto state = bindings.bind<int64_t>("state", values.data());
		auto engine = NautilusEngine(cache::createCompiler(options), options);
		auto module = engine.createModule();
		module.setRuntimeBindings(bindings);
		module.registerFunction<val<int64_t>(val<int64_t>)>(
		    "execute", [state, &traces](val<int64_t> delta) -> val<int64_t> {
			    ++traces;
			    auto pointer = state.get() + cacheLiteral<std::size_t {0}>();
			    *++pointer += delta;
			    auto previous = pointer++;
			    *previous += delta;
			    *pointer += delta;
			    auto next = pointer--;
			    *next += delta;
			    *--pointer += delta;
			    return *pointer;
		    });
		module.registerFunction<val<int64_t*>(val<uint32_t>)>(
		    "back", [state](val<uint32_t> offset) { return (state.get() + cacheLiteral<std::size_t {3}>()) - offset; });
		module.registerFunction<val<int64_t*>(val<uint32_t*>)>("back_ref", [state](val<uint32_t*> offset) {
			return (state.get() + cacheLiteral<std::size_t {3}>()) - *offset;
		});
		return module.compile();
	};
	const auto check = [](CompiledModule& module, std::array<int64_t, 4>& values, int64_t delta) {
		auto expected = values;
		expected[0] += delta;
		expected[1] += 2 * delta;
		expected[2] += 2 * delta;
		REQUIRE(module.getFunction<int64_t(int64_t)>("execute")(delta) == expected[0]);
		REQUIRE(values == expected);
		for (uint32_t offset : {0, 1, 2, 3}) {
			CAPTURE(offset);
			REQUIRE(module.getFunction<int64_t*(uint32_t)>("back")(offset) == values.data() + 3 - offset);
			REQUIRE(module.getFunction<int64_t*(uint32_t*)>("back_ref")(&offset) == values.data() + 3 - offset);
		}
		REQUIRE(values == expected);
	};
	std::array<int64_t, 4> first {11, 23, 37, 53}, second {101, 211, 307, 401};
	REQUIRE(first.data() != second.data());
	const auto firstBefore = first;
	const auto secondBefore = second;
	auto cold = compile(first);
	REQUIRE(first == firstBefore);
	REQUIRE(bindingStat<int64_t>(cold, "cache.scalarCertificate") == 1);
	REQUIRE(bindingStat<std::string>(cold, "cache.scalarRejection").empty());
	REQUIRE(bindingStat<std::string>(cold, "cache.fallback") == "none");
	REQUIRE(bindingStat<std::string>(cold, "cache.object") == "written");
	REQUIRE(bindingStat<std::string>(cold, "cache.mlir") == "written");
	REQUIRE(bindingStat<int64_t>(cold, "cache.tracingRan") == 1);
	const auto coldTraces = traces;
	REQUIRE(coldTraces > 0);
	check(cold, first, 7);
	const auto firstAfter = first;
	auto warm = compile(second);
	REQUIRE(second == secondBefore);
	REQUIRE(bindingStat<std::string>(warm, "cache.fallback") == "none");
	REQUIRE(bindingStat<std::string>(warm, "cache.object") == "hit");
	REQUIRE(bindingStat<int64_t>(warm, "cache.tracingRan") == 0);
	REQUIRE(bindingStat<std::string>(warm, "cache.key") == bindingStat<std::string>(cold, "cache.key"));
	REQUIRE(traces == coldTraces);
	check(warm, second, -3);
	REQUIRE(first == firstAfter);
	const auto secondAfter = second;
	check(cold, first, 2);
	REQUIRE(second == secondAfter);
	REQUIRE(traces == coldTraces);
}

TEST_CASE("RuntimeBindings retains folded zero evidence without disabling legacy cache publication",
          "[runtime-bindings][cache][guard]") {
	const bool fold = GENERATE(false, true);
	const bool optimize = GENERATE(false, true);
	const bool opaque = GENERATE(false, true);
	CAPTURE(fold, optimize, opaque);
	BindingCacheDirectory cache;
	auto options = bindingCacheOptions(cache.path(), "folded-zero-cache-evidence-v1");
	options.setOption("engine.foldStaticConstants", fold);
	options.setOption("ir.runOptimizationPasses", optimize);
	options.setOption("ir.maxPipelineIterations", 8);
	std::array<int64_t, 2> values {11, 31};
	int traces = 0;
	std::string key;
	auto consume = +[](int64_t* state, int64_t increment) noexcept {
		*state += increment;
		return *state;
	};
	for (std::size_t iteration = 0; iteration < values.size(); ++iteration) {
		CAPTURE(iteration);
		RuntimeBindings bindings;
		auto state = bindings.bind<int64_t>("state", &values[iteration]);
		auto engine = NautilusEngine(cache::createCompiler(options), options);
		auto module = engine.createModule();
		module.setRuntimeBindings(bindings);
		module.registerFunction<val<int64_t>()>("execute", [=, &traces] {
			++traces;
			auto pointer = state.get() + 0;
			auto increment = cacheLiteral<int64_t {7}>();
			if (opaque) {
				return invoke(consume, pointer, increment);
			}
			return static_cast<val<int64_t>>(*pointer) + increment;
		});
		const auto before = values;
		const auto priorTraces = traces;
		auto compiled = module.compile();
		INFO(compiled.getStatistics()->toString());
		REQUIRE(values == before);
		const bool published = fold && !opaque;
		const bool hit = published && iteration == 1;
		REQUIRE(bindingStat<int64_t>(compiled, "cache.tracingRan") == (hit ? 0 : 1));
		REQUIRE((hit ? traces == priorTraces : traces > priorTraces));
		if (!hit) {
			REQUIRE(bindingStat<int64_t>(compiled, "cache.scalarCertificate") == 0);
			REQUIRE_THAT(bindingStat<std::string>(compiled, "cache.scalarRejection"),
			             Catch::Matchers::ContainsSubstring("uncertified_scalar"));
		}
		if (!published) {
			REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") == "non_relocatable_pointer");
			REQUIRE_THAT(bindingStat<std::string>(compiled, "cache.rejection"),
			             Catch::Matchers::ContainsSubstring(opaque ? "opaque_call" : "Constant"));
			REQUIRE(bindingStat<std::string>(compiled, "cache.object") == "miss");
			REQUIRE(bindingStat<std::string>(compiled, "cache.mlir") == "miss");
			for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
				REQUIRE(bindingArtifact(cache.path(), extension).empty());
			}
		} else {
			REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") == "none");
			REQUIRE(bindingStat<std::string>(compiled, "cache.object") == (hit ? "hit" : "written"));
			REQUIRE(bindingStat<std::string>(compiled, "cache.mlir") == (hit ? "not_checked" : "written"));
			if (hit) {
				REQUIRE(bindingStat<std::string>(compiled, "cache.key") == key);
			} else {
				key = bindingStat<std::string>(compiled, "cache.key");
			}
		}
		const auto compiledTraces = traces;
		REQUIRE(compiled.getFunction<int64_t()>("execute")() == before[iteration] + 7);
		REQUIRE(values[iteration] == before[iteration] + (opaque ? 7 : 0));
		REQUIRE(values[1 - iteration] == before[1 - iteration]);
		REQUIRE(traces == compiledTraces);
		if (!published) {
			for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
				REQUIRE(bindingArtifact(cache.path(), extension).empty());
			}
		}
	}
}

TEST_CASE("RuntimeBindings does not infer scalar certification from equal numeric values",
          "[runtime-bindings][cache]") {
	for (const bool certified : {false, true}) {
		CAPTURE(certified);
		BindingCacheDirectory cache;
		const auto options = bindingCacheOptions(
		    cache.path(), "same-numeric-origin-v1/certified=" + std::to_string(certified) + "/increment=7");
		int traces = 0;
		auto consume = +[](int64_t* state, int64_t increment) noexcept {
			*state += increment;
			return *state;
		};
		for (int iteration = 0; iteration < 2; ++iteration) {
			int64_t value = 11 + iteration;
			RuntimeBindings bindings;
			auto state = bindings.bind<int64_t>("state", &value);
			auto engine = NautilusEngine(cache::createCompiler(options), options);
			auto module = engine.createModule();
			module.setRuntimeBindings(bindings);
			module.registerFunction<val<int64_t>()>("execute", [=, &traces] {
				++traces;
				auto increment = certified ? cacheLiteral<int64_t {7}>() : val<int64_t>(7);
				return invoke(consume, state.get(), increment);
			});
			const auto priorTraces = traces;
			auto compiled = module.compile();
			REQUIRE(value == 11 + iteration);
			REQUIRE(compiled.getFunction<int64_t()>("execute")() == 18 + iteration);
			const bool hit = certified && iteration == 1;
			REQUIRE(bindingStat<int64_t>(compiled, "cache.tracingRan") == (hit ? 0 : 1));
			REQUIRE((hit ? traces == priorTraces : traces > priorTraces));
			if (!hit) {
				REQUIRE(bindingStat<int64_t>(compiled, "cache.scalarCertificate") == (certified ? 1 : 0));
			}
			if (certified) {
				REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") == "none");
				REQUIRE(bindingStat<std::string>(compiled, "cache.object") == (hit ? "hit" : "written"));
			} else {
				REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") == "non_relocatable_pointer");
				REQUIRE(bindingStat<std::string>(compiled, "cache.scalarRejection").find("uncertified_scalar") !=
				        std::string::npos);
				for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
					REQUIRE(bindingArtifact(cache.path(), extension).empty());
				}
			}
		}
	}
}

TEST_CASE("Cache safety analyses are mandatory independently of optimization scheduling",
          "[runtime-bindings][cache][guard]") {
	for (const std::string_view mode : {"passes disabled", "optimization disabled", "one iteration", "fixed point"}) {
		for (const std::string_view kind : {"certified", "uncertified", "discarded uncertified", "legacy"}) {
			CAPTURE(mode, kind);
			BindingCacheDirectory cache;
			auto options = bindingCacheOptions(cache.path(), "mandatory-cache-safety-v1/" + std::string(kind));
			options.setOption("ir.runPasses", mode != "passes disabled");
			options.setOption("ir.runOptimizationPasses", mode != "optimization disabled");
			options.setOption("ir.maxPipelineIterations", mode == "one iteration" ? 1 : 8);
			int traces = 0;
			auto consume = +[](int64_t* state, int64_t increment) noexcept {
				*state += increment;
				return *state;
			};
			const bool certified = kind == "certified";
			const bool published = certified || kind == "legacy";
			for (int iteration = 0; iteration < 2; ++iteration) {
				int64_t value = 11 + iteration;
				RuntimeBindings bindings;
				auto state = bindings.bind<int64_t>("state", &value);
				auto engine = NautilusEngine(cache::createCompiler(options), options);
				auto module = engine.createModule();
				module.setRuntimeBindings(bindings);
				module.registerFunction<val<int64_t>()>("execute", [=, &traces] {
					++traces;
					if (kind == "discarded uncertified") {
						val<int64_t> unused(11);
						(void) unused;
					}
					auto increment =
					    kind == "uncertified" || kind == "legacy" ? val<int64_t>(7) : cacheLiteral<int64_t {7}>();
					if (kind == "legacy") {
						return static_cast<val<int64_t>>(*state.get()) + increment;
					}
					return invoke(consume, state.get(), increment);
				});
				const auto priorTraces = traces;
				auto compiled = module.compile();
				REQUIRE(value == 11 + iteration);
				REQUIRE(compiled.getFunction<int64_t()>("execute")() == 18 + iteration);
				REQUIRE(value == (kind == "legacy" ? 11 : 18) + iteration);
				const bool hit = published && iteration == 1;
				REQUIRE(bindingStat<int64_t>(compiled, "cache.tracingRan") == (hit ? 0 : 1));
				REQUIRE((hit ? traces == priorTraces : traces > priorTraces));
				if (!hit) {
					REQUIRE(bindingStat<int64_t>(compiled, "cache.scalarCertificate") == (certified ? 1 : 0));
					const auto rejection = bindingStat<std::string>(compiled, "cache.scalarRejection");
					if (certified) {
						REQUIRE(rejection.empty());
					} else {
						REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("uncertified_scalar"));
					}
				}
				if (published) {
					REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") == "none");
					REQUIRE(bindingStat<std::string>(compiled, "cache.object") == (hit ? "hit" : "written"));
				} else {
					REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") == "non_relocatable_pointer");
					REQUIRE_THAT(bindingStat<std::string>(compiled, "cache.rejection"),
					             Catch::Matchers::ContainsSubstring("opaque_call"));
					for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
						REQUIRE(bindingArtifact(cache.path(), extension).empty());
					}
				}
			}
		}
	}
}

TEST_CASE("RuntimeBindings keeps the guard for mixed certified data and encoded heap fragments",
          "[runtime-bindings][cache]") {
	for (const bool foldConstants : {false, true}) {
		for (const std::string_view kind : {"integer", "partial bytes", "boolean bits", "callback", "cleanup"}) {
			CAPTURE(foldConstants, kind);
			BindingCacheDirectory cache;
			auto options = bindingCacheOptions(cache.path(), "mixed-certified-capture-v1/kind=" + std::string(kind));
			options.setOption("ir.runOptimizationPasses", true);
			options.setOption("ir.disableConstantFolding", !foldConstants);
			std::array<std::unique_ptr<int64_t>, 2> values;
			int traces = 0;
			for (std::size_t iteration = 0; iteration < values.size(); ++iteration) {
				values[iteration] = std::make_unique<int64_t>(41 + iteration);
				if (iteration != 0) {
					REQUIRE(values[0].get() != values[1].get());
				}
				const auto encoded = reinterpret_cast<uintptr_t>(values[iteration].get());
				uintptr_t scratch = encoded;
				std::array<bool, sizeof(uintptr_t) * 8> bits {};
				int64_t marker = 0;
				RuntimeBindings bindings;
				auto slot = bindings.bind<uintptr_t>("scratch", &scratch);
				auto bitStorage = bindings.bind<bool>("bits", bits.data());
				auto marked = bindings.bind<int64_t>("marker", &marker);
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
				auto engine = NautilusEngine(cache::createCompiler(options), options);
				auto module = engine.createModule();
				module.setRuntimeBindings(bindings);
				module.registerFunction<val<int64_t>(val<bool>)>("execute", [=, &callback, &traces](val<bool> fail) {
					++traces;
					*marked.get() = cacheLiteral<int64_t {9}>();
					if (kind == "partial bytes") {
						auto bytes = static_cast<val<uint8_t*>>(slot.get());
						const auto encodedBytes = std::bit_cast<std::array<uint8_t, sizeof(uintptr_t)>>(encoded);
						for (static_val<std::size_t> index = 0; index < sizeof(uintptr_t) / 2; ++index) {
							bytes[cacheInvariant(static_cast<std::size_t>(index))] = encodedBytes[index];
						}
						return invoke(consumeBytes, slot.get(), marked.get());
					}
					if (kind == "boolean bits") {
						for (static_val<std::size_t> index = 0; index < sizeof(uintptr_t) * 8; ++index) {
							bitStorage.get()[cacheInvariant(static_cast<std::size_t>(index))] =
							    bool((encoded >> static_cast<std::size_t>(index)) & 1);
						}
						return invoke(consumeBits, bitStorage.get(), marked.get());
					}
					if (kind == "callback") {
						return invoke(consumeCallback, callback.getFuncPtr(), marked.get());
					}
					if (kind == "cleanup") {
						val<int64_t*> address = val<uintptr_t>(encoded) + cacheLiteral<uintptr_t {0}>();
						tracing::registerDestructor(address.getState(), reinterpret_cast<void*>(cleanup));
						auto result = invoke(throwing, marked.get(), fail);
						tracing::unregisterDestructor(address.getState());
						return result;
					}
					return invoke(consumeInteger, val<uintptr_t>(encoded) + cacheLiteral<uintptr_t {0}>(),
					              marked.get());
				});
				const auto priorTraces = traces;
				auto compiled = module.compile();
				REQUIRE(marker == 0);
				REQUIRE(bindingStat<int64_t>(compiled, "cache.scalarCertificate") == 0);
				REQUIRE(bindingStat<std::string>(compiled, "cache.scalarRejection").find("uncertified_scalar") !=
				        std::string::npos);
				REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") == "non_relocatable_pointer");
				REQUIRE(bindingStat<int64_t>(compiled, "cache.tracingRan") == 1);
				REQUIRE(traces > priorTraces);
				auto execute = compiled.getFunction<int64_t(bool)>("execute");
				REQUIRE(execute(false) == (kind == "cleanup" ? 10 : *values[iteration] + 10));
				REQUIRE(marker == 10);
				if (kind == "cleanup") {
					const auto before = *values[iteration];
					REQUIRE_THROWS_WITH(execute(true), "mixed certified cleanup");
					REQUIRE(*values[iteration] == before + 1);
					REQUIRE(marker == 10);
				}
				for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
					REQUIRE(bindingArtifact(cache.path(), extension).empty());
				}
			}
		}
	}
}

TEST_CASE("RuntimeBindings cache keeps simultaneous module state independent", "[runtime-bindings][cache]") {
	BindingCacheDirectory cache;
	const auto options = bindingCacheOptions(cache.path(), "independent-binding-modules-v1");
	std::atomic<int> traces {0}, warmTraces {0};
	int64_t leftA = 5, rightA = 101, leftB = 19, rightB = 211;
	auto first = compileBindingPair(options, &leftA, &rightA, false, traces);
	REQUIRE(bindingStat<std::string>(first, "cache.object") == "written");
	REQUIRE(bindingStat<int64_t>(first, "cache.tracingRan") == 1);
	const auto coldTraces = traces.load();
	REQUIRE(coldTraces > 0);
	createBindingHistory();
	auto second = compileBindingPair(options, &leftB, &rightB, true, warmTraces);
	REQUIRE(bindingStat<std::string>(second, "cache.object") == "hit");
	REQUIRE(bindingStat<int64_t>(second, "cache.tracingRan") == 0);
	REQUIRE(warmTraces.load() == 0);
	REQUIRE(bindingStat<std::string>(first, "cache.key") == bindingStat<std::string>(second, "cache.key"));
	REQUIRE(first.getExecutable() != second.getExecutable());
	REQUIRE(traces.load() == coldTraces);
	requireBindingAddresses(first, &leftA, &rightA);
	requireBindingAddresses(second, &leftB, &rightB);

	auto executeA = first.getFunction<int64_t(int64_t)>("execute");
	auto executeB = second.getFunction<int64_t(int64_t)>("execute");
	REQUIRE(executeA(3) == 101);
	REQUIRE(executeB(7) == 211);
	REQUIRE(leftA == 8);
	REQUIRE(leftB == 26);
	rightA = 307;
	rightB = 401;
	std::atomic<int> failures {0};
	std::thread threadA([&] {
		for (int iteration = 0; iteration < 1000; ++iteration) {
			if (executeA(2) != 307) {
				failures.fetch_add(1);
			}
		}
	});
	std::thread threadB([&] {
		for (int iteration = 0; iteration < 1000; ++iteration) {
			if (executeB(3) != 401) {
				failures.fetch_add(1);
			}
		}
	});
	threadA.join();
	threadB.join();
	REQUIRE(failures.load() == 0);
	REQUIRE(leftA == 2008);
	REQUIRE(leftB == 3026);
	REQUIRE(traces.load() == coldTraces);
	REQUIRE(warmTraces.load() == 0);
}

TEST_CASE("RuntimeBindings cache preserves identities even when addresses alias", "[runtime-bindings][cache]") {
	BindingCacheDirectory cache;
	const auto options = bindingCacheOptions(cache.path(), "alias-binding-module-v1");
	std::atomic<int> traces {0};
	int64_t sharedCold = 10;
	auto cold = compileBindingPair(options, &sharedCold, &sharedCold, false, traces);
	REQUIRE(bindingStat<std::string>(cold, "cache.object") == "written");
	requireBindingAddresses(cold, &sharedCold, &sharedCold);
	REQUIRE(cold.getFunction<int64_t(int64_t)>("execute")(3) == 13);
	REQUIRE(sharedCold == 13);
	const auto coldTraces = traces.load();

	int64_t separateLeft = 20, separateRight = 47;
	auto separated = compileBindingPair(options, &separateLeft, &separateRight, true, traces);
	REQUIRE(bindingStat<std::string>(separated, "cache.object") == "hit");
	requireBindingAddresses(separated, &separateLeft, &separateRight);
	REQUIRE(separated.getFunction<int64_t(int64_t)>("execute")(5) == 47);
	REQUIRE(separateLeft == 25);
	REQUIRE(separateRight == 47);

	int64_t sharedWarm = 71;
	auto aliased = compileBindingPair(options, &sharedWarm, &sharedWarm, true, traces);
	REQUIRE(bindingStat<std::string>(aliased, "cache.object") == "hit");
	requireBindingAddresses(aliased, &sharedWarm, &sharedWarm);
	REQUIRE(aliased.getFunction<int64_t(int64_t)>("execute")(7) == 78);
	REQUIRE(sharedWarm == 78);
	REQUIRE(cold.getFunction<int64_t(int64_t)>("execute")(2) == 15);
	REQUIRE(sharedCold == 15);
	REQUIRE(traces.load() == coldTraces);
}

TEST_CASE("RuntimeBindings MLIR cache preserves argument const and subobject aliases", "[runtime-bindings][cache]") {
	const bool certified = GENERATE(false, true);
	CAPTURE(certified);
	BindingCacheDirectory cache;
	const auto options =
	    bindingCacheOptions(cache.path(), "overlapping-binding-module-v1/certified=" + std::to_string(certified));
	int traces = 0;
	auto compile = [&](BindingAliasState& object, int64_t& member, const int64_t& observed) {
		RuntimeBindings bindings;
		auto whole = bindings.bind<BindingAliasState>("object", &object);
		auto part = bindings.bind<int64_t>("member", &member);
		auto readOnly = bindings.bind<const int64_t>("observed", &observed);
		auto engine = NautilusEngine(cache::createCompiler(options), options);
		auto module = engine.createModule();
		module.setRuntimeBindings(bindings);
		module.registerFunction<val<int64_t>(val<int64_t*>)>(
		    "argument_alias", [part, certified, &traces](val<int64_t*> argument) -> val<int64_t> {
			    ++traces;
			    auto address = part.get();
			    val<int64_t> before = *address;
			    if (certified) {
				    *argument += cacheLiteral<int64_t {7}>();
			    } else {
				    *argument += int64_t {7};
			    }
			    val<int64_t> after = *address;
			    return after - before;
		    });
		module.registerFunction<val<bool>(val<int64_t*>)>(
		    "same_address", [part](val<int64_t*> argument) { return part.get() == argument; });
		module.registerFunction<val<int64_t>(val<int64_t>)>("const_alias",
		                                                    [part, readOnly](val<int64_t> delta) -> val<int64_t> {
			                                                    auto address = readOnly.get();
			                                                    val<int64_t> before = *address;
			                                                    *part.get() += delta;
			                                                    val<int64_t> after = *address;
			                                                    return after - before;
		                                                    });
		module.registerFunction<val<int64_t>(val<int64_t>)>(
		    "member_write", [whole, part, certified](val<int64_t> delta) -> val<int64_t> {
			    if (certified) {
				    auto objectAddress = static_cast<val<int64_t*>>(
				        static_cast<val<uint8_t*>>(whole.get()) + cacheLiteral<offsetof(BindingAliasState, second)>());
				    val<int64_t> before = *objectAddress;
				    *part.get() += delta;
				    val<int64_t> after = *objectAddress;
				    return after - before;
			    }
			    auto objectAddress = whole.get();
			    val<int64_t> before = objectAddress.get(&BindingAliasState::second);
			    *part.get() += delta;
			    val<int64_t> after = objectAddress.get(&BindingAliasState::second);
			    return after - before;
		    });
		module.registerFunction<val<int64_t>(val<int64_t>)>(
		    "object_write", [whole, part, certified](val<int64_t> delta) -> val<int64_t> {
			    auto address = part.get();
			    val<int64_t> before = *address;
			    if (certified) {
				    auto objectAddress = static_cast<val<int64_t*>>(
				        static_cast<val<uint8_t*>>(whole.get()) + cacheLiteral<offsetof(BindingAliasState, second)>());
				    *objectAddress += delta;
			    } else {
				    auto objectAddress = whole.get();
				    objectAddress.get(&BindingAliasState::second) += delta;
			    }
			    val<int64_t> after = *address;
			    return after - before;
		    });
		return module.compile();
	};
	auto check = [](CompiledModule& module, BindingAliasState& object, int64_t& member, const int64_t& observed) {
		const auto first = object.first;
		auto argument = module.getFunction<int64_t(int64_t*)>("argument_alias");
		auto sameAddress = module.getFunction<bool(int64_t*)>("same_address");
		auto memberBefore = member;
		REQUIRE(sameAddress(&member));
		REQUIRE(argument(&member) == 7);
		REQUIRE(member == memberBefore + 7);
		int64_t unrelated = 333;
		memberBefore = member;
		REQUIRE_FALSE(sameAddress(&unrelated));
		REQUIRE(argument(&unrelated) == 0);
		REQUIRE(unrelated == 340);
		REQUIRE(member == memberBefore);
		REQUIRE(module.getFunction<int64_t(int64_t)>("const_alias")(11) == (&member == &observed ? 11 : 0));
		REQUIRE(member == memberBefore + 11);
		const bool overlaps = &member == &object.second;
		memberBefore = member;
		auto secondBefore = object.second;
		REQUIRE(module.getFunction<int64_t(int64_t)>("member_write")(13) == (overlaps ? 13 : 0));
		REQUIRE(member == memberBefore + 13);
		REQUIRE(object.second == secondBefore + (overlaps ? 13 : 0));
		memberBefore = member;
		secondBefore = object.second;
		REQUIRE(module.getFunction<int64_t(int64_t)>("object_write")(17) == (overlaps ? 17 : 0));
		REQUIRE(member == memberBefore + (overlaps ? 17 : 0));
		REQUIRE(object.second == secondBefore + 17);
		REQUIRE(object.first == first);
	};

	const auto requireCachePath = [&](const CompiledModule& compiled, bool expectHit, bool repair, int priorTraces) {
		INFO(compiled.getStatistics()->toString());
		const bool hit = certified && expectHit;
		REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") ==
		        (certified ? (repair ? "invalid_object" : "none") : "non_relocatable_pointer"));
		REQUIRE(bindingStat<std::string>(compiled, repair ? "cache.mlir" : "cache.object") ==
		        (certified ? (expectHit ? "hit" : "written") : "miss"));
		REQUIRE(bindingStat<int64_t>(compiled, "cache.tracingRan") == (hit ? 0 : 1));
		REQUIRE((hit ? traces == priorTraces : traces > priorTraces));
		if (!hit) {
			REQUIRE(bindingStat<int64_t>(compiled, "cache.scalarCertificate") == (certified ? 1 : 0));
			if (certified) {
				REQUIRE(bindingStat<std::string>(compiled, "cache.scalarRejection").empty());
				REQUIRE(bindingStat<std::string>(compiled, "cache.mlir") == "written");
			} else {
				REQUIRE_THAT(bindingStat<std::string>(compiled, "cache.scalarRejection"),
				             Catch::Matchers::ContainsSubstring("uncertified_scalar"));
				for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
					REQUIRE(bindingArtifact(cache.path(), extension).empty());
				}
			}
		}
	};

	BindingAliasState coldState {19, 41};
	auto cold = compile(coldState, coldState.second, coldState.second);
	requireCachePath(cold, false, false, 0);
	const auto coldTraces = traces;
	REQUIRE(coldTraces > 0);
	check(cold, coldState, coldState.second, coldState.second);
	REQUIRE(traces == coldTraces);
	const auto coldSecond = coldState.second;

	BindingAliasState separateState {101, 211};
	int64_t separateMember = 23;
	const int64_t separateObserved = 71;
	auto separate = compile(separateState, separateMember, separateObserved);
	requireCachePath(separate, true, false, coldTraces);
	const auto separateTraces = traces;
	check(separate, separateState, separateMember, separateObserved);
	REQUIRE(separateObserved == 71);
	REQUIRE(traces == separateTraces);

	BindingAliasState warmState {223, 293};
	auto warm = compile(warmState, warmState.second, warmState.second);
	requireCachePath(warm, true, false, separateTraces);
	const auto warmTraces = traces;
	check(warm, warmState, warmState.second, warmState.second);
	REQUIRE(traces == warmTraces);

	if (certified) {
		corruptBindingArtifact(bindingArtifact(cache.path(), ".o"));
	}
	BindingAliasState repairedState {307, 401};
	auto repaired = compile(repairedState, repairedState.second, repairedState.second);
	requireCachePath(repaired, true, true, warmTraces);
	const auto repairedTraces = traces;
	check(repaired, repairedState, repairedState.second, repairedState.second);
	REQUIRE(coldState.second == coldSecond);
	REQUIRE(traces == repairedTraces);
	if (certified) {
		REQUIRE(traces == coldTraces);
	} else {
		for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
			REQUIRE(bindingArtifact(cache.path(), extension).empty());
		}
	}
}

TEST_CASE("RuntimeBindings cache repairs objects from MLIR and retraces corrupted artifacts",
          "[runtime-bindings][cache]") {
	BindingCacheDirectory cache;
	const auto options = bindingCacheOptions(cache.path(), "repair-binding-module-v1");
	std::atomic<int> traces {0};
	std::array<int64_t, 10> values {1, 11, 2, 22, 3, 33, 4, 44, 5, 55};
	{
		auto cold = compileBindingPair(options, &values[0], &values[1], false, traces);
		REQUIRE(bindingStat<std::string>(cold, "cache.object") == "written");
		REQUIRE(bindingStat<std::string>(cold, "cache.mlir") == "written");
	}
	const auto coldTraces = traces.load();
	const auto object = bindingArtifact(cache.path(), ".o");
	const auto bytecode = bindingArtifact(cache.path(), ".mlirbc");
	REQUIRE(!object.empty());
	REQUIRE(!bytecode.empty());
	corruptBindingArtifact(object);
	{
		auto repaired = compileBindingPair(options, &values[2], &values[3], true, traces);
		REQUIRE(bindingStat<std::string>(repaired, "cache.mlir") == "hit");
		REQUIRE(bindingStat<int64_t>(repaired, "cache.tracingRan") == 0);
		requireBindingAddresses(repaired, &values[2], &values[3]);
		REQUIRE(repaired.getFunction<int64_t(int64_t)>("execute")(10) == 22);
		REQUIRE(values[2] == 12);
		REQUIRE(traces.load() == coldTraces);
	}
	{
		auto warm = compileBindingPair(options, &values[4], &values[5], false, traces);
		REQUIRE(bindingStat<std::string>(warm, "cache.object") == "hit");
		requireBindingAddresses(warm, &values[4], &values[5]);
		REQUIRE(warm.getFunction<int64_t(int64_t)>("execute")(20) == 33);
		REQUIRE(values[4] == 23);
		REQUIRE(traces.load() == coldTraces);
	}
	corruptBindingArtifact(object);
	corruptBindingArtifact(bytecode);
	{
		auto retraced = compileBindingPair(options, &values[6], &values[7], true, traces);
		REQUIRE(bindingStat<int64_t>(retraced, "cache.tracingRan") == 1);
		REQUIRE(bindingStat<std::string>(retraced, "cache.object") == "written");
		REQUIRE(bindingStat<std::string>(retraced, "cache.mlir") == "written");
		requireBindingAddresses(retraced, &values[6], &values[7]);
		REQUIRE(retraced.getFunction<int64_t(int64_t)>("execute")(30) == 44);
		REQUIRE(values[6] == 34);
		REQUIRE(traces.load() > coldTraces);
	}
	const auto retraceCount = traces.load();
	auto warm = compileBindingPair(options, &values[8], &values[9], false, traces);
	REQUIRE(bindingStat<std::string>(warm, "cache.object") == "hit");
	requireBindingAddresses(warm, &values[8], &values[9]);
	REQUIRE(warm.getFunction<int64_t(int64_t)>("execute")(40) == 55);
	REQUIRE(values[8] == 45);
	REQUIRE(traces.load() == retraceCount);
}

TEST_CASE("RuntimeBindings cache schema includes unused registrations", "[runtime-bindings][cache]") {
	BindingCacheDirectory cache;
	const auto options = bindingCacheOptions(cache.path(), "complete-binding-schema-v1");
	int64_t value = 42;
	int32_t small = 1;
	uint32_t unsignedSmall = 2;
	const int32_t constantSmall = 3;
	BindingSchemaFirst firstType {};
	BindingSchemaSecond secondType {};
	BindingSchemaAligned alignedType {};
	RuntimeBindings source;
	auto state = source.bind<int64_t>("state", &value);
	int traces = 0;
	auto compile = [&](const RuntimeBindings& bindings) {
		auto engine = NautilusEngine(cache::createCompiler(options), options);
		auto module = engine.createModule();
		module.setRuntimeBindings(bindings);
		module.registerFunction<val<int64_t>()>("execute", [state, &traces]() -> val<int64_t> {
			++traces;
			return *state.get();
		});
		return module.compile();
	};
	std::vector<RuntimeBindings> schemas {source};
	auto addSchema = [&](auto* address, const std::string& identity) {
		auto registry = source;
		(void) registry.bind(identity, address);
		schemas.push_back(std::move(registry));
	};
	addSchema(&small, "unused");
	addSchema(&unsignedSmall, "unused");
	addSchema(&constantSmall, "unused");
	addSchema(&value, "unused");
	addSchema(&small, "renamed_unused");
	addSchema(&firstType, "unused");
	addSchema(&secondType, "unused");
	addSchema(&alignedType, "unused");
	for (std::size_t index = 0; index < schemas.size(); ++index) {
		CAPTURE(index);
		const auto before = traces;
		auto cold = compile(schemas[index]);
		REQUIRE(bindingStat<std::string>(cold, "cache.object") == "written");
		REQUIRE(bindingStat<int64_t>(cold, "cache.tracingRan") == 1);
		REQUIRE(traces > before);
		REQUIRE(cold.getFunction<int64_t()>("execute")() == 42);
		const auto after = traces;
		auto warm = compile(schemas[index]);
		REQUIRE(bindingStat<std::string>(warm, "cache.object") == "hit");
		REQUIRE(traces == after);
	}
	auto original = compile(source);
	REQUIRE(bindingStat<std::string>(original, "cache.object") == "hit");
}

TEST_CASE("RuntimeBindings cache misses incompatible registries before rejecting captured handles",
          "[runtime-bindings][cache]") {
	BindingCacheDirectory cache;
	const auto options = bindingCacheOptions(cache.path(), "incompatible-binding-schema-v1");
	int64_t value = 42;
	RuntimeBindings source;
	auto state = source.bind<int64_t>("state", &value);
	int traces = 0;
	auto compile = [&](const RuntimeBindings* bindings) {
		auto engine = NautilusEngine(cache::createCompiler(options), options);
		auto module = engine.createModule();
		if (bindings != nullptr) {
			module.setRuntimeBindings(*bindings);
		}
		module.registerFunction<val<int64_t>()>("execute", [state, &traces]() -> val<int64_t> {
			++traces;
			return *state.get();
		});
		return module.compile();
	};
	{
		auto cold = compile(&source);
		REQUIRE(bindingStat<std::string>(cold, "cache.object") == "written");
		REQUIRE(cold.getFunction<int64_t()>("execute")() == 42);
	}
	const auto coldTraces = traces;
	uint64_t wrongType = 99;
	int32_t wrongSize = 13;
	const int64_t wrongConst = 57;
	RuntimeBindings incompatible;
	const RuntimeBindings* environment = &incompatible;
	SECTION("no registry") {
		environment = nullptr;
	}
	SECTION("empty registry") {
	}
	SECTION("wrong identity") {
		(void) incompatible.bind<int64_t>("renamed", &value);
	}
	SECTION("same-sized wrong type") {
		(void) incompatible.bind<uint64_t>("state", &wrongType);
	}
	SECTION("wrong size") {
		(void) incompatible.bind<int32_t>("state", &wrongSize);
	}
	SECTION("wrong const qualification") {
		(void) incompatible.bind<const int64_t>("state", &wrongConst);
	}
	REQUIRE_THROWS_AS(compile(environment), std::invalid_argument);
	REQUIRE(traces > coldTraces);
	auto original = compile(&source);
	REQUIRE(bindingStat<std::string>(original, "cache.object") == "hit");
	REQUIRE(original.getFunction<int64_t()>("execute")() == 42);
}

TEST_CASE("RuntimeBindings does not make captured raw pointers persistable", "[runtime-bindings][cache]") {
	bool unselected = false;
	SECTION("direct capture") {
	}
	SECTION("unselected capture") {
		unselected = true;
	}
	BindingCacheDirectory cache;
	const auto options = bindingCacheOptions(cache.path(), "mixed-raw-binding-module-v1");
	int64_t value = 21;
	RuntimeBindings bindings;
	auto state = bindings.bind<int64_t>("state", &value);
	int traces = 0;
	for (int iteration = 0; iteration < 2; ++iteration) {
		auto engine = NautilusEngine(cache::createCompiler(options), options);
		auto module = engine.createModule();
		module.setRuntimeBindings(bindings);
		module.registerFunction<val<int64_t>()>("execute", [state, &value, &traces, unselected]() -> val<int64_t> {
			++traces;
			val<int64_t> bound = *state.get();
			val<int64_t*> raw = &value;
			if (unselected) {
				raw = select(val<bool>(false), raw, state.get());
			}
			return bound + *raw;
		});
		auto compiled = module.compile();
		REQUIRE(compiled.getFunction<int64_t()>("execute")() == 42);
		REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") == "non_relocatable_pointer");
		REQUIRE(bindingStat<int64_t>(compiled, "cache.tracingRan") == 1);
	}
	REQUIRE(traces >= 2);
	REQUIRE(bindingArtifact(cache.path(), ".manifest").empty());
}

TEST_CASE("RuntimeBindings rejects integer-encoded heap addresses without publishing artifacts",
          "[runtime-bindings][cache]") {
	for (const bool foldConstants : {false, true}) {
		for (const auto* expression :
		     {"direct", "arithmetic", "runtime offset", "branch", "loop", "null base", "null pointer offset",
		      "pointer difference", "callee result", "callee argument", "indirect result", "indirect argument",
		      "selected result", "nonnull capture", "nonnull difference", "null branch", "unrelated null guard"}) {
			CAPTURE(foldConstants, expression);
			BindingCacheDirectory cache;
			auto options = bindingCacheOptions(cache.path(), "integer-encoded-binding-module-v1");
			options.setOption("ir.runOptimizationPasses", true);
			options.setOption("ir.disableConstantFolding", !foldConstants);
			std::array<std::unique_ptr<std::array<int64_t, 4>>, 2> storage;
			int traces = 0;
			for (std::size_t iteration = 0; iteration < storage.size(); ++iteration) {
				CAPTURE(iteration);
				storage[iteration] = std::make_unique<std::array<int64_t, 4>>();
				auto& values = *storage[iteration];
				values = {11 + static_cast<int64_t>(iteration), 22, 33, 44};
				const auto encoded = reinterpret_cast<uintptr_t>(values.data());
				NautilusFunction encodedAddress {"encoded_address", [encoded] { return val<uintptr_t>(encoded); }};
				NautilusFunction offsetEncoded {
				    "offset_encoded", [encoded](val<uintptr_t> delta) { return val<uintptr_t>(encoded) + delta; }};
				NautilusFunction shiftedEncoded {
				    "shifted_encoded", [encoded](val<uintptr_t> delta) { return val<uintptr_t>(encoded) - delta; }};
				NautilusFunction dereference {"dereference", [](val<uintptr_t> address) -> val<int64_t> {
					                              val<int64_t*> pointer = address;
					                              return *pointer;
				                              }};
				RuntimeBindings bindings;
				auto state = bindings.bind<int64_t>("state", &values[3]);
				auto engine = NautilusEngine(cache::createCompiler(options), options);
				auto module = engine.createModule();
				module.setRuntimeBindings(bindings);
				module.registerFunction<val<int64_t>(val<uintptr_t>, val<int64_t*>)>(
				    "execute",
				    [=, &traces, &encodedAddress, &dereference, &offsetEncoded,
				     &shiftedEncoded](val<uintptr_t> offset, val<int64_t*> runtime) -> val<int64_t> {
					    ++traces;
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
						    return *pointer + *state.get();
					    } else if (kind == "pointer difference" || kind == "nonnull difference") {
						    const auto base = static_cast<val<uintptr_t>>(runtime);
						    address = (base + address) - base;
					    } else if (kind == "callee result") {
						    address = encodedAddress();
					    } else if (kind == "callee argument") {
						    return dereference(address) + *state.get();
					    } else if (kind == "indirect result") {
						    address = encodedAddress.getFuncPtr()();
					    } else if (kind == "indirect argument") {
						    return dereference.getFuncPtr()(address) + *state.get();
					    } else if (kind == "selected result") {
						    auto callback = offsetEncoded.getFuncPtr();
						    if (offset > 0) {
							    callback = shiftedEncoded.getFuncPtr();
						    }
						    address = callback(val<uintptr_t>(0));
					    }
					    if (kind == "null branch" || kind == "unrelated null guard") {
						    auto base = select(offset > 0, static_cast<val<int8_t*>>(runtime), val<int8_t*>(nullptr));
						    if (kind == "null branch") {
							    if (base == nullptr) {
								    auto pointer = static_cast<val<int64_t*>>(base + address);
								    return *pointer + *state.get();
							    }
						    } else if (runtime != nullptr) {
							    auto delta = select(offset > 0, val<uintptr_t>(0), address);
							    auto integerBase = static_cast<val<uintptr_t>>(base);
							    val<int64_t*> pointer = integerBase + delta;
							    return *pointer + *state.get();
						    }
						    return *runtime + *state.get();
					    }
					    val<int64_t*> pointer = address;
					    if (kind == "nonnull capture" || kind == "nonnull difference") {
						    if (pointer != nullptr) {
							    return pointer[1] + *state.get();
						    }
						    return *state.get();
					    }
					    return *pointer + *state.get();
				    });
				auto compiled = module.compile();
				auto execute = compiled.getFunction<int64_t(uintptr_t, int64_t*)>("execute");
				const bool guarded = std::string_view(expression) == "nonnull capture" ||
				                     std::string_view(expression) == "nonnull difference";
				REQUIRE(execute(0, values.data()) == values[guarded ? 1 : 0] + values[3]);
				const bool advances =
				    std::string_view(expression) == "runtime offset" || std::string_view(expression) == "loop";
				REQUIRE(execute(2, values.data()) == values[guarded ? 1 : advances ? 2 : 0] + values[3]);
				REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") == "non_relocatable_pointer");
				REQUIRE(bindingStat<int64_t>(compiled, "cache.tracingRan") == 1);
				for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
					REQUIRE(bindingArtifact(cache.path(), extension).empty());
				}
			}
			REQUIRE(traces >= 2);
		}
	}
}

TEST_CASE("RuntimeBindings rejects memory and call laundering of captured addresses", "[runtime-bindings][cache]") {
	for (const bool foldConstants : {false, true}) {
		for (const std::string_view kind : {"binding",
		                                    "binding alias",
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
			CAPTURE(foldConstants, kind);
			BindingCacheDirectory cache;
			auto options = bindingCacheOptions(cache.path(), "memory-encoded-binding-module-v1");
			options.setOption("ir.runOptimizationPasses", true);
			options.setOption("ir.disableConstantFolding", !foldConstants);
			std::array<std::unique_ptr<int64_t>, 2> values;
			int traces = 0;
			for (std::size_t iteration = 0; iteration < values.size(); ++iteration) {
				CAPTURE(iteration);
				values[iteration] = std::make_unique<int64_t>(101 + iteration * 17);
				if (iteration != 0) {
					REQUIRE(values[0].get() != values[1].get());
				}
				const auto encoded = reinterpret_cast<uintptr_t>(values[iteration].get());
				std::array<uintptr_t, 2> scratch {};
				auto* scratchAddress = &scratch[1];
				RuntimeBindings bindings;
				auto scratchBinding = bindings.bind<uintptr_t>("scratch", scratch.data());
				auto aliasBinding = bindings.bind<uintptr_t>("alias", scratchAddress);
				auto tableBinding = bindings.bind<uintptr_t*>("table", &scratchAddress);
				int64_t observed = 0;
				auto observedBinding = bindings.bind<int64_t>("observed", &observed);
				NautilusFunction storeAddress {"store_address", [encoded](val<uintptr_t*> slot) { *slot = encoded; }};
				NautilusFunction loadAddress {"load_address",
				                              [](val<uintptr_t*> slot) -> val<uintptr_t> { return *slot; }};
				NautilusFunction encodedCallback {"encoded_callback", [encoded] { return val<uintptr_t>(encoded); }};
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
				auto engine = NautilusEngine(cache::createCompiler(options), options);
				auto module = engine.createModule();
				module.setRuntimeBindings(bindings);
				if (kind == "cross export") {
					module.registerFunction<val<uintptr_t>()>("initialize", [=]() -> val<uintptr_t> {
						scratchBinding.get()[1] = encoded;
						return encoded;
					});
				}
				module.registerFunction<val<int64_t>(val<uintptr_t*>, val<uintptr_t (*)(uintptr_t)>)>(
				    "execute",
				    [=, &storeAddress, &loadAddress, &encodedCallback, &offsetCallback, &callbackFactory,
				     &traces](val<uintptr_t*> argument, val<uintptr_t (*)(uintptr_t)> callback) -> val<int64_t> {
					    ++traces;
					    if (kind == "native callback") {
						    return invoke(nativeCallback, encodedCallback.getFuncPtr());
					    }
					    if (kind == "native returned callback") {
						    return invoke(nativeOffsetCallback, callbackFactory(),
						                  static_cast<val<uintptr_t>>(aliasBinding.get()));
					    }
					    if (kind == "native callback offset") {
						    return invoke(nativeOffsetCallback, offsetCallback.getFuncPtr(),
						                  static_cast<val<uintptr_t>>(aliasBinding.get()));
					    }
					    auto slot = kind == "native memory difference" || kind == "native consumer" ||
					                        kind == "native void consumer"
					                    ? aliasBinding.get()
					                    : scratchBinding.get() + 1;
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
						    const auto encodedBytes = std::bit_cast<std::array<uint8_t, sizeof(uintptr_t)>>(encoded);
						    for (static_val<std::size_t> index = 0; index < encodedBytes.size(); ++index) {
							    bytes[index] = encodedBytes[index];
						    }
					    } else if (kind != "cross export" && kind != "native result" &&
					               kind != "native pointer result" && kind != "indirect result" &&
					               kind != "native difference" && kind != "native integer consumer") {
						    *slot = encoded;
					    }
					    if (kind == "binding alias") {
						    slot = aliasBinding.get();
					    } else if (kind == "argument alias") {
						    slot = argument;
					    } else if (kind == "indirect load") {
						    slot = *tableBinding.get();
					    }
					    if (kind == "native consumer") {
						    return invoke(nativeConsumer, slot);
					    }
					    if (kind == "native integer consumer") {
						    return invoke(nativeIntegerConsumer, val<uintptr_t>(encoded));
					    }
					    if (kind == "native void consumer") {
						    invoke(nativeVoidConsumer, slot, observedBinding.get());
						    return *observedBinding.get();
					    }
					    if (kind == "pointer load") {
						    val<int64_t*> pointer = *static_cast<val<int64_t**>>(slot);
						    return *pointer;
					    }
					    if (kind == "native pointer result") {
						    auto pointer = invoke(nativePointer, val<uintptr_t>(encoded));
						    return *pointer;
					    }
					    val<uintptr_t> address;
					    if (kind == "pointer cancellation") {
						    val<uintptr_t> loaded = *slot;
						    address = static_cast<val<uintptr_t>>(slot) - loaded;
					    } else if (kind == "native difference") {
						    auto base = static_cast<val<uintptr_t>>(scratchBinding.get());
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
				const auto before = traces;
				auto compiled = module.compile();
				if (kind == "cross export") {
					REQUIRE(compiled.getFunction<uintptr_t()>("initialize")() == encoded);
				}
				auto execute = compiled.getFunction<int64_t(uintptr_t*, uintptr_t (*)(uintptr_t))>("execute");
				REQUIRE(execute(scratchAddress, nativeIdentity) == *values[iteration]);
				REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") ==
				        (kind == "alloca" ? "unsupported_allocation_metadata" : "non_relocatable_pointer"));
				if (kind == "alloca") {
					REQUIRE(bindingStat<std::string>(compiled, "cache.rejection") ==
					        "allocation_metadata_origins_unavailable");
				}
				REQUIRE(bindingStat<int64_t>(compiled, "cache.tracingRan") == 1);
				REQUIRE(traces > before);
				for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
					REQUIRE(bindingArtifact(cache.path(), extension).empty());
				}
			}
		}
	}
}

TEST_CASE("RuntimeBindings rejects captured addresses consumed only by native cleanup", "[runtime-bindings][cache]") {
	BindingCacheDirectory cache;
	const auto options = bindingCacheOptions(cache.path(), "encoded-cleanup-module-v1");
	std::array<std::unique_ptr<int64_t>, 2> values;
	for (std::size_t iteration = 0; iteration < values.size(); ++iteration) {
		CAPTURE(iteration);
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
		auto engine = NautilusEngine(cache::createCompiler(options), options);
		auto module = engine.createModule();
		module.registerFunction<val<int64_t>(val<int64_t*>, val<int64_t>)>(
		    "execute", [=](val<int64_t*> base, val<int64_t> value) {
			    val<int64_t*> address = static_cast<val<uintptr_t>>(base) + encoded;
			    tracing::registerDestructor(address.getState(), reinterpret_cast<void*>(cleanup));
			    auto result = invoke(nativeCall, value);
			    tracing::unregisterDestructor(address.getState());
			    return result;
		    });
		auto compiled = module.compile();
		auto execute = compiled.getFunction<int64_t(int64_t*, int64_t)>("execute");
		REQUIRE(execute(nullptr, 7) == 7);
		const auto before = *values[iteration];
		REQUIRE_THROWS_AS(execute(nullptr, -1), std::runtime_error);
		REQUIRE(*values[iteration] == before + 1);
		REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") == "non_relocatable_pointer");
		REQUIRE(bindingStat<int64_t>(compiled, "cache.tracingRan") == 1);
		for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
			REQUIRE(bindingArtifact(cache.path(), extension).empty());
		}
	}
}

TEST_CASE("RuntimeBindings caches runtime pointer spills", "[runtime-bindings][cache]") {
	for (const std::string_view kind : {"binding", "alloca", "callee", "native"}) {
		CAPTURE(kind);
		BindingCacheDirectory cache;
		auto options = bindingCacheOptions(cache.path(), "runtime-pointer-spill-module-v1");
		std::array<std::unique_ptr<std::array<int64_t, 3>>, 2> values;
		int traces = 0;
		for (std::size_t iteration = 0; iteration < values.size(); ++iteration) {
			CAPTURE(iteration);
			values[iteration] = std::make_unique<std::array<int64_t, 3>>(
			    std::array<int64_t, 3> {11 + static_cast<int64_t>(iteration), 22, 33});
			uintptr_t scratch = 0;
			RuntimeBindings bindings;
			auto state = bindings.bind<int64_t>("state", values[iteration]->data());
			auto scratchBinding = bindings.bind<uintptr_t>("scratch", &scratch);
			NautilusFunction storeAddress {"store_runtime_address",
			                               [](val<uintptr_t*> slot, val<uintptr_t> address) { *slot = address; }};
			auto nativeStore = +[](uintptr_t* slot, uintptr_t address) noexcept {
				*slot = address;
			};
			auto engine = NautilusEngine(cache::createCompiler(options), options);
			auto module = engine.createModule();
			module.setRuntimeBindings(bindings);
			module.registerFunction<val<int64_t>(val<uintptr_t>)>(
			    "execute", [=, &storeAddress, &traces](val<uintptr_t> index) -> val<int64_t> {
				    ++traces;
				    auto slot = scratchBinding.get();
				    if (kind == "alloca") {
					    slot = nautilus::details::nautilus_alloca<uintptr_t>();
				    }
				    auto address = static_cast<val<uintptr_t>>(state.get());
				    if (kind == "callee") {
					    storeAddress(slot, address);
				    } else if (kind == "native") {
					    invoke(nativeStore, slot, address);
				    } else {
					    *slot = address;
				    }
				    val<uintptr_t> loaded = *slot;
				    val<int64_t*> pointer = loaded;
				    return pointer[index];
			    });
			const auto before = traces;
			auto compiled = module.compile();
			const bool allocationMetadata = kind == "alloca";
			const bool hit = !allocationMetadata && iteration == 1;
			REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") ==
			        (allocationMetadata ? "unsupported_allocation_metadata" : "none"));
			REQUIRE(bindingStat<std::string>(compiled, "cache.object") == (allocationMetadata ? "miss"
			                                                               : hit              ? "hit"
			                                                                                  : "written"));
			REQUIRE(bindingStat<int64_t>(compiled, "cache.tracingRan") == (hit ? 0 : 1));
			REQUIRE((hit ? traces == before : traces > before));
			if (allocationMetadata) {
				REQUIRE(bindingStat<std::string>(compiled, "cache.rejection") ==
				        "allocation_metadata_origins_unavailable");
				for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
					REQUIRE(bindingArtifact(cache.path(), extension).empty());
				}
			}
			auto execute = compiled.getFunction<int64_t(uintptr_t)>("execute");
			for (std::size_t index = 0; index < values[iteration]->size(); ++index) {
				REQUIRE(execute(index) == (*values[iteration])[index]);
			}
		}
	}
}

TEST_CASE("RuntimeBindings caches runtime-derived integer pointer arithmetic", "[runtime-bindings][cache]") {
	const bool certified = GENERATE(false, true);
	CAPTURE(certified);
	for (const bool foldConstants : {false, true}) {
		CAPTURE(foldConstants);
		BindingCacheDirectory cache;
		auto options = bindingCacheOptions(cache.path(),
		                                   "runtime-integer-pointer-module-v1/certified=" + std::to_string(certified));
		options.setOption("ir.runOptimizationPasses", true);
		options.setOption("ir.disableConstantFolding", !foldConstants);
		auto first = std::make_unique<std::array<int64_t, 4>>(std::array<int64_t, 4> {11, 22, 33, 44});
		auto second = std::make_unique<std::array<int64_t, 4>>(std::array<int64_t, 4> {55, 66, 77, 88});
		int traces = 0;
		std::string key;
		for (int iteration = 0; iteration < 3; ++iteration) {
			CAPTURE(iteration);
			auto* values = (iteration == 0 ? first : second)->data();
			RuntimeBindings bindings;
			auto state = bindings.bind<int64_t>("state", values);
			auto engine = NautilusEngine(cache::createCompiler(options), options);
			auto module = engine.createModule();
			module.setRuntimeBindings(bindings);
			module.registerFunction<val<int64_t>(val<uintptr_t>)>(
			    "bound_offset", [state, certified, &traces](val<uintptr_t> index) -> val<int64_t> {
				    ++traces;
				    auto base = static_cast<val<uintptr_t>>(state.get());
				    val<int64_t*> pointer =
				        certified ? base + (index + cacheLiteral<uintptr_t {1}>()) * cacheLiteral<sizeof(int64_t)>()
				                  : base + (index + 1) * sizeof(int64_t);
				    return *pointer;
			    });
			module.registerFunction<val<int64_t>(val<int64_t*>, val<uintptr_t>)>(
			    "pointer_offset", [certified](val<int64_t*> base, val<uintptr_t> index) -> val<int64_t> {
				    auto address = static_cast<val<uintptr_t>>(base);
				    val<int64_t*> pointer =
				        certified ? cacheLiteral<sizeof(int64_t)>() + address + index * cacheLiteral<sizeof(int64_t)>()
				                  : sizeof(int64_t) + address + index * sizeof(int64_t);
				    return *(certified ? pointer - cacheLiteral<uintptr_t {1}>() : pointer - 1);
			    });
			module.registerFunction<val<int64_t>(val<uintptr_t>)>("integer_argument",
			                                                      [](val<uintptr_t> address) -> val<int64_t> {
				                                                      val<int64_t*> pointer = address;
				                                                      return *pointer;
			                                                      });
			module.registerFunction<val<int64_t>(val<uintptr_t>, val<uintptr_t>)>(
			    "integer_arguments", [](val<uintptr_t> address, val<uintptr_t> offset) -> val<int64_t> {
				    val<int64_t*> pointer = address + offset;
				    return *pointer;
			    });
			module.registerFunction<val<int64_t>(val<int64_t*>, val<uintptr_t>)>(
			    "integer_induction", [certified](val<int64_t*> base, val<uintptr_t> count) -> val<int64_t> {
				    auto address = static_cast<val<uintptr_t>>(base);
				    for (val<uintptr_t> index = certified ? cacheLiteral<uintptr_t {0}>() : val<uintptr_t>(0);
				         index < count; ++index) {
					    address = certified ? address + cacheLiteral<sizeof(int64_t)>() : address + sizeof(int64_t);
				    }
				    val<int64_t*> pointer = address;
				    return *pointer;
			    });
			module.registerFunction<val<int64_t>(val<int64_t*>, val<bool>, val<uintptr_t>)>(
			    "merged_pointer",
			    [state, certified](val<int64_t*> other, val<bool> useBinding, val<uintptr_t> index) -> val<int64_t> {
				    auto address = static_cast<val<uintptr_t>>(other);
				    if (useBinding) {
					    address = static_cast<val<uintptr_t>>(state.get());
				    }
				    val<int64_t*> pointer = certified ? address + index * cacheLiteral<sizeof(int64_t)>()
				                                      : address + index * sizeof(int64_t);
				    return *pointer;
			    });
			module.registerFunction<val<int64_t>(val<int64_t**>, val<uintptr_t>)>(
			    "table_pointer", [certified](val<int64_t**> table, val<uintptr_t> index) -> val<int64_t> {
				    val<int64_t*> base = *table;
				    auto address = static_cast<val<uintptr_t>>(base);
				    val<int64_t*> pointer = certified ? address + index * cacheLiteral<sizeof(int64_t)>()
				                                      : address + index * sizeof(int64_t);
				    return *pointer;
			    });
			NautilusFunction offsetPointer {
			    "offset_pointer", [certified](val<uintptr_t> address, val<uintptr_t> index) {
				    val<int64_t*> pointer = certified ? address + index * cacheLiteral<sizeof(int64_t)>()
				                                      : address + index * sizeof(int64_t);
				    return pointer;
			    }};
			module.registerFunction<val<int64_t>(val<int64_t*>, val<uintptr_t>)>(
			    "callee_pointer", [&offsetPointer](val<int64_t*> base, val<uintptr_t> index) -> val<int64_t> {
				    return *offsetPointer(static_cast<val<uintptr_t>>(base), index);
			    });
			module.registerFunction<val<int64_t>(val<int64_t*>, val<uintptr_t>)>(
			    "indirect_pointer", [&offsetPointer](val<int64_t*> base, val<uintptr_t> index) -> val<int64_t> {
				    return *offsetPointer.getFuncPtr()(static_cast<val<uintptr_t>>(base), index);
			    });
			NautilusFunction alternatePointer {"alternate_pointer", [](val<uintptr_t> address, val<uintptr_t> index) {
				                                   val<int64_t*> pointer = address;
				                                   return pointer + index;
			                                   }};
			module.registerFunction<val<int64_t>(val<int64_t*>, val<bool>, val<uintptr_t>)>(
			    "selected_pointer",
			    [&offsetPointer, &alternatePointer](val<int64_t*> base, val<bool> select,
			                                        val<uintptr_t> index) -> val<int64_t> {
				    auto callback = offsetPointer.getFuncPtr();
				    if (select) {
					    callback = alternatePointer.getFuncPtr();
				    }
				    return *callback(static_cast<val<uintptr_t>>(base), index);
			    });
			module.registerFunction<val<int64_t>(val<int64_t*>, val<double>)>(
			    "numeric_offset", [certified](val<int64_t*> base, val<double> index) -> val<int64_t> {
				    auto address = static_cast<val<uintptr_t>>(base);
				    val<int64_t*> pointer =
				        certified ? address + static_cast<val<uintptr_t>>(index) * cacheLiteral<sizeof(int64_t)>()
				                  : address + static_cast<val<uintptr_t>>(index) * sizeof(int64_t);
				    return *pointer;
			    });
			module.registerFunction<val<int64_t>(val<uintptr_t>)>(
			    "null_iterator", [state, certified](val<uintptr_t> count) -> val<int64_t> {
				    val<int64_t*> iterator = nullptr;
				    if (iterator != nullptr) {
					    for (val<uintptr_t> index = certified ? cacheLiteral<uintptr_t {0}>() : val<uintptr_t>(0);
					         index < count; ++index) {
						    iterator = certified ? iterator + cacheLiteral<uintptr_t {1}>() : iterator + 1;
					    }
					    return certified ? iterator[cacheLiteral<uintptr_t {3}>()] : iterator[3];
				    }
				    return certified ? state.get()[cacheLiteral<uintptr_t {1}>()] : state.get()[1];
			    });
			module.registerFunction<val<int64_t>()>("null_cache_merge", [state, certified]() -> val<int64_t> {
				val<int64_t*> cached = nullptr;
				if (!(certified ? cacheLiteral<false>() : val<bool>(false))) {
					cached = state.get();
				}
				return certified ? cached[cacheLiteral<uintptr_t {2}>()] : cached[2];
			});
			for (const bool condition : {false, true}) {
				const auto suffix = condition ? "_true" : "_false";
				module.registerFunction<val<int64_t>()>(std::string("null_select") + suffix, [state, condition,
				                                                                              certified] {
					auto base = state.get();
					auto pointer = condition ? select(certified ? cacheLiteral<true>() : val<bool>(true), base,
					                                  val<int64_t*>(nullptr))
					                         : select(certified ? cacheLiteral<false>() : val<bool>(false),
					                                  val<int64_t*>(nullptr), base);
					return static_cast<val<int64_t>>(certified ? pointer[cacheLiteral<uintptr_t {1}>()] : pointer[1]);
				});
				module.registerFunction<val<int64_t>(val<uintptr_t>)>(
				    std::string("integer_select") + suffix,
				    [condition, certified](val<uintptr_t> address) -> val<int64_t> {
					    val<int64_t*> pointer =
					        condition ? select(certified ? cacheLiteral<true>() : val<bool>(true), address,
					                           certified ? cacheLiteral<uintptr_t {0}>() : val<uintptr_t>(0))
					                  : select(certified ? cacheLiteral<false>() : val<bool>(false),
					                           certified ? cacheLiteral<uintptr_t {0}>() : val<uintptr_t>(0), address);
					    return certified ? pointer[cacheLiteral<uintptr_t {1}>()] : pointer[1];
				    });
				module.registerFunction<val<bool>(val<bool>)>(
				    std::string("bool_select") + suffix, [condition, certified](val<bool> input) {
					    return condition ? select(certified ? cacheLiteral<true>() : val<bool>(true), input,
					                              certified ? cacheLiteral<false>() : val<bool>(false))
					                     : select(certified ? cacheLiteral<false>() : val<bool>(false),
					                              certified ? cacheLiteral<false>() : val<bool>(false), input);
				    });
			}
			for (const bool equal : {false, true}) {
				for (const bool negate : {false, true}) {
					for (const bool nullOnLeft : {false, true}) {
						const auto name =
						    "null_guard_" + std::to_string(equal) + std::to_string(negate) + std::to_string(nullOnLeft);
						module.registerFunction<val<int64_t>(val<bool>, val<uintptr_t>)>(
						    name, [=](val<bool> available, val<uintptr_t> count) -> val<int64_t> {
							    val<int64_t*> pointer = nullptr;
							    if (available) {
								    pointer = state.get();
							    }
							    count = certified ? count & cacheLiteral<uintptr_t {3}>() : count & uintptr_t {3};
							    val<int64_t*> null = nullptr;
							    auto condition = nullOnLeft ? (equal ? null == pointer : null != pointer)
							                                : (equal ? pointer == null : pointer != null);
							    if (negate) {
								    condition = !condition;
							    }
							    const auto follow = [count, certified](val<int64_t*> current) -> val<int64_t> {
								    for (val<uintptr_t> index = certified ? cacheLiteral<uintptr_t {0}>()
								                                          : val<uintptr_t>(0);
								         index < count; ++index) {
									    current = certified ? current + cacheLiteral<uintptr_t {1}>() : current + 1;
								    }
								    return *current;
							    };
							    if (equal == negate) {
								    if (condition) {
									    return follow(pointer);
								    }
							    } else {
								    if (condition) {
									    return certified ? cacheLiteral<int64_t {-1}>() : val<int64_t>(-1);
								    }
								    return follow(pointer);
							    }
							    return certified ? cacheLiteral<int64_t {-1}>() : val<int64_t>(-1);
						    });
					}
				}
			}
			module.registerFunction<val<int64_t>(val<bool>, val<uintptr_t>)>(
			    "guarded_header", [state, certified](val<bool> available, val<uintptr_t> count) -> val<int64_t> {
				    auto pointer = select(available, state.get(), val<int64_t*>(nullptr));
				    if (pointer == nullptr) {
					    return certified ? cacheLiteral<int64_t {-1}>() : val<int64_t>(-1);
				    }
				    val<int64_t> sum = certified ? cacheLiteral<int64_t {0}>() : val<int64_t>(0);
				    for (val<uintptr_t> index = certified ? cacheLiteral<uintptr_t {0}>() : val<uintptr_t>(0);
				         index < count; ++index) {
					    sum += pointer[index];
				    }
				    return sum;
			    });
			module.registerFunction<val<int64_t>(val<bool>, val<uintptr_t>)>(
			    "null_backedge", [state, certified](val<bool> available, val<uintptr_t> count) -> val<int64_t> {
				    auto pointer = select(available, state.get(), val<int64_t*>(nullptr));
				    val<int64_t> sum = certified ? cacheLiteral<int64_t {0}>() : val<int64_t>(0);
				    for (val<uintptr_t> index = certified ? cacheLiteral<uintptr_t {0}>() : val<uintptr_t>(0);
				         index < count; ++index) {
					    if (pointer != nullptr) {
						    sum += certified ? pointer[cacheLiteral<uintptr_t {0}>()] : pointer[0];
						    pointer = certified ? pointer + cacheLiteral<uintptr_t {1}>() : pointer + 1;
					    }
					    if (certified ? index == cacheLiteral<uintptr_t {1}>() : index == 1) {
						    pointer = nullptr;
					    }
				    }
				    return sum;
			    });
			const auto priorTraces = traces;
			auto compiled = module.compile();
			INFO(compiled.getStatistics()->toString());
			const bool hit = certified && iteration != 0;
			REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") ==
			        (certified ? (iteration == 2 ? "invalid_object" : "none") : "non_relocatable_pointer"));
			REQUIRE(bindingStat<int64_t>(compiled, "cache.tracingRan") == (hit ? 0 : 1));
			REQUIRE((hit ? traces == priorTraces : traces > priorTraces));
			REQUIRE(bindingStat<std::string>(compiled, iteration == 2 ? "cache.mlir" : "cache.object") ==
			        (certified ? (iteration == 0 ? "written" : "hit") : "miss"));
			if (!hit) {
				REQUIRE(bindingStat<int64_t>(compiled, "cache.scalarCertificate") == (certified ? 1 : 0));
				if (certified) {
					REQUIRE(bindingStat<std::string>(compiled, "cache.scalarRejection").empty());
				} else {
					REQUIRE_THAT(bindingStat<std::string>(compiled, "cache.scalarRejection"),
					             Catch::Matchers::ContainsSubstring("uncertified_scalar"));
					for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
						REQUIRE(bindingArtifact(cache.path(), extension).empty());
					}
				}
			}
			const auto currentKey = bindingStat<std::string>(compiled, "cache.key");
			if (certified) {
				REQUIRE(currentKey == bindingArtifact(cache.path(), ".manifest").stem().string());
			}
			if (iteration == 0) {
				key = currentKey;
			} else {
				REQUIRE(currentKey == key);
			}
			const auto compiledTraces = traces;
			REQUIRE(compiled.getFunction<int64_t(uintptr_t)>("bound_offset")(1) == values[2]);
			REQUIRE(compiled.getFunction<int64_t(uintptr_t)>("null_iterator")(2) == values[1]);
			REQUIRE(compiled.getFunction<int64_t()>("null_cache_merge")() == values[2]);
			for (const bool equal : {false, true}) {
				for (const bool negate : {false, true}) {
					for (const bool nullOnLeft : {false, true}) {
						const auto name =
						    "null_guard_" + std::to_string(equal) + std::to_string(negate) + std::to_string(nullOnLeft);
						CAPTURE(name);
						auto execute = compiled.getFunction<int64_t(bool, uintptr_t)>(name);
						for (uintptr_t count = 0; count < 8; ++count) {
							REQUIRE(execute(true, count) == values[count & 3]);
							REQUIRE(execute(false, count) == -1);
						}
					}
				}
			}
			for (const bool available : {false, true}) {
				for (const uintptr_t count : {0, 1, 2, 4}) {
					int64_t sum = available ? 0 : -1;
					if (available) {
						for (uintptr_t index = 0; index < count; ++index) {
							sum += values[index];
						}
					}
					REQUIRE(compiled.getFunction<int64_t(bool, uintptr_t)>("guarded_header")(available, count) == sum);
					const auto expected = !available || count == 0 ? 0 : values[0] + (count > 1 ? values[1] : 0);
					REQUIRE(compiled.getFunction<int64_t(bool, uintptr_t)>("null_backedge")(available, count) ==
					        expected);
				}
			}
			for (auto* runtime : {first->data(), second->data()}) {
				const auto integer = reinterpret_cast<uintptr_t>(runtime);
				for (const auto* suffix : {"_false", "_true"}) {
					REQUIRE(compiled.getFunction<int64_t()>(std::string("null_select") + suffix)() == values[1]);
					REQUIRE(compiled.getFunction<int64_t(uintptr_t)>(std::string("integer_select") + suffix)(integer) ==
					        runtime[1]);
					for (const bool input : {false, true}) {
						REQUIRE(compiled.getFunction<bool(bool)>(std::string("bool_select") + suffix)(input) == input);
					}
				}
				REQUIRE(compiled.getFunction<int64_t(int64_t*, uintptr_t)>("pointer_offset")(runtime, 2) == runtime[2]);
				REQUIRE(compiled.getFunction<int64_t(uintptr_t)>("integer_argument")(integer) == runtime[0]);
				REQUIRE(compiled.getFunction<int64_t(int64_t*, uintptr_t)>("callee_pointer")(runtime, 2) == runtime[2]);
				REQUIRE(compiled.getFunction<int64_t(int64_t*, uintptr_t)>("indirect_pointer")(runtime, 2) ==
				        runtime[2]);
				REQUIRE(compiled.getFunction<int64_t(int64_t*, double)>("numeric_offset")(runtime, 2.0) == runtime[2]);
				for (const bool select : {false, true}) {
					REQUIRE(compiled.getFunction<int64_t(int64_t*, bool, uintptr_t)>("selected_pointer")(
					            runtime, select, 2) == runtime[2]);
				}
				REQUIRE(compiled.getFunction<int64_t(uintptr_t, uintptr_t)>("integer_arguments")(
				            integer, 2 * sizeof(int64_t)) == runtime[2]);
				REQUIRE(compiled.getFunction<int64_t(int64_t*, uintptr_t)>("integer_induction")(runtime, 2) ==
				        runtime[2]);
				REQUIRE(compiled.getFunction<int64_t(int64_t*, bool, uintptr_t)>("merged_pointer")(runtime, false, 2) ==
				        runtime[2]);
				REQUIRE(compiled.getFunction<int64_t(int64_t*, bool, uintptr_t)>("merged_pointer")(runtime, true, 2) ==
				        values[2]);
				REQUIRE(compiled.getFunction<int64_t(int64_t**, uintptr_t)>("table_pointer")(&runtime, 2) ==
				        runtime[2]);
				for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
					if (!certified) {
						REQUIRE(bindingArtifact(cache.path(), extension).empty());
						continue;
					}
					std::ifstream input(bindingArtifact(cache.path(), extension), std::ios::binary);
					REQUIRE(input.good());
					const std::string bytes(std::istreambuf_iterator<char> {input}, std::istreambuf_iterator<char> {});
					const std::string raw(reinterpret_cast<const char*>(&integer), sizeof(integer));
					REQUIRE(bytes.find(raw) == std::string::npos);
					REQUIRE(bytes.find(std::to_string(integer)) == std::string::npos);
				}
			}
			REQUIRE(traces == compiledTraces);
			if (certified && iteration == 1) {
				REQUIRE(std::filesystem::remove(bindingArtifact(cache.path(), ".o")));
			}
		}
	}
}

#ifdef __linux__
TEST_CASE("RuntimeBindings cache rebinds ASLR addresses across fresh exec processes", "[runtime-bindings][cache]") {
	static constexpr auto CHILD_DIRECTORY = "NAUTILUS_BINDING_CHILD_DIRECTORY";
	static constexpr auto CHILD_MODE = "NAUTILUS_BINDING_CHILD_MODE";
	if (const auto* directory = std::getenv(CHILD_DIRECTORY)) {
		const auto personality = ::personality(0xffffffffUL);
		REQUIRE(personality != -1);
		REQUIRE((personality & ADDR_NO_RANDOMIZE) == 0);
		const auto* mode = std::getenv(CHILD_MODE);
		REQUIRE(mode != nullptr);
		const bool warm = std::string_view(mode) == "warm";
		REQUIRE((warm || std::string_view(mode) == "cold"));
		if (warm) {
			createBindingHistory();
			NautilusEngine unrelated(bindingOptions("mlir"));
			auto module = unrelated.createModule();
			module.registerFunction<val<int64_t>(val<int64_t>)>("history",
			                                                    [](val<int64_t> value) { return value + 99; });
			auto compiled = module.compile();
			REQUIRE(compiled.getFunction<int64_t(int64_t)>("history")(1) == 100);
		}
		auto storage = std::make_unique<std::array<int64_t, 32>>();
		auto* left = &(*storage)[warm ? 17 : 0];
		auto* right = &(*storage)[warm ? 23 : 1];
		*left = warm ? 100 : 10;
		*right = warm ? 211 : 31;
		std::atomic<int> traces {0};
		auto module = compileBindingPair(bindingCacheOptions(directory, "fresh-exec-binding-module-v1"), left, right,
		                                 warm, traces);
		REQUIRE(bindingStat<std::string>(module, "cache.object") == (warm ? "hit" : "written"));
		REQUIRE(bindingStat<int64_t>(module, "cache.tracingRan") == (warm ? 0 : 1));
		REQUIRE((warm ? traces.load() == 0 : traces.load() > 0));
		requireBindingAddresses(module, left, right);
		REQUIRE(module.getFunction<int64_t(int64_t)>("execute")(7) == (warm ? 211 : 31));
		REQUIRE(*left == (warm ? 107 : 17));
		std::ofstream report(std::filesystem::path(directory) / (std::string(mode) + ".addresses"));
		report << reinterpret_cast<uintptr_t>(left) << ' ' << reinterpret_cast<uintptr_t>(right) << '\n';
		report << module.getStatistics()->toString();
		report.close();
		REQUIRE(report.good());
		return;
	}

	BindingCacheDirectory cache;
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
			        "RuntimeBindings cache rebinds ASLR addresses across fresh exec processes", "--reporter", "compact",
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
	std::array<uintptr_t, 2> cold {}, warm {};
	std::ifstream coldReport(cache.path() / "cold.addresses");
	std::ifstream warmReport(cache.path() / "warm.addresses");
	REQUIRE(static_cast<bool>(coldReport >> cold[0] >> cold[1]));
	REQUIRE(static_cast<bool>(warmReport >> warm[0] >> warm[1]));
	CAPTURE(cold[0], cold[1], warm[0], warm[1]);
	REQUIRE(cold[0] != warm[0]);
	REQUIRE(cold[1] != warm[1]);
	for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
		std::ifstream input(bindingArtifact(cache.path(), extension), std::ios::binary);
		REQUIRE(input.good());
		const std::string bytes(std::istreambuf_iterator<char> {input}, std::istreambuf_iterator<char> {});
		for (const auto address : {cold[0], cold[1], warm[0], warm[1]}) {
			const std::string raw(reinterpret_cast<const char*>(&address), sizeof(address));
			REQUIRE(bytes.find(raw) == std::string::npos);
			REQUIRE(bytes.find(std::to_string(address)) == std::string::npos);
		}
	}
}
#endif

TEST_CASE("RuntimeBindings keeps shared-engine concurrent cache loads and wrapper counters independent",
          "[runtime-bindings][cache][concurrent]") {
	BindingCacheDirectory cache;
	const auto options = bindingCacheOptions(cache.path(), "shared-engine-binding-modules-v1");
	std::array<int64_t, 10> values {5, 101, 19, 211, 23, 307, 29, 401, 31, 503};
	std::array<std::atomic<int>, 5> wrappers {};
	std::array<std::optional<CompiledModule>, 5> modules;
	std::array<std::exception_ptr, 4> errors;
	{
		NautilusEngine engine(cache::createCompiler(options), options);
		modules[0].emplace(compileBindingPair(engine, &values[0], &values[1], false, wrappers[0]));
		REQUIRE(bindingStat<std::string>(*modules[0], "cache.object") == "written");
		REQUIRE(wrappers[0].load() > 0);
		std::barrier synchronize(4);
		std::array<std::thread, 4> workers;
		for (std::size_t index = 0; index < workers.size(); ++index) {
			workers[index] = std::thread([&, index] {
				synchronize.arrive_and_wait();
				try {
					const auto moduleIndex = index + 1;
					modules[moduleIndex].emplace(compileBindingPair(engine, &values[moduleIndex * 2],
					                                                &values[moduleIndex * 2 + 1], index % 2 != 0,
					                                                wrappers[moduleIndex]));
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
	const auto key = bindingStat<std::string>(*modules[0], "cache.key");
	const auto coldWrappers = wrappers[0].load();
	for (std::size_t index = 0; index < modules.size(); ++index) {
		CAPTURE(index);
		REQUIRE(modules[index].has_value());
		auto& module = *modules[index];
		REQUIRE(bindingStat<std::string>(module, "cache.key") == key);
		if (index != 0) {
			REQUIRE(bindingStat<std::string>(module, "cache.object") == "hit");
			REQUIRE(wrappers[index].load() == 0);
			REQUIRE(module.getExecutable() != modules[0]->getExecutable());
		}
		requireBindingAddresses(module, &values[index * 2], &values[index * 2 + 1]);
		const auto before = values;
		REQUIRE(module.getFunction<int64_t(int64_t)>("execute")(7) == values[index * 2 + 1]);
		for (std::size_t slot = 0; slot < values.size(); ++slot) {
			REQUIRE(values[slot] == before[slot] + (slot == index * 2 ? 7 : 0));
		}
	}
	REQUIRE(wrappers[0].load() == coldWrappers);
}

TEST_CASE("RuntimeBindings cannot bypass an unknown code-generating backend hook on an existing cache key",
          "[runtime-bindings][cache][guard]") {
	BindingCacheDirectory cache;
	const auto options = bindingCacheOptions(cache.path(), "bound-active-backend-hook-v1");
	std::array<int64_t, 6> values {5, 101, 19, 211, 23, 307};
	std::array<std::atomic<int>, 3> wrappers {};
	auto cold = compileBindingPair(options, &values[0], &values[1], false, wrappers[0]);
	INFO(cold.getStatistics()->toString());
	REQUIRE(bindingStat<std::string>(cold, "cache.object") == "written");
	struct ScopedHooks {
		compiler::mlir::LLVMBackendHooks saved = compiler::mlir::getLLVMBackendHooks();
		~ScopedHooks() {
			compiler::mlir::getLLVMBackendHooks() = std::move(saved);
		}
	};
	int transforms = 0;
	{
		ScopedHooks guard;
		compiler::mlir::getLLVMBackendHooks().preOptModuleTransform = [&](llvm::Module& module) {
			REQUIRE(module.getFunction("execute") != nullptr);
			++transforms;
		};
		auto declined = compileBindingPair(options, &values[2], &values[3], true, wrappers[1]);
		REQUIRE(bindingStat<std::string>(declined, "cache.fallback") == "active_backend_hooks_unsupported");
		REQUIRE(bindingStat<std::string>(declined, "cache.object") == "not_used");
		REQUIRE(bindingStat<std::string>(declined, "cache.mlir") == "not_used");
		REQUIRE(bindingStat<int64_t>(declined, "cache.tracingRan") == 1);
		REQUIRE(wrappers[1].load() > 0);
		requireBindingAddresses(declined, &values[2], &values[3]);
		REQUIRE(declined.getFunction<int64_t(int64_t)>("execute")(7) == 211);
		REQUIRE(transforms == 0);
		REQUIRE(values[2] == 26);
		REQUIRE(values[0] == 5);
		auto selectedOptions = options;
		selectedOptions.setOption("mlir.inline_invoke_calls", true);
		std::atomic<int> selectedWrappers {0};
		int64_t selectedLeft = 31, selectedRight = 503;
		auto selected = compileBindingPair(selectedOptions, &selectedLeft, &selectedRight, false, selectedWrappers);
		REQUIRE(bindingStat<std::string>(selected, "cache.fallback") == "inline_invoke_calls_unsupported");
		REQUIRE(bindingStat<std::string>(selected, "cache.object") == "not_used");
		REQUIRE(bindingStat<int64_t>(selected, "cache.tracingRan") == 1);
		REQUIRE(selectedWrappers.load() > 0);
		requireBindingAddresses(selected, &selectedLeft, &selectedRight);
		REQUIRE(selected.getFunction<int64_t(int64_t)>("execute")(9) == 503);
		REQUIRE(selectedLeft == 40);
		REQUIRE(transforms > 0);
	}
	auto warm = compileBindingPair(options, &values[4], &values[5], false, wrappers[2]);
	REQUIRE(bindingStat<std::string>(warm, "cache.object") == "hit");
	REQUIRE(wrappers[2].load() == 0);
	requireBindingAddresses(warm, &values[4], &values[5]);
	REQUIRE(warm.getFunction<int64_t(int64_t)>("execute")(11) == 307);
	REQUIRE(values[4] == 34);
	REQUIRE(values[2] == 26);
	REQUIRE(values[0] == 5);
}

} // namespace nautilus::engine
