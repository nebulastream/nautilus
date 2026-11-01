#include "nautilus/compiler/backends/mlir/MLIRNirBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/mlir/JITCompiler.hpp"
#include "nautilus/compiler/backends/mlir/LLVMIROptimizer.hpp"
#include "nautilus/compiler/backends/mlir/MLIRExecutable.hpp"
#include "nautilus/compiler/backends/mlir/MLIRNirLowering.hpp"
#include "nautilus/compiler/backends/mlir/MLIRPassManager.hpp"
#include "nautilus/compiler/backends/mlir/MLIRThreadState.hpp"
#include "nautilus/compiler/backends/mlir/debug/DebugInfoOptions.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRMemoryIntrinsics.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <atomic>
#include <chrono>
#include <fmt/format.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/TargetSelect.h>
#include <mlir/IR/BuiltinOps.h>
#include <unistd.h>

namespace nautilus::compiler::mlir {

MLIRNirBackend::MLIRNirBackend() {
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	// the default intrinsics (the plugins register theirs from their own initializers)
	RegisterMLIRMemoryIntrinsicPlugin();
}

std::unique_ptr<Executable> MLIRNirBackend::compile(std::shared_ptr<const nir::Module> module,
                                                    const nirbackend::CompileContext& context) const {
	static std::atomic<uint64_t> counter {0};
	return compileNamed(std::move(module), context,
	                    "nir_" + std::to_string(::getpid()) + "_" + std::to_string(counter.fetch_add(1)));
}

std::unique_ptr<Executable> MLIRNirBackend::compileNamed(std::shared_ptr<const nir::Module> module,
                                                         const nirbackend::CompileContext& compileContext,
                                                         const std::string& unit) const {
	if (compileContext.cancellation != nullptr && compileContext.cancellation->cancelled()) {
		return nullptr;
	}
	const auto& options = compileContext.options;
	auto* statistics = compileContext.statistics;
	const auto& dumpHandler = compileContext.dumpHandler;
	const auto backendStart = std::chrono::steady_clock::now();

	const auto debugInfo = debugInfoOptionsFromEngineOptions(options);
	if (debugInfo.emitDebugInfo()) {
		throw NotImplementedException("mlir: debug information from NIR comes with the debug producers (M4.9)");
	}

	// The context and the target machines of this thread are reused (M4.5, #561).
	auto& state = MLIRThreadState::current();
	auto& context = state.context(options.getOptionOrDefault("mlir.enableMultithreading", false));

	MLIRIntrinsicManager intrinsicManager;
	if (options.getOptionOrDefault("mlir.enableIntrinsics", true)) {
		MLIRIntrinsicPluginRegistry::instance().registerAllIntrinsics(intrinsicManager);
	}

	const auto loweringStart = std::chrono::steady_clock::now();
	auto lowered = lowerNirToMLIR(context, *module, compileContext.host, options, &intrinsicManager);
	if (statistics != nullptr) {
		statistics->recordTimingMs("mlir.loweringFromNir.ms", loweringStart);
		int64_t opCount = 0;
		lowered.module->walk([&](::mlir::Operation*) { ++opCount; });
		statistics->set("mlir.module.ops", opCount);
	}
	if (compileContext.cancellation != nullptr && compileContext.cancellation->cancelled()) {
		return nullptr;
	}

	dumpHandler.dump("after_mlir_generation", "mlir", [&]() {
		::mlir::OpPrintingFlags flags;
		std::string result;
		auto output = llvm::raw_string_ostream(result);
		lowered.module->print(output, flags);
		return result;
	});

	const auto pipelineStart = std::chrono::steady_clock::now();
	if (MLIRPassManager::lowerAndOptimizeMLIRModule(lowered.module, {}, debugInfo)) {
		throw RuntimeException("Could not lower and optimize MLIR module.");
	}
	if (statistics != nullptr) {
		statistics->recordTimingMs("mlir.pipeline.ms", pipelineStart);
	}
	if (compileContext.cancellation != nullptr && compileContext.cancellation->cancelled()) {
		return nullptr;
	}

	const auto optPipelineStart = std::chrono::steady_clock::now();
	auto optPipeline = LLVMIROptimizer::getLLVMOptimizerPipeline(options, dumpHandler);
	if (statistics != nullptr) {
		statistics->recordTimingMs("llvm.optimizerBuild.ms", optPipelineStart);
	}

	const auto jitStart = std::chrono::steady_clock::now();
	auto jit =
	    JITCompiler::jitCompileModule(lowered.module, optPipeline, lowered.proxySymbols, lowered.proxyAddresses,
	                                  llvm::CodeGenOptLevel::Aggressive, false, false, true, true, true, false, unit);
	// ORC generates machine code on the first lookup of a symbol; with eager compilation it is generated here instead,
	// inside the compile and its statistics.
	if (options.getOptionOrDefault("mlir.eager_compilation", false)) {
		if (auto err = jit->materialize()) {
			throw RuntimeException(fmt::format("Could not generate machine code: {}", llvm::toString(std::move(err))));
		}
	}
	if (statistics != nullptr) {
		statistics->recordTimingMs("jit.compile.ms", jitStart);
		statistics->recordTimingMs("backend.totalMs", backendStart);
		statistics->set("mlir.contextsCreated", static_cast<int64_t>(state.contextsCreated()));
		statistics->set("mlir.targetMachinesCreated", static_cast<int64_t>(state.targetMachinesCreated()));
	}
	return std::make_unique<MLIRExecutable>(std::move(jit), std::move(module));
}

} // namespace nautilus::compiler::mlir
