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
#include "nautilus/compiler/cache/PersistentModuleCache.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
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

} // namespace

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
	REQUIRE(compiler::containsNonRelocatablePointer(graph, {"execute"}) == rejected);
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
