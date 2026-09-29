#include "nautilus/config.hpp"

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/RuntimeBinding.hpp"
#include "nautilus/common/ExecutableImage.hpp"
#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/mlir/ExceptionPersonality.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
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
#include "nautilus/compiler/ir/passes/CacheSafetyAnalysis.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/region.hpp"
#include "nautilus/select.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/SSAVerifier.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/val_std.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <list>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/BinaryFormat/ELF.h>
#include <llvm/Config/llvm-config.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/MC/MCAsmInfo.h>
#include <llvm/MC/MCContext.h>
#include <llvm/MC/MCDisassembler/MCDisassembler.h>
#include <llvm/MC/MCInstrAnalysis.h>
#include <llvm/MC/MCSubtargetInfo.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Object/ELFObjectFile.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>
#include <memory>
#include <mlir/Bytecode/BytecodeWriter.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/Verifier.h>
#include <mlir/Parser/Parser.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace nautilus::engine {
namespace {

class ScopedLLVMBackendHooks {
public:
	ScopedLLVMBackendHooks() : saved_(compiler::mlir::getLLVMBackendHooks()) {
	}

	~ScopedLLVMBackendHooks() {
		compiler::mlir::getLLVMBackendHooks() = std::move(saved_);
	}

	ScopedLLVMBackendHooks(const ScopedLLVMBackendHooks&) = delete;
	ScopedLLVMBackendHooks& operator=(const ScopedLLVMBackendHooks&) = delete;

private:
	compiler::mlir::LLVMBackendHooks saved_;
};

int32_t nativeFunctionAddressOf(int32_t* calls, int32_t value) noexcept {
	++*calls;
	return value * 3 + 7;
}

struct PersonalityCleanup {
	int32_t* cleanups;

	explicit PersonalityCleanup(int32_t* cleanups) noexcept : cleanups(cleanups) {
	}

	~PersonalityCleanup() noexcept {
		++*cleanups;
	}
};

int32_t personalityProxy(int32_t value) {
	if (value < 0) {
		throw std::runtime_error("personality cleanup");
	}
	return value + 1;
}

compiler::ir::FunctionOperation* scalarCertificateFunction(compiler::ir::IRGraph& graph,
                                                           std::vector<compiler::ir::BasicBlock*> blocks,
                                                           Type result = Type::v, const std::string& name = "execute") {
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

TEST_CASE("Cache safety analysis passes preserve graphs and reset independent results",
          "[runtime-bindings][cache][guard]") {
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
		scalarCertificateFunction(*graph, {block}, Type::ptr);
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
	requireUnchanged();

	for (auto* graph : {&accepted, &rejected, &accepted, &rejected}) {
		CAPTURE(graph->getId());
		const bool expected = graph == &accepted;
		REQUIRE_FALSE(scalarPass.apply(*graph));
		const auto scalarResult = scalar.getResult();
		REQUIRE(scalarResult.certified == expected);
		REQUIRE(scalarResult.rejection.empty() == expected);
		requireUnchanged();
		REQUIRE_FALSE(pointerPass.apply(*graph));
		const auto pointerResult = pointer.getResult();
		REQUIRE(pointerResult.relocatable == expected);
		REQUIRE(pointerResult.rejection.empty() == expected);
		requireUnchanged();
		if (!expected) {
			REQUIRE_THAT(scalarResult.rejection, Catch::Matchers::ContainsSubstring("embedded_non_null_pointer"));
			REQUIRE_THAT(pointerResult.rejection, Catch::Matchers::ContainsSubstring("embedded_non_null_pointer"));
		}

		REQUIRE_FALSE(scalarPass.apply(*graph));
		REQUIRE(scalar.getResult().certified == scalarResult.certified);
		REQUIRE(scalar.getResult().rejection == scalarResult.rejection);
		REQUIRE_FALSE(pointerPass.apply(*graph));
		REQUIRE(pointer.getResult().relocatable == pointerResult.relocatable);
		REQUIRE(pointer.getResult().rejection == pointerResult.rejection);
		requireUnchanged();
		REQUIRE(independentScalar.getResult().certified == independentScalarResult.certified);
		REQUIRE(independentScalar.getResult().rejection == independentScalarResult.rejection);
		REQUIRE(independentPointer.getResult().relocatable == independentPointerResult.relocatable);
		REQUIRE(independentPointer.getResult().rejection == independentPointerResult.rejection);
	}

	const auto scalarResult = scalar.getResult();
	const auto pointerResult = pointer.getResult();
	REQUIRE_FALSE(independentScalar.apply(accepted));
	REQUIRE(independentScalar.getResult().certified);
	REQUIRE(independentScalar.getResult().rejection.empty());
	REQUIRE_FALSE(independentPointer.apply(accepted));
	REQUIRE(independentPointer.getResult().relocatable);
	REQUIRE(independentPointer.getResult().rejection.empty());
	REQUIRE(scalar.getResult().certified == scalarResult.certified);
	REQUIRE(scalar.getResult().rejection == scalarResult.rejection);
	REQUIRE(pointer.getResult().relocatable == pointerResult.relocatable);
	REQUIRE(pointer.getResult().rejection == pointerResult.rejection);
	REQUIRE(scalarPass.getName() == "cacheScalarValidation");
	REQUIRE(pointerPass.getName() == "pointerRelocatability");
	requireUnchanged();
	for (int invalidation = 0; invalidation < 2; ++invalidation) {
		accepted.invalidateConstantOrigins();
		REQUIRE_FALSE(accepted.hasRecordedConstantOrigins());
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

TEST_CASE("Trace conversion preserves constant origin availability in every overload",
          "[runtime-bindings][cache][guard]") {
	using namespace compiler::ir;
	for (const std::string_view entryPoint : {"single", "module", "module context"}) {
		for (const bool pooled : {false, true}) {
			common::ArenaPool irArenaPool;
			CacheScalarValidationPass scalar;
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
				REQUIRE_FALSE(scalar.apply(*ir));
				REQUIRE(scalar.getResult().certified == enabled);
				REQUIRE(scalar.getResult().rejection == (enabled ? "" : "constant_origins_not_recorded"));
				REQUIRE(ir->toString() == before);
				REQUIRE(ir->hasRecordedConstantOrigins() == enabled);
			}
		}
	}
}

TEST_CASE("Mixed trace modules cannot certify an uncalled untracked constant-free function",
          "[runtime-bindings][cache][guard]") {
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
			CacheScalarValidationPass scalar;
			REQUIRE_FALSE(scalar.apply(*ir));
			REQUIRE_FALSE(scalar.getResult().certified);
			REQUIRE(scalar.getResult().rejection == "constant_origins_not_recorded");
			PointerRelocatabilityPass pointer({"execute"});
			REQUIRE_FALSE(pointer.apply(*ir));
			REQUIRE(pointer.getResult().relocatable);
			REQUIRE(pointer.getResult().rejection.empty());
			REQUIRE(ir->toString() == before);
			REQUIRE_FALSE(ir->hasRecordedConstantOrigins());
		}
	}
}

TEST_CASE("Default compilation cannot certify constant-free or optimized-away scalar origins",
          "[runtime-bindings][cache][guard]") {
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
		CacheScalarValidationPass scalar;
		std::size_t checks = 0;
		auto ir =
		    pipeline.compileToIR(functions, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
		                         [&](IRGraph& graph) {
			                         ++checks;
			                         REQUIRE_FALSE(graph.hasRecordedConstantOrigins());
			                         const auto before = graph.toString();
			                         REQUIRE_FALSE(scalar.apply(graph));
			                         REQUIRE_FALSE(scalar.getResult().certified);
			                         REQUIRE(scalar.getResult().rejection == "constant_origins_not_recorded");
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
		REQUIRE_FALSE(scalar.apply(*ir));
		REQUIRE_FALSE(scalar.getResult().certified);
		REQUIRE(scalar.getResult().rejection == "constant_origins_not_recorded");
		REQUIRE(ir->toString() == before);
	}
}

TEST_CASE("Constant origin tracking reaches every function and nested region independently of IR scheduling",
          "[runtime-bindings][cache][guard]") {
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
			CacheScalarValidationPass scalar;
			std::size_t checks = 0;
			const auto beforeOptimization = [&](IRGraph& graph) {
				++checks;
				REQUIRE(graph.hasRecordedConstantOrigins() == enabled);
				REQUIRE(graph.getFunctionOperations().size() == 4);
				const auto before = graph.toString();
				REQUIRE_FALSE(scalar.apply(graph));
				REQUIRE(scalar.getResult().certified == enabled);
				REQUIRE(scalar.getResult().rejection == (enabled ? "" : "constant_origins_not_recorded"));
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
			REQUIRE(scalar.getResult().certified == enabled);
			REQUIRE(scalar.getResult().rejection == (enabled ? "" : "constant_origins_not_recorded"));
		}
	}
}

TEST_CASE("Legacy pointer analysis still accepts runtime addresses and rejects embedded addresses without origins",
          "[runtime-bindings][cache][guard]") {
	using namespace compiler::ir;
	auto storage = std::make_unique<int64_t>(42);
	for (const std::string_view kind : {"runtime", "null", "raw heap", "integer encoded"}) {
		CAPTURE(kind);
		Options options;
		options.setOption("ir.runPasses", false);
		common::ArenaPool traceArenaPool, irArenaPool;
		compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
		std::list<compiler::CompilableFunction> functions;
		functions.emplace_back("execute", details::createFunctionWrapper(
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
		auto ir = pipeline.compileToIR(functions, options.deriveModuleOptions());
		REQUIRE_FALSE(ir->hasRecordedConstantOrigins());
		const auto before = ir->toString();
		CacheScalarValidationPass scalar;
		REQUIRE_FALSE(scalar.apply(*ir));
		REQUIRE_FALSE(scalar.getResult().certified);
		REQUIRE(scalar.getResult().rejection == "constant_origins_not_recorded");
		PointerRelocatabilityPass pointer({"execute"});
		REQUIRE_FALSE(pointer.apply(*ir));
		const bool relocatable = kind == "runtime" || kind == "null";
		REQUIRE(pointer.getResult().relocatable == relocatable);
		if (relocatable) {
			REQUIRE(pointer.getResult().rejection.empty());
		} else {
			REQUIRE_THAT(pointer.getResult().rejection,
			             Catch::Matchers::ContainsSubstring(kind == "raw heap" ? "embedded_non_null_pointer"
			                                                                   : "pointer_expression"));
		}
		REQUIRE(ir->toString() == before);
		REQUIRE_FALSE(ir->hasRecordedConstantOrigins());
	}
}

TEST_CASE("MLIR scalar certification checks every scalar leaf and rejects unsupported operands",
          "[runtime-bindings][cache][guard]") {
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
	CacheScalarValidationPass pass;
	REQUIRE_FALSE(pass.apply(graph));
	const auto& rejection = pass.getResult().rejection;
	REQUIRE(pass.getResult().certified == cause.empty());
	if (cause.empty()) {
		REQUIRE(rejection.empty());
	} else {
		REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("cause=" + cause));
		REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("function="));
	}
}

TEST_CASE("MLIR scalar certification validates branch ownership and argument schemas",
          "[runtime-bindings][cache][guard]") {
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
	CacheScalarValidationPass pass;
	REQUIRE_FALSE(pass.apply(graph));
	const auto& rejection = pass.getResult().rejection;
	REQUIRE(pass.getResult().certified == cause.empty());
	if (cause.empty()) {
		REQUIRE(rejection.empty());
	} else {
		REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("cause=" + cause));
	}
}

TEST_CASE("MLIR scalar certification validates called addressed and unused function targets",
          "[runtime-bindings][cache][guard]") {
	using namespace compiler::ir;
	for (const std::string_view use : {"call", "function address", "unused"}) {
		for (const std::string_view kind : {"valid", "missing target", "missing native address",
		                                    "missing internal definition", "foreign internal definition"}) {
			if (use == "unused" && kind == "missing target") {
				continue;
			}
			CAPTURE(use, kind);
			IRGraph graph("scalar-certificate-function-targets");
			auto& arena = graph.getArena();
			auto* argument = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::i32);
			auto* block =
			    arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {argument});
			auto* native = reinterpret_cast<void*>(personalityProxy);
			const auto target = graph.internCallee({.key = native,
			                                        .mangledName = "personalityProxy",
			                                        .demangledName = "personalityProxy",
			                                        .customName = "personalityProxy",
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
			}
			const auto callee = kind == "missing target" ? INVALID_FUNCTION_ID : target;
			if (use == "call") {
				block->addOperation<CallOperation>("personalityProxy", "personalityProxy", native,
				                                   OperationIdentifier(1), std::vector<Operation*> {argument},
				                                   Type::i32, FunctionAttributes {}, callee);
			} else if (use == "function address") {
				block->addOperation<FunctionAddressOfOperation>("personalityProxy", "personalityProxy", native,
				                                                OperationIdentifier(1), callee);
			}
			block->addOperation<ReturnOperation>();
			scalarCertificateFunction(graph, {block});
			CacheScalarValidationPass pass;
			REQUIRE_FALSE(pass.apply(graph));
			const auto& rejection = pass.getResult().rejection;
			REQUIRE(pass.getResult().certified == cause.empty());
			if (cause.empty()) {
				REQUIRE(rejection.empty());
			} else {
				REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("cause=" + cause));
			}
		}
	}
}

TEST_CASE("MLIR scalar certification rejects mismatched native call signatures", "[runtime-bindings][cache][guard]") {
	using namespace compiler::ir;
	IRGraph graph("scalar-certificate-signatures");
	auto& arena = graph.getArena();
	auto* argument = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::i32);
	auto* block = arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {argument});
	auto* native = reinterpret_cast<void*>(personalityProxy);
	const auto target = graph.internCallee({.key = native,
	                                        .mangledName = "personalityProxy",
	                                        .demangledName = "personalityProxy",
	                                        .customName = "personalityProxy",
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
	block->addOperation<CallOperation>("personalityProxy", "personalityProxy", native, OperationIdentifier(2),
	                                   arguments, result, FunctionAttributes {}, target);
	block->addOperation<ReturnOperation>();
	scalarCertificateFunction(graph, {block});
	CacheScalarValidationPass pass;
	REQUIRE_FALSE(pass.apply(graph));
	const auto& rejection = pass.getResult().rejection;
	REQUIRE_FALSE(pass.getResult().certified);
	REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("cause=" + cause));
}

TEST_CASE("MLIR scalar certification walks direct and indirect destructor-only operand trees",
          "[runtime-bindings][cache][guard]") {
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
			bool rawPointer = false, missingAddress = false;
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
			scalarCertificateFunction(graph, {block});
			REQUIRE(std::ranges::find(block->getOperations(), address) == block->getOperations().end());
			REQUIRE(std::ranges::find(block->getOperations(), offset) == block->getOperations().end());
			CacheScalarValidationPass pass;
			REQUIRE_FALSE(pass.apply(graph));
			const auto& rejection = pass.getResult().rejection;
			REQUIRE(pass.getResult().certified == cause.empty());
			if (cause.empty()) {
				REQUIRE(rejection.empty());
			} else {
				REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("cause=" + cause));
			}
			if (cause == "uncertified_scalar" || rawPointer) {
				PointerRelocatabilityPass pointerPass({"execute"});
				REQUIRE_FALSE(pointerPass.apply(graph));
				REQUIRE_FALSE(pointerPass.getResult().relocatable);
			}
		}
	}
}

TEST_CASE("MLIR scalar certification observes region expressions before optimization discards origins",
          "[runtime-bindings][cache][guard]") {
	using namespace compiler::ir;
	for (const std::string_view kind : {"certified", "ordinary folded operand", "unselected ordinary operand"}) {
		CAPTURE(kind);
		Options options;
		options.setOption("engine.backend", std::string("mlir"));
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
		CacheScalarValidationPass scalar;
		CacheScalarValidationPass::Result beforeOptimization;
		std::size_t checks = 0, additionsBefore = 0;
		auto ir = pipeline.compileToIR(
		    functions, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
		    [&](IRGraph& graph) {
			    ++checks;
			    REQUIRE(graph.hasRecordedConstantOrigins());
			    const auto before = graph.toString();
			    REQUIRE_FALSE(scalar.apply(graph));
			    beforeOptimization = scalar.getResult();
			    const auto& rejection = scalar.getResult().rejection;
			    REQUIRE(scalar.getResult().certified == (kind == "certified"));
			    if (kind != "certified") {
				    REQUIRE_THAT(rejection, Catch::Matchers::ContainsSubstring("uncertified_scalar"));
			    } else {
				    REQUIRE(rejection.empty());
			    }
			    REQUIRE_FALSE(scalar.apply(graph));
			    REQUIRE(scalar.getResult().certified == beforeOptimization.certified);
			    REQUIRE(scalar.getResult().rejection == beforeOptimization.rejection);
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
		REQUIRE(scalar.getResult().certified == beforeOptimization.certified);
		REQUIRE(scalar.getResult().rejection == beforeOptimization.rejection);
		const auto afterOptimization = ir->toString();
		PointerRelocatabilityPass pointer({"execute"});
		REQUIRE_FALSE(pointer.apply(*ir));
		REQUIRE(pointer.getResult().relocatable);
		REQUIRE(pointer.getResult().rejection.empty());
		REQUIRE(ir->toString() == afterOptimization);
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

TEST_CASE("MLIR native function addresses reuse hook-renamed declarations", "[cache][codegen]") {
	using namespace compiler::ir;
	using NativeCallback = int32_t (*)(int32_t*, int32_t);
	const std::string originalName = "nativeFunctionAddressOf";
	const std::string overriddenName = "hook_native_function_address";
	auto* targetAddress = reinterpret_cast<void*>(nativeFunctionAddressOf);
	auto* consume = +[](NativeCallback first, NativeCallback second, int32_t* calls, int32_t value) noexcept {
		if (first != nativeFunctionAddressOf || second != nativeFunctionAddressOf) {
			return int32_t {-1};
		}
		return first(calls, value) + second(calls, value + 1);
	};
	ScopedLLVMBackendHooks scopedHooks;
	compiler::mlir::getLLVMBackendHooks().callNameOverride =
	    [targetAddress, overriddenName](void* address) -> std::optional<std::string> {
		if (address == targetAddress) {
			return overriddenName;
		}
		return std::nullopt;
	};

	for (const bool callFirst : {false, true}) {
		DYNAMIC_SECTION("native call before address=" << callFirst) {
			auto graph = std::make_shared<IRGraph>("native-function-address-hook");
			auto& arena = graph->getArena();
			const auto native = graph->internCallee({.key = targetAddress,
			                                         .mangledName = originalName,
			                                         .demangledName = originalName,
			                                         .customName = originalName,
			                                         .resultType = Type::i32,
			                                         .paramTypes = {Type::ptr, Type::i32},
			                                         .attrs = {.noUnwind = true}});
			const auto consumer = graph->internCallee({.key = reinterpret_cast<void*>(consume),
			                                           .mangledName = "consume_native_function_addresses",
			                                           .demangledName = "consume_native_function_addresses",
			                                           .customName = "consume_native_function_addresses",
			                                           .resultType = Type::i32,
			                                           .paramTypes = {Type::ptr, Type::ptr, Type::ptr, Type::i32},
			                                           .attrs = {.noUnwind = true}});
			auto* calls = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::ptr);
			auto* value = arena.create<BasicBlockArgument>(OperationIdentifier(1), Type::i32);
			auto* block =
			    arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {calls, value});
			const auto addDirectCall = [&] {
				return block->addOperation<CallOperation>(
				    originalName, originalName, targetAddress, OperationIdentifier(2),
				    std::vector<Operation*> {calls, value}, Type::i32, FunctionAttributes {.noUnwind = true}, native);
			};
			auto* direct = callFirst ? addDirectCall() : nullptr;
			auto* first = block->addOperation<FunctionAddressOfOperation>(originalName, originalName, targetAddress,
			                                                              OperationIdentifier(3), native);
			auto* second = block->addOperation<FunctionAddressOfOperation>(originalName, originalName, targetAddress,
			                                                               OperationIdentifier(4), native);
			if (!callFirst) {
				direct = addDirectCall();
			}
			const auto& consumerTarget = graph->getFunctionTarget(consumer);
			auto* indirect = block->addOperation<CallOperation>(
			    consumerTarget.getName().forEmission(), consumerTarget.getName().get(), consumerTarget.getAddress(),
			    OperationIdentifier(5), std::vector<Operation*> {first, second, calls, value}, Type::i32,
			    consumerTarget.getAttributes(), consumer);
			auto* result = block->addOperation<AddOperation>(OperationIdentifier(6), direct, indirect);
			block->addOperation<ReturnOperation>(result);
			auto* function = graph->addFunctionOperation(arena.create<FunctionOperation>(
			    "execute", std::vector<BasicBlock*> {block}, std::vector<Type> {Type::ptr, Type::i32},
			    std::vector<std::string> {"calls", "value"}, Type::i32));
			const auto definition = graph->internCallee({.kind = CalleeDescriptor::Kind::Internal,
			                                             .key = function,
			                                             .mangledName = "execute",
			                                             .demangledName = "execute",
			                                             .customName = "execute",
			                                             .resultType = Type::i32,
			                                             .paramTypes = {Type::ptr, Type::i32},
			                                             .attrs = {.noUnwind = true}});
			graph->defineFunction(definition, function);

			Options options;
			options.setOption("mlir.enableMultithreading", false);
			options.setOption("mlir.inline_invoke_calls", true);
			::mlir::MLIRContext context;
			context.disableMultithreading();
			compiler::mlir::MLIRIntrinsicManager intrinsicManager;
			compiler::mlir::MLIRLoweringProvider lowering(context, options, intrinsicManager);
			auto module = lowering.generateModuleFromIR(graph);
			REQUIRE(static_cast<bool>(module));
			REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
			const auto helperName = "__nautilus_fptr_" + overriddenName;
			std::vector<std::string> names;
			for (auto emitted : module->getOps<::mlir::func::FuncOp>()) {
				names.push_back(emitted.getSymName().str());
			}
			REQUIRE(names.size() == 4);
			REQUIRE(std::ranges::count(names, originalName) == 0);
			REQUIRE(std::ranges::count(names, overriddenName) == 1);
			REQUIRE(std::ranges::count(names, helperName) == 1);
			auto declaration = module->lookupSymbol<::mlir::func::FuncOp>(overriddenName);
			REQUIRE(declaration.isExternal());
			auto helper = module->lookupSymbol<::mlir::func::FuncOp>(helperName);
			std::size_t constants = 0;
			helper.walk([&](::mlir::func::ConstantOp constant) {
				++constants;
				auto symbol = constant->getAttrOfType<::mlir::FlatSymbolRefAttr>("value");
				REQUIRE(static_cast<bool>(symbol));
				REQUIRE(symbol.getValue() == overriddenName);
			});
			REQUIRE(constants == 1);
			std::size_t helperCalls = 0, directCalls = 0;
			module->walk([&](::mlir::func::CallOp call) {
				helperCalls += call.getCallee() == helperName;
				directCalls += call.getCallee() == overriddenName;
			});
			REQUIRE(helperCalls == 2);
			REQUIRE(directCalls == 1);
			const auto symbols = lowering.getJitProxyFunctionSymbols();
			const auto addresses = lowering.getJitProxyTargetAddresses();
			REQUIRE(symbols.size() == 2);
			REQUIRE(addresses.size() == symbols.size());
			const auto symbol = std::ranges::find(symbols, overriddenName);
			REQUIRE(symbol != symbols.end());
			REQUIRE(addresses.at(std::distance(symbols.begin(), symbol)) == targetAddress);

			const compiler::DumpHandler dump(options, graph->getId());
			compiler::mlir::MLIRCompilationBackend backend;
			auto executable = backend.compile(graph, dump, options);
			REQUIRE(executable != nullptr);
			auto execute = executable->getInvocableMember<int32_t, int32_t*, int32_t>("execute");
			for (const int32_t input : {0, 7, -5}) {
				int32_t count = 0;
				REQUIRE(execute(&count, input) == input * 9 + 24);
				REQUIRE(count == 3);
			}
		}
	}
}

TEST_CASE("MLIR guarded cache imports the exception personality and still validates its declaration",
          "[cache][guard]") {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.enableMultithreading", false);
	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", details::createFunctionWrapper([](val<int32_t*> cleanups, val<int32_t> value) {
		                       val<PersonalityCleanup> cleanup(cleanups);
		                       return invoke(personalityProxy, value);
	                       }));
	auto ir =
	    pipeline.compileToIR(functions, options.deriveModuleOptions(), nullptr, pipeline.irOptimizationLevel({"mlir"}));
	const auto* function = ir->getFunctionOperation("execute");
	REQUIRE(function != nullptr);
	REQUIRE(function->exceptionRegion.has_value());
	REQUIRE(function->exceptionRegion->pads.size() == 1);
	REQUIRE(function->exceptionRegion->callSites.size() == 1);
	REQUIRE(function->exceptionRegion->callSites.front().hasPad());
	const auto check = [](compiler::Executable& executable) {
		REQUIRE(executable.getExceptionPropagationMode("execute") == compiler::ExceptionPropagationMode::NativeUnwind);
		int32_t cleanups = 0;
		auto execute = executable.getInvocableMember<int32_t, int32_t*, int32_t>("execute");
		REQUIRE(execute(&cleanups, 7) == 8);
		REQUIRE(cleanups == 1);
		REQUIRE_THROWS_WITH(execute(&cleanups, -1), "personality cleanup");
		REQUIRE(cleanups == 2);
		REQUIRE(execute(&cleanups, 19) == 20);
		REQUIRE(cleanups == 3);
	};
	const compiler::DumpHandler dump(options, "guarded-personality-validation");
	compiler::mlir::MLIRCompilationBackend backend;
	compiler::mlir::MLIRCacheArtifacts artifacts;
	auto executable = backend.compileWithCacheArtifacts(ir, {"execute"}, dump, options, nullptr, artifacts);
	REQUIRE(executable != nullptr);
	check(*executable);
	const auto symbol = std::ranges::find(artifacts.externalSymbols, "__gxx_personality_v0");
	REQUIRE(symbol != artifacts.externalSymbols.end());
	const auto index = std::distance(artifacts.externalSymbols.begin(), symbol);
	auto* address = artifacts.externalAddresses.at(index);
	REQUIRE(address == compiler::mlir::getExceptionPersonalityAddress());
	REQUIRE(address != nullptr);
#if defined(__linux__) && __has_include(<unwind.h>) && !defined(__arm__) && !defined(__USING_SJLJ_EXCEPTIONS__)
	auto process = llvm::sys::DynamicLibrary::getPermanentLibrary(nullptr);
	auto* runtimePersonality = process.getAddressOfSymbol("__gxx_personality_v0");
	REQUIRE(runtimePersonality != nullptr);
	REQUIRE(address != runtimePersonality);
	const auto image = common::locateExecutableAddress(address);
	REQUIRE(image.has_value());
	REQUIRE_FALSE(image->buildId.empty());
	REQUIRE(common::resolveExecutableAddress(*image) == address);
#endif
	REQUIRE_FALSE(artifacts.object.empty());

	auto loaded = backend.compileCachedBytecode(artifacts.bytecode, artifacts.moduleManifest, artifacts.externalSymbols,
	                                            artifacts.externalAddresses, {"execute"}, dump, options, nullptr);
	REQUIRE(loaded != nullptr);
	check(*loaded);
	auto loadedObject = backend.compileCachedObject(artifacts.object, artifacts.externalSymbols,
	                                                artifacts.externalAddresses, {"execute"}, options, nullptr);
	REQUIRE(loadedObject != nullptr);
	check(*loadedObject);
	artifacts.externalSymbols.erase(symbol);
	artifacts.externalAddresses.erase(artifacts.externalAddresses.begin() + index);
	REQUIRE_THROWS_WITH(backend.compileCachedBytecode(artifacts.bytecode, artifacts.moduleManifest,
	                                                  artifacts.externalSymbols, artifacts.externalAddresses,
	                                                  {"execute"}, dump, options, nullptr),
	                    Catch::Matchers::StartsWith("Cached MLIR module contains an undeclared external function"));
}

TEST_CASE("MLIR guarded cache checks operands present only in destructor metadata", "[cache][guard]") {
	using namespace compiler::ir;
	IRGraph graph("cleanup-only-operands");
	auto& arena = graph.getArena();
	auto storage = std::make_unique<int32_t>(42);
	auto* argument = arena.create<BasicBlockArgument>(OperationIdentifier(0), Type::ui64);
	auto* block = arena.create<BasicBlock>(arena, BlockIdentifier(0), std::vector<BasicBlockArgument*> {argument});
	Operation* address = nullptr;
	bool rejected = true;
	SECTION("raw heap constant") {
		address = arena.create<ConstPtrOperation>(arena, OperationIdentifier(1), storage.get());
	}
	SECTION("integer encoded heap constant through casts") {
		auto* integer = arena.create<ConstIntOperation>(arena, OperationIdentifier(1),
		                                                reinterpret_cast<uintptr_t>(storage.get()), Type::ui64);
		auto* pointer = arena.create<CastOperation>(arena, OperationIdentifier(2), integer, Type::ptr);
		address = arena.create<CastOperation>(arena, OperationIdentifier(3), pointer, Type::ptr);
	}
	SECTION("runtime integer address") {
		address = arena.create<CastOperation>(arena, OperationIdentifier(1), argument, Type::ptr);
		rejected = false;
	}
	SECTION("null address") {
		address = arena.create<ConstPtrOperation>(arena, OperationIdentifier(1), nullptr);
		rejected = false;
	}
	REQUIRE(address != nullptr);
	auto* destructor = +[](int32_t* value) noexcept {
		++*value;
	};
	auto* throwing = +[]() {
		throw std::runtime_error("cleanup metadata");
	};
	const auto callee = graph.internCallee({.key = reinterpret_cast<void*>(throwing),
	                                        .mangledName = "throwing",
	                                        .demangledName = "throwing",
	                                        .customName = "throwing",
	                                        .paramTypes = {},
	                                        .attrs = {}});
	const auto cleanup = graph.internCallee({.key = reinterpret_cast<void*>(destructor),
	                                         .mangledName = "cleanup",
	                                         .demangledName = "cleanup",
	                                         .customName = "cleanup",
	                                         .paramTypes = {Type::ptr},
	                                         .attrs = {.noUnwind = true}});
	const auto& cleanupTarget = graph.getFunctionTarget(cleanup);
	auto* call = block->addOperation<CallOperation>(
	    "throwing", "throwing", reinterpret_cast<void*>(throwing), OperationIdentifier(4), std::vector<Operation*> {},
	    Type::v, FunctionAttributes {}, callee,
	    std::vector<CallOperation::Destructor> {{address, cleanupTarget.getName().forEmission(),
	                                             cleanupTarget.getName().get(), cleanupTarget.getAddress()}},
	    true);
	block->addOperation<ReturnOperation>();
	auto* function = graph.addFunctionOperation(
	    arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {block}, std::vector<Type> {Type::ui64},
	                                    std::vector<std::string> {"address"}, Type::v));
	const auto definition = graph.internCallee({.kind = CalleeDescriptor::Kind::Internal,
	                                            .key = function,
	                                            .mangledName = "execute",
	                                            .demangledName = "execute",
	                                            .customName = "execute",
	                                            .paramTypes = {Type::ui64},
	                                            .attrs = {}});
	graph.defineFunction(definition, function);
	REQUIRE(graph.getEmissionName(function) == "execute");
	REQUIRE(call->getDestructors().size() == 1);
	REQUIRE(call->getDestructors().front().address == address);
	REQUIRE(call->getInputs().empty());
	REQUIRE(std::ranges::find(block->getOperations(), address) == block->getOperations().end());
	PointerRelocatabilityPass pass({"execute"});
	REQUIRE_FALSE(pass.apply(graph));
	REQUIRE(pass.getResult().relocatable == !rejected);
}

TEST_CASE("RuntimeBindings validates cached MLIR declarations before the module manifest",
          "[runtime-bindings][cache]") {
	int64_t value = 42, replacement = 97;
	RuntimeBindings bindings;
	auto state = bindings.bind<int64_t>("state", &value);
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.enableMultithreading", false);
	options.setRuntimeBindings(bindings);
	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute",
	                       details::createFunctionWrapper([state]() -> val<int64_t> { return *state.get(); }));
	auto ir =
	    pipeline.compileToIR(functions, options.deriveModuleOptions(), nullptr, pipeline.irOptimizationLevel({"mlir"}));
	const compiler::CompilationUnitID id = "runtime-binding-validation";
	const compiler::DumpHandler dump(options, id);
	compiler::mlir::MLIRCompilationBackend backend;
	compiler::mlir::MLIRCacheArtifacts artifacts;
	auto pristine = backend.compileWithCacheArtifacts(ir, {"execute"}, dump, options, nullptr, artifacts);
	REQUIRE(pristine != nullptr);
	REQUIRE(pristine->getInvocableMember<int64_t>("execute")() == 42);
	REQUIRE_FALSE(artifacts.bytecode.empty());
	REQUIRE_FALSE(artifacts.moduleManifest.empty());
	REQUIRE(artifacts.externalSymbols.size() == 1);
	REQUIRE(artifacts.externalSymbols[0] == bindings.entries().at("state")->symbol);
	REQUIRE(artifacts.externalAddresses.size() == 1);
	REQUIRE(artifacts.externalAddresses[0] == &value);

	::mlir::MLIRContext context;
	context.disableMultithreading();
	context.loadDialect<::mlir::LLVM::LLVMDialect>();
	auto module = ::mlir::parseSourceString<::mlir::ModuleOp>(artifacts.bytecode, &context);
	REQUIRE(static_cast<bool>(module));
	auto global = module->lookupSymbol<::mlir::LLVM::GlobalOp>(artifacts.externalSymbols[0]);
	REQUIRE(static_cast<bool>(global));
	std::string expected = "Cached MLIR module manifest mismatch";
	SECTION("pristine bytecode reaches the manifest check") {
	}
	SECTION("absent schema") {
		module->getOperation()->removeAttr("nautilus.runtime_binding.schema");
		expected = "Cached MLIR runtime binding schema mismatch";
	}
	SECTION("mismatched schema") {
		module->getOperation()->setAttr("nautilus.runtime_binding.schema",
		                                ::mlir::StringAttr::get(&context, RuntimeBindings {}.schema()));
		expected = "Cached MLIR runtime binding schema mismatch";
	}
	SECTION("same schema loaded without its registry") {
		options.setRuntimeBindings(RuntimeBindings {});
		expected = "Cached MLIR runtime binding schema mismatch";
	}
	SECTION("absent binding identity") {
		global->removeAttr("nautilus.runtime_binding.identity");
		expected = "Cached MLIR runtime binding declaration mismatch";
	}
	SECTION("mismatched binding identity") {
		global->setAttr("nautilus.runtime_binding.identity", ::mlir::StringAttr::get(&context, "different-state"));
		expected = "Cached MLIR runtime binding declaration mismatch";
	}
	SECTION("absent binding type") {
		global->removeAttr("nautilus.runtime_binding.type");
		expected = "Cached MLIR runtime binding declaration mismatch";
	}
	SECTION("same-sized wrong binding type") {
		global->setAttr("nautilus.runtime_binding.type",
		                ::mlir::StringAttr::get(&context, runtime_binding::typeSchema<uint64_t>()));
		expected = "Cached MLIR runtime binding declaration mismatch";
	}
	SECTION("wrong global element type") {
		global.setGlobalType(::mlir::IntegerType::get(&context, 64));
		expected = "Cached MLIR runtime binding declaration mismatch";
	}
	SECTION("constant binding global") {
		global.setConstant(true);
		expected = "Cached MLIR runtime binding declaration mismatch";
	}
	SECTION("binding global with an initializer") {
		global.setValueAttr(::mlir::IntegerAttr::get(global.getType(), 0));
		expected = "Cached MLIR runtime binding declaration mismatch";
	}
	SECTION("address differs from the load environment") {
		artifacts.externalAddresses[0] = &replacement;
		expected = "Cached MLIR runtime binding address does not match the load environment";
	}
	SECTION("null binding address") {
		artifacts.externalAddresses[0] = nullptr;
		expected = "Cached MLIR external symbol manifest is invalid";
	}
	SECTION("binding address missing from both vectors") {
		artifacts.externalSymbols.clear();
		artifacts.externalAddresses.clear();
		expected = "Cached MLIR runtime binding address is missing";
	}
	SECTION("stale address after a compatible registry rebind") {
		RuntimeBindings rebound;
		(void) rebound.bind<int64_t>("state", &replacement);
		REQUIRE(rebound.schema() == bindings.schema());
		options.setRuntimeBindings(rebound);
		expected = "Cached MLIR runtime binding address does not match the load environment";
	}
	SECTION("compatible registry and external address reload pristine bytecode") {
		RuntimeBindings rebound;
		(void) rebound.bind<int64_t>("state", &replacement);
		REQUIRE(rebound.schema() == bindings.schema());
		options.setRuntimeBindings(rebound);
		artifacts.externalAddresses[0] = &replacement;
		auto loaded =
		    backend.compileCachedBytecode(artifacts.bytecode, artifacts.moduleManifest, artifacts.externalSymbols,
		                                  artifacts.externalAddresses, {"execute"}, dump, options, nullptr);
		REQUIRE(loaded != nullptr);
		REQUIRE(loaded->getInvocableMember<int64_t>("execute")() == 97);
		replacement = 113;
		REQUIRE(loaded->getInvocableMember<int64_t>("execute")() == 113);
		REQUIRE(pristine->getInvocableMember<int64_t>("execute")() == 42);
		return;
	}

	REQUIRE(::mlir::succeeded(::mlir::verify(*module)));
	std::string bytecode;
	llvm::raw_string_ostream output(bytecode);
	REQUIRE(::mlir::succeeded(::mlir::writeBytecodeToFile(module->getOperation(), output)));
	output.flush();
	REQUIRE_THROWS_WITH(backend.compileCachedBytecode(bytecode, "deliberately-wrong-module-manifest",
	                                                  artifacts.externalSymbols, artifacts.externalAddresses,
	                                                  {"execute"}, dump, options, nullptr),
	                    Catch::Matchers::StartsWith(expected));
	REQUIRE(value == 42);
	REQUIRE(replacement == 97);
}

TEST_CASE("RuntimeBindings hoists invariant binding addresses out of native loops on Linux x86-64",
          "[runtime-bindings][codegen]") {
	const llvm::Triple host(llvm::sys::getProcessTriple());
	if (!host.isOSLinux() || host.getArch() != llvm::Triple::x86_64) {
		SKIP("Native binding relocation placement is checked only on Linux x86-64");
	}
	std::array<uint64_t, 32> values {};
	RuntimeBindings bindings;
	auto state = bindings.bind<uint64_t>("loop/state", values.data());
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.enableMultithreading", false);
	options.setOption("mlir.optimizationLevel", 3);
	options.setOption("dump.before_llvm_optimization", true);
	options.setOption("dump.after_llvm_generation", true);
	options.setRuntimeBindings(bindings);
	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	compiler::CompilationPipeline pipeline(options, traceArenaPool, irArenaPool);
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", details::createFunctionWrapper([state](val<int32_t> count) {
		                       val<uint64_t> checksum = 0;
		                       for (val<int32_t> index = 0; index < count; index = index + 1) {
			                       auto address = state.get();
			                       auto slot = index & 31;
			                       val<uint64_t> previous = address[slot];
			                       auto next = previous + static_cast<val<uint64_t>>(index) + uint64_t {1};
			                       address[slot] = next;
			                       checksum = checksum + next;
		                       }
		                       return checksum;
	                       }));
	auto ir =
	    pipeline.compileToIR(functions, options.deriveModuleOptions(), nullptr, pipeline.irOptimizationLevel({"mlir"}));
	const compiler::CompilationUnitID id =
	    "runtime-binding-codegen-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
	const compiler::DumpHandler dump(options, id);
	compiler::mlir::MLIRCompilationBackend backend;
	compiler::mlir::MLIRCacheArtifacts artifacts;
	auto executable = backend.compileWithCacheArtifacts(ir, {"execute"}, dump, options, nullptr, artifacts);
	REQUIRE(executable != nullptr);
	auto expected = values;
	uint64_t checksum = 0;
	for (uint64_t index = 0; index < 97; ++index) {
		expected[index & 31] += index + 1;
		checksum += expected[index & 31];
	}
	REQUIRE(executable->getInvocableMember<uint64_t, int32_t>("execute")(97) == checksum);
	REQUIRE(values == expected);

	const auto& symbol = bindings.entries().at("loop/state")->symbol;
	for (const bool optimized : {false, true}) {
		CAPTURE(optimized);
		const auto& path =
		    dump.getGeneratedFiles().at(optimized ? "after_llvm_generation" : "before_llvm_optimization");
		llvm::LLVMContext context;
		llvm::SMDiagnostic diagnostic;
		auto module = llvm::parseIRFile(path, diagnostic, context);
		REQUIRE(module != nullptr);
		auto* function = module->getFunction("execute");
		REQUIRE(function != nullptr);
		llvm::DominatorTree dominators(*function);
		llvm::LoopInfo loops(dominators);
		REQUIRE_FALSE(loops.empty());
		std::size_t addresses = 0;
		for (const auto& block : *function) {
			for (const auto& instruction : block) {
				const auto* call = llvm::dyn_cast<llvm::CallInst>(&instruction);
				if (call != nullptr && call->isInlineAsm() && call->arg_size() == 1 &&
				    call->getArgOperand(0)->stripPointerCasts() == module->getGlobalVariable(symbol)) {
					++addresses;
					if (!optimized) {
						REQUIRE(loops.getLoopFor(&block) != nullptr);
					}
				}
			}
		}
		REQUIRE(addresses == 1);
		std::filesystem::remove(path);
	}

	REQUIRE_FALSE(llvm::InitializeNativeTargetDisassembler());
	std::string targetError;
	const auto* target = llvm::TargetRegistry::lookupTarget(host, targetError);
	INFO(targetError);
	REQUIRE(target != nullptr);
#if LLVM_VERSION_MAJOR >= 22
	const auto& targetTriple = host;
#else
	const auto targetTriple = host.str();
#endif
	std::unique_ptr<llvm::MCRegisterInfo> registers(target->createMCRegInfo(targetTriple));
	REQUIRE(registers != nullptr);
	std::unique_ptr<llvm::MCAsmInfo> assembly(target->createMCAsmInfo(*registers, targetTriple, {}));
	REQUIRE(assembly != nullptr);
	std::unique_ptr<llvm::MCSubtargetInfo> subtarget(
	    target->createMCSubtargetInfo(targetTriple, llvm::sys::getHostCPUName(), ""));
	REQUIRE(subtarget != nullptr);
	llvm::MCContext context(host, assembly.get(), registers.get(), subtarget.get());
	std::unique_ptr<llvm::MCDisassembler> disassembler(target->createMCDisassembler(*subtarget, context));
	REQUIRE(disassembler != nullptr);
	std::unique_ptr<llvm::MCInstrInfo> instructions(target->createMCInstrInfo());
	REQUIRE(instructions != nullptr);
	std::unique_ptr<llvm::MCInstrAnalysis> analysis(target->createMCInstrAnalysis(instructions.get()));
	REQUIRE(analysis != nullptr);

	auto object = llvm::object::ObjectFile::createObjectFile(llvm::MemoryBufferRef(artifacts.object, id));
	REQUIRE(static_cast<bool>(object));
	REQUIRE((*object)->isELF());
	REQUIRE((*object)->getArch() == llvm::Triple::x86_64);
	const auto symbols = (*object)->symbols();
	auto functionSymbol = symbols.end();
	for (auto current = symbols.begin(); current != symbols.end(); ++current) {
		auto name = current->getName();
		REQUIRE(static_cast<bool>(name));
		if (*name == "execute") {
			functionSymbol = current;
			break;
		}
	}
	REQUIRE(functionSymbol != symbols.end());
	auto functionSection = functionSymbol->getSection();
	REQUIRE(static_cast<bool>(functionSection));
	REQUIRE(*functionSection != (*object)->section_end());
	REQUIRE((*functionSection)->isText());
	auto functionAddress = functionSymbol->getAddress();
	REQUIRE(static_cast<bool>(functionAddress));
	REQUIRE(*functionAddress >= (*functionSection)->getAddress());
	const auto functionStart = *functionAddress - (*functionSection)->getAddress();
	const auto functionSize = llvm::object::ELFSymbolRef(*functionSymbol).getSize();
	REQUIRE(functionSize > 0);
	auto contents = (*functionSection)->getContents();
	REQUIRE(static_cast<bool>(contents));
	REQUIRE(functionStart <= contents->size());
	REQUIRE(functionSize <= contents->size() - functionStart);
	const auto functionEnd = functionStart + functionSize;
	const llvm::ArrayRef<uint8_t> bytes(contents->bytes_begin() + functionStart, functionSize);
	std::vector<std::pair<uint64_t, uint64_t>> nativeLoops;
	for (auto offset = functionStart; offset < functionEnd;) {
		llvm::MCInst instruction;
		uint64_t size = 0;
		CAPTURE(offset);
		REQUIRE(disassembler->getInstruction(instruction, size, bytes.drop_front(offset - functionStart), offset,
		                                     llvm::nulls()) == llvm::MCDisassembler::Success);
		REQUIRE(size > 0);
		if (analysis->isBranch(instruction)) {
			REQUIRE_FALSE(analysis->isIndirectBranch(instruction));
			uint64_t destination = 0;
			REQUIRE(analysis->evaluateBranch(instruction, offset, size, destination));
			if (destination <= offset) {
				REQUIRE(destination >= functionStart);
				nativeLoops.emplace_back(destination, offset + size);
			}
		}
		offset += size;
	}
	REQUIRE_FALSE(nativeLoops.empty());

	std::size_t bindingRelocations = 0;
	for (const auto& section : (*object)->sections()) {
		auto relocatedSection = section.getRelocatedSection();
		REQUIRE(static_cast<bool>(relocatedSection));
		if (*relocatedSection != *functionSection) {
			continue;
		}
		for (const auto& relocation : section.relocations()) {
			const auto offset = relocation.getOffset();
			if (offset < functionStart || offset >= functionEnd) {
				continue;
			}
			auto relocatedSymbol = relocation.getSymbol();
			if (relocatedSymbol == (*object)->symbol_end()) {
				continue;
			}
			auto name = relocatedSymbol->getName();
			REQUIRE(static_cast<bool>(name));
			if (*name == symbol) {
				++bindingRelocations;
				CAPTURE(offset);
				REQUIRE(relocation.getType() == llvm::ELF::R_X86_64_64);
				for (const auto& [loopStart, loopEnd] : nativeLoops) {
					CAPTURE(loopStart, loopEnd);
					REQUIRE((offset + sizeof(uint64_t) <= loopStart || offset >= loopEnd));
				}
			}
		}
	}
	REQUIRE(bindingRelocations == 1);
}

} // namespace nautilus::engine
#endif
