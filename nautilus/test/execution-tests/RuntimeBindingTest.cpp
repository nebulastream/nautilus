#include "ExecutionTest.hpp"
#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"
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
#include <barrier>
#include <cstdint>
#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace nautilus::engine {
namespace {

std::vector<std::string> bindingBackends(bool includeInterpreter = true) {
	std::vector<std::string> result;
	if (includeInterpreter) {
		result.emplace_back("interpreter");
	}
#ifdef ENABLE_TRACING
	const auto compiled = testing::availableBackends();
	result.insert(result.end(), compiled.begin(), compiled.end());
#endif
	return result;
}

Options bindingOptions(const std::string& backend) {
	Options options;
	if (backend == "interpreter") {
		options.setOption("engine.Compilation", false);
	} else if (backend == "tbc-jit") {
		options.setOption("engine.backend", std::string("tbc"));
		options.setOption("tbc.mode", std::string("jit"));
	} else {
		options.setOption("engine.backend", backend);
	}
	testing::applyIrVerifyEnvHook(options);
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

} // namespace

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

TEST_CASE("RuntimeBindings preserves identities and aliases with LocalCSE enabled", "[runtime-bindings][B3]") {
	for (const auto& backend : bindingBackends()) {
		DYNAMIC_SECTION(backend) {
			bool aliased = false;
			SECTION("distinct binding identities with distinct addresses") {
			}
			SECTION("distinct binding identities sharing one address") {
				aliased = true;
			}
			int64_t left = 10, right = 100;
			auto* rightAddress = aliased ? &left : &right;
			RuntimeBindings bindings;
			auto leftBinding = bindings.bind<int64_t>("left", &left);
			auto rightBinding = bindings.bind<int64_t>("right", rightAddress);
			auto observed = bindings.bind<const int64_t>("observed", &left);
			auto repeated = leftBinding;
			auto snapshot = bindings;
			REQUIRE(snapshot.entries() == bindings.entries());
			REQUIRE(bindings.entries().at("left") != bindings.entries().at("right"));
			auto options = bindingOptions(backend);
			options.setOption("ir.enableLocalCSE", true);
			options.setOption("ir.runOptimizationPasses", true);
			options.setOption("ir.verifyAfterEachPass", true);
			options.setOption("ir.failOnVerifyError", true);
			options.setRuntimeBindings(snapshot);
			NautilusEngine engine(options);
			auto module = engine.createModule();
			REQUIRE(module.getOptions().getOptionOrDefault("ir.enableLocalCSE", false));
			module.registerFunction<val<bool>()>(
			    "same_identity", [leftBinding, repeated] { return leftBinding.get() == repeated.get(); });
			module.registerFunction<val<bool>()>(
			    "same_address", [leftBinding, rightBinding] { return leftBinding.get() == rightBinding.get(); });
			module.registerFunction<val<int64_t*>()>("left_address", [leftBinding] { return leftBinding.get(); });
			module.registerFunction<val<int64_t*>()>("right_address", [rightBinding] { return rightBinding.get(); });
			module.registerFunction<val<const int64_t*>()>("observed_address", [observed] { return observed.get(); });
			module.registerFunction<val<int64_t>(val<int64_t>)>(
			    "execute", [leftBinding, rightBinding, repeated, observed](val<int64_t> delta) -> val<int64_t> {
				    auto first = leftBinding.get();
				    auto second = rightBinding.get();
				    *first += delta;
				    *second += delta * int64_t {2};
				    val<int64_t> leftValue = *repeated.get();
				    val<int64_t> rightValue = *rightBinding.get();
				    val<int64_t> readOnlyValue = *observed.get();
				    return leftValue + rightValue + readOnlyValue;
			    });
			auto compiled = module.compile();
			REQUIRE(left == 10);
			REQUIRE(right == 100);
			const auto statistics = compiled.getStatistics();
			if (backend == "interpreter") {
				REQUIRE(statistics == nullptr);
			} else {
				REQUIRE(statistics != nullptr);
				REQUIRE(statistics->contains("irPasses.LocalCSE.ms"));
			}
			REQUIRE(compiled.getFunction<bool()>("same_identity")());
			REQUIRE(compiled.getFunction<bool()>("same_address")() == aliased);
			REQUIRE(compiled.getFunction<int64_t*()>("left_address")() == &left);
			REQUIRE(compiled.getFunction<int64_t*()>("right_address")() == rightAddress);
			REQUIRE(compiled.getFunction<const int64_t*()>("observed_address")() == &left);
			auto execute = compiled.getFunction<int64_t(int64_t)>("execute");
			for (const int64_t delta : {3, -2, 0, 7}) {
				CAPTURE(delta);
				auto expectedLeft = left + delta;
				auto expectedRight = right;
				(aliased ? expectedLeft : expectedRight) += delta * 2;
				REQUIRE(execute(delta) == expectedLeft * 2 + (aliased ? expectedLeft : expectedRight));
				REQUIRE(left == expectedLeft);
				REQUIRE(right == expectedRight);
				left += 11;
				right += 13;
			}
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
TEST_CASE("RuntimeBindings validates standalone trace entry points from options", "[runtime-bindings]") {
	const auto tracking = GENERATE(ConstantOriginTracking::Disabled, ConstantOriginTracking::Enabled);
	const auto trace = [tracking](auto& wrapper, const Options& options, common::Arena& arena) {
		return tracing::TraceContext::trace(wrapper, options, arena, tracking);
	};
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
	const auto tracking = GENERATE(ConstantOriginTracking::Disabled, ConstantOriginTracking::Enabled);
	const auto trace = [tracking](auto& wrapper, const Options& options, common::Arena& arena) {
		return tracing::TraceContext::trace(wrapper, options, arena, tracking);
	};
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
	const auto tracking = GENERATE(ConstantOriginTracking::Disabled, ConstantOriginTracking::Enabled);
	const auto trace = [tracking](auto& wrapper, const Options& options, common::Arena& arena) {
		return tracing::TraceContext::trace(wrapper, options, arena, tracking);
	};
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
	const auto tracking = GENERATE(ConstantOriginTracking::Disabled, ConstantOriginTracking::Enabled);
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
			REQUIRE_THROWS_AS(tracing::TraceContext::trace(wrapper, options, arena, tracking), std::invalid_argument);
			REQUIRE(paused);
			REQUIRE_FALSE(tracing::inTracer());
			auto fresh = details::createFunctionWrapper([state] { return state.get(); });
			REQUIRE_NOTHROW(tracing::TraceContext::trace(fresh, options, arena, tracking));
			REQUIRE_FALSE(tracing::inTracer());
		}
	}
}

TEST_CASE("RuntimeBindings keeps concurrent standalone tracing environments independent", "[runtime-bindings]") {
	std::array<int64_t, 2> values {42, 97};
	std::array<RuntimeBindings, 2> bindings;
	std::array<RuntimeBinding<int64_t>, 2> handles {bindings[0].bind<int64_t>("state", &values[0]),
	                                                bindings[1].bind<int64_t>("state", &values[1])};
	std::array<std::exception_ptr, 2> errors;
	std::array<bool, 2> rejected {}, cleared {}, recorded {}, originsCorrect {}, sawConstant {};
	std::barrier synchronize(2);
	auto worker = [&](size_t index) {
		try {
			Options options;
			options.setRuntimeBindings(bindings[index]);
			common::Arena arena;
			const auto tracking = index == 0 ? ConstantOriginTracking::Enabled : ConstantOriginTracking::Disabled;
			auto wrapper = details::createFunctionWrapper([&] {
				synchronize.arrive_and_wait();
				(void) cacheLiteral<int64_t {7}>();
				try {
					tracing::traceRuntimeBinding(*bindings[1 - index].entries().at("state"));
				} catch (const std::invalid_argument&) {
					rejected[index] = true;
				}
				return handles[index].get();
			});
			auto executionTrace = tracing::TraceContext::trace(wrapper, options, arena, tracking);
			originsCorrect[index] = executionTrace->recordsConstantOrigins() == (index == 0);
			cleared[index] = !tracing::inTracer();
			for (const auto* block : executionTrace->getBlocks()) {
				for (const auto* operation : block->operations) {
					if (operation->op == tracing::Op::RUNTIME_BINDING) {
						const auto* entry = std::get<const runtime_binding::Entry*>(operation->input[0]);
						recorded[index] = entry->address == &values[index];
					} else if (operation->op == tracing::Op::CONST) {
						sawConstant[index] = true;
						originsCorrect[index] &=
						    operation->constantOrigin ==
						    (index == 0 ? ConstantOrigin::CacheInvariant : ConstantOrigin::Unspecified);
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
		REQUIRE(originsCorrect[index]);
		REQUIRE(sawConstant[index]);
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

TEST_CASE("RuntimeBindings option overrides inherit clear or replace module snapshots", "[runtime-bindings][B3]") {
	for (const auto& backend : bindingBackends()) {
		DYNAMIC_SECTION(backend) {
			int64_t value = 42, replacement = 97, inheritedOnly = 13, replacementOnly = 29;
			RuntimeBindings inherited, rebound;
			auto state = inherited.bind<int64_t>("state", &value);
			(void) inherited.bind<int64_t>("inherited-only", &inheritedOnly);
			auto reboundState = rebound.bind<int64_t>("state", &replacement);
			(void) rebound.bind<int64_t>("replacement-only", &replacementOnly);
			auto options = bindingOptions(backend);
			options.setRuntimeBindings(inherited);
			NautilusEngine engine(options);
			ModuleOptions overrides;
			overrides.setOption("ir.enableLocalCSE", true);
			RuntimeBindings expected = inherited;
			auto selected = state;
			bool inherits = true;
			SECTION("unspecified registry inherits the engine snapshot") {
				REQUIRE(overrides.getRuntimeBindings().entries().empty());
			}
			SECTION("explicitly empty registry clears the engine snapshot") {
				overrides.setRuntimeBindings(RuntimeBindings {});
				expected = RuntimeBindings {};
				selected = RuntimeBinding<int64_t> {};
				inherits = false;
			}
			SECTION("replacement registry replaces rather than merges the engine snapshot") {
				overrides.setRuntimeBindings(rebound);
				expected = rebound;
				selected = reboundState;
				inherits = false;
			}
			auto effective = options;
			effective.applyOverrides(overrides);
			REQUIRE(effective.getRuntimeBindings().entries() == expected.entries());
			REQUIRE(effective.getRuntimeBindings().schema() == expected.schema());
			REQUIRE(effective.getOptionOrDefault("ir.enableLocalCSE", false));
			REQUIRE(options.getRuntimeBindings().entries() == inherited.entries());
			auto module = engine.createModule(overrides);
			REQUIRE(module.getOptions().getRuntimeBindings().entries() == expected.entries());
			REQUIRE(module.getOptions().getRuntimeBindings().schema() == expected.schema());
			REQUIRE(module.getOptions().getOptionOrDefault("ir.enableLocalCSE", false));
			module.registerFunction<val<int64_t>(val<int64_t>)>("execute",
			                                                    [selected](val<int64_t> fallback) -> val<int64_t> {
				                                                    if (selected.isBound()) {
					                                                    return *selected.get();
				                                                    }
				                                                    return fallback;
			                                                    });
			auto compiled = module.compile();
			auto execute = compiled.getFunction<int64_t(int64_t)>("execute");
			REQUIRE(execute(71) == (selected.isBound() ? (inherits ? value : replacement) : 71));
			value = 54;
			replacement = 113;
			REQUIRE(execute(-8) == (selected.isBound() ? (inherits ? value : replacement) : -8));
			auto unchanged = engine.createModule();
			REQUIRE(unchanged.getOptions().getRuntimeBindings().entries() == inherited.entries());
			unchanged.registerFunction<val<int64_t>()>("execute", [state]() -> val<int64_t> { return *state.get(); });
			REQUIRE(unchanged.compile().getFunction<int64_t()>("execute")() == value);
			if (!inherits) {
				auto excluded = engine.createModule(overrides);
				int calls = 0;
				excluded.registerFunction<val<int64_t>()>("execute", [state, &calls]() -> val<int64_t> {
					++calls;
					return *state.get();
				});
				if (backend == "interpreter") {
					auto interpreted = excluded.compile();
					REQUIRE(calls == 0);
					REQUIRE(interpreted.getFunction<int64_t()>("execute")() == value);
					REQUIRE(calls == 1);
				} else {
					REQUIRE_THROWS_AS(excluded.compile(), std::invalid_argument);
					REQUIRE(calls > 0);
				}
			}
			REQUIRE(value == 54);
			REQUIRE(replacement == 113);
			REQUIRE(inheritedOnly == 13);
			REQUIRE(replacementOnly == 29);
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

} // namespace nautilus::engine
