

#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/mlir/JITCompiler.hpp"
#include "nautilus/compiler/backends/mlir/LLVMIROptimizer.hpp"
#include "nautilus/compiler/backends/mlir/MLIRExecutable.hpp"
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include "nautilus/compiler/backends/mlir/MLIRPassManager.hpp"
#include "nautilus/compiler/backends/mlir/debug/DebugInfoOptions.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRMemoryIntrinsics.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/IRLocationMap.hpp"
#include "nautilus/compiler/ir/passes/IRLocationPass.hpp"
#include <chrono>
#include <fstream>
#include <llvm/Support/TargetSelect.h>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/ControlFlow/IR/ControlFlow.h>
#include <mlir/Dialect/Func/Extensions/AllExtensions.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/Transforms/InlinerInterfaceImpl.h>
#include <mlir/Dialect/Math/IR/Math.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/Target/LLVMIR/Dialect/All.h>
#include <mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h>
#include <mlir/Transforms/Inliner.h>
namespace nautilus::compiler::mlir {

MLIRCompilationBackend::MLIRCompilationBackend() {
	// Initialize information about the local machine in LLVM.
	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();

	// Register default MLIR intrinsics
	RegisterMLIRMemoryIntrinsicPlugin();
}

std::unique_ptr<Executable> MLIRCompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                            const DumpHandler& dumpHandler,
                                                            const engine::Options& options,
                                                            CompilationStatistics* statistics) const {
	const auto backendStart = std::chrono::steady_clock::now();

	// 1. Create the MLIRLoweringProvider and lower the given Nautilus IR. Return
	// an MLIR module.
	::mlir::DialectRegistry registry;
	registry.insert<::mlir::arith::ArithDialect, ::mlir::cf::ControlFlowDialect, ::mlir::math::MathDialect,
	                ::mlir::LLVM::LLVMDialect, ::mlir::func::FuncDialect>();
	::mlir::func::registerAllExtensions(registry);
	registerBuiltinDialectTranslation(registry);
	registerLLVMDialectTranslation(registry);

	::mlir::LLVM::registerInlinerInterface(registry);

	::mlir::MLIRContext context(registry);
	if (not options.getOptionOrDefault("mlir.enableMultithreading", true)) {
		context.disableMultithreading();
	}

	// Register all intrinsics in the intrinsic manager
	MLIRIntrinsicManager intrinsicManager;
	if (options.getOptionOrDefault("mlir.enableIntrinsics", true)) {
		MLIRIntrinsicPluginRegistry::instance().registerAllIntrinsics(intrinsicManager);
	}

	// Build debug-info options once; downstream steps read from this struct.
	// Not const: a source-file write failure below patches `sourceFile` to a
	// fallback path before lowering ever reads it.
	auto debugInfo = debugInfoOptionsFromEngineOptions(options);

	// Prepare the Nautilus-IR "source file" used for debugging. The file
	// must exist before lowering runs so the FileLineColLocs attached by
	// MLIRLoweringProvider point at real content that GDB/LLDB (or, in
	// perf-only mode, `perf annotate`) can read.  `emitDebugInfo()` rather
	// than `enableDebug`: a perf-only compile needs this file just as much
	// as a debug one.
	//
	// computeIRLocations() must run after the last pass that mutates `ir`:
	// it records where each operation lands in this rendering, and an
	// operation minted or removed afterwards would invalidate every line.
	std::shared_ptr<const ir::IRLocationMap> locationMap;
	if (debugInfo.emitDebugInfo()) {
		// Normally computed by IRLocationPass as the pipeline's last pass and
		// published on the graph. Falling back keeps a direct compileIR() call
		// (one that never ran the pipeline's passes) working.
		locationMap = ir->getLocationMap();
		if (locationMap == nullptr) {
			ir::IRLocationPass locationPass;
			locationPass.apply(*ir);
			locationMap = locationPass.getResult();
		}
		if (!debugInfo.sourceFile.empty()) {
			std::ofstream out(debugInfo.sourceFile);
			out << locationMap->text;
			// $TMPDIR is almost always writable; the perf-only default (the
			// working directory -- see DebugInfoOptions.cpp) need not be (a
			// daemon, a read-only container). An unchecked failure here would
			// leave the DWARF naming a file that was never written, and
			// `perf annotate` / GDB's `list` would silently show nothing.
			// This must happen BEFORE loweringProvider->setDebugInfo() below:
			// the path is baked into every op's FileLineColLoc during
			// lowering and cannot be patched up afterwards.
			if (!out) {
				const auto fallback = debugSourceFallbackPath("ir");
				llvm::errs() << "nautilus: could not write debug source file '" << debugInfo.sourceFile
				             << "'; falling back to '" << fallback << "'\n";
				debugInfo.sourceFile = fallback;
				std::ofstream fallbackOut(debugInfo.sourceFile);
				fallbackOut << locationMap->text;
			}
		}
	}

	// The lowering's per-block frames and identifier maps bump this arena
	// instead of the heap. The handle outlives the provider (declared first,
	// destroyed last) and recycles the arena's chunks into the pool for the
	// next compile once the provider is gone.
	auto loweringArena = loweringArenaPool_.acquire();
	auto loweringProvider = std::make_unique<MLIRLoweringProvider>(context, options, intrinsicManager, *loweringArena);
	if (debugInfo.emitDebugInfo() && locationMap) {
		loweringProvider->setDebugInfo(debugInfo, locationMap);
	}

	const auto loweringStart = std::chrono::steady_clock::now();
	auto mlirModule = loweringProvider->generateModuleFromIR(ir);
	if (*mlirModule == nullptr) {
		throw RuntimeException("verification of MLIR module failed!");
	};
	if (statistics != nullptr) {
		statistics->recordTimingMs("mlir.loweringFromIR.ms", loweringStart);
		// Count ops by walking the module once — cheap and bounded by
		// the already-generated IR.
		int64_t opCount = 0;
		(*mlirModule)->walk([&](::mlir::Operation*) { ++opCount; });
		statistics->set("mlir.module.ops", opCount);
	}

	// 2.a dump MLIR to console or a file
	dumpHandler.dump("after_mlir_generation", "mlir", [&]() {
		::mlir::OpPrintingFlags flags;
		std::string result;
		auto output = llvm::raw_string_ostream(result);
		mlirModule->print(output, flags);
		return result;
	});

	// 2.b Take the MLIR module from the MLIRLoweringProvider and apply lowering
	// and optimization passes.
	const auto pipelineStart = std::chrono::steady_clock::now();
	if (mlir::MLIRPassManager::lowerAndOptimizeMLIRModule(mlirModule, {}, debugInfo)) {
		throw RuntimeException("Could not lower and optimize MLIR module.");
	}
	if (statistics != nullptr) {
		statistics->recordTimingMs("mlir.pipeline.ms", pipelineStart);
	}

	// 3. Lower MLIR module to LLVM IR and create LLVM IR optimization pipeline.
	const auto optPipelineStart = std::chrono::steady_clock::now();
	auto optPipeline = LLVMIROptimizer::getLLVMOptimizerPipeline(options, dumpHandler);
	if (statistics != nullptr) {
		statistics->recordTimingMs("llvm.optimizerBuild.ms", optPipelineStart);
	}

	// 4. JIT compile LLVM IR module and return engine that provides access
	// compiled execute function.
	const auto jitStart = std::chrono::steady_clock::now();

	// With debug info active the IR optimizer is clamped to -O0; match
	// that in the JIT's MC layer at -O1-equivalent (`Less`) so the
	// register allocator does not fold distinct $N SSA values into the
	// same physical register.  `None` triggers fast-regalloc which
	// spills every SSA value and confuses LLVM's DWARF asmprinter when
	// dbg.value operands live on the stack rather than in registers.
	// Keyed on `enableDebug` alone: perf-only mode keeps the codegen level
	// the rest of the pipeline chose.
	const auto jitCodeGenLevel =
	    debugInfo.enableDebug ? llvm::CodeGenOptLevel::Less : llvm::CodeGenOptLevel::Aggressive;
	auto engine = JITCompiler::jitCompileModule(
	    mlirModule, optPipeline, loweringProvider->getJitProxyFunctionSymbols(),
	    loweringProvider->getJitProxyTargetAddresses(), jitCodeGenLevel, debugInfo.enableDebug, debugInfo.enablePerf,
	    debugInfo.perfEmitDebugInfo, debugInfo.perfEmitUnwindInfo, debugInfo.perfRegionSymbols,
	    debugInfo.enableSampleSymbols, ir->getId());
	if (options.getOptionOrDefault("mlir.eager_compilation", false)) {
		auto result = engine->lookupPacked("execute");
		if (!result) {
			llvm::errs() << "Could not compile function" << result.takeError() << "\n";
		}
	}
	if (statistics != nullptr) {
		statistics->recordTimingMs("jit.compile.ms", jitStart);
		statistics->recordTimingMs("backend.totalMs", backendStart);
	}
	// 5. Get execution function from engine. Create and return execution context.
	return std::make_unique<MLIRExecutable>(std::move(engine));
}

} // namespace nautilus::compiler::mlir
