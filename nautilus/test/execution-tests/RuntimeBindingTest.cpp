#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/RuntimeBinding.hpp"
#include "nautilus/config.hpp"
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
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
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

template <typename T>
concept CacheInvariantScalar = requires(T&& value) { cacheInvariant(std::forward<T>(value)); };

std::vector<std::string> bindingBackends(bool includeInterpreter = true) {
	std::vector<std::string> result;
	if (includeInterpreter) {
		result.emplace_back("interpreter");
	}
#if defined(ENABLE_TRACING) && defined(ENABLE_C_BACKEND)
	result.emplace_back("cpp");
#endif
#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
	result.emplace_back("mlir");
#endif
#if defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND)
	result.emplace_back("bc");
#endif
#if defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND)
	result.emplace_back("tbc");
#endif
#if defined(ENABLE_TRACING) && defined(ENABLE_ASMJIT_BACKEND)
	result.emplace_back("asmjit");
#endif
	return result;
}

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

auto bindingLoop(RuntimeBinding<int64_t> left, RuntimeBinding<int64_t> right) {
	return [left, right](val<int64_t> count, val<int64_t> split) -> val<int64_t> {
		val<int64_t> total = 0;
		for (val<int64_t> index = 0; index < count; ++index) {
			auto address = left.get();
			if (index >= split) {
				address = right.get();
			}
			*address += index + 1;
			total += *address;
		}
		return total;
	};
}

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
struct CertifiedBindingState {
	int64_t total = 0;
	int64_t calls = 0;
	int64_t cleanups = 0;
	int64_t live = 0;
	uint8_t byte = 0;
	bool bit = false;

	bool operator==(const CertifiedBindingState&) const = default;
};

struct CertifiedBindingCleanup {
	CertifiedBindingState* state;

	explicit CertifiedBindingCleanup(CertifiedBindingState* state) noexcept : state(state) {
		++state->live;
	}

	~CertifiedBindingCleanup() noexcept {
		--state->live;
		++state->cleanups;
	}
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
#ifndef __linux__
	SKIP("Persistent module caching requires Linux ELF build IDs");
#endif
	auto options = bindingOptions("mlir");
	options.setOption("engine.Blob.CacheDir", directory.string());
	options.setOption("engine.Blob.CacheKey", key);
	return options;
}

template <typename T>
T bindingStat(const CompiledModule& module, const std::string& key) {
	const auto stats = module.getStatistics();
	REQUIRE(stats != nullptr);
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

CompiledModule compileBindingPair(const Options& options, int64_t* left, int64_t* right, bool reverseOrder,
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

	NautilusEngine engine(options);
	auto module = engine.createModule();
	module.setRuntimeBindings(bindings);
	auto execute = [leftBinding, rightBinding, &traces](val<int64_t> delta) -> val<int64_t> {
		traces.fetch_add(1);
		auto leftAddress = leftBinding.get();
		*leftAddress += delta;
		return *rightBinding.get();
	};
	auto sameAddress = [leftBinding, rightBinding] {
		return leftBinding.get() == rightBinding.get();
	};
	if (reverseOrder) {
		module.registerFunction<val<bool>()>("same_address", sameAddress);
		module.registerFunction<val<int64_t>(val<int64_t>)>("execute", execute);
	} else {
		module.registerFunction<val<int64_t>(val<int64_t>)>("execute", execute);
		module.registerFunction<val<bool>()>("same_address", sameAddress);
	}
	module.registerFunction<val<int64_t*>()>("left_address", [leftBinding] { return leftBinding.get(); });
	module.registerFunction<val<int64_t*>()>("right_address", [rightBinding] { return rightBinding.get(); });
	return module.compile();
}

void requireBindingAddresses(CompiledModule& module, int64_t* left, int64_t* right) {
	REQUIRE(module.getFunction<int64_t*()>("left_address")() == left);
	REQUIRE(module.getFunction<int64_t*()>("right_address")() == right);
	REQUIRE(module.getFunction<bool()>("same_address")() == (left == right));
}
#endif

} // namespace

TEST_CASE("Cache-invariant scalar factories preserve nontracing values and types", "[runtime-bindings][cache]") {
	enum class ScalarEnum : int32_t { Value = 7 };
	static_assert(CacheInvariantScalar<int32_t>);
	static_assert(CacheInvariantScalar<const int64_t&>);
	static_assert(CacheInvariantScalar<bool&>);
	static_assert(CacheInvariantScalar<double>);
	static_assert(!CacheInvariantScalar<int64_t*>);
	static_assert(!CacheInvariantScalar<std::nullptr_t>);
	static_assert(!CacheInvariantScalar<ScalarEnum>);
	static_assert(!CacheInvariantScalar<val<int64_t>>);
	static_assert(!CacheInvariantScalar<static_val<int64_t>>);
	static_assert(std::is_same_v<decltype(cacheLiteral<true>()), val<bool>>);
	static_assert(std::is_same_v<decltype(cacheLiteral<int32_t {7}>()), val<int32_t>>);
	static_assert(std::is_same_v<decltype(cacheInvariant(std::declval<const int64_t&>())), val<int64_t>>);
	const auto check = []<typename T>(T input) {
		const T constant = input;
		auto copied = cacheInvariant(constant);
		auto moved = cacheInvariant(std::move(input));
		static_assert(std::is_same_v<decltype(copied), val<T>>);
		REQUIRE(nautilus::details::RawValueResolver<T>::getRawValue(copied) == constant);
		REQUIRE(nautilus::details::RawValueResolver<T>::getRawValue(moved) == constant);
	};
	check(int8_t {-7});
	check(uint8_t {251});
	check(int16_t {-319});
	check(uint16_t {65000});
	check(int32_t {-123456});
	check(uint32_t {3456789012U});
	check(int64_t {-1234567890123});
	check(uint64_t {123456789012345});
	check(float {1.25});
	check(double {-2.5});
	check(false);
	check(true);
	REQUIRE(nautilus::details::RawValueResolver<int64_t>::getRawValue(cacheLiteral<int64_t {-17}>()) == -17);
	REQUIRE(nautilus::details::RawValueResolver<bool>::getRawValue(cacheLiteral<true>()));
	REQUIRE_FALSE(nautilus::details::RawValueResolver<bool>::getRawValue(cacheLiteral<false>()));
	REQUIRE(nautilus::details::RawValueResolver<int64_t>::getRawValue(val<int64_t> {}) == 0);
	REQUIRE_FALSE(nautilus::details::RawValueResolver<bool>::getRawValue(val<bool> {}));
	auto number = cacheLiteral<int64_t {7}>();
	REQUIRE(nautilus::details::RawValueResolver<int64_t>::getRawValue(++number) == 8);
	REQUIRE(nautilus::details::RawValueResolver<int64_t>::getRawValue(number--) == 8);
	REQUIRE(nautilus::details::RawValueResolver<int64_t>::getRawValue(-number) == -7);
}

TEST_CASE("RuntimeBindings validates registrations and preserves pointer types", "[runtime-bindings]") {
	RuntimeBinding<int64_t> unbound;
	REQUIRE_FALSE(unbound.isBound());
	REQUIRE_THROWS_AS(unbound.get(), std::logic_error);

	int64_t value = 17;
	const int64_t readOnly = 29;
	RuntimeBindings bindings;
	REQUIRE_THROWS_AS(bindings.bind<int64_t>("", &value), std::invalid_argument);
	REQUIRE_THROWS_AS(bindings.bind<int64_t>("state", nullptr), std::invalid_argument);
	auto state = bindings.bind<int64_t>("state", &value);
	auto constant = bindings.bind<const int64_t>("constant", &readOnly);
	static_assert(std::is_same_v<decltype(state.get()), val<int64_t*>>);
	static_assert(std::is_same_v<decltype(constant.get()), val<const int64_t*>>);
	REQUIRE(state.isBound());
	REQUIRE(constant.isBound());
	REQUIRE_THROWS_AS(bindings.bind<int64_t>("state", &value), std::invalid_argument);
	REQUIRE_THROWS_AS(bindings.bind<const int64_t>("state", &readOnly), std::invalid_argument);

	auto alias = bindings.bind<int64_t>("alias", &value);
	REQUIRE(nautilus::details::RawValueResolver<int64_t*>::getRawValue(state.get()) == &value);
	REQUIRE(nautilus::details::RawValueResolver<int64_t*>::getRawValue(alias.get()) == &value);
	REQUIRE(nautilus::details::RawValueResolver<const int64_t*>::getRawValue(constant.get()) == &readOnly);
	*state.get() = int64_t {41};
	REQUIRE(value == 41);
	value = 73;
	val<int64_t> loaded = *alias.get();
	REQUIRE(nautilus::details::RawValueResolver<int64_t>::getRawValue(loaded) == 73);
}

TEST_CASE("RuntimeBindings supports mutable state without persistent caching", "[runtime-bindings]") {
	for (const auto& backend : bindingBackends()) {
		DYNAMIC_SECTION(backend) {
			int64_t value = 10;
			const int64_t factor = 3;
			RuntimeBindings bindings;
			auto state = bindings.bind<int64_t>("state", &value);
			auto scale = bindings.bind<const int64_t>("factor", &factor);
			NautilusEngine engine(bindingOptions(backend));
			auto module = engine.createModule();
			module.setRuntimeBindings(bindings);
			module.registerFunction<val<int64_t>(val<int64_t>)>("execute",
			                                                    [state, scale](val<int64_t> delta) -> val<int64_t> {
				                                                    auto address = state.get();
				                                                    val<int64_t> multiplier = *scale.get();
				                                                    *address += delta * multiplier;
				                                                    return *address;
			                                                    });
			auto compiled = module.compile();
			REQUIRE(value == 10);
			auto execute = compiled.getFunction<int64_t(int64_t)>("execute");
			REQUIRE(execute(4) == 22);
			REQUIRE(value == 22);
			value = 100;
			REQUIRE(execute(-2) == 94);
			REQUIRE(value == 94);
		}
	}
}

TEST_CASE("RuntimeBindings survives loop backedges and branch merges", "[runtime-bindings]") {
	for (const auto& backend : bindingBackends()) {
		DYNAMIC_SECTION(backend) {
			int64_t left = 10, right = 100;
			RuntimeBindings bindings;
			auto leftBinding = bindings.bind<int64_t>("left", &left);
			auto rightBinding = bindings.bind<int64_t>("right", &right);
			NautilusEngine engine(bindingOptions(backend));
			auto module = engine.createModule();
			module.setRuntimeBindings(bindings);
			module.registerFunction<val<int64_t>(val<int64_t>, val<int64_t>)>("execute",
			                                                                  bindingLoop(leftBinding, rightBinding));
			auto compiled = module.compile();
			REQUIRE(left == 10);
			REQUIRE(right == 100);
			auto execute = compiled.getFunction<int64_t(int64_t, int64_t)>("execute");
			for (const auto& [count, split] :
			     std::array<std::pair<int64_t, int64_t>, 6> {{{0, 0}, {1, 1}, {5, 2}, {6, 0}, {7, 10}, {8, 4}}}) {
				CAPTURE(count, split);
				auto expectedLeft = left;
				auto expectedRight = right;
				int64_t expectedTotal = 0;
				for (int64_t index = 0; index < count; ++index) {
					auto& selected = index >= split ? expectedRight : expectedLeft;
					selected += index + 1;
					expectedTotal += selected;
				}
				REQUIRE(execute(count, split) == expectedTotal);
				REQUIRE(left == expectedLeft);
				REQUIRE(right == expectedRight);
			}
		}
	}
}

TEST_CASE("RuntimeBindings remains available in nested tracing regions", "[runtime-bindings]") {
	for (const auto& backend : bindingBackends()) {
		DYNAMIC_SECTION(backend) {
			int64_t value = 10;
			RuntimeBindings bindings;
			auto state = bindings.bind<int64_t>("region/state", &value);
			NautilusEngine engine(bindingOptions(backend));
			auto module = engine.createModule();
			module.setRuntimeBindings(bindings);
			module.registerFunction<val<int64_t>(val<int64_t>)>("execute", [state](val<int64_t> count) -> val<int64_t> {
				region("outer", [&] {
					region("inner", [&] {
						for (val<int64_t> index = 0; index < count; ++index) {
							*state.get() += index + 1;
						}
					});
				});
				return *state.get();
			});
			auto compiled = module.compile();
			REQUIRE(value == 10);
			auto execute = compiled.getFunction<int64_t(int64_t)>("execute");
			REQUIRE(execute(4) == 20);
			REQUIRE(value == 20);
			REQUIRE(execute(0) == 20);
			REQUIRE(execute(2) == 23);
			REQUIRE(value == 23);
		}
	}
}

TEST_CASE("RuntimeBindings is available to nested Nautilus functions", "[runtime-bindings]") {
	for (const auto& backend : bindingBackends()) {
		DYNAMIC_SECTION(backend) {
			int64_t value = 10;
			RuntimeBindings bindings;
			auto state = bindings.bind<int64_t>("nested/state", &value);
			NautilusFunction update {"update_bound_state", [state](val<int64_t> delta) -> val<int64_t> {
				                         auto address = state.get();
				                         *address += delta;
				                         return *address;
			                         }};
			NautilusFunction twice {"update_bound_state_twice", [&update](val<int64_t> delta) {
				                        update(delta);
				                        return update(delta + 1);
			                        }};
			NautilusEngine engine(bindingOptions(backend));
			auto module = engine.createModule();
			module.setRuntimeBindings(bindings);
			module.registerFunction<val<int64_t>(val<int64_t>)>("execute",
			                                                    [&twice](val<int64_t> delta) { return twice(delta); });
			auto compiled = module.compile();
			REQUIRE(value == 10);
			auto execute = compiled.getFunction<int64_t(int64_t)>("execute");
			REQUIRE(execute(3) == 17);
			REQUIRE(value == 17);
			value = 100;
			REQUIRE(execute(-2) == 97);
			REQUIRE(value == 97);
		}
	}
}

#ifdef ENABLE_TRACING
TEST_CASE("Cache-invariant scalar origins survive nested regions and trace cloning", "[runtime-bindings][cache]") {
	auto wrapper = details::createFunctionWrapper([] {
		val<double> result;
		region("scalar origins", [&] {
			region("nested scalar origins", [&] {
				auto integer = cacheLiteral<int64_t {7}>();
				val<int64_t> ordinaryInteger = 7;
				auto boolean = cacheLiteral<true>();
				val<bool> ordinaryBoolean = true;
				auto floating = cacheInvariant(2.5);
				val<double> ordinaryFloating = 2.5;
				result = select(boolean && ordinaryBoolean,
				                static_cast<val<double>>(integer + ordinaryInteger) + floating, ordinaryFloating);
			});
		});
		return result;
	});
	common::Arena arena, clonedArena;
	auto trace = tracing::TraceContext::trace(wrapper, Options {}, arena);
	REQUIRE(trace != nullptr);
	std::array<std::array<std::size_t, 2>, 3> origins {};
	for (const auto* block : trace->getBlocks()) {
		for (const auto* operation : block->operations) {
			if (operation->op != tracing::Op::CONST || operation->regionIndex == tracing::NO_REGION) {
				continue;
			}
			const auto& literal = std::get<ConstantLiteral>(operation->input[0]);
			std::size_t type = 0;
			if (operation->resultType == Type::i64) {
				REQUIRE(std::get<int64_t>(literal) == 7);
			} else if (operation->resultType == Type::b) {
				type = 1;
				REQUIRE(std::get<bool>(literal));
			} else {
				type = 2;
				REQUIRE(operation->resultType == Type::f64);
				REQUIRE(std::get<double>(literal) == 2.5);
			}
			++origins[type][operation->constantOrigin == ConstantOrigin::CacheInvariant ? 1 : 0];
			auto* clone = tracing::cloneTraceOp(clonedArena, *operation);
			REQUIRE(clone != operation);
			REQUIRE(clone->input.data() != operation->input.data());
			REQUIRE(clone->constantOrigin == operation->constantOrigin);
			REQUIRE(clone->regionIndex == operation->regionIndex);
			REQUIRE(clone->resultType == operation->resultType);
			REQUIRE(std::get<ConstantLiteral>(clone->input[0]) == literal);
		}
	}
	for (const auto& counts : origins) {
		REQUIRE(counts[0] > 0);
		REQUIRE(counts[1] > 0);
	}
	REQUIRE_FALSE(tracing::inTracer());
}

TEST_CASE("Cache-invariant scalar replay disagreement never upgrades an ordinary constant",
          "[runtime-bindings][cache]") {
	for (const bool initiallyCertified : {false, true}) {
		CAPTURE(initiallyCertified);
		int iterations = 0;
		auto wrapper = details::createFunctionWrapper([&](val<bool> condition) {
			++iterations;
			const auto origin =
			    (iterations == 1) == initiallyCertified ? ConstantOrigin::CacheInvariant : ConstantOrigin::Unspecified;
			auto ref = tracing::traceConstant(int64_t {7}, origin);
			val<int64_t> value(ref);
			if (condition) {
				return value;
			}
			return -value;
		});
		common::Arena arena;
		auto trace = tracing::TraceContext::trace(wrapper, Options {}, arena);
		REQUIRE(iterations >= 2);
		std::size_t constants = 0;
		for (const auto* block : trace->getBlocks()) {
			for (const auto* operation : block->operations) {
				if (operation->op == tracing::Op::CONST && operation->resultType == Type::i64 &&
				    std::get<int64_t>(std::get<ConstantLiteral>(operation->input[0])) == 7) {
					++constants;
					REQUIRE(operation->constantOrigin == ConstantOrigin::Unspecified);
				}
			}
		}
		REQUIRE(constants > 0);
		REQUIRE_FALSE(tracing::inTracer());
	}
}

TEST_CASE("RuntimeBindings validates standalone trace entry points from options", "[runtime-bindings]") {
	const auto trace = tracing::TraceContext::trace;
	int64_t left = 10, right = 100;
	RuntimeBindings bindings;
	auto leftBinding = bindings.bind<int64_t>("left", &left);
	auto rightBinding = bindings.bind<int64_t>("right", &right);
	Options options;
	options.setRuntimeBindings(bindings);
	auto wrapper = details::createFunctionWrapper(bindingLoop(leftBinding, rightBinding));
	common::Arena arena;
	auto executionTrace = trace(wrapper, options, arena);
	REQUIRE(executionTrace != nullptr);
	REQUIRE_FALSE(tracing::inTracer());
	REQUIRE(left == 10);
	REQUIRE(right == 100);
	bool foundLeft = false, foundRight = false;
	for (const auto* block : executionTrace->getBlocks()) {
		for (const auto* operation : block->operations) {
			if (operation->op != tracing::Op::RUNTIME_BINDING) {
				continue;
			}
			REQUIRE(operation->resultType == Type::ptr);
			const auto* entry = std::get<const runtime_binding::Entry*>(operation->input[0]);
			REQUIRE(entry != nullptr);
			const auto registered = bindings.entries().find(entry->identity);
			REQUIRE(registered != bindings.entries().end());
			REQUIRE(entry->type == registered->second->type);
			REQUIRE(entry->symbol == registered->second->symbol);
			REQUIRE(entry->address == registered->second->address);
			foundLeft |= entry->identity == "left";
			foundRight |= entry->identity == "right";
		}
	}
	REQUIRE(foundLeft);
	REQUIRE(foundRight);
	auto ssa = tracing::SSACreationPhase().apply(std::shared_ptr<tracing::ExecutionTrace>(std::move(executionTrace)));
	const auto verification = tracing::VerifySSA(*ssa);
	for (const auto& error : verification.errors) {
		INFO(error);
		REQUIRE(verification.valid);
	}
	REQUIRE(verification.valid);

	common::Arena rejectedArena;
	REQUIRE_THROWS_AS(trace(wrapper, Options {}, rejectedArena), std::invalid_argument);
	REQUIRE_FALSE(tracing::inTracer());
	REQUIRE(nautilus::details::RawValueResolver<int64_t*>::getRawValue(leftBinding.get()) == &left);
	RuntimeBindings foreign;
	(void) foreign.bind<int64_t>("left", &left);
	(void) foreign.bind<int64_t>("right", &right);
	Options foreignOptions;
	foreignOptions.setRuntimeBindings(foreign);
	REQUIRE_THROWS_AS(trace(wrapper, foreignOptions, rejectedArena), std::invalid_argument);
	REQUIRE_FALSE(tracing::inTracer());
	REQUIRE_NOTHROW(trace(wrapper, options, rejectedArena));
	REQUIRE_FALSE(tracing::inTracer());
}

TEST_CASE("RuntimeBindings tracing API rejects entries without initialized trace state", "[runtime-bindings]") {
	int64_t value = 42;
	RuntimeBindings bindings;
	(void) bindings.bind<int64_t>("state", &value);
	tracing::TraceContext context;
	{
		tracing::ActiveTracerGuard guard;
		tracing::setActiveTracer(&context);
		REQUIRE_THROWS_AS(tracing::traceRuntimeBinding(*bindings.entries().at("state")), std::invalid_argument);
	}
	REQUIRE_FALSE(tracing::inTracer());
}

TEST_CASE("RuntimeBindings tracing API requires the exact registered entry", "[runtime-bindings]") {
	const auto trace = tracing::TraceContext::trace;
	int64_t value = 42;
	RuntimeBindings bindings, foreign;
	auto state = bindings.bind<int64_t>("state", &value);
	(void) foreign.bind<int64_t>("state", &value);
	const auto& registered = *bindings.entries().at("state");
	const auto& other = *foreign.entries().at("state");
	REQUIRE(&registered != &other);
	REQUIRE(registered.identity == other.identity);
	REQUIRE(registered.type == other.type);
	REQUIRE(registered.symbol == other.symbol);
	REQUIRE(registered.address == other.address);
	Options options;
	options.setRuntimeBindings(bindings);
	const auto copy = registered;
	for (const auto* entry : {&other, &copy}) {
		common::Arena arena;
		std::function<void()> wrapper = [&] {
			tracing::traceRuntimeBinding(*entry);
		};
		REQUIRE_THROWS_AS(trace(wrapper, options, arena), std::invalid_argument);
		REQUIRE_FALSE(tracing::inTracer());
	}
	common::Arena arena;
	std::function<void()> wrapper = [&] {
		auto address = tracing::traceRuntimeBinding(registered);
		tracing::traceReturnOperation(Type::ptr, address);
	};
	REQUIRE_NOTHROW(trace(wrapper, options, arena));
	REQUIRE_FALSE(tracing::inTracer());
	REQUIRE(nautilus::details::RawValueResolver<int64_t*>::getRawValue(state.get()) == &value);
}

TEST_CASE("RuntimeBindings rejects foreign handles while following a recorded prefix", "[runtime-bindings]") {
	const auto trace = tracing::TraceContext::trace;
	for (const bool useHandle : {false, true}) {
		DYNAMIC_SECTION("handle=" << useHandle) {
			int64_t value = 42;
			RuntimeBindings bindings, foreign;
			auto state = bindings.bind<int64_t>("state", &value);
			auto other = foreign.bind<int64_t>("state", &value);
			Options options;
			options.setRuntimeBindings(bindings);
			int iterations = 0;
			auto wrapper = details::createFunctionWrapper([&](val<bool> condition) {
				++iterations;
				auto address = useHandle ? (iterations == 1 ? state : other).get()
				                         : val<int64_t*>(tracing::traceRuntimeBinding(
				                               *(iterations == 1 ? bindings : foreign).entries().at("state")));
				if (condition) {
					return address;
				}
				return address;
			});
			common::Arena arena;
			REQUIRE_THROWS_AS(trace(wrapper, options, arena), std::invalid_argument);
			REQUIRE(iterations == 2);
			REQUIRE_FALSE(tracing::inTracer());
			auto fresh = details::createFunctionWrapper([state] { return state.get(); });
			REQUIRE_NOTHROW(trace(fresh, options, arena));
			REQUIRE_FALSE(tracing::inTracer());
		}
	}
}

TEST_CASE("RuntimeBindings validates handles even while tracing is paused", "[runtime-bindings]") {
	for (const bool useHandle : {false, true}) {
		DYNAMIC_SECTION("handle=" << useHandle) {
			int64_t value = 42;
			RuntimeBindings bindings, foreign;
			auto state = bindings.bind<int64_t>("state", &value);
			auto other = foreign.bind<int64_t>("state", &value);
			Options options;
			options.setRuntimeBindings(bindings);
			bool paused = false;
			std::function<void()> wrapper = [&] {
				val<bool> condition = true;
				while (condition) {
				}
				paused = tracing::traceRuntimeBinding(*bindings.entries().at("state")).type == Type::v;
				if (useHandle) {
					(void) other.get();
				} else {
					tracing::traceRuntimeBinding(*foreign.entries().at("state"));
				}
			};
			common::Arena arena;
			REQUIRE_THROWS_AS(tracing::TraceContext::trace(wrapper, options, arena), std::invalid_argument);
			REQUIRE(paused);
			REQUIRE_FALSE(tracing::inTracer());
			auto fresh = details::createFunctionWrapper([state] { return state.get(); });
			REQUIRE_NOTHROW(tracing::TraceContext::trace(fresh, options, arena));
			REQUIRE_FALSE(tracing::inTracer());
		}
	}
}

TEST_CASE("RuntimeBindings keeps concurrent standalone tracing environments independent", "[runtime-bindings]") {
	const auto trace = tracing::TraceContext::trace;
	std::array<int64_t, 2> values {42, 97};
	std::array<RuntimeBindings, 2> bindings;
	std::array<RuntimeBinding<int64_t>, 2> handles {bindings[0].bind<int64_t>("state", &values[0]),
	                                                bindings[1].bind<int64_t>("state", &values[1])};
	std::array<std::exception_ptr, 2> errors;
	std::array<bool, 2> rejected {}, cleared {}, recorded {};
	std::barrier synchronize(2);
	auto worker = [&](size_t index) {
		try {
			Options options;
			options.setRuntimeBindings(bindings[index]);
			common::Arena arena;
			auto wrapper = details::createFunctionWrapper([&] {
				synchronize.arrive_and_wait();
				try {
					tracing::traceRuntimeBinding(*bindings[1 - index].entries().at("state"));
				} catch (const std::invalid_argument&) {
					rejected[index] = true;
				}
				return handles[index].get();
			});
			auto executionTrace = trace(wrapper, options, arena);
			cleared[index] = !tracing::inTracer();
			for (const auto* block : executionTrace->getBlocks()) {
				for (const auto* operation : block->operations) {
					if (operation->op == tracing::Op::RUNTIME_BINDING) {
						const auto* entry = std::get<const runtime_binding::Entry*>(operation->input[0]);
						recorded[index] = entry->address == &values[index];
					}
				}
			}
		} catch (...) {
			errors[index] = std::current_exception();
		}
	};
	std::thread first(worker, 0), second(worker, 1);
	first.join();
	second.join();
	for (size_t index = 0; index < values.size(); ++index) {
		if (errors[index]) {
			std::rethrow_exception(errors[index]);
		}
		REQUIRE(rejected[index]);
		REQUIRE(cleared[index]);
		REQUIRE(recorded[index]);
	}
	REQUIRE_FALSE(tracing::inTracer());
}
#endif

TEST_CASE("RuntimeBindings allows fallback only when the caller checks isBound", "[runtime-bindings]") {
	for (const auto& backend : bindingBackends()) {
		DYNAMIC_SECTION(backend) {
			RuntimeBinding<int64_t> optional;
			NautilusEngine engine(bindingOptions(backend));
			auto module = engine.createModule();
			module.registerFunction<val<int64_t>(val<int64_t>)>("execute",
			                                                    [optional](val<int64_t> fallback) -> val<int64_t> {
				                                                    if (optional.isBound()) {
					                                                    return *optional.get();
				                                                    }
				                                                    return fallback;
			                                                    });
			auto compiled = module.compile();
			REQUIRE(compiled.getFunction<int64_t(int64_t)>("execute")(71) == 71);
			auto unchecked = engine.createModule();
			unchecked.registerFunction<val<int64_t>()>("execute",
			                                           [optional]() -> val<int64_t> { return *optional.get(); });
			if (backend == "interpreter") {
				auto uncheckedCompiled = unchecked.compile();
				REQUIRE_THROWS_AS(uncheckedCompiled.getFunction<int64_t()>("execute")(), std::logic_error);
			} else {
				REQUIRE_THROWS_AS(unchecked.compile(), std::logic_error);
			}
		}
	}
}

TEST_CASE("RuntimeBindings snapshots outlive registry mutation and destruction", "[runtime-bindings]") {
	for (const auto& backend : bindingBackends()) {
		DYNAMIC_SECTION(backend) {
			int64_t value = 12;
			int64_t replacement = 99;
			NautilusEngine engine(bindingOptions(backend));
			auto module = engine.createModule();
			{
				RuntimeBindings source;
				auto state = source.bind<int64_t>("state", &value);
				auto snapshot = source;
				(void) source.bind<int64_t>("source_only", &replacement);
				REQUIRE_NOTHROW(snapshot.bind<int64_t>("source_only", &value));
				module.setRuntimeBindings(snapshot);
				module.registerFunction<val<int64_t>()>("execute", [state]() -> val<int64_t> { return *state.get(); });
				snapshot = RuntimeBindings {};
				(void) snapshot.bind<int64_t>("state", &replacement);
			}
			auto compiled = module.compile();
			REQUIRE(compiled.getFunction<int64_t()>("execute")() == 12);
			value = 54;
			REQUIRE(compiled.getFunction<int64_t()>("execute")() == 54);
			REQUIRE(replacement == 99);
		}
	}
}

TEST_CASE("RuntimeBindings rejects handles absent from traced module snapshots", "[runtime-bindings]") {
	const auto backends = bindingBackends(false);
	if (backends.empty()) {
		SKIP("No compilation backend available");
	}
	for (const auto& backend : backends) {
		DYNAMIC_SECTION(backend) {
			int64_t value = 42;
			RuntimeBindings source;
			auto state = source.bind<int64_t>("state", &value);
			NautilusEngine engine(bindingOptions(backend));
			auto module = engine.createModule();
			SECTION("no environment") {
			}
			SECTION("empty environment") {
				module.setRuntimeBindings(RuntimeBindings {});
			}
			SECTION("foreign same-name handle even at the same address") {
				RuntimeBindings foreign;
				(void) foreign.bind<int64_t>("state", &value);
				module.setRuntimeBindings(foreign);
			}
			SECTION("registration after module snapshot") {
				module.setRuntimeBindings(source);
				state = source.bind<int64_t>("late", &value);
			}
			SECTION("registration after registry copy") {
				auto copy = source;
				state = source.bind<int64_t>("late", &value);
				module.setRuntimeBindings(copy);
			}
			int traces = 0;
			module.registerFunction<val<int64_t>()>("execute", [state, &traces]() -> val<int64_t> {
				++traces;
				return *state.get();
			});
			REQUIRE_THROWS_AS(module.compile(), std::invalid_argument);
			REQUIRE(traces > 0);
		}
	}
}

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
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
			NautilusEngine engine(options);
			auto module = engine.createModule();
			module.setRuntimeBindings(bindings);
			module.registerFunction<val<int64_t>(val<int64_t>)>("execute", [=, &traces](val<int64_t> delta) {
				++traces;
				val<CertifiedBindingCleanup> cleanup(state.get());
				auto ready = invoke(certifiedBindingReady, state.get(), cacheInvariant(threshold));
				auto count = invoke(certifiedBindingCount, state.get(),
				                    delta * cacheInvariant(scale) + cacheLiteral<int64_t {2}>());
				auto result = count + select(ready, cacheLiteral<int64_t {1}>(), cacheLiteral<int64_t {0}>()) +
				              cacheInvariant(bias) +
				              static_cast<val<int64_t>>(cacheInvariant(1.5) * cacheInvariant(2.0));
				*total.get() = result;
				*byte.get() = static_cast<val<uint8_t>>(result & cacheLiteral<int64_t {255}>());
				*bit.get() = ready && cacheLiteral<true>();
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
			NautilusEngine engine(options);
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
				NautilusEngine engine(options);
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
	std::atomic<int> traces {0};
	int64_t leftA = 5, rightA = 101, leftB = 19, rightB = 211;
	auto first = compileBindingPair(options, &leftA, &rightA, false, traces);
	REQUIRE(bindingStat<std::string>(first, "cache.object") == "written");
	REQUIRE(bindingStat<int64_t>(first, "cache.tracingRan") == 1);
	const auto coldTraces = traces.load();
	REQUIRE(coldTraces > 0);
	createBindingHistory();
	auto second = compileBindingPair(options, &leftB, &rightB, true, traces);
	REQUIRE(bindingStat<std::string>(second, "cache.object") == "hit");
	REQUIRE(bindingStat<int64_t>(second, "cache.tracingRan") == 0);
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
	BindingCacheDirectory cache;
	const auto options = bindingCacheOptions(cache.path(), "overlapping-binding-module-v1");
	int traces = 0;
	auto compile = [&](BindingAliasState& object, int64_t& member, const int64_t& observed) {
		RuntimeBindings bindings;
		auto whole = bindings.bind<BindingAliasState>("object", &object);
		auto part = bindings.bind<int64_t>("member", &member);
		auto readOnly = bindings.bind<const int64_t>("observed", &observed);
		NautilusEngine engine(options);
		auto module = engine.createModule();
		module.setRuntimeBindings(bindings);
		module.registerFunction<val<int64_t>(val<int64_t*>)>("argument_alias",
		                                                     [part, &traces](val<int64_t*> argument) -> val<int64_t> {
			                                                     ++traces;
			                                                     auto address = part.get();
			                                                     val<int64_t> before = *address;
			                                                     *argument += int64_t {7};
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
		    "member_write", [whole, part](val<int64_t> delta) -> val<int64_t> {
			    auto objectAddress = whole.get();
			    val<int64_t> before = objectAddress.get(&BindingAliasState::second);
			    *part.get() += delta;
			    val<int64_t> after = objectAddress.get(&BindingAliasState::second);
			    return after - before;
		    });
		module.registerFunction<val<int64_t>(val<int64_t>)>("object_write",
		                                                    [whole, part](val<int64_t> delta) -> val<int64_t> {
			                                                    auto address = part.get();
			                                                    val<int64_t> before = *address;
			                                                    auto objectAddress = whole.get();
			                                                    objectAddress.get(&BindingAliasState::second) += delta;
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

	BindingAliasState coldState {19, 41};
	auto cold = compile(coldState, coldState.second, coldState.second);
	REQUIRE(bindingStat<std::string>(cold, "cache.object") == "written");
	check(cold, coldState, coldState.second, coldState.second);
	const auto coldTraces = traces;
	REQUIRE(coldTraces > 0);
	const auto coldSecond = coldState.second;

	BindingAliasState separateState {101, 211};
	int64_t separateMember = 23;
	const int64_t separateObserved = 71;
	auto separate = compile(separateState, separateMember, separateObserved);
	REQUIRE(bindingStat<std::string>(separate, "cache.object") == "hit");
	REQUIRE(bindingStat<int64_t>(separate, "cache.tracingRan") == 0);
	check(separate, separateState, separateMember, separateObserved);
	REQUIRE(separateObserved == 71);

	BindingAliasState warmState {223, 293};
	auto warm = compile(warmState, warmState.second, warmState.second);
	REQUIRE(bindingStat<std::string>(warm, "cache.object") == "hit");
	REQUIRE(bindingStat<int64_t>(warm, "cache.tracingRan") == 0);
	check(warm, warmState, warmState.second, warmState.second);

	corruptBindingArtifact(bindingArtifact(cache.path(), ".o"));
	BindingAliasState repairedState {307, 401};
	auto repaired = compile(repairedState, repairedState.second, repairedState.second);
	REQUIRE(bindingStat<std::string>(repaired, "cache.mlir") == "hit");
	REQUIRE(bindingStat<int64_t>(repaired, "cache.tracingRan") == 0);
	check(repaired, repairedState, repairedState.second, repairedState.second);
	REQUIRE(coldState.second == coldSecond);
	REQUIRE(traces == coldTraces);
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
		NautilusEngine engine(options);
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
		NautilusEngine engine(options);
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
		NautilusEngine engine(options);
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
				NautilusEngine engine(options);
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
				NautilusEngine engine(options);
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
				REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") == "non_relocatable_pointer");
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
		NautilusEngine engine(options);
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
			NautilusEngine engine(options);
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
			REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") == "none");
			REQUIRE(bindingStat<std::string>(compiled, "cache.object") == (iteration == 0 ? "written" : "hit"));
			REQUIRE(bindingStat<int64_t>(compiled, "cache.tracingRan") == (iteration == 0 ? 1 : 0));
			REQUIRE((iteration == 0 ? traces > before : traces == before));
			auto execute = compiled.getFunction<int64_t(uintptr_t)>("execute");
			for (std::size_t index = 0; index < values[iteration]->size(); ++index) {
				REQUIRE(execute(index) == (*values[iteration])[index]);
			}
		}
	}
}

TEST_CASE("RuntimeBindings caches runtime-derived integer pointer arithmetic", "[runtime-bindings][cache]") {
	for (const bool foldConstants : {false, true}) {
		CAPTURE(foldConstants);
		BindingCacheDirectory cache;
		auto options = bindingCacheOptions(cache.path(), "runtime-integer-pointer-module-v1");
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
			NautilusEngine engine(options);
			auto module = engine.createModule();
			module.setRuntimeBindings(bindings);
			module.registerFunction<val<int64_t>(val<uintptr_t>)>(
			    "bound_offset", [state, &traces](val<uintptr_t> index) -> val<int64_t> {
				    ++traces;
				    auto base = static_cast<val<uintptr_t>>(state.get());
				    val<int64_t*> pointer = base + (index + 1) * sizeof(int64_t);
				    return *pointer;
			    });
			module.registerFunction<val<int64_t>(val<int64_t*>, val<uintptr_t>)>(
			    "pointer_offset", [](val<int64_t*> base, val<uintptr_t> index) -> val<int64_t> {
				    auto address = static_cast<val<uintptr_t>>(base);
				    val<int64_t*> pointer = sizeof(int64_t) + address + index * sizeof(int64_t);
				    return *(pointer - 1);
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
			    "integer_induction", [](val<int64_t*> base, val<uintptr_t> count) -> val<int64_t> {
				    auto address = static_cast<val<uintptr_t>>(base);
				    for (val<uintptr_t> index = 0; index < count; ++index) {
					    address = address + sizeof(int64_t);
				    }
				    val<int64_t*> pointer = address;
				    return *pointer;
			    });
			module.registerFunction<val<int64_t>(val<int64_t*>, val<bool>, val<uintptr_t>)>(
			    "merged_pointer",
			    [state](val<int64_t*> other, val<bool> useBinding, val<uintptr_t> index) -> val<int64_t> {
				    auto address = static_cast<val<uintptr_t>>(other);
				    if (useBinding) {
					    address = static_cast<val<uintptr_t>>(state.get());
				    }
				    val<int64_t*> pointer = address + index * sizeof(int64_t);
				    return *pointer;
			    });
			module.registerFunction<val<int64_t>(val<int64_t**>, val<uintptr_t>)>(
			    "table_pointer", [](val<int64_t**> table, val<uintptr_t> index) -> val<int64_t> {
				    val<int64_t*> base = *table;
				    auto address = static_cast<val<uintptr_t>>(base);
				    val<int64_t*> pointer = address + index * sizeof(int64_t);
				    return *pointer;
			    });
			NautilusFunction offsetPointer {"offset_pointer", [](val<uintptr_t> address, val<uintptr_t> index) {
				                                val<int64_t*> pointer = address + index * sizeof(int64_t);
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
			    "numeric_offset", [](val<int64_t*> base, val<double> index) -> val<int64_t> {
				    auto address = static_cast<val<uintptr_t>>(base);
				    val<int64_t*> pointer = address + static_cast<val<uintptr_t>>(index) * sizeof(int64_t);
				    return *pointer;
			    });
			module.registerFunction<val<int64_t>(val<uintptr_t>)>(
			    "null_iterator", [state](val<uintptr_t> count) -> val<int64_t> {
				    val<int64_t*> iterator = nullptr;
				    if (iterator != nullptr) {
					    for (val<uintptr_t> index = 0; index < count; ++index) {
						    iterator = iterator + 1;
					    }
					    return iterator[3];
				    }
				    return state.get()[1];
			    });
			module.registerFunction<val<int64_t>()>("null_cache_merge", [state]() -> val<int64_t> {
				val<int64_t*> cached = nullptr;
				if (!val<bool>(false)) {
					cached = state.get();
				}
				return cached[2];
			});
			for (const bool condition : {false, true}) {
				const auto suffix = condition ? "_true" : "_false";
				module.registerFunction<val<int64_t>()>(std::string("null_select") + suffix, [state, condition] {
					auto base = state.get();
					auto pointer = condition ? select(val<bool>(true), base, val<int64_t*>(nullptr))
					                         : select(val<bool>(false), val<int64_t*>(nullptr), base);
					return static_cast<val<int64_t>>(pointer[1]);
				});
				module.registerFunction<val<int64_t>(val<uintptr_t>)>(
				    std::string("integer_select") + suffix, [condition](val<uintptr_t> address) -> val<int64_t> {
					    val<int64_t*> pointer = condition ? select(val<bool>(true), address, val<uintptr_t>(0))
					                                      : select(val<bool>(false), val<uintptr_t>(0), address);
					    return pointer[1];
				    });
				module.registerFunction<val<bool>(val<bool>)>(
				    std::string("bool_select") + suffix, [condition](val<bool> input) {
					    return condition ? select(val<bool>(true), input, val<bool>(false))
					                     : select(val<bool>(false), val<bool>(false), input);
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
							    count = count & uintptr_t {3};
							    val<int64_t*> null = nullptr;
							    auto condition = nullOnLeft ? (equal ? null == pointer : null != pointer)
							                                : (equal ? pointer == null : pointer != null);
							    if (negate) {
								    condition = !condition;
							    }
							    const auto follow = [count](val<int64_t*> current) -> val<int64_t> {
								    for (val<uintptr_t> index = 0; index < count; ++index) {
									    current = current + 1;
								    }
								    return *current;
							    };
							    if (equal == negate) {
								    if (condition) {
									    return follow(pointer);
								    }
							    } else {
								    if (condition) {
									    return -1;
								    }
								    return follow(pointer);
							    }
							    return -1;
						    });
					}
				}
			}
			module.registerFunction<val<int64_t>(val<bool>, val<uintptr_t>)>(
			    "guarded_header", [state](val<bool> available, val<uintptr_t> count) -> val<int64_t> {
				    auto pointer = select(available, state.get(), val<int64_t*>(nullptr));
				    if (pointer == nullptr) {
					    return -1;
				    }
				    val<int64_t> sum = 0;
				    for (val<uintptr_t> index = 0; index < count; ++index) {
					    sum += pointer[index];
				    }
				    return sum;
			    });
			module.registerFunction<val<int64_t>(val<bool>, val<uintptr_t>)>(
			    "null_backedge", [state](val<bool> available, val<uintptr_t> count) -> val<int64_t> {
				    auto pointer = select(available, state.get(), val<int64_t*>(nullptr));
				    val<int64_t> sum = 0;
				    for (val<uintptr_t> index = 0; index < count; ++index) {
					    if (pointer != nullptr) {
						    sum += pointer[0];
						    pointer = pointer + 1;
					    }
					    if (index == 1) {
						    pointer = nullptr;
					    }
				    }
				    return sum;
			    });
			const auto priorTraces = traces;
			auto compiled = module.compile();
			REQUIRE(bindingStat<std::string>(compiled, "cache.fallback") ==
			        (iteration == 2 ? "invalid_object" : "none"));
			REQUIRE(bindingStat<int64_t>(compiled, "cache.tracingRan") == (iteration == 0 ? 1 : 0));
			REQUIRE((iteration == 0 ? traces > priorTraces : traces == priorTraces));
			REQUIRE(bindingStat<std::string>(compiled, iteration == 2 ? "cache.mlir" : "cache.object") ==
			        (iteration == 0 ? "written" : "hit"));
			const auto currentKey = bindingStat<std::string>(compiled, "cache.key");
			REQUIRE(currentKey == bindingArtifact(cache.path(), ".manifest").stem().string());
			if (iteration == 0) {
				key = currentKey;
			} else {
				REQUIRE(currentKey == key);
			}
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
					std::ifstream input(bindingArtifact(cache.path(), extension), std::ios::binary);
					REQUIRE(input.good());
					const std::string bytes(std::istreambuf_iterator<char> {input}, std::istreambuf_iterator<char> {});
					const std::string raw(reinterpret_cast<const char*>(&integer), sizeof(integer));
					REQUIRE(bytes.find(raw) == std::string::npos);
					REQUIRE(bytes.find(std::to_string(integer)) == std::string::npos);
				}
			}
			if (iteration == 1) {
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
#endif

} // namespace nautilus::engine
