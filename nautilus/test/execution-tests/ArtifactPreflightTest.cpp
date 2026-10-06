#include "nautilus/config.hpp"

#if defined(ENABLE_TRACING) && defined(ENABLE_COMPILER)
#include "nautilus/Engine.hpp"
#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/compiler/artifact/ArtifactPreflight.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
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
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/region.hpp"
#include "nautilus/select.hpp"
#include "nautilus/static.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/SSAVerifier.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <cstdint>
#include <limits>
#include <list>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace nautilus::engine {
namespace {

int32_t nativeScalarProxy(int32_t value) {
	if (value < 0) {
		throw std::runtime_error("scalar proxy");
	}
	return value + 1;
}

struct ScalarCertificate {
	bool certified = false;
	std::string rejection;
};

ScalarCertificate certifyScalars(const compiler::ir::IRGraph& graph) {
	ScalarCertificate result;
	result.certified = compiler::artifact::hasOnlyInvariantScalars(graph, &result.rejection);
	return result;
}

void requireScalarCertificate(const compiler::ir::IRGraph& graph, const std::string& cause = {}) {
	const auto certificate = certifyScalars(graph);
	REQUIRE(certificate.certified == cause.empty());
	if (cause.empty()) {
		REQUIRE(certificate.rejection.empty());
	} else {
		REQUIRE_THAT(certificate.rejection, Catch::Matchers::ContainsSubstring("cause=" + cause));
	}
}

compiler::ir::FunctionOperation* scalarCertificateFunction(compiler::ir::IRGraph& graph,
                                                           std::vector<compiler::ir::BasicBlock*> blocks,
                                                           Type result = Type::v, const std::string& name = "execute",
                                                           std::vector<compiler::ir::AllocaSpec> allocaSpecs = {}) {
	using namespace compiler::ir;
	std::vector<Type> types;
	std::vector<std::string> names;
	for (const auto* argument : blocks.front()->getArguments()) {
		types.push_back(argument->getStamp());
		names.push_back(argument->getIdentifier().toString());
	}
	auto* function = graph.addFunctionOperation(graph.getArena().create<FunctionOperation>(
	    name, std::move(blocks), types, std::move(names), result, std::move(allocaSpecs)));
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

void addIdentityTrace(tracing::ExecutionTrace& trace) {
	const auto argument = trace.setArgument(Type::i64, 0);
	tracing::Snapshot snapshot;
	trace.addReturn(snapshot, argument.type, argument);
	REQUIRE(tracing::VerifySSA(trace).valid);
}

void requireIdentityFunction(const compiler::ir::FunctionOperation* function) {
	REQUIRE(function != nullptr);
	REQUIRE(function->getOutputArg() == Type::i64);
	REQUIRE(function->getBasicBlocks().size() == 1);
	const auto* block = function->getBasicBlocks().front();
	REQUIRE(block->getArguments().size() == 1);
	REQUIRE(block->getOperations().size() == 1);
	const auto* returned = block->getOperations().front()->dynCast<compiler::ir::ReturnOperation>();
	REQUIRE(returned != nullptr);
	REQUIRE(returned->getReturnValue() == block->getArguments().front());
}

} // namespace

TEST_CASE("Artifact preflight preserves graphs and independent diagnostics", "[artifact][preflight]") {
	using namespace compiler::ir;
	IRGraph accepted("artifact-accepted"), rejected("artifact-rejected");
	for (auto* graph : {&accepted, &rejected}) {
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
		scalarCertificateFunction(*graph, {block}, Type::ptr);
	}
	const auto acceptedBefore = accepted.toString();
	const auto rejectedBefore = rejected.toString();
	std::string reason = "stale";
	std::string independent;
	REQUIRE_FALSE(compiler::artifact::hasOnlyInvariantScalars(rejected, &independent));
	const auto previous = independent;
	for (auto* graph : {&accepted, &rejected, &accepted, &rejected}) {
		const bool expected = graph == &accepted;
		REQUIRE(compiler::artifact::hasOnlyInvariantScalars(*graph, &reason) == expected);
		REQUIRE(compiler::artifact::hasOnlyInvariantScalars(*graph) == expected);
		REQUIRE(reason.empty() == expected);
		if (!expected) {
			REQUIRE_THAT(reason, Catch::Matchers::ContainsSubstring("embedded_non_null_pointer"));
		}
		REQUIRE(independent == previous);
		REQUIRE(accepted.toString() == acceptedBefore);
		REQUIRE(rejected.toString() == rejectedBefore);
	}
	for (int invalidation = 0; invalidation < 2; ++invalidation) {
		accepted.invalidateConstantOrigins();
		REQUIRE_FALSE(accepted.hasRecordedConstantOrigins());
		REQUIRE_FALSE(compiler::artifact::hasOnlyInvariantScalars(accepted, &reason));
		REQUIRE(reason == "constant_origins_not_recorded");
		REQUIRE(accepted.toString() == acceptedBefore);
	}
	REQUIRE(rejected.hasRecordedConstantOrigins());
}

TEST_CASE("Invariant scalar origins survive nested regions and trace cloning", "[artifact][constant-origin]") {
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
				result = select(select(boolean, ordinaryBoolean, boolean),
				                static_cast<val<double>>(integer + ordinaryInteger) + floating, ordinaryFloating);
			});
		});
		return result;
	});
	common::Arena arena, clonedArena;
	auto trace = tracing::TraceContext::trace(wrapper, Options {}, arena, ConstantOriginTracking::Enabled);
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

TEST_CASE("Invariant scalar replay disagreement never upgrades an ordinary constant", "[artifact][constant-origin]") {
	for (const bool initiallyCertified : {false, true}) {
		for (const bool replayCertified : {false, true}) {
			CAPTURE(initiallyCertified, replayCertified);
			int iterations = 0;
			auto wrapper = details::createFunctionWrapper([&](val<bool> condition) {
				++iterations;
				const auto origin = (iterations == 1 ? initiallyCertified : replayCertified)
				                        ? ConstantOrigin::CacheInvariant
				                        : ConstantOrigin::Unspecified;
				auto ref = tracing::traceConstant(int64_t {7}, origin);
				val<int64_t> value(ref);
				if (condition) {
					return value;
				}
				return -value;
			});
			common::Arena arena;
			auto trace = tracing::TraceContext::trace(wrapper, Options {}, arena, ConstantOriginTracking::Enabled);
			REQUIRE(iterations >= 2);
			std::size_t constants = 0;
			for (const auto* block : trace->getBlocks()) {
				for (const auto* operation : block->operations) {
					if (operation->op == tracing::Op::CONST && operation->resultType == Type::i64 &&
					    std::get<int64_t>(std::get<ConstantLiteral>(operation->input[0])) == 7) {
						++constants;
						if (initiallyCertified && replayCertified) {
							REQUIRE(operation->constantOrigin == ConstantOrigin::CacheInvariant);
						} else {
							REQUIRE(operation->constantOrigin == ConstantOrigin::Unspecified);
						}
					}
				}
			}
			REQUIRE(constants > 0);
			REQUIRE_FALSE(tracing::inTracer());
		}
	}
}

TEST_CASE("Trace conversion preserves constant origin availability in every overload", "[artifact][preflight]") {
	using namespace compiler::ir;
	for (const std::string_view entryPoint : {"single", "module", "module context"}) {
		for (const bool pooled : {false, true}) {
			common::ArenaPool irArenaPool;
			ScalarCertificate scalar;
			for (const auto tracking :
			     {ConstantOriginTracking::Enabled, ConstantOriginTracking::Disabled, ConstantOriginTracking::Enabled}) {
				CAPTURE(entryPoint, pooled, tracking);
				const bool enabled = tracking == ConstantOriginTracking::Enabled;
				common::Arena traceArena;
				Options options;
				auto wrapper = details::createFunctionWrapper([](val<int64_t> input) { return input; });
				tracing::SSACreationPhase ssa;
				tracing::TraceToIRConversionPhase conversion;
				std::shared_ptr<IRGraph> ir;
				if (entryPoint == "single") {
					std::shared_ptr<tracing::ExecutionTrace> trace =
					    enabled ? tracing::TraceContext::trace(wrapper, options, traceArena, tracking)
					            : tracing::TraceContext::trace(wrapper, options, traceArena);
					REQUIRE(trace->recordsConstantOrigins() == enabled);
					trace = ssa.apply(std::move(trace));
					REQUIRE(trace->recordsConstantOrigins() == enabled);
					REQUIRE(tracing::VerifySSA(*trace).valid);
					ir = pooled ? conversion.apply(trace, irArenaPool) : conversion.apply(trace);
				} else {
					std::list<compiler::CompilableFunction> functions;
					functions.emplace_back("execute", wrapper);
					std::shared_ptr<tracing::TraceModule> module;
					if (entryPoint == "module context") {
						tracing::TraceContext context;
						module = enabled ? context.startTrace(functions, options, traceArena, tracking)
						                 : context.startTrace(functions, options, traceArena);
					} else {
						module = enabled ? tracing::TraceContext::Trace(functions, options, traceArena, tracking)
						                 : tracing::TraceContext::Trace(functions, options, traceArena);
					}
					REQUIRE(module->getFunction("execute")->recordsConstantOrigins() == enabled);
					module = ssa.apply(std::move(module));
					REQUIRE(module->getFunction("execute")->recordsConstantOrigins() == enabled);
					REQUIRE(tracing::VerifySSA(*module->getFunction("execute")).valid);
					ir = pooled ? conversion.apply(module, irArenaPool) : conversion.apply(module);
				}
				REQUIRE_FALSE(tracing::inTracer());
				REQUIRE(ir->getFunctionOperations().size() == 1);
				requireIdentityFunction(ir->getFunctionOperation("execute"));
				const auto before = ir->toString();
				REQUIRE(ir->hasRecordedConstantOrigins() == enabled);
				scalar = certifyScalars(*ir);
				REQUIRE(scalar.certified == enabled);
				REQUIRE(scalar.rejection == (enabled ? "" : "constant_origins_not_recorded"));
				REQUIRE(ir->toString() == before);
				REQUIRE(ir->hasRecordedConstantOrigins() == enabled);
			}
		}
	}
}

TEST_CASE("Mixed trace modules cannot certify an uncalled untracked constant-free function", "[artifact][preflight]") {
	using namespace compiler::ir;
	for (const std::string unusedName : {"aaa_unused", "zzz_unused"}) {
		for (const bool pooled : {false, true}) {
			CAPTURE(unusedName, pooled);
			common::Arena traceArena;
			common::ArenaPool irArenaPool;
			auto module = std::make_shared<tracing::TraceModule>();
			auto& entry = module->addNewFunction("execute", traceArena, ConstantOriginTracking::Enabled);
			auto& unused = module->addNewFunction(unusedName, traceArena);
			addIdentityTrace(entry);
			addIdentityTrace(unused);
			REQUIRE(entry.recordsConstantOrigins());
			REQUIRE_FALSE(unused.recordsConstantOrigins());
			const auto names = module->getFunctionNames();
			REQUIRE(names.size() == 2);
			REQUIRE(names.front() == (unusedName == "aaa_unused" ? unusedName : "execute"));
			tracing::TraceToIRConversionPhase conversion;
			auto ir = pooled ? conversion.apply(module, irArenaPool) : conversion.apply(module);
			REQUIRE(ir->getFunctionOperations().size() == 2);
			requireIdentityFunction(ir->getFunctionOperation("execute"));
			requireIdentityFunction(ir->getFunctionOperation(unusedName));
			REQUIRE_FALSE(ir->hasRecordedConstantOrigins());
			const auto before = ir->toString();
			ScalarCertificate scalar;
			scalar = certifyScalars(*ir);
			REQUIRE_FALSE(scalar.certified);
			REQUIRE(scalar.rejection == "constant_origins_not_recorded");
			REQUIRE(ir->toString() == before);
			REQUIRE_FALSE(ir->hasRecordedConstantOrigins());
		}
	}
}

TEST_CASE("Default compilation cannot certify constant-free or optimized-away scalar origins",
          "[artifact][preflight]") {
	using namespace compiler::ir;
	for (const bool discardedConstants : {false, true}) {
		CAPTURE(discardedConstants);
		Options options;
		options.setOption("ir.runOptimizationPasses", true);
		common::ArenaPool traceArenaPool, irArenaPool;
		compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
		std::list<compiler::CompilableFunction> functions;
		functions.emplace_back("execute", details::createFunctionWrapper([discardedConstants](val<int64_t> input) {
			                       if (discardedConstants) {
				                       return select(cacheLiteral<true>(), input, val<int64_t>(7));
			                       }
			                       return input;
		                       }));
		ScalarCertificate scalar;
		std::size_t checks = 0;
		auto ir =
		    pipeline.compileToIR(functions, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
		                         [&](IRGraph& graph) {
			                         ++checks;
			                         REQUIRE_FALSE(graph.hasRecordedConstantOrigins());
			                         const auto before = graph.toString();
			                         scalar = certifyScalars(graph);
			                         REQUIRE_FALSE(scalar.certified);
			                         REQUIRE(scalar.rejection == "constant_origins_not_recorded");
			                         REQUIRE(graph.toString() == before);
			                         std::size_t constants = 0;
			                         for (const auto* block : graph.getFunctionOperation("execute")->getBasicBlocks()) {
				                         for (const auto* operation : block->getOperations()) {
					                         constants += operation->isConstOperation();
				                         }
			                         }
			                         REQUIRE(constants == (discardedConstants ? 2 : 0));
		                         });
		REQUIRE(checks == 1);
		requireIdentityFunction(ir->getFunctionOperation("execute"));
		REQUIRE_FALSE(ir->hasRecordedConstantOrigins());
		const auto before = ir->toString();
		scalar = certifyScalars(*ir);
		REQUIRE_FALSE(scalar.certified);
		REQUIRE(scalar.rejection == "constant_origins_not_recorded");
		REQUIRE(ir->toString() == before);
	}
}

TEST_CASE("Constant origin tracking reaches every function and nested region independently of IR scheduling",
          "[artifact][preflight]") {
	using namespace compiler::ir;
	for (const std::string_view scheduling : {"full", "argument pruning", "none", "passes disabled",
	                                          "optimization disabled", "individual passes disabled"}) {
		for (const auto tracking : {ConstantOriginTracking::Disabled, ConstantOriginTracking::Enabled}) {
			CAPTURE(scheduling, tracking);
			const bool enabled = tracking == ConstantOriginTracking::Enabled;
			Options options;
			options.setOption("ir.runPasses", scheduling != "passes disabled");
			if (scheduling == "optimization disabled") {
				options.setOption("ir.runOptimizationPasses", false);
			}
			if (scheduling == "individual passes disabled") {
				for (const auto* option : {"ir.disableConstantFolding", "ir.disableAlgebraicSimplification",
				                           "ir.disableConstantBranchFolding", "ir.disableEmptyBlockElimination",
				                           "ir.disableBlockMerging", "ir.disableDeadCodeElimination",
				                           "ir.disableBlockArgumentPruning", "ir.disableAttributeInference"}) {
					options.setOption(option, true);
				}
				for (const auto* option : {"ir.enableLocalCSE", "ir.enableStrengthReduction", "ir.enableLICM"}) {
					options.setOption(option, false);
				}
			}
			const auto optimization = scheduling == "none" ? compiler::IROptimizationLevel::None
			                          : scheduling == "argument pruning"
			                              ? compiler::IROptimizationLevel::ArgumentPruning
			                              : compiler::IROptimizationLevel::Full;
			NautilusFunction inner {"origin_inner", [](val<int64_t> input) {
				                        val<int64_t> result = input;
				                        region("outer origins", [&] {
					                        result = result + cacheLiteral<int64_t {7}>();
					                        region("inner origins",
					                               [&] { result = result + cacheLiteral<int64_t {11}>(); });
				                        });
				                        return result;
			                        }};
			NautilusFunction middle {
			    "origin_middle", [&inner](val<int64_t> input) { return inner(input + cacheLiteral<int64_t {13}>()); }};
			std::list<compiler::CompilableFunction> functions;
			functions.emplace_back("execute", details::createFunctionWrapper([&middle](val<int64_t> input) {
				                       return middle(input + cacheLiteral<int64_t {17}>());
			                       }));
			functions.emplace_back("unused_export", details::createFunctionWrapper([](val<int64_t> input) {
				                       return input + cacheLiteral<int64_t {19}>();
			                       }));
			common::ArenaPool traceArenaPool, irArenaPool;
			compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
			ScalarCertificate scalar;
			std::size_t checks = 0;
			const auto beforeOptimization = [&](IRGraph& graph) {
				++checks;
				REQUIRE(graph.hasRecordedConstantOrigins() == enabled);
				REQUIRE(graph.getFunctionOperations().size() == 4);
				const auto before = graph.toString();
				scalar = certifyScalars(graph);
				REQUIRE(scalar.certified == enabled);
				REQUIRE(scalar.rejection == (enabled ? "" : "constant_origins_not_recorded"));
				REQUIRE(graph.toString() == before);
				for (const auto* name : {"execute", "unused_export", "origin_middle", "origin_inner"}) {
					CAPTURE(name);
					const auto* function = graph.getFunctionOperation(name);
					REQUIRE(function != nullptr);
					std::size_t constants = 0, regionConstants = 0;
					for (const auto* block : function->getBasicBlocks()) {
						for (const auto* operation : block->getOperations()) {
							if (const auto* constant = operation->dynCast<ConstIntOperation>()) {
								++constants;
								regionConstants += constant->getRegionIndex() != NO_REGION;
								REQUIRE(constant->getConstantOrigin() ==
								        (enabled ? ConstantOrigin::CacheInvariant : ConstantOrigin::Unspecified));
							}
						}
					}
					const bool nested = function->getName() == "origin_inner";
					REQUIRE(constants == (nested ? 2 : 1));
					REQUIRE(regionConstants == (nested ? 2 : 0));
					if (nested) {
						const auto& regions = function->getRegionSpecs();
						REQUIRE(regions.size() == 2);
						REQUIRE(regions[0].parent == NO_REGION);
						REQUIRE(regions[1].parent == 0);
					}
				}
			};
			auto ir = enabled ? pipeline.compileToIR(functions, options.deriveModuleOptions(), nullptr, optimization,
			                                         beforeOptimization, tracking)
			                  : pipeline.compileToIR(functions, options.deriveModuleOptions(), nullptr, optimization,
			                                         beforeOptimization);
			REQUIRE(checks == 1);
			REQUIRE(ir->hasRecordedConstantOrigins() == enabled);
			REQUIRE(ir->getFunctionOperations().size() == 4);
			REQUIRE(scalar.certified == enabled);
			REQUIRE(scalar.rejection == (enabled ? "" : "constant_origins_not_recorded"));
		}
	}
}

TEST_CASE("Frontend pointer folding preserves scalar certification boundaries before optimization",
          "[artifact][preflight]") {
	using namespace compiler::ir;
	const bool fold = GENERATE(false, true);
	const bool optimize = GENERATE(false, true);
	const bool bytePointer = GENERATE(false, true);
	const auto tracking = GENERATE(ConstantOriginTracking::Disabled, ConstantOriginTracking::Enabled);
	const bool enabled = tracking == ConstantOriginTracking::Enabled;
	for (const std::string_view kind :
	     {"raw zero", "raw false", "raw subtract zero", "static zero", "static nonzero", "raw increment",
	      "raw decrement", "certified zero", "certified nonzero", "certified unsigned subtraction", "prefix increment",
	      "prefix decrement", "postfix increment", "postfix decrement"}) {
		CAPTURE(fold, optimize, bytePointer, tracking, kind);
		const bool certifiedOffset = kind.starts_with("certified");
		const bool certified = certifiedOffset || kind.starts_with("prefix") || kind.starts_with("postfix");
		const bool zero =
		    kind == "raw zero" || kind == "raw false" || kind == "raw subtract zero" || kind == "static zero";
		Options options;
		options.setOption("engine.foldStaticConstants", fold);
		options.setOption("ir.runOptimizationPasses", optimize);
		options.setOption("ir.maxPipelineIterations", 8);
		std::size_t iterations = 0;
		const auto makeWrapper = [&]<typename T>() {
			return details::createFunctionWrapper([kind, &iterations](val<T*> input, val<bool> condition) {
				++iterations;
				auto pointer = input;
				if (kind == "raw zero") {
					pointer = input + 0;
				} else if (kind == "raw false") {
					pointer = input + false;
				} else if (kind == "raw subtract zero") {
					pointer = input - 0;
				} else if (kind == "static zero") {
					static_val<std::size_t> offset = 0;
					pointer = input + offset;
				} else if (kind == "static nonzero") {
					static_val<std::size_t> offset = 1;
					pointer = input + offset;
				} else if (kind == "raw increment") {
					pointer = input + std::size_t {1};
				} else if (kind == "raw decrement") {
					pointer = input - std::size_t {1};
				} else if (kind == "certified zero") {
					pointer = input + cacheLiteral<std::size_t {0}>();
				} else if (kind == "certified nonzero") {
					pointer = input + cacheLiteral<std::size_t {1}>();
				} else if (kind == "certified unsigned subtraction") {
					pointer = input - cacheLiteral<uint32_t {1}>();
				} else if (kind == "prefix increment") {
					++pointer;
				} else if (kind == "prefix decrement") {
					--pointer;
				} else if (kind == "postfix increment") {
					pointer++;
				} else if (kind == "postfix decrement") {
					pointer--;
				}
				if (condition) {
					return pointer;
				}
				return input;
			});
		};
		std::list<compiler::CompilableFunction> functions;
		functions.emplace_back("execute",
		                       bytePointer ? makeWrapper.operator()<uint8_t>() : makeWrapper.operator()<int64_t>());
		common::ArenaPool traceArenaPool, irArenaPool;
		compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
		ScalarCertificate scalar;
		std::size_t checks = 0;
		auto ir = pipeline.compileToIR(
		    functions, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
		    [&](IRGraph& graph) {
			    ++checks;
			    REQUIRE(graph.hasRecordedConstantOrigins() == enabled);
			    const auto before = graph.toString();
			    scalar = certifyScalars(graph);
			    REQUIRE(scalar.certified == (enabled && certified));
			    if (!enabled) {
				    REQUIRE(scalar.rejection == "constant_origins_not_recorded");
			    } else if (certified) {
				    REQUIRE(scalar.rejection.empty());
			    } else {
				    REQUIRE_THAT(scalar.rejection, Catch::Matchers::ContainsSubstring("uncertified_scalar"));
			    }
			    std::size_t constants = 0, unspecified = 0, zeros = 0, multiplications = 0, subtractions = 0;
			    for (const auto* block : graph.getFunctionOperation("execute")->getBasicBlocks()) {
				    for (const auto* operation : block->getOperations()) {
					    multiplications += operation->getOperationType() == Operation::OperationType::MulOp;
					    if (operation->getOperationType() == Operation::OperationType::SubOp) {
						    ++subtractions;
						    REQUIRE(operation->getStamp() == tracing::TypeResolver<std::size_t>::to_type());
					    }
					    if (const auto* constant = operation->dynCast<ConstIntOperation>()) {
						    ++constants;
						    unspecified += constant->getConstantOrigin() == ConstantOrigin::Unspecified;
						    if (zero && constant->getValue() == 0) {
							    ++zeros;
							    REQUIRE(constant->getConstantOrigin() == ConstantOrigin::Unspecified);
						    }
					    }
				    }
			    }
			    REQUIRE((constants == 0) == (!enabled && fold && zero));
			    if (enabled) {
				    REQUIRE((unspecified == 0) == certified);
				    if (zero) {
					    REQUIRE(zeros > 0);
				    }
			    } else {
				    REQUIRE(unspecified == constants);
			    }
			    REQUIRE((multiplications > 0) == (!fold || (certifiedOffset && !bytePointer)));
			    REQUIRE((subtractions > 0) == (kind == "certified unsigned subtraction"));
			    REQUIRE(graph.toString() == before);
		    },
		    tracking);
		REQUIRE(checks == 1);
		REQUIRE(iterations >= 2);
		REQUIRE_FALSE(tracing::inTracer());
		REQUIRE(ir->hasRecordedConstantOrigins() == enabled);
		REQUIRE(scalar.certified == (enabled && certified));
	}
}

TEST_CASE("Materialized logical booleans stay uncertified in both boolean configurations", "[artifact][preflight]") {
	using namespace compiler::ir;
	const bool useOr = GENERATE(false, true);
	const bool symbolic = GENERATE(false, true);
	const bool optimize = GENERATE(false, true);
	const auto tracking = GENERATE(ConstantOriginTracking::Disabled, ConstantOriginTracking::Enabled);
	const bool enabled = tracking == ConstantOriginTracking::Enabled;
	CAPTURE(useOr, symbolic, optimize, tracking);
	Options options;
	options.setOption("ir.runOptimizationPasses", optimize);
	options.setOption("ir.maxPipelineIterations", 8);
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back(
	    "execute", details::createFunctionWrapper([useOr, symbolic](val<bool> left, val<bool> right) {
		    if (symbolic) {
			    return useOr ? select(left, cacheLiteral<true>(), right) : select(left, right, cacheLiteral<false>());
		    }
		    const bool materialized = useOr ? left || right : left && right;
		    return val<bool>(materialized);
	    }));
	common::ArenaPool traceArenaPool, irArenaPool;
	compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
	std::size_t checks = 0;
	auto ir = pipeline.compileToIR(
	    functions, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
	    [&](IRGraph& graph) {
		    ++checks;
		    const auto before = graph.toString();
		    ScalarCertificate scalar;
		    scalar = certifyScalars(graph);
		    REQUIRE(scalar.certified == (enabled && symbolic));
		    if (!enabled) {
			    REQUIRE(scalar.rejection == "constant_origins_not_recorded");
		    } else if (symbolic) {
			    REQUIRE(scalar.rejection.empty());
		    } else {
			    REQUIRE_THAT(scalar.rejection, Catch::Matchers::ContainsSubstring("ConstBooleanOp"));
			    REQUIRE_THAT(scalar.rejection, Catch::Matchers::ContainsSubstring("uncertified_scalar"));
		    }
		    std::array<std::size_t, 2> values {};
		    for (const auto* block : graph.getFunctionOperation("execute")->getBasicBlocks()) {
			    for (const auto* operation : block->getOperations()) {
				    if (const auto* constant = operation->dynCast<ConstBooleanOperation>()) {
					    ++values[constant->getValue()];
					    REQUIRE(constant->getConstantOrigin() ==
					            (enabled && symbolic ? ConstantOrigin::CacheInvariant : ConstantOrigin::Unspecified));
				    }
			    }
		    }
		    if (symbolic) {
			    REQUIRE(values[useOr] > 0);
		    } else {
			    REQUIRE(values[false] > 0);
			    REQUIRE(values[true] > 0);
		    }
		    REQUIRE(graph.toString() == before);
	    },
	    tracking);
	REQUIRE(checks == 1);
	REQUIRE(ir->hasRecordedConstantOrigins() == enabled);
}

TEST_CASE("Field stores keep uncertified offsets even when the stored value is certified", "[artifact][preflight]") {
	using namespace compiler::ir;
	struct Fields {
		int64_t first;
		int64_t second;
	};
	const bool fold = GENERATE(false, true);
	const bool optimize = GENERATE(false, true);
	const auto tracking = GENERATE(ConstantOriginTracking::Disabled, ConstantOriginTracking::Enabled);
	const bool enabled = tracking == ConstantOriginTracking::Enabled;
	for (const auto member : {&Fields::first, &Fields::second}) {
		const auto offset = field_offset(member);
		CAPTURE(fold, optimize, tracking, offset);
		Options options;
		options.setOption("engine.foldStaticConstants", fold);
		options.setOption("ir.runOptimizationPasses", optimize);
		options.setOption("ir.maxPipelineIterations", 8);
		std::size_t iterations = 0;
		std::list<compiler::CompilableFunction> functions;
		functions.emplace_back(
		    "execute", details::createFunctionWrapper([member, &iterations](val<Fields*> pointer, val<bool> condition) {
			    ++iterations;
			    pointer.set(member, cacheLiteral<int64_t {7}>());
			    if (condition) {
				    return pointer;
			    }
			    return pointer;
		    }));
		common::ArenaPool traceArenaPool, irArenaPool;
		compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
		ScalarCertificate scalar;
		std::size_t checks = 0;
		auto ir = pipeline.compileToIR(
		    functions, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
		    [&](IRGraph& graph) {
			    ++checks;
			    REQUIRE(graph.hasRecordedConstantOrigins() == enabled);
			    const auto before = graph.toString();
			    scalar = certifyScalars(graph);
			    REQUIRE_FALSE(scalar.certified);
			    if (enabled) {
				    REQUIRE_THAT(scalar.rejection, Catch::Matchers::ContainsSubstring("uncertified_scalar"));
			    } else {
				    REQUIRE(scalar.rejection == "constant_origins_not_recorded");
			    }
			    std::size_t values = 0, offsets = 0, stores = 0, additions = 0, multiplications = 0;
			    for (const auto* block : graph.getFunctionOperation("execute")->getBasicBlocks()) {
				    for (const auto* operation : block->getOperations()) {
					    stores += operation->getOperationType() == Operation::OperationType::StoreOp;
					    additions += operation->getOperationType() == Operation::OperationType::AddOp;
					    multiplications += operation->getOperationType() == Operation::OperationType::MulOp;
					    if (const auto* constant = operation->dynCast<ConstIntOperation>()) {
						    if (constant->getStamp() == Type::i64 && constant->getValue() == 7) {
							    ++values;
							    REQUIRE(constant->getConstantOrigin() ==
							            (enabled ? ConstantOrigin::CacheInvariant : ConstantOrigin::Unspecified));
						    } else if (constant->getStamp() == tracing::TypeResolver<std::size_t>::to_type() &&
						               constant->getValue() == static_cast<int64_t>(offset)) {
							    ++offsets;
							    REQUIRE(constant->getConstantOrigin() == ConstantOrigin::Unspecified);
						    }
					    }
				    }
			    }
			    REQUIRE(values == 1);
			    REQUIRE(offsets == (enabled || offset != 0 ? 1 : 0));
			    REQUIRE(stores == 1);
			    REQUIRE(additions == (offset == 0 ? 0 : 1));
			    REQUIRE(multiplications == (!fold && offset != 0 ? 1 : 0));
			    REQUIRE(graph.toString() == before);
		    },
		    tracking);
		REQUIRE(checks == 1);
		REQUIRE(iterations >= 2);
		REQUIRE_FALSE(tracing::inTracer());
		REQUIRE(ir->hasRecordedConstantOrigins() == enabled);
		REQUIRE_FALSE(scalar.certified);
	}
}

TEST_CASE("Frontend scalar conversions never certify ordinary constructors static values or mixed raw operands",
          "[artifact][preflight]") {
	using namespace compiler::ir;
	const bool fold = GENERATE(false, true);
	const bool optimize = GENERATE(false, true);
	const auto tracking = GENERATE(ConstantOriginTracking::Disabled, ConstantOriginTracking::Enabled);
	const bool enabled = tracking == ConstantOriginTracking::Enabled;
	for (const std::string_view kind :
	     {"integer constructor", "boolean constructor", "floating constructor", "enum conversion", "static conversion",
	      "mixed integer right", "mixed integer left", "mixed floating", "mixed integer floating",
	      "certified conversion"}) {
		CAPTURE(fold, optimize, tracking, kind);
		const bool certified = kind == "certified conversion";
		Options options;
		options.setOption("engine.foldStaticConstants", fold);
		options.setOption("ir.runOptimizationPasses", optimize);
		options.setOption("ir.maxPipelineIterations", 8);
		std::list<compiler::CompilableFunction> functions;
		functions.emplace_back("execute",
		                       details::createFunctionWrapper([kind](val<int64_t> input, val<double> number) {
			                       if (kind == "integer constructor") {
				                       (void) val<int64_t>(0);
			                       } else if (kind == "boolean constructor") {
				                       (void) val<bool>(false);
			                       } else if (kind == "floating constructor") {
				                       (void) val<double>(0.0);
			                       } else if (kind == "enum conversion") {
				                       enum class Value : int64_t { Zero = 0 };
				                       (void) static_cast<val<int64_t>>(val<Value>(Value::Zero));
			                       } else if (kind == "static conversion") {
				                       static_val<int64_t> value = 0;
				                       (void) val<int64_t>(value);
			                       } else if (kind == "mixed integer right") {
				                       (void) (input + int32_t {1});
			                       } else if (kind == "mixed integer left") {
				                       (void) (int32_t {1} + input);
			                       } else if (kind == "mixed floating") {
				                       (void) (number + float {1.25});
			                       } else if (kind == "mixed integer floating") {
				                       (void) (number + int32_t {1});
			                       } else if (kind == "certified conversion") {
				                       (void) static_cast<val<double>>(cacheLiteral<int32_t {1}>());
			                       }
			                       return input;
		                       }));
		common::ArenaPool traceArenaPool, irArenaPool;
		compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
		ScalarCertificate scalar;
		std::size_t checks = 0;
		auto ir = pipeline.compileToIR(
		    functions, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
		    [&](IRGraph& graph) {
			    ++checks;
			    REQUIRE(graph.hasRecordedConstantOrigins() == enabled);
			    const auto before = graph.toString();
			    scalar = certifyScalars(graph);
			    REQUIRE(scalar.certified == (enabled && certified));
			    if (!enabled) {
				    REQUIRE(scalar.rejection == "constant_origins_not_recorded");
			    } else if (certified) {
				    REQUIRE(scalar.rejection.empty());
			    } else {
				    REQUIRE_THAT(scalar.rejection, Catch::Matchers::ContainsSubstring("uncertified_scalar"));
			    }
			    std::size_t constants = 0;
			    for (const auto* block : graph.getFunctionOperation("execute")->getBasicBlocks()) {
				    for (const auto* operation : block->getOperations()) {
					    std::optional<ConstantOrigin> origin;
					    if (const auto* integer = operation->dynCast<ConstIntOperation>()) {
						    origin = integer->getConstantOrigin();
					    } else if (const auto* boolean = operation->dynCast<ConstBooleanOperation>()) {
						    origin = boolean->getConstantOrigin();
					    } else if (const auto* floating = operation->dynCast<ConstFloatOperation>()) {
						    origin = floating->getConstantOrigin();
					    }
					    if (origin) {
						    ++constants;
						    REQUIRE(*origin == (enabled && certified ? ConstantOrigin::CacheInvariant
						                                             : ConstantOrigin::Unspecified));
					    }
				    }
			    }
			    REQUIRE(constants == 1);
			    REQUIRE(graph.toString() == before);
		    },
		    tracking);
		REQUIRE(checks == 1);
		REQUIRE(ir->hasRecordedConstantOrigins() == enabled);
		REQUIRE(scalar.certified == (enabled && certified));
	}
}

TEST_CASE("Artifact scalar certification checks every scalar leaf and rejects unsupported operands",
          "[artifact][preflight]") {
	using namespace compiler::ir;
	IRGraph graph("complete-scalar-certificate");
	auto& arena = graph.getArena();
	auto* argument = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::ui64);
	auto* block = arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {argument});
	std::vector<BasicBlock*> blocks {block};
	Operation* leaf =
	    arena.create<ConstIntOperation>(arena, OperationIdentifier(1), 7, Type::ui64, ConstantOrigin::CacheInvariant);
	std::string cause;
	SECTION("certified integer boolean floating and null constants") {
		auto* boolean =
		    block->addOperation<ConstBooleanOperation>(OperationIdentifier(2), true, ConstantOrigin::CacheInvariant);
		auto* floating = block->addOperation<ConstFloatOperation>(OperationIdentifier(3), 2.5, Type::f64,
		                                                          ConstantOrigin::CacheInvariant);
		block->addOperation<ConstPtrOperation>(OperationIdentifier(4), nullptr);
		REQUIRE(boolean->getConstantOrigin() == ConstantOrigin::CacheInvariant);
		REQUIRE(floating->getConstantOrigin() == ConstantOrigin::CacheInvariant);
	}
	SECTION("ordinary integer with the same numeric value in an operand-only tree") {
		leaf = arena.create<ConstIntOperation>(arena, OperationIdentifier(2), 7, Type::ui64);
		REQUIRE(leaf->dynCast<ConstIntOperation>()->getValue() == 7);
		REQUIRE(leaf->dynCast<ConstIntOperation>()->getConstantOrigin() == ConstantOrigin::Unspecified);
		cause = "uncertified_scalar";
	}
	SECTION("ordinary boolean") {
		auto* ordinary = arena.create<ConstBooleanOperation>(arena, OperationIdentifier(2), true);
		REQUIRE(ordinary->getConstantOrigin() == ConstantOrigin::Unspecified);
		leaf = arena.create<CastOperation>(arena, OperationIdentifier(3), ordinary, Type::ui64);
		cause = "uncertified_scalar";
	}
	SECTION("ordinary floating point") {
		auto* ordinary = arena.create<ConstFloatOperation>(arena, OperationIdentifier(2), 2.5, Type::f64);
		REQUIRE(ordinary->getConstantOrigin() == ConstantOrigin::Unspecified);
		leaf = arena.create<CastOperation>(arena, OperationIdentifier(3), ordinary, Type::ui64);
		cause = "uncertified_scalar";
	}
	SECTION("raw heap pointer") {
		auto* pointer = arena.create<ConstPtrOperation>(arena, OperationIdentifier(2), argument);
		leaf = arena.create<CastOperation>(arena, OperationIdentifier(3), pointer, Type::ui64);
		cause = "embedded_non_null_pointer";
	}
	SECTION("foreign block argument with an identical identifier") {
		leaf = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::ui64);
		cause = "block_argument_outside_graph";
	}
	SECTION("unsupported operation") {
		leaf = arena.create<Operation>(Operation::OperationType::MLIR_YIELD, OperationIdentifier(2), Type::ui64);
		cause = "unsupported_operation";
	}
	SECTION("constant in an unreachable block") {
		auto* unreachable = arena.create<BasicBlock>(arena, BlockIdentifier(1), std::vector<BasicBlockArgument*> {});
		auto* ordinary = unreachable->addOperation<ConstIntOperation>(OperationIdentifier(2), 7, Type::ui64);
		unreachable->addOperation<ReturnOperation>(ordinary);
		blocks.push_back(unreachable);
		cause = "uncertified_scalar";
	}
	SECTION("constant in an uncalled internal function") {
		auto* unused = arena.create<BasicBlock>(arena, BlockIdentifier(1), std::vector<BasicBlockArgument*> {});
		auto* ordinary = unused->addOperation<ConstIntOperation>(OperationIdentifier(2), 7, Type::ui64);
		unused->addOperation<ReturnOperation>(ordinary);
		scalarCertificateFunction(graph, {unused}, Type::ui64, "unused");
		cause = "uncertified_scalar";
	}
	auto* sum = block->addOperation<AddOperation>(OperationIdentifier(5), argument, leaf);
	block->addOperation<ReturnOperation>(sum);
	scalarCertificateFunction(graph, std::move(blocks), Type::ui64);
	ScalarCertificate pass;
	pass = certifyScalars(graph);
	const auto& rejection = pass.rejection;
	REQUIRE(pass.certified == cause.empty());
	if (cause.empty()) {
		REQUIRE(rejection.empty());
	} else {
		REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("cause=" + cause));
		REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("function="));
	}
}

TEST_CASE("Artifact scalar certification validates branch ownership and argument schemas", "[artifact][preflight]") {
	using namespace compiler::ir;
	IRGraph graph("scalar-certificate-branches");
	auto& arena = graph.getArena();
	auto* input = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::ui64);
	auto* output = arena.create<BasicBlockArgument>(OperationIdentifier(1), Type::ui64);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {input});
	auto* next = arena.create<BasicBlock>(arena, BlockIdentifier(1), std::vector<BasicBlockArgument*> {output});
	next->addOperation<ReturnOperation>(output);
	std::vector<BasicBlock*> blocks {entry, next};
	std::vector<Operation*> arguments {input};
	std::string cause;
	SECTION("valid branch arguments") {
	}
	SECTION("foreign target") {
		blocks.pop_back();
		cause = "branch_target_outside_function";
	}
	SECTION("target belongs to another function") {
		blocks.pop_back();
		scalarCertificateFunction(graph, {next}, Type::ui64, "foreign");
		cause = "branch_target_outside_function";
	}
	SECTION("missing branch argument") {
		arguments.clear();
		cause = "branch_argument_count_mismatch";
	}
	SECTION("wrong branch argument type") {
		arguments[0] =
		    entry->addOperation<ConstBooleanOperation>(OperationIdentifier(2), true, ConstantOrigin::CacheInvariant);
		cause = "branch_argument_type_mismatch=0";
	}
	entry->addNextBlock(next, arguments);
	scalarCertificateFunction(graph, std::move(blocks), Type::ui64);
	ScalarCertificate pass;
	pass = certifyScalars(graph);
	const auto& rejection = pass.rejection;
	REQUIRE(pass.certified == cause.empty());
	if (cause.empty()) {
		REQUIRE(rejection.empty());
	} else {
		REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("cause=" + cause));
	}
}

TEST_CASE("Artifact scalar certification rejects shared definitions and cross-function operands",
          "[artifact][preflight]") {
	using namespace compiler::ir;
	const std::string_view kind =
	    GENERATE("same owner tree", "foreign argument", "foreign operation", "foreign nested operand",
	             "shared block operation", "shared arena operand", "shared argument", "same function shared operation");
	const bool reverse = GENERATE(false, true);
	CAPTURE(kind, reverse);
	IRGraph graph("scalar-certificate-ownership");
	auto& arena = graph.getArena();
	auto* input = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::ui64);
	auto* foreignInput =
	    kind == "shared argument" ? input : arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::ui64);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {input});
	auto* foreign =
	    arena.create<BasicBlock>(arena, BlockIdentifier(1), std::vector<BasicBlockArgument*> {foreignInput});
	auto* foreignValue =
	    foreign->addOperation<ConstIntOperation>(OperationIdentifier(1), 7, Type::ui64, ConstantOrigin::CacheInvariant);
	auto* literal =
	    arena.create<ConstIntOperation>(arena, OperationIdentifier(1), 7, Type::ui64, ConstantOrigin::CacheInvariant);
	Operation* leaf = arena.create<CastOperation>(arena, OperationIdentifier(2), literal, Type::ui64);
	std::string cause;
	if (kind == "foreign argument") {
		leaf = foreignInput;
		cause = "block_argument_outside_function";
	} else if (kind == "foreign operation") {
		leaf = foreignValue;
		cause = "operand_outside_function";
	} else if (kind == "foreign nested operand") {
		leaf = arena.create<CastOperation>(arena, OperationIdentifier(2), foreignValue, Type::ui64);
		cause = "operand_outside_function";
	} else if (kind == "shared block operation" || kind == "same function shared operation") {
		entry->addOperation(foreignValue);
		cause = "shared_operation";
	} else if (kind == "shared arena operand") {
		cause = "operand_outside_function";
	} else if (kind == "shared argument") {
		cause = "shared_operation";
	}
	auto* sum = entry->addOperation<AddOperation>(OperationIdentifier(3), input, leaf);
	auto* repeated = entry->addOperation<AddOperation>(OperationIdentifier(4), sum, leaf);
	entry->addOperation<ReturnOperation>(repeated);
	foreign->addOperation<ReturnOperation>(kind == "shared arena operand" ? leaf : foreignValue);
	if (kind == "same function shared operation") {
		scalarCertificateFunction(graph, {entry, foreign}, Type::ui64);
	} else if (reverse) {
		scalarCertificateFunction(graph, {foreign}, Type::ui64, "foreign");
		scalarCertificateFunction(graph, {entry}, Type::ui64);
	} else {
		scalarCertificateFunction(graph, {entry}, Type::ui64);
		scalarCertificateFunction(graph, {foreign}, Type::ui64, "foreign");
	}
	requireScalarCertificate(graph, cause);
}

TEST_CASE("Artifact function definitions require consistent table bindings", "[artifact][preflight]") {
	using namespace compiler::ir;
	const std::string_view kind =
	    GENERATE("valid", "unbound root", "unbound unused", "native binding", "other definition binding");
	CAPTURE(kind);
	IRGraph graph("scalar-certificate-definition-bindings");
	auto& arena = graph.getArena();
	auto* input = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::i32);
	auto* block = arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {input});
	block->addOperation<ReturnOperation>(input);
	std::string cause;
	if (kind == "unbound root") {
		graph.addFunctionOperation(arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {block},
		                                                           std::vector<Type> {Type::i32},
		                                                           std::vector<std::string> {"input"}, Type::i32));
		cause = "missing_function_binding";
	} else {
		auto* root = scalarCertificateFunction(graph, {block}, Type::i32);
		const auto id = graph.getFunctionTable().findByDefinition(root);
		if (kind == "native binding") {
			auto& target = graph.getFunctionTableMut().getMut(id);
			target = FunctionTarget(id, target.getName(),
			                        NativeTarget {.address = reinterpret_cast<void*>(nativeScalarProxy),
			                                      .resultType = Type::i32,
			                                      .paramTypes = {Type::i32},
			                                      .attrs = {}});
			cause = "mismatched_function_binding=";
		} else if (kind == "unbound unused" || kind == "other definition binding") {
			auto* unused = arena.create<BasicBlock>(arena, BlockIdentifier(1), std::vector<BasicBlockArgument*> {});
			unused->addOperation<ReturnOperation>();
			if (kind == "unbound unused") {
				graph.addFunctionOperation(arena.create<FunctionOperation>("unused", std::vector<BasicBlock*> {unused},
				                                                           std::vector<Type> {},
				                                                           std::vector<std::string> {}, Type::v));
				cause = "missing_function_binding";
			} else {
				auto* definition = scalarCertificateFunction(graph, {unused}, Type::v, "unused");
				graph.getFunctionTableMut().getMut(id).setDefinition(definition);
				cause = "mismatched_function_binding=";
			}
		}
	}
	requireScalarCertificate(graph, cause);
	std::list<compiler::CompilableFunction> roots;
	roots.emplace_back("execute", details::createFunctionWrapper([](val<int32_t> value) { return value; }),
	                   std::unordered_map<std::string, std::string> {},
	                   compiler::CompilableFunction::Signature {Type::i32, {Type::i32}});
	if (cause.empty()) {
		REQUIRE_NOTHROW(compiler::artifact::validateArtifactPreflight(graph, roots));
	} else {
		REQUIRE_THROWS_MATCHES(compiler::artifact::validateArtifactPreflight(graph, roots), RuntimeException,
		                       Catch::Matchers::MessageMatches(Catch::Matchers::ContainsSubstring(cause)));
		if (kind == "unbound unused") {
			REQUIRE_NOTHROW(compiler::artifact::validateArtifactRoots(graph, roots));
		}
	}
}

TEST_CASE("Artifact allocations use only valid specs from their owning function", "[artifact][preflight]") {
	using namespace compiler::ir;
	const std::string_view kind =
	    GENERATE("block allocation", "operand-only allocation", "missing table", "index outside table", "foreign table",
	             "zero size", "zero alignment", "non-power-of-two alignment", "overflowing alignment",
	             "oversized allocation", "invalid unused spec");
	CAPTURE(kind);
	IRGraph graph("scalar-certificate-allocations");
	auto& arena = graph.getArena();
	auto* block = arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {});
	std::vector<AllocaSpec> specs {{8, 8}};
	uint32_t index = 0;
	std::string cause;
	if (kind == "missing table" || kind == "foreign table") {
		specs.clear();
		cause = "alloca_index_outside_function=";
		if (kind == "foreign table") {
			auto* unused = arena.create<BasicBlock>(arena, BlockIdentifier(1), std::vector<BasicBlockArgument*> {});
			unused->addOperation<ReturnOperation>();
			scalarCertificateFunction(graph, {unused}, Type::v, "unused", {{8, 8}});
		}
	} else if (kind == "index outside table") {
		index = 1;
		cause = "alloca_index_outside_function=";
	} else if (kind != "block allocation" && kind != "operand-only allocation") {
		cause = "invalid_alloca_spec=";
		if (kind == "zero size") {
			specs[0].size = 0;
		} else if (kind == "zero alignment") {
			specs[0].align = 0;
		} else if (kind == "non-power-of-two alignment") {
			specs[0].align = 3;
		} else if (kind == "overflowing alignment") {
			specs[0].align = static_cast<std::size_t>(uint64_t {1} << 32);
		} else if (kind == "oversized allocation") {
			specs[0].size = std::numeric_limits<std::size_t>::max();
		} else {
			specs.push_back({8, 3});
		}
	}
	auto* allocation = kind == "operand-only allocation"
	                       ? arena.create<AllocaOperation>(arena, OperationIdentifier(0), index)
	                       : block->addOperation<AllocaOperation>(OperationIdentifier(0), index);
	block->addOperation<ReturnOperation>(allocation);
	scalarCertificateFunction(graph, {block}, Type::ptr, "execute", std::move(specs));
	requireScalarCertificate(graph, cause);
}

TEST_CASE("Artifact returns match the owning function result and the actual returned operand",
          "[artifact][preflight]") {
	using namespace compiler::ir;
	const std::string_view kind =
	    GENERATE("value", "void", "void function returns value", "missing value", "wrong value type",
	             "rewritten operand type", "wrong result type", "two return values");
	CAPTURE(kind);
	IRGraph graph("scalar-certificate-returns");
	auto& arena = graph.getArena();
	auto* integer = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::i32);
	auto* pointer = arena.create<BasicBlockArgument>(OperationIdentifier(1), Type::ptr);
	auto* block =
	    arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {integer, pointer});
	Type result = Type::i32;
	std::string cause;
	if (kind == "void" || kind == "void function returns value") {
		result = Type::v;
	} else if (kind == "wrong result type") {
		result = Type::ptr;
	}
	if (kind == "two return values") {
		block->addOperation(arena.create<Operation>(arena, Operation::OperationType::ReturnOp, OperationIdentifier(0),
		                                            Type::i32, std::vector<Operation*> {integer, integer}));
		cause = "invalid_return_arity";
	} else {
		auto* returned = kind == "void" || kind == "missing value"
		                     ? block->addOperation<ReturnOperation>()
		                     : block->addOperation<ReturnOperation>(kind == "wrong value type" ? pointer : integer);
		if (kind == "rewritten operand type") {
			returned->setReturnValue(pointer);
		}
		if (kind != "value" && kind != "void") {
			cause = "return_signature_mismatch";
		}
	}
	scalarCertificateFunction(graph, {block}, result);
	requireScalarCertificate(graph, cause);
}

TEST_CASE("Artifact scalar certification validates called addressed and unused function targets",
          "[artifact][preflight]") {
	using namespace compiler::ir;
	for (const std::string_view use : {"call", "function address", "unused"}) {
		for (const std::string_view kind :
		     {"valid", "missing target", "missing native address", "missing internal definition",
		      "foreign internal definition", "inconsistent internal binding"}) {
			if (use == "unused" && kind == "missing target") {
				continue;
			}
			CAPTURE(use, kind);
			IRGraph graph("scalar-certificate-function-targets");
			auto& arena = graph.getArena();
			auto* argument = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::i32);
			auto* block =
			    arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {argument});
			auto* native = reinterpret_cast<void*>(nativeScalarProxy);
			const auto target = graph.internCallee({.key = native,
			                                        .mangledName = "nativeScalarProxy",
			                                        .demangledName = "nativeScalarProxy",
			                                        .customName = "nativeScalarProxy",
			                                        .resultType = Type::i32,
			                                        .paramTypes = {Type::i32},
			                                        .attrs = {}});
			std::string cause;
			if (kind == "missing target") {
				cause = "missing_function_target=";
			} else if (kind == "missing native address") {
				graph.getFunctionTableMut().getMut(target).getNativeMut()->address = nullptr;
				cause = "missing_native_address=";
			} else if (kind == "missing internal definition" || kind == "foreign internal definition") {
				FunctionOperation* definition = nullptr;
				if (kind == "foreign internal definition") {
					definition = arena.create<FunctionOperation>("foreign", std::vector<BasicBlock*> {block},
					                                             std::vector<Type> {Type::i32},
					                                             std::vector<std::string> {"value"}, Type::i32);
				}
				auto& entry = graph.getFunctionTableMut().getMut(target);
				entry = FunctionTarget(target, entry.getName(), definition);
				cause = "internal_definition_outside_graph=";
			} else if (kind == "inconsistent internal binding") {
				auto* input = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::i32);
				auto* body =
				    arena.create<BasicBlock>(arena, BlockIdentifier(1), std::vector<BasicBlockArgument*> {input});
				body->addOperation<ReturnOperation>(input);
				auto* definition = scalarCertificateFunction(graph, {body}, Type::i32, "internal");
				auto& entry = graph.getFunctionTableMut().getMut(target);
				entry = FunctionTarget(target, entry.getName(), definition);
				graph.defineFunction(target, definition);
				REQUIRE(graph.getFunctionTable().findByDefinition(definition) != target);
				cause = "internal_definition_binding_mismatch=";
			}
			const auto callee = kind == "missing target" ? INVALID_FUNCTION_ID : target;
			if (use == "call") {
				block->addOperation<CallOperation>("nativeScalarProxy", "nativeScalarProxy", native,
				                                   OperationIdentifier(1), std::vector<Operation*> {argument},
				                                   Type::i32, FunctionAttributes {}, callee);
			} else if (use == "function address") {
				block->addOperation<FunctionAddressOfOperation>("nativeScalarProxy", "nativeScalarProxy", native,
				                                                OperationIdentifier(1), callee);
			}
			block->addOperation<ReturnOperation>();
			scalarCertificateFunction(graph, {block});
			ScalarCertificate pass;
			pass = certifyScalars(graph);
			const auto& rejection = pass.rejection;
			REQUIRE(pass.certified == cause.empty());
			if (cause.empty()) {
				REQUIRE(rejection.empty());
			} else {
				REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("cause=" + cause));
			}
		}
	}
}

TEST_CASE("Artifact scalar certification rejects mismatched native call signatures", "[artifact][preflight]") {
	using namespace compiler::ir;
	IRGraph graph("scalar-certificate-signatures");
	auto& arena = graph.getArena();
	auto* argument = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::i32);
	auto* block = arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {argument});
	auto* native = reinterpret_cast<void*>(nativeScalarProxy);
	const auto target = graph.internCallee({.key = native,
	                                        .mangledName = "nativeScalarProxy",
	                                        .demangledName = "nativeScalarProxy",
	                                        .customName = "nativeScalarProxy",
	                                        .resultType = Type::i32,
	                                        .paramTypes = {Type::i32},
	                                        .attrs = {}});
	std::vector<Operation*> arguments {argument};
	Type result = Type::i32;
	std::string cause = "callee_signature_mismatch";
	SECTION("wrong result type") {
		result = Type::i64;
	}
	SECTION("missing argument") {
		arguments.clear();
	}
	SECTION("wrong argument type") {
		arguments[0] = block->addOperation<ConstIntOperation>(OperationIdentifier(1), 7, Type::i64,
		                                                      ConstantOrigin::CacheInvariant);
		cause = "callee_argument_type_mismatch=0";
	}
	block->addOperation<CallOperation>("nativeScalarProxy", "nativeScalarProxy", native, OperationIdentifier(2),
	                                   arguments, result, FunctionAttributes {}, target);
	block->addOperation<ReturnOperation>();
	scalarCertificateFunction(graph, {block});
	ScalarCertificate pass;
	pass = certifyScalars(graph);
	const auto& rejection = pass.rejection;
	REQUIRE_FALSE(pass.certified);
	REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("cause=" + cause));
}

TEST_CASE("Artifact scalar certification walks direct and indirect destructor-only operand trees",
          "[artifact][preflight]") {
	using namespace compiler::ir;
	for (const bool indirect : {false, true}) {
		DYNAMIC_SECTION("indirect=" << indirect) {
			IRGraph graph("scalar-certificate-cleanup-trees");
			auto& arena = graph.getArena();
			auto* base = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::ptr);
			auto* block = arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {base});
			Operation* offset = arena.create<ConstIntOperation>(arena, OperationIdentifier(1), 7, Type::ui64,
			                                                    ConstantOrigin::CacheInvariant);
			std::string cause;
			bool rawPointer = false, missingAddress = false, wrongAddressType = false, allocationAddress = false;
			Operation* foreignAddress = nullptr;
			auto* cleanup = reinterpret_cast<void*>(+[](int32_t* pointer) noexcept { ++*pointer; });
			SECTION("certified operand tree") {
			}
			SECTION("ordinary integer only in cleanup") {
				offset = arena.create<ConstIntOperation>(arena, OperationIdentifier(1), 7, Type::ui64);
				cause = "uncertified_scalar";
			}
			SECTION("ordinary boolean only in cleanup") {
				auto* bit = arena.create<ConstBooleanOperation>(arena, OperationIdentifier(1), true);
				offset = arena.create<CastOperation>(arena, OperationIdentifier(2), bit, Type::ui64);
				cause = "uncertified_scalar";
			}
			SECTION("ordinary floating point only in cleanup") {
				auto* floating = arena.create<ConstFloatOperation>(arena, OperationIdentifier(1), 7.0, Type::f64);
				offset = arena.create<CastOperation>(arena, OperationIdentifier(2), floating, Type::ui64);
				cause = "uncertified_scalar";
			}
			SECTION("raw pointer only in cleanup") {
				rawPointer = true;
				cause = "embedded_non_null_pointer";
			}
			SECTION("non-pointer cleanup address") {
				wrongAddressType = true;
				cause = "invalid_cleanup_address_type";
			}
			SECTION("allocation only in cleanup") {
				allocationAddress = true;
			}
			SECTION("foreign function argument only in cleanup") {
				auto* input = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::ptr);
				auto* body =
				    arena.create<BasicBlock>(arena, BlockIdentifier(1), std::vector<BasicBlockArgument*> {input});
				body->addOperation<ReturnOperation>(input);
				scalarCertificateFunction(graph, {body}, Type::ptr, "cleanup_owner");
				foreignAddress = input;
				cause = "block_argument_outside_function";
			}
			SECTION("foreign function argument in a recursive cleanup expression") {
				auto* input = arena.create<BasicBlockArgument>(OperationIdentifier(1), Type::ui64);
				auto* body =
				    arena.create<BasicBlock>(arena, BlockIdentifier(1), std::vector<BasicBlockArgument*> {input});
				body->addOperation<ReturnOperation>(input);
				scalarCertificateFunction(graph, {body}, Type::ui64, "cleanup_owner");
				offset = input;
				cause = "block_argument_outside_function";
			}
			SECTION("missing cleanup address") {
				missingAddress = true;
				cause = "null_operand";
			}
			SECTION("missing cleanup function") {
				cleanup = nullptr;
				cause = "missing_cleanup_function";
			}
			auto* integer = arena.create<CastOperation>(arena, OperationIdentifier(3), base, Type::ui64);
			auto* sum = arena.create<AddOperation>(arena, OperationIdentifier(4), integer, offset);
			Operation* address = arena.create<CastOperation>(arena, OperationIdentifier(5), sum, Type::ptr);
			if (rawPointer) {
				address = arena.create<ConstPtrOperation>(arena, OperationIdentifier(5), base);
			} else if (missingAddress) {
				address = nullptr;
			} else if (wrongAddressType) {
				address = sum;
			} else if (allocationAddress) {
				address = arena.create<AllocaOperation>(arena, OperationIdentifier(5), 0);
			} else if (foreignAddress != nullptr) {
				address = foreignAddress;
			}
			auto* throwing = reinterpret_cast<void*>(+[]() { throw std::runtime_error("scalar cleanup metadata"); });
			const auto target = graph.internCallee({.key = throwing,
			                                        .mangledName = "throwing",
			                                        .demangledName = "throwing",
			                                        .customName = "throwing",
			                                        .paramTypes = {},
			                                        .attrs = {}});
			if (indirect) {
				auto* callback = block->addOperation<FunctionAddressOfOperation>("throwing", "throwing", throwing,
				                                                                 OperationIdentifier(6), target);
				auto* call = block->addOperation<IndirectCallOperation>(
				    OperationIdentifier(7), callback, std::vector<Operation*> {}, Type::v, FunctionAttributes {},
				    std::vector<IndirectCallOperation::Destructor> {{address, "cleanup", "cleanup", cleanup}}, true);
				REQUIRE(call->getInputArguments().empty());
				REQUIRE(call->getDestructors().front().address == address);
			} else {
				auto* call = block->addOperation<CallOperation>(
				    "throwing", "throwing", throwing, OperationIdentifier(7), std::vector<Operation*> {}, Type::v,
				    FunctionAttributes {}, target,
				    std::vector<CallOperation::Destructor> {{address, "cleanup", "cleanup", cleanup}}, true);
				REQUIRE(call->getInputs().empty());
				REQUIRE(call->getDestructors().front().address == address);
			}
			block->addOperation<ReturnOperation>();
			const std::vector<AllocaSpec> specs =
			    allocationAddress ? std::vector<AllocaSpec> {{8, 8}} : std::vector<AllocaSpec> {};
			scalarCertificateFunction(graph, {block}, Type::v, "execute", specs);
			REQUIRE(std::ranges::find(block->getOperations(), address) == block->getOperations().end());
			REQUIRE(std::ranges::find(block->getOperations(), offset) == block->getOperations().end());
			ScalarCertificate pass;
			pass = certifyScalars(graph);
			const auto& rejection = pass.rejection;
			REQUIRE(pass.certified == cause.empty());
			if (cause.empty()) {
				REQUIRE(rejection.empty());
			} else {
				REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("cause=" + cause));
			}
		}
	}
}

TEST_CASE("Artifact scalar certification observes region expressions before optimization discards origins",
          "[artifact][preflight]") {
	using namespace compiler::ir;
	for (const std::string_view kind : {"certified", "ordinary folded operand", "unselected ordinary operand"}) {
		CAPTURE(kind);
		Options options;
		options.setOption("ir.runOptimizationPasses", true);
		options.setOption("ir.disableConstantFolding", false);
		common::ArenaPool traceArenaPool, irArenaPool;
		compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
		std::list<compiler::CompilableFunction> functions;
		functions.emplace_back("execute", details::createFunctionWrapper([kind](val<int64_t> input) {
			                       val<int64_t> result;
			                       region("folded scalars", [&] {
				                       region("inner folded scalars", [&] {
					                       auto left = cacheLiteral<int64_t {21}>();
					                       auto right = kind == "ordinary folded operand"
					                                        ? val<int64_t>(21)
					                                        : cacheLiteral<int64_t {21}>();
					                       auto unused = kind == "unselected ordinary operand"
					                                         ? val<int64_t>(7)
					                                         : cacheLiteral<int64_t {7}>();
					                       auto folded = left + right;
					                       result = input + select(cacheLiteral<true>(), folded, unused);
				                       });
			                       });
			                       return result;
		                       }));
		ScalarCertificate scalar;
		ScalarCertificate beforeOptimization;
		std::size_t checks = 0, additionsBefore = 0;
		auto ir = pipeline.compileToIR(
		    functions, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
		    [&](IRGraph& graph) {
			    ++checks;
			    REQUIRE(graph.hasRecordedConstantOrigins());
			    const auto before = graph.toString();
			    scalar = certifyScalars(graph);
			    beforeOptimization = scalar;
			    const auto& rejection = scalar.rejection;
			    REQUIRE(scalar.certified == (kind == "certified"));
			    if (kind != "certified") {
				    REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("uncertified_scalar"));
			    } else {
				    REQUIRE(rejection.empty());
			    }
			    scalar = certifyScalars(graph);
			    REQUIRE(scalar.certified == beforeOptimization.certified);
			    REQUIRE(scalar.rejection == beforeOptimization.rejection);
			    REQUIRE(graph.toString() == before);
			    const auto* function = graph.getFunctionOperation("execute");
			    REQUIRE(function != nullptr);
			    REQUIRE(function->getRegionSpecs().size() == 2);
			    std::size_t regionConstants = 0;
			    for (const auto* block : function->getBasicBlocks()) {
				    for (const auto* operation : block->getOperations()) {
					    additionsBefore += operation->getOperationType() == Operation::OperationType::AddOp;
					    if (const auto* constant = operation->dynCast<ConstIntOperation>();
					        constant != nullptr && constant->getRegionIndex() != NO_REGION) {
						    ++regionConstants;
						    const bool ordinary = kind == "unselected ordinary operand" && constant->getValue() == 7;
						    if (kind != "ordinary folded operand") {
							    REQUIRE(constant->getConstantOrigin() ==
							            (ordinary ? ConstantOrigin::Unspecified : ConstantOrigin::CacheInvariant));
						    }
					    }
				    }
			    }
			    REQUIRE(regionConstants >= 3);
		    },
		    ConstantOriginTracking::Enabled);
		REQUIRE(checks == 1);
		REQUIRE(ir->hasRecordedConstantOrigins());
		REQUIRE(scalar.certified == beforeOptimization.certified);
		REQUIRE(scalar.rejection == beforeOptimization.rejection);
		std::size_t additionsAfter = 0, foldedConstants = 0;
		for (const auto* block : ir->getFunctionOperation("execute")->getBasicBlocks()) {
			for (const auto* operation : block->getOperations()) {
				additionsAfter += operation->getOperationType() == Operation::OperationType::AddOp;
				if (const auto* constant = operation->dynCast<ConstIntOperation>()) {
					foldedConstants += constant->getValue() == 42;
				}
				REQUIRE(operation->getOperationType() != Operation::OperationType::SelectOp);
			}
		}
		REQUIRE(additionsBefore > additionsAfter);
		REQUIRE(additionsAfter == 1);
		REQUIRE(foldedConstants == 1);
	}
}

TEST_CASE("Artifact roots match traced entry arguments and return types before optimization", "[artifact][preflight]") {
	using namespace compiler::ir;
	using Signature = compiler::CompilableFunction::Signature;
	const std::string_view kind =
	    GENERATE("valid", "missing signature", "wrong result", "wrong arguments", "missing argument", "missing root",
	             "empty name", "duplicate name", "empty exports");
	const bool optimize = GENERATE(false, true);
	CAPTURE(kind, optimize);
	Options options;
	options.setOption("ir.runOptimizationPasses", optimize);
	common::ArenaPool traceArenaPool, irArenaPool;
	compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", details::createFunctionWrapper([](val<int64_t> value, val<void*>) {
		                       return value + cacheLiteral<int64_t {7}>();
	                       }),
	                       std::unordered_map<std::string, std::string> {},
	                       Signature {Type::i64, {Type::i64, Type::ptr}});
	functions.emplace_back("notify", details::createFunctionWrapper([](val<void*>) {}),
	                       std::unordered_map<std::string, std::string> {}, Signature {Type::v, {Type::ptr}});
	std::optional<Signature> signature = *functions.front().getSignature();
	std::string name = "execute";
	std::string reason = "Traced root signature";
	if (kind == "missing signature") {
		signature.reset();
		reason = "no declared signature";
	} else if (kind == "wrong result") {
		signature->returnType = Type::ptr;
	} else if (kind == "wrong arguments") {
		signature->argumentTypes = {Type::ptr, Type::i64};
	} else if (kind == "missing argument") {
		signature->argumentTypes.pop_back();
	} else if (kind == "missing root") {
		name = "not_traced";
	} else if (kind == "empty name") {
		name.clear();
		reason = "Invalid artifact export name";
	} else if (kind == "duplicate name") {
		reason = "Invalid artifact export name";
	} else if (kind == "empty exports") {
		reason = "Artifact exports are empty";
	}
	std::list<compiler::CompilableFunction> roots;
	roots.emplace_back(name, functions.front().getFunction(), std::unordered_map<std::string, std::string> {},
	                   signature);
	roots.push_back(functions.back());
	if (kind == "duplicate name") {
		roots.push_back(roots.front());
	} else if (kind == "empty exports") {
		roots.clear();
	}
	std::size_t checks = 0;
	const auto beforeOptimization = [&](IRGraph& graph) {
		++checks;
		const auto* entry = graph.getFunctionOperation("execute");
		REQUIRE(entry != nullptr);
		REQUIRE(entry->getInputArgs().empty());
		REQUIRE(entry->getEntryBlock()->getArguments().size() == 2);
		REQUIRE(entry->getOutputArg() == Type::i64);
		const auto before = graph.toString();
		if (kind == "valid") {
			REQUIRE_NOTHROW(compiler::artifact::validateArtifactPreflight(graph, roots));
		} else {
			REQUIRE_THROWS_MATCHES(compiler::artifact::validateArtifactPreflight(graph, roots), RuntimeException,
			                       Catch::Matchers::MessageMatches(Catch::Matchers::ContainsSubstring(reason)));
		}
		REQUIRE(graph.toString() == before);
	};
	auto ir =
	    pipeline.compileToIR(functions, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
	                         beforeOptimization, ConstantOriginTracking::Enabled);
	REQUIRE(ir != nullptr);
	REQUIRE(checks == 1);
}

TEST_CASE("Standalone artifact preflight rejects allocation metadata without origin evidence",
          "[artifact][preflight]") {
	using namespace compiler::ir;
	IRGraph graph("allocation-metadata");
	auto& arena = graph.getArena();
	auto* block = arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {});
	auto* allocation = block->addOperation<AllocaOperation>(OperationIdentifier(0), 0);
	block->addOperation<ReturnOperation>(allocation);
	scalarCertificateFunction(graph, {block}, Type::ptr, "execute", {{16, 8}});
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back(
	    "execute", [] {}, std::unordered_map<std::string, std::string> {}, nullptr, SourceLocation {},
	    compiler::CompilableFunction::Signature {Type::ptr, {}});
	REQUIRE(compiler::artifact::hasOnlyInvariantScalars(graph));
	REQUIRE_THROWS_MATCHES(
	    compiler::artifact::validateArtifactPreflight(graph, functions), RuntimeException,
	    Catch::Matchers::MessageMatches(Catch::Matchers::ContainsSubstring("allocation_metadata_origins_unavailable")));
}

TEST_CASE("Artifact root exports reject sanitized emission names before optimization", "[artifact][preflight]") {
	using namespace compiler::ir;
	const std::string name = GENERATE("execute_", "execute value", "9execute");
	const bool optimize = GENERATE(false, true);
	CAPTURE(name, optimize);
	Options options;
	options.setOption("ir.runOptimizationPasses", optimize);
	common::ArenaPool traceArenaPool, irArenaPool;
	compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back(name, details::createFunctionWrapper([](val<int64_t> value) { return value; }),
	                       std::unordered_map<std::string, std::string> {},
	                       compiler::CompilableFunction::Signature {Type::i64, {Type::i64}});
	std::size_t checks = 0;
	const auto beforeOptimization = [&](IRGraph& graph) {
		++checks;
		const auto* root = graph.getFunctionOperation(name);
		REQUIRE(root != nullptr);
		REQUIRE(graph.getEmissionName(root) != name);
		requireScalarCertificate(graph);
		compiler::artifact::validateArtifactPreflight(graph, functions);
	};
	REQUIRE_THROWS_MATCHES(pipeline.compileToIR(functions, options.deriveModuleOptions(), nullptr,
	                                            compiler::IROptimizationLevel::Full, beforeOptimization,
	                                            ConstantOriginTracking::Enabled),
	                       RuntimeException,
	                       Catch::Matchers::MessageMatches(
	                           Catch::Matchers::ContainsSubstring("export name does not match traced emission")));
	REQUIRE(checks == 1);
	REQUIRE_FALSE(tracing::inTracer());
}

TEST_CASE("Artifact root exports cannot resolve to a same-signature helper after emission uniquification",
          "[artifact][preflight]") {
	using namespace compiler::ir;
	const bool optimize = GENERATE(false, true);
	CAPTURE(optimize);
	Options options;
	options.setOption("ir.runOptimizationPasses", optimize);
	NautilusFunction helper {"execute_", [](val<int64_t> value) { return value + cacheLiteral<int64_t {7}>(); }};
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", details::createFunctionWrapper([&helper](val<int64_t> value) {
		                       return helper(value) + cacheLiteral<int64_t {11}>();
	                       }),
	                       std::unordered_map<std::string, std::string> {},
	                       compiler::CompilableFunction::Signature {Type::i64, {Type::i64}});
	common::ArenaPool traceArenaPool, irArenaPool;
	compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
	std::size_t checks = 0;
	const auto beforeOptimization = [&](IRGraph& graph) {
		++checks;
		const auto* root = graph.getFunctionOperation("execute");
		const auto* callee = graph.getFunctionOperation("execute_");
		REQUIRE(root != nullptr);
		REQUIRE(callee != nullptr);
		REQUIRE(root->getOutputArg() == callee->getOutputArg());
		REQUIRE(root->getEntryBlock()->getArguments().size() == 1);
		REQUIRE(callee->getEntryBlock()->getArguments().size() == 1);
		REQUIRE(root->getEntryBlock()->getArguments().front()->getStamp() ==
		        callee->getEntryBlock()->getArguments().front()->getStamp());
		REQUIRE(graph.getEmissionName(callee) == "execute");
		REQUIRE(graph.getEmissionName(root) == "execute_2");
		requireScalarCertificate(graph);
		compiler::artifact::validateArtifactPreflight(graph, functions);
	};
	REQUIRE_THROWS_MATCHES(
	    pipeline.compileToIR(functions, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
	                         beforeOptimization, ConstantOriginTracking::Enabled),
	    RuntimeException,
	    Catch::Matchers::MessageMatches(Catch::Matchers::ContainsSubstring("emitted as 'execute_2'")));
	REQUIRE(checks == 1);
	REQUIRE_FALSE(tracing::inTracer());
}

TEST_CASE("Strict artifact preflight rejects scalar evidence before scheduling can discard it",
          "[artifact][preflight]") {
	using namespace compiler::ir;
	const bool passes = GENERATE(false, true);
	const bool optimize = GENERATE(false, true);
	const int iterations = GENERATE(1, 8);
	const auto tracking = GENERATE(ConstantOriginTracking::Disabled, ConstantOriginTracking::Enabled);
	CAPTURE(passes, optimize, iterations, tracking);
	Options options;
	options.setOption("ir.runPasses", passes);
	options.setOption("ir.runOptimizationPasses", optimize);
	options.setOption("ir.maxPipelineIterations", iterations);
	common::ArenaPool traceArenaPool, irArenaPool;
	compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", details::createFunctionWrapper([](val<int64_t> input) {
		                       return select(cacheLiteral<true>(), input, val<int64_t>(7));
	                       }),
	                       std::unordered_map<std::string, std::string> {},
	                       compiler::CompilableFunction::Signature {Type::i64, {Type::i64}});
	std::size_t checks = 0;
	const auto beforeOptimization = [&](IRGraph& graph) {
		++checks;
		compiler::artifact::validateArtifactPreflight(graph, functions);
	};
	const auto reason =
	    tracking == ConstantOriginTracking::Enabled ? "uncertified_scalar" : "constant_origins_not_recorded";
	REQUIRE_THROWS_MATCHES(pipeline.compileToIR(functions, options.deriveModuleOptions(), nullptr,
	                                            compiler::IROptimizationLevel::Full, beforeOptimization, tracking),
	                       RuntimeException,
	                       Catch::Matchers::MessageMatches(Catch::Matchers::ContainsSubstring(reason)));
	REQUIRE(checks == 1);
	REQUIRE_FALSE(tracing::inTracer());
}

TEST_CASE("Fixed internal scalar introductions retain invariant origins", "[artifact][constant-origin]") {
	using namespace compiler::ir;
	Options options;
	options.setOption("ir.runPasses", false);
	common::ArenaPool traceArenaPool, irArenaPool;
	compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", details::createFunctionWrapper([](val<int64_t> input) {
		                       val<int64_t> zero;
		                       val<bool> condition;
		                       ++input;
		                       input--;
		                       return select(condition, zero, -input);
	                       }),
	                       std::unordered_map<std::string, std::string> {},
	                       compiler::CompilableFunction::Signature {Type::i64, {Type::i64}});
	auto ir = pipeline.compileToIR(
	    functions, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
	    [&](IRGraph& graph) { compiler::artifact::validateArtifactPreflight(graph, functions); },
	    ConstantOriginTracking::Enabled);
	std::size_t integers = 0, booleans = 0;
	for (const auto* block : ir->getFunctionOperation("execute")->getBasicBlocks()) {
		for (const auto* operation : block->getOperations()) {
			if (const auto* constant = operation->dynCast<ConstIntOperation>()) {
				++integers;
				REQUIRE(constant->getConstantOrigin() == ConstantOrigin::CacheInvariant);
			} else if (const auto* constant = operation->dynCast<ConstBooleanOperation>()) {
				++booleans;
				REQUIRE_FALSE(constant->getValue());
				REQUIRE(constant->getConstantOrigin() == ConstantOrigin::CacheInvariant);
			}
		}
	}
	REQUIRE(integers == 4);
	REQUIRE(booleans == 1);
}

} // namespace nautilus::engine
#endif
