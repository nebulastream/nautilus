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
#include <cstdlib>
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

	// D14: with `debug`, `perf` or `perf.sample` the locations point into the text form of the module, which is written
	// next to the code for a debugger or `perf annotate` to read; the printer's line table replaces 0.3's
	// IRLocationPass. The regions come from the module's debug sections, which the NIR tracer records for those
	// options.
	auto debugInfo = debugInfoOptionsFromEngineOptions(options);
	nir::PrintLines printLines;
	NirDebugInfo nirDebug;
	if (debugInfo.emitDebugInfo()) {
		const auto text = nir::print(*module, printLines);
		writeDebugSourceFile(debugInfo, text);
		nirDebug.sourceFile = debugInfo.sourceFile;
		nirDebug.lines = &printLines;
		nirDebug.storeValues = debugInfo.enableDebug;
	}

	// The context and the target machines of this thread are reused (M4.5, #561).
	auto& state = MLIRThreadState::current();
	auto& context = state.context(options.getOptionOrDefault("mlir.enableMultithreading", false));

	MLIRIntrinsicManager intrinsicManager;
	if (options.getOptionOrDefault("mlir.enableIntrinsics", true)) {
		MLIRIntrinsicPluginRegistry::instance().registerAllIntrinsics(intrinsicManager);
	}

	const auto loweringStart = std::chrono::steady_clock::now();
	auto lowered = lowerNirToMLIR(context, *module, compileContext.host, options, &intrinsicManager,
	                              debugInfo.emitDebugInfo() ? &nirDebug : nullptr);
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
	// the module was verified once after the lowering; the pass pipeline does not verify after every pass
	const char* verifyEnv = std::getenv("NAUTILUS_IR_VERIFY");
	const bool verifyPasses = verifyEnv != nullptr && std::string(verifyEnv) == "1";
	if (MLIRPassManager::lowerAndOptimizeMLIRModule(lowered.module, {}, debugInfo, verifyPasses)) {
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
	// With `debug` the IR optimizer is clamped to -O0; the machine code is generated at -O1 level so that the register
	// allocator does not fold distinct values into one register (see MLIRCompilationBackend for the reason). `perf`
	// and `perf.sample` keep the level of the rest of the pipeline.
	const auto jitCodeGenLevel =
	    debugInfo.enableDebug ? llvm::CodeGenOptLevel::Less : llvm::CodeGenOptLevel::Aggressive;
	auto jit = JITCompiler::jitCompileModule(lowered.module, optPipeline, lowered.proxySymbols, lowered.proxyAddresses,
	                                         jitCodeGenLevel, debugInfo.enableDebug, debugInfo.enablePerf,
	                                         debugInfo.perfEmitDebugInfo, debugInfo.perfEmitUnwindInfo,
	                                         debugInfo.perfRegionSymbols, debugInfo.enableSampleSymbols, unit);
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
