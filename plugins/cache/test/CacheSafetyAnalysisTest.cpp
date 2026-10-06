#include "nautilus/config.hpp"

#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)
#include "CacheSafetyAnalysis.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/compiler/artifact/ArtifactPreflight.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/select.hpp"
#include "nautilus/static.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/val_std.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <cstdint>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace nautilus::engine {
namespace {

compiler::ir::FunctionOperation* analysisFunction(compiler::ir::IRGraph& graph,
                                                  std::vector<compiler::ir::BasicBlock*> blocks, Type result = Type::v,
                                                  const std::string& name = "execute") {
	using namespace compiler::ir;
	std::vector<Type> types;
	std::vector<std::string> names;
	for (const auto* argument : blocks.front()->getArguments()) {
		types.push_back(argument->getStamp());
		names.push_back(argument->getIdentifier().toString());
	}
	auto* function = graph.addFunctionOperation(
	    graph.getArena().create<FunctionOperation>(name, std::move(blocks), types, std::move(names), result));
	const auto definition = graph.internCallee({.kind = CalleeDescriptor::Kind::Internal,
	                                            .key = function,
	                                            .mangledName = name,
	                                            .demangledName = name,
	                                            .customName = name,
	                                            .resultType = result,
	                                            .paramTypes = types,
	                                            .attrs = {}});
	graph.defineFunction(definition, function);
	return function;
}

struct SafetyResults {
	compiler::ir::CacheScalarValidationPass::Result scalar;
	compiler::ir::PointerRelocatabilityPass::Result pointer;
};

SafetyResults analyzeSafety(compiler::ir::IRGraph& graph, const std::vector<std::string>& exports = {"execute"}) {
	using namespace compiler::ir;
	const auto before = graph.toString();
	const auto recorded = graph.hasRecordedConstantOrigins();
	CacheScalarValidationPass scalar;
	PointerRelocatabilityPass pointer(exports);
	REQUIRE_FALSE(scalar.apply(graph));
	REQUIRE_FALSE(pointer.apply(graph));
	const SafetyResults result {scalar.getResult(), pointer.getResult()};
	std::string reason = "stale";
	REQUIRE(compiler::artifact::hasOnlyInvariantScalars(graph, &reason) == result.scalar.certified);
	REQUIRE(reason == result.scalar.rejection);
	REQUIRE_FALSE(scalar.apply(graph));
	REQUIRE_FALSE(pointer.apply(graph));
	REQUIRE(scalar.getResult().certified == result.scalar.certified);
	REQUIRE(scalar.getResult().rejection == result.scalar.rejection);
	REQUIRE(pointer.getResult().relocatable == result.pointer.relocatable);
	REQUIRE(pointer.getResult().rejection == result.pointer.rejection);
	REQUIRE(graph.toString() == before);
	REQUIRE(graph.hasRecordedConstantOrigins() == recorded);
	return result;
}

SafetyResults traceSafety(std::list<compiler::CompilableFunction>& functions, const Options& options,
                          ConstantOriginTracking tracking = ConstantOriginTracking::Enabled,
                          const std::vector<std::string>& exports = {"execute"}) {
	common::ArenaPool traceArenaPool, irArenaPool;
	compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
	SafetyResults result;
	std::size_t checks = 0;
	auto graph = pipeline.compileToIR(
	    functions, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
	    [&](compiler::ir::IRGraph& ir) {
		    ++checks;
		    REQUIRE(ir.hasRecordedConstantOrigins() == (tracking == ConstantOriginTracking::Enabled));
		    result = analyzeSafety(ir, exports);
	    },
	    tracking);
	REQUIRE(checks == 1);
	REQUIRE(graph != nullptr);
	REQUIRE(graph->hasRecordedConstantOrigins() == (tracking == ConstantOriginTracking::Enabled));
	REQUIRE_FALSE(tracing::inTracer());
	return result;
}

void requireUncertified(const SafetyResults& result, ConstantOriginTracking tracking) {
	REQUIRE_FALSE(result.scalar.certified);
	if (tracking == ConstantOriginTracking::Enabled) {
		REQUIRE_THAT(result.scalar.rejection, Catch::Matchers::ContainsSubstring("uncertified_scalar"));
	} else {
		REQUIRE(result.scalar.rejection == "constant_origins_not_recorded");
	}
}

} // namespace

TEST_CASE("Cache safety pass adapters preserve graphs and reset independent results", "[cache][safety]") {
	using namespace compiler::ir;
	IRGraph accepted("cache-analysis-accepted"), rejected("cache-analysis-rejected");
	for (auto* graph : {&accepted, &rejected}) {
		REQUIRE(graph->hasRecordedConstantOrigins());
		auto& arena = graph->getArena();
		auto* argument = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::ui64);
		auto* block = arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {argument});
		Operation* value;
		if (graph == &rejected) {
			value = block->addOperation<ConstPtrOperation>(OperationIdentifier(1), &rejected);
		} else {
			value = block->addOperation<CastOperation>(OperationIdentifier(1), argument, Type::ptr);
		}
		block->addOperation<ReturnOperation>(value);
		analysisFunction(*graph, {block}, Type::ptr);
	}
	const auto acceptedBefore = accepted.toString();
	const auto rejectedBefore = rejected.toString();
	const auto requireUnchanged = [&] {
		REQUIRE(accepted.toString() == acceptedBefore);
		REQUIRE(rejected.toString() == rejectedBefore);
	};
	CacheScalarValidationPass scalar, independentScalar;
	std::vector<std::string> exports {"execute"};
	PointerRelocatabilityPass pointer(exports), independentPointer({"execute"});
	exports.clear();
	IRPass& scalarPass = scalar;
	IRPass& pointerPass = pointer;
	REQUIRE(scalarPass.getName() == "cacheScalarValidation");
	REQUIRE(pointerPass.getName() == "pointerRelocatability");
	REQUIRE_FALSE(scalar.getResult().certified);
	REQUIRE(scalar.getResult().rejection.empty());
	REQUIRE_FALSE(pointer.getResult().relocatable);
	REQUIRE(pointer.getResult().rejection.empty());
	REQUIRE_FALSE(independentScalar.apply(rejected));
	REQUIRE_FALSE(independentPointer.apply(rejected));
	const auto independentScalarResult = independentScalar.getResult();
	const auto independentPointerResult = independentPointer.getResult();
	REQUIRE_FALSE(independentScalarResult.certified);
	REQUIRE_FALSE(independentPointerResult.relocatable);
	for (auto* graph : {&accepted, &rejected, &accepted, &rejected}) {
		CAPTURE(graph->getId());
		const bool expected = graph == &accepted;
		REQUIRE_FALSE(scalarPass.apply(*graph));
		REQUIRE_FALSE(pointerPass.apply(*graph));
		const auto scalarResult = scalar.getResult();
		const auto pointerResult = pointer.getResult();
		REQUIRE(scalarResult.certified == expected);
		REQUIRE(scalarResult.rejection.empty() == expected);
		REQUIRE(pointerResult.relocatable == expected);
		REQUIRE(pointerResult.rejection.empty() == expected);
		if (!expected) {
			REQUIRE_THAT(scalarResult.rejection, Catch::Matchers::ContainsSubstring("embedded_non_null_pointer"));
			REQUIRE_THAT(pointerResult.rejection, Catch::Matchers::ContainsSubstring("embedded_non_null_pointer"));
		}
		REQUIRE_FALSE(scalarPass.apply(*graph));
		REQUIRE_FALSE(pointerPass.apply(*graph));
		REQUIRE(scalar.getResult().certified == scalarResult.certified);
		REQUIRE(scalar.getResult().rejection == scalarResult.rejection);
		REQUIRE(pointer.getResult().relocatable == pointerResult.relocatable);
		REQUIRE(pointer.getResult().rejection == pointerResult.rejection);
		REQUIRE(independentScalar.getResult().certified == independentScalarResult.certified);
		REQUIRE(independentScalar.getResult().rejection == independentScalarResult.rejection);
		REQUIRE(independentPointer.getResult().relocatable == independentPointerResult.relocatable);
		REQUIRE(independentPointer.getResult().rejection == independentPointerResult.rejection);
		requireUnchanged();
	}
	const auto scalarResult = scalar.getResult();
	const auto pointerResult = pointer.getResult();
	REQUIRE_FALSE(independentScalar.apply(accepted));
	REQUIRE_FALSE(independentPointer.apply(accepted));
	REQUIRE(independentScalar.getResult().certified);
	REQUIRE(independentScalar.getResult().rejection.empty());
	REQUIRE(independentPointer.getResult().relocatable);
	REQUIRE(independentPointer.getResult().rejection.empty());
	REQUIRE(scalar.getResult().certified == scalarResult.certified);
	REQUIRE(scalar.getResult().rejection == scalarResult.rejection);
	REQUIRE(pointer.getResult().relocatable == pointerResult.relocatable);
	REQUIRE(pointer.getResult().rejection == pointerResult.rejection);
	for (int invalidation = 0; invalidation < 2; ++invalidation) {
		accepted.invalidateConstantOrigins();
		REQUIRE_FALSE(scalarPass.apply(accepted));
		REQUIRE_FALSE(scalar.getResult().certified);
		REQUIRE(scalar.getResult().rejection == "constant_origins_not_recorded");
		REQUIRE_FALSE(pointerPass.apply(accepted));
		REQUIRE(pointer.getResult().relocatable);
		REQUIRE(pointer.getResult().rejection.empty());
		requireUnchanged();
	}
	REQUIRE(rejected.hasRecordedConstantOrigins());
}

TEST_CASE("Legacy cache analysis accepts runtime addresses without certifying unavailable origins", "[cache][safety]") {
	using namespace compiler::ir;
	auto storage = std::make_unique<int64_t>(42);
	for (const auto tracking : {ConstantOriginTracking::Enabled, ConstantOriginTracking::Disabled}) {
		for (const std::string_view kind : {"runtime", "null", "raw heap", "integer encoded"}) {
			CAPTURE(tracking, kind);
			Options options;
			options.setOption("ir.runPasses", false);
			std::list<compiler::CompilableFunction> functions;
			functions.emplace_back("execute",
			                       details::createFunctionWrapper(
			                           [kind, pointer = storage.get()](val<uintptr_t> address) -> val<int64_t*> {
				                           if (kind == "raw heap") {
					                           return val<int64_t*>(pointer);
				                           }
				                           if (kind == "null") {
					                           return val<int64_t*>(nullptr);
				                           }
				                           if (kind == "integer encoded") {
					                           address = val<uintptr_t>(reinterpret_cast<uintptr_t>(pointer));
				                           }
				                           val<int64_t*> result = address;
				                           return result;
			                           }));
			const auto result = traceSafety(functions, options, tracking);
			const bool relocatable = kind == "runtime" || kind == "null";
			REQUIRE(result.scalar.certified == (relocatable && tracking == ConstantOriginTracking::Enabled));
			REQUIRE(result.pointer.relocatable == relocatable);
			if (tracking == ConstantOriginTracking::Disabled) {
				REQUIRE(result.scalar.rejection == "constant_origins_not_recorded");
			}
			if (relocatable) {
				REQUIRE(result.pointer.rejection.empty());
			} else {
				REQUIRE_THAT(result.pointer.rejection,
				             Catch::Matchers::ContainsSubstring(kind == "raw heap" ? "embedded_non_null_pointer"
				                                                                   : "pointer_expression"));
			}
		}
	}
}

TEST_CASE("Both cache analyses run before optional optimization can discard evidence", "[cache][safety][mandatory]") {
	for (const std::string_view mode : {"passes disabled", "optimization disabled", "one iteration", "fixed point"}) {
		for (const std::string_view kind : {"certified", "uncertified", "discarded uncertified", "legacy"}) {
			CAPTURE(mode, kind);
			Options options;
			options.setOption("ir.runPasses", mode != "passes disabled");
			options.setOption("ir.runOptimizationPasses", mode != "optimization disabled");
			options.setOption("ir.maxPipelineIterations", mode == "one iteration" ? 1 : 8);
			int traces = 0;
			auto consume = +[](int64_t* state, int64_t increment) noexcept {
				*state += increment;
				return *state;
			};
			std::list<compiler::CompilableFunction> functions;
			functions.emplace_back("execute", details::createFunctionWrapper([=, &traces](val<int64_t*> state) {
				                       ++traces;
				                       if (kind == "discarded uncertified") {
					                       val<int64_t> unused(11);
					                       (void) unused;
				                       }
				                       auto increment = kind == "uncertified" || kind == "legacy"
				                                            ? val<int64_t>(7)
				                                            : cacheLiteral<int64_t {7}>();
				                       if (kind == "legacy") {
					                       return static_cast<val<int64_t>>(*state) + increment;
				                       }
				                       return invoke(consume, state, increment);
			                       }));
			const auto result = traceSafety(functions, options);
			REQUIRE(traces > 0);
			REQUIRE(result.scalar.certified == (kind == "certified"));
			REQUIRE(result.pointer.relocatable == (kind == "legacy"));
			REQUIRE((result.scalar.certified || result.pointer.relocatable) ==
			        (kind == "certified" || kind == "legacy"));
			if (kind == "certified") {
				REQUIRE(result.scalar.rejection.empty());
			} else {
				requireUncertified(result, ConstantOriginTracking::Enabled);
			}
			if (kind == "legacy") {
				REQUIRE(result.pointer.rejection.empty());
			} else {
				REQUIRE_THAT(result.pointer.rejection, Catch::Matchers::ContainsSubstring("opaque_call"));
			}
		}
	}
}

TEST_CASE("Cache analysis retains raw folded zero without upgrading equal certified values", "[cache][safety]") {
	using namespace compiler::ir;
	for (const bool fold : {false, true}) {
		for (const bool optimize : {false, true}) {
			for (const bool opaque : {false, true}) {
				for (const auto tracking : {ConstantOriginTracking::Enabled, ConstantOriginTracking::Disabled}) {
					CAPTURE(fold, optimize, opaque, tracking);
					Options options;
					options.setOption("engine.foldStaticConstants", fold);
					options.setOption("ir.runOptimizationPasses", optimize);
					options.setOption("ir.maxPipelineIterations", 8);
					auto consume = +[](int64_t* state, int64_t increment) noexcept {
						*state += increment;
						return *state;
					};
					std::list<compiler::CompilableFunction> functions;
					functions.emplace_back("execute",
					                       details::createFunctionWrapper([opaque, consume](val<int64_t*> state) {
						                       auto pointer = state + 0;
						                       auto increment = cacheLiteral<int64_t {7}>();
						                       if (opaque) {
							                       return invoke(consume, pointer, increment);
						                       }
						                       return static_cast<val<int64_t>>(*pointer) + increment;
					                       }));
					common::ArenaPool traceArenaPool, irArenaPool;
					compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
					CacheScalarValidationPass scalar;
					PointerRelocatabilityPass pointer({"execute"});
					SafetyResults result;
					std::size_t checks = 0;
					auto graph = pipeline.compileToIR(
					    functions, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
					    [&](IRGraph& ir) {
						    ++checks;
						    const auto before = ir.toString();
						    REQUIRE_FALSE(scalar.apply(ir));
						    REQUIRE_FALSE(pointer.apply(ir));
						    result = {scalar.getResult(), pointer.getResult()};
						    std::size_t zeros = 0;
						    for (const auto* block : ir.getFunctionOperation("execute")->getBasicBlocks()) {
							    for (const auto* operation : block->getOperations()) {
								    if (const auto* constant = operation->dynCast<ConstIntOperation>();
								        constant != nullptr && constant->getValue() == 0) {
									    ++zeros;
									    REQUIRE(constant->getConstantOrigin() == ConstantOrigin::Unspecified);
								    }
							    }
						    }
						    if (tracking == ConstantOriginTracking::Enabled) {
							    REQUIRE(zeros > 0);
						    }
						    REQUIRE(ir.toString() == before);
					    },
					    tracking);
					REQUIRE(checks == 1);
					REQUIRE(graph != nullptr);
					requireUncertified(result, tracking);
					REQUIRE(result.pointer.relocatable == (fold && !opaque));
					REQUIRE(scalar.getResult().certified == result.scalar.certified);
					REQUIRE(scalar.getResult().rejection == result.scalar.rejection);
					REQUIRE(pointer.getResult().relocatable == result.pointer.relocatable);
					REQUIRE(pointer.getResult().rejection == result.pointer.rejection);
				}
			}
		}
	}
}

TEST_CASE("Cache address analysis rejects encoded captures through branches loops and callees", "[cache][safety]") {
	const bool fold = GENERATE(false, true);
	CAPTURE(fold);
	for (const auto tracking : {ConstantOriginTracking::Enabled, ConstantOriginTracking::Disabled}) {
		for (const std::string_view kind :
		     {"direct", "arithmetic", "runtime offset", "branch", "loop", "null base", "null pointer offset",
		      "pointer difference", "callee result", "callee argument", "indirect result", "indirect argument",
		      "selected result", "nonnull capture", "nonnull difference", "null branch", "unrelated null guard"}) {
			CAPTURE(tracking, kind);
			auto storage = std::make_unique<std::array<int64_t, 4>>(std::array<int64_t, 4> {11, 22, 33, 44});
			const auto encoded = reinterpret_cast<uintptr_t>(storage->data());
			NautilusFunction encodedAddress {"encoded_address", [encoded] { return val<uintptr_t>(encoded); }};
			NautilusFunction offsetEncoded {
			    "offset_encoded", [encoded](val<uintptr_t> delta) { return val<uintptr_t>(encoded) + delta; }};
			NautilusFunction shiftedEncoded {
			    "shifted_encoded", [encoded](val<uintptr_t> delta) { return val<uintptr_t>(encoded) - delta; }};
			NautilusFunction dereference {"dereference", [](val<uintptr_t> address) -> val<int64_t> {
				                              val<int64_t*> pointer = address;
				                              return *pointer;
			                              }};
			std::list<compiler::CompilableFunction> functions;
			functions.emplace_back(
			    "execute",
			    details::createFunctionWrapper([=, &encodedAddress, &dereference, &offsetEncoded, &shiftedEncoded](
			                                       val<uintptr_t> offset, val<int64_t*> runtime) -> val<int64_t> {
				    val<uintptr_t> address = encoded;
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
					    return *pointer + cacheLiteral<int64_t {7}>();
				    } else if (kind == "pointer difference" || kind == "nonnull difference") {
					    const auto base = static_cast<val<uintptr_t>>(runtime);
					    address = (base + address) - base;
				    } else if (kind == "callee result") {
					    address = encodedAddress();
				    } else if (kind == "callee argument") {
					    return dereference(address) + cacheLiteral<int64_t {7}>();
				    } else if (kind == "indirect result") {
					    address = encodedAddress.getFuncPtr()();
				    } else if (kind == "indirect argument") {
					    return dereference.getFuncPtr()(address) + cacheLiteral<int64_t {7}>();
				    } else if (kind == "selected result") {
					    auto callback = offsetEncoded.getFuncPtr();
					    if (offset > 0) {
						    callback = shiftedEncoded.getFuncPtr();
					    }
					    address = callback(cacheLiteral<uintptr_t {0}>());
				    }
				    if (kind == "null branch" || kind == "unrelated null guard") {
					    auto base = select(offset > 0, static_cast<val<int8_t*>>(runtime), val<int8_t*>(nullptr));
					    if (kind == "null branch") {
						    if (base == nullptr) {
							    auto pointer = static_cast<val<int64_t*>>(base + address);
							    return *pointer;
						    }
					    } else if (runtime != nullptr) {
						    auto delta = select(offset > 0, cacheLiteral<uintptr_t {0}>(), address);
						    val<int64_t*> pointer = static_cast<val<uintptr_t>>(base) + delta;
						    return *pointer;
					    }
					    return *runtime;
				    }
				    val<int64_t*> pointer = address;
				    if (kind == "nonnull capture" || kind == "nonnull difference") {
					    if (pointer != nullptr) {
						    return pointer[1];
					    }
					    return cacheLiteral<int64_t {0}>();
				    }
				    return *pointer + cacheLiteral<int64_t {7}>();
			    }));
			Options options;
			options.setOption("ir.runOptimizationPasses", true);
			options.setOption("ir.maxPipelineIterations", 8);
			options.setOption("engine.foldStaticConstants", fold);
			options.setOption("ir.disableConstantFolding", !fold);
			const auto result = traceSafety(functions, options, tracking);
			requireUncertified(result, tracking);
			REQUIRE_FALSE(result.pointer.relocatable);
			REQUIRE_FALSE(result.pointer.rejection.empty());
		}
	}
}

TEST_CASE("Cache analysis rejects memory and callback laundering of encoded heap fragments", "[cache][safety]") {
	const bool fold = GENERATE(false, true);
	CAPTURE(fold);
	for (const auto tracking : {ConstantOriginTracking::Enabled, ConstantOriginTracking::Disabled}) {
		for (const std::string_view kind : {"store",
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
		                                    "partial bytes",
		                                    "boolean bits",
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
			CAPTURE(tracking, kind);
			auto storage = std::make_unique<int64_t>(101);
			const auto encoded = reinterpret_cast<uintptr_t>(storage.get());
			NautilusFunction storeAddress {"store_address", [encoded](val<uintptr_t*> slot) { *slot = encoded; }};
			NautilusFunction loadAddress {"load_address", [](val<uintptr_t*> slot) -> val<uintptr_t> { return *slot; }};
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
			auto nativeBits = +[](bool* bits) noexcept {
				uintptr_t address = 0;
				for (std::size_t index = 0; index < sizeof(uintptr_t) * 8; ++index) {
					address |= static_cast<uintptr_t>(bits[index]) << index;
				}
				return *reinterpret_cast<int64_t*>(address);
			};
			std::list<compiler::CompilableFunction> functions;
			std::vector<std::string> exports {"execute"};
			if (kind == "cross export") {
				functions.emplace_back(
				    "initialize", details::createFunctionWrapper([encoded](val<uintptr_t*> slot) { *slot = encoded; }));
				exports.emplace_back("initialize");
			}
			functions.emplace_back(
			    "execute",
			    details::createFunctionWrapper(
			        [=, &storeAddress, &loadAddress, &encodedCallback, &offsetCallback, &callbackFactory](
			            val<uintptr_t*> scratch, val<uintptr_t*> alias, val<uintptr_t**> table, val<bool*> bits,
			            val<int64_t*> observed, val<uintptr_t (*)(uintptr_t)> callback) -> val<int64_t> {
				        (void) cacheLiteral<int64_t {9}>();
				        if (kind == "native callback") {
					        return invoke(nativeCallback, encodedCallback.getFuncPtr());
				        }
				        if (kind == "native returned callback") {
					        return invoke(nativeOffsetCallback, callbackFactory(), static_cast<val<uintptr_t>>(alias));
				        }
				        if (kind == "native callback offset") {
					        return invoke(nativeOffsetCallback, offsetCallback.getFuncPtr(),
					                      static_cast<val<uintptr_t>>(alias));
				        }
				        auto slot = scratch;
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
				        } else if (kind == "partial bytes") {
					        auto bytes = static_cast<val<uint8_t*>>(slot);
					        const auto encodedBytes = std::bit_cast<std::array<uint8_t, sizeof(uintptr_t)>>(encoded);
					        for (static_val<std::size_t> index = 0; index < sizeof(uintptr_t) / 2; ++index) {
						        bytes[cacheInvariant(static_cast<std::size_t>(index))] = encodedBytes[index];
					        }
				        } else if (kind == "boolean bits") {
					        for (static_val<std::size_t> index = 0; index < sizeof(uintptr_t) * 8; ++index) {
						        bits[cacheInvariant(static_cast<std::size_t>(index))] =
						            bool((encoded >> static_cast<std::size_t>(index)) & 1);
					        }
					        return invoke(nativeBits, bits);
				        } else if (kind != "cross export" && kind != "native result" &&
				                   kind != "native pointer result" && kind != "indirect result" &&
				                   kind != "native difference" && kind != "native integer consumer") {
					        *slot = encoded;
				        }
				        if (kind == "argument alias") {
					        slot = alias;
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
					        auto pointer = invoke(nativePointer, val<uintptr_t>(encoded));
					        return *pointer;
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
			        }));
			Options options;
			options.setOption("ir.runOptimizationPasses", true);
			options.setOption("ir.maxPipelineIterations", 8);
			options.setOption("engine.foldStaticConstants", fold);
			options.setOption("ir.disableConstantFolding", !fold);
			const auto result = traceSafety(functions, options, tracking, exports);
			requireUncertified(result, tracking);
			REQUIRE_FALSE(result.pointer.relocatable);
			REQUIRE_FALSE(result.pointer.rejection.empty());
		}
	}
}

TEST_CASE("Cache analyses walk raw encoded and scalar leaves used only by native cleanup", "[cache][safety]") {
	using namespace compiler::ir;
	for (const bool indirect : {false, true}) {
		for (const bool recorded : {false, true}) {
			for (const std::string_view kind :
			     {"runtime pointer", "runtime integer", "null", "raw heap", "encoded integer", "integer fragment",
			      "boolean fragment", "floating fragment", "ordinary offset", "certified offset"}) {
				CAPTURE(indirect, recorded, kind);
				IRGraph graph("cleanup-only-cache-evidence");
				if (!recorded) {
					graph.invalidateConstantOrigins();
				}
				auto& arena = graph.getArena();
				auto storage = std::make_unique<int64_t>(42);
				const auto encoded = reinterpret_cast<uintptr_t>(storage.get());
				auto* base = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::ptr);
				auto* runtime = arena.create<BasicBlockArgument>(OperationIdentifier(1), Type::ui64);
				auto* block = arena.create<BasicBlock>(arena, BlockIdentifier(0),
				                                       std::vector<BasicBlockArgument*> {base, runtime});
				Operation* address = base;
				const bool safe = kind == "runtime pointer" || kind == "runtime integer" || kind == "null";
				if (kind == "runtime integer") {
					address = arena.create<CastOperation>(arena, OperationIdentifier(2), runtime, Type::ptr);
				} else if (kind == "null" || kind == "raw heap") {
					address = arena.create<ConstPtrOperation>(arena, OperationIdentifier(2),
					                                          kind == "null" ? nullptr : storage.get());
				} else if (kind == "encoded integer") {
					auto* integer = arena.create<ConstIntOperation>(arena, OperationIdentifier(2), encoded, Type::ui64);
					auto* pointer = arena.create<CastOperation>(arena, OperationIdentifier(3), integer, Type::ptr);
					address = arena.create<CastOperation>(arena, OperationIdentifier(4), pointer, Type::ptr);
				} else if (!safe) {
					Operation* offset;
					if (kind == "boolean fragment") {
						auto* bit =
						    arena.create<ConstBooleanOperation>(arena, OperationIdentifier(2), bool(encoded & 1));
						offset = arena.create<CastOperation>(arena, OperationIdentifier(3), bit, Type::ui64);
					} else if (kind == "floating fragment") {
						auto* fragment = arena.create<ConstFloatOperation>(arena, OperationIdentifier(2),
						                                                   double(encoded & 0xff), Type::f64);
						offset = arena.create<CastOperation>(arena, OperationIdentifier(3), fragment, Type::ui64);
					} else {
						offset = arena.create<ConstIntOperation>(
						    arena, OperationIdentifier(2), kind == "integer fragment" ? encoded & 0xff : 7, Type::ui64,
						    kind == "certified offset" ? ConstantOrigin::CacheInvariant : ConstantOrigin::Unspecified);
					}
					auto* integer = arena.create<CastOperation>(arena, OperationIdentifier(4), base, Type::ui64);
					auto* sum = arena.create<AddOperation>(arena, OperationIdentifier(5), integer, offset);
					address = arena.create<CastOperation>(arena, OperationIdentifier(6), sum, Type::ptr);
					REQUIRE(std::ranges::find(block->getOperations(), offset) == block->getOperations().end());
				}
				auto* throwing = reinterpret_cast<void*>(+[]() { throw std::runtime_error("cleanup metadata"); });
				auto* cleanup = reinterpret_cast<void*>(+[](int64_t* pointer) noexcept { ++*pointer; });
				const auto callee = graph.internCallee({.key = throwing,
				                                        .mangledName = "throwing",
				                                        .demangledName = "throwing",
				                                        .customName = "throwing",
				                                        .paramTypes = {},
				                                        .attrs = {}});
				if (indirect) {
					auto* callback = block->addOperation<FunctionAddressOfOperation>("throwing", "throwing", throwing,
					                                                                 OperationIdentifier(7), callee);
					auto* call = block->addOperation<IndirectCallOperation>(
					    OperationIdentifier(8), callback, std::vector<Operation*> {}, Type::v, FunctionAttributes {},
					    std::vector<IndirectCallOperation::Destructor> {{address, "cleanup", "cleanup", cleanup}},
					    true);
					REQUIRE(call->getInputArguments().empty());
					REQUIRE(call->getDestructors().front().address == address);
				} else {
					auto* call = block->addOperation<CallOperation>(
					    "throwing", "throwing", throwing, OperationIdentifier(8), std::vector<Operation*> {}, Type::v,
					    FunctionAttributes {}, callee,
					    std::vector<CallOperation::Destructor> {{address, "cleanup", "cleanup", cleanup}}, true);
					REQUIRE(call->getInputs().empty());
					REQUIRE(call->getDestructors().front().address == address);
				}
				block->addOperation<ReturnOperation>();
				analysisFunction(graph, {block});
				REQUIRE(std::ranges::find(block->getOperations(), address) == block->getOperations().end());
				const auto result = analyzeSafety(graph);
				REQUIRE(result.scalar.certified == (recorded && (safe || kind == "certified offset")));
				REQUIRE(result.pointer.relocatable == safe);
				if (!recorded) {
					REQUIRE(result.scalar.rejection == "constant_origins_not_recorded");
				} else if (!safe && kind != "certified offset") {
					REQUIRE_THAT(result.scalar.rejection,
					             Catch::Matchers::ContainsSubstring(kind == "raw heap" ? "embedded_non_null_pointer"
					                                                                   : "uncertified_scalar"));
				}
				if (!safe) {
					REQUIRE_FALSE(result.pointer.rejection.empty());
				}
			}
		}
	}
}

TEST_CASE("Legacy cache fixed point keeps runtime pointer spills arithmetic and guarded backedges", "[cache][safety]") {
	for (const auto tracking : {ConstantOriginTracking::Enabled, ConstantOriginTracking::Disabled}) {
		for (const std::string_view kind :
		     {"integer", "offset", "loop", "merge", "spill", "native spill", "direct callee", "indirect callee",
		      "selected callee", "guarded nullable", "null backedge"}) {
			CAPTURE(tracking, kind);
			NautilusFunction offsetPointer {"offset_pointer", [](val<uintptr_t> address, val<uintptr_t> index) {
				                                val<int64_t*> pointer =
				                                    address + index * cacheLiteral<sizeof(int64_t)>();
				                                return pointer;
			                                }};
			NautilusFunction alternatePointer {"alternate_pointer", [](val<uintptr_t> address, val<uintptr_t> index) {
				                                   val<int64_t*> pointer = address;
				                                   return pointer + index;
			                                   }};
			auto nativeStore = +[](uintptr_t* slot, uintptr_t address) noexcept {
				*slot = address;
			};
			std::list<compiler::CompilableFunction> functions;
			functions.emplace_back(
			    "execute",
			    details::createFunctionWrapper([kind, nativeStore, &offsetPointer, &alternatePointer](
			                                       val<int64_t*> runtime, val<uintptr_t*> scratch, val<bool> available,
			                                       val<uintptr_t> count) -> val<int64_t> {
				    auto address = static_cast<val<uintptr_t>>(runtime);
				    if (kind == "offset") {
					    address += count * cacheLiteral<sizeof(int64_t)>();
				    } else if (kind == "loop") {
					    for (val<uintptr_t> index = cacheLiteral<uintptr_t {0}>(); index < count; ++index) {
						    address += cacheLiteral<sizeof(int64_t)>();
					    }
				    } else if (kind == "merge") {
					    if (available) {
						    address = static_cast<val<uintptr_t>>(runtime + count);
					    }
				    } else if (kind == "spill" || kind == "native spill") {
					    if (kind == "native spill") {
						    invoke(nativeStore, scratch, address);
					    } else {
						    *scratch = address;
					    }
					    address = *scratch;
				    } else if (kind == "direct callee") {
					    return *offsetPointer(address, count);
				    } else if (kind == "indirect callee") {
					    return *offsetPointer.getFuncPtr()(address, count);
				    } else if (kind == "selected callee") {
					    auto callback = offsetPointer.getFuncPtr();
					    if (available) {
						    callback = alternatePointer.getFuncPtr();
					    }
					    return *callback(address, count);
				    } else if (kind == "guarded nullable" || kind == "null backedge") {
					    auto pointer = select(available, runtime, val<int64_t*>(nullptr));
					    val<int64_t> sum;
					    for (val<uintptr_t> index = cacheLiteral<uintptr_t {0}>(); index < count; ++index) {
						    if (pointer != nullptr) {
							    sum += static_cast<val<int64_t>>(*pointer);
							    ++pointer;
						    }
						    if (kind == "null backedge" && index == cacheLiteral<uintptr_t {1}>()) {
							    pointer = nullptr;
						    }
					    }
					    return sum;
				    }
				    val<int64_t*> pointer = address;
				    return *pointer;
			    }));
			Options options;
			options.setOption("ir.runPasses", false);
			const auto result = traceSafety(functions, options, tracking);
			INFO(result.scalar.rejection);
			INFO(result.pointer.rejection);
			const bool certified =
			    tracking == ConstantOriginTracking::Enabled && kind != "guarded nullable" && kind != "null backedge";
			REQUIRE(result.scalar.certified == certified);
			if (!certified && tracking == ConstantOriginTracking::Enabled) {
				REQUIRE_THAT(result.scalar.rejection, Catch::Matchers::ContainsSubstring("uncertified_scalar"));
			}
			if (tracking == ConstantOriginTracking::Disabled) {
				REQUIRE(result.scalar.rejection == "constant_origins_not_recorded");
			}
			const bool relocatable = tracking == ConstantOriginTracking::Enabled || kind == "integer" ||
			                         kind == "spill" || kind == "native spill";
			REQUIRE(result.pointer.relocatable == relocatable);
			REQUIRE(result.pointer.rejection.empty() == relocatable);
		}
	}
}

TEST_CASE("Legacy arithmetic retains uncertified offset contamination from runtime pointers", "[cache][safety]") {
	for (const auto tracking : {ConstantOriginTracking::Enabled, ConstantOriginTracking::Disabled}) {
		for (const bool certified : {false, true}) {
			CAPTURE(tracking, certified);
			Options options;
			options.setOption("ir.runPasses", false);
			std::list<compiler::CompilableFunction> functions;
			functions.emplace_back("execute", details::createFunctionWrapper([certified](val<uint8_t*> base) {
				                       const auto address = static_cast<val<uintptr_t>>(base);
				                       const auto offset =
				                           certified ? cacheLiteral<uintptr_t {37}>() : val<uintptr_t>(37);
				                       val<uint8_t*> result = address + offset;
				                       return result;
			                       }));
			const auto result = traceSafety(functions, options, tracking);
			const bool expected = certified && tracking == ConstantOriginTracking::Enabled;
			REQUIRE(result.pointer.relocatable == expected);
			REQUIRE(result.scalar.certified == expected);
			if (!expected) {
				REQUIRE_THAT(result.pointer.rejection, Catch::Matchers::ContainsSubstring("Constant"));
			}
		}
	}
}

} // namespace nautilus::engine
#endif
