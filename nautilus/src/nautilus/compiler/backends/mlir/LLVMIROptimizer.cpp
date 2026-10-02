

#include "nautilus/compiler/backends/mlir/LLVMIROptimizer.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/compiler/backends/mlir/debug/DebugInfoOptions.hpp"
#include <chrono>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassInstrumentation.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/FileCollector.h>
#include <mlir/ExecutionEngine/OptUtils.h>

namespace nautilus::compiler::mlir {

int getOptimizationLevel(const engine::Options& options) {
	// When debug info is active, clamp to -O0 regardless of the user's
	// request.  At -O3 the IR pipeline runs SROA/DSE/GVN and codegen runs
	// aggressive register allocation — every $N SSA value lives only as
	// long as the single register holding it, so once the next add
	// overwrites that register GDB reads the wrong value for the earlier
	// variable.  -O0 skips those passes and keeps a one-to-one mapping
	// between our dbg.value records and the values the user sees.  The
	// explicit `optimizationLevel` option still takes precedence so
	// a caller can override this for diagnostic purposes.
	const int defaultLevel = debugInfoOptionsFromEngineOptions(options).enableDebug ? 0 : 3;
	return options.getOptionOrDefault("optimizationLevel", defaultLevel);
}

llvm::CodeGenOptLevel getCodeGenOptLevel(const engine::Options& options) {
	if (options.hasOption("mlir.codegenOptLevel")) {
		switch (options.getOptionOrDefault("mlir.codegenOptLevel", 3)) {
		case 0:
			return llvm::CodeGenOptLevel::None;
		case 1:
			return llvm::CodeGenOptLevel::Less;
		case 2:
			return llvm::CodeGenOptLevel::Default;
		default:
			return llvm::CodeGenOptLevel::Aggressive;
		}
	}
	return debugInfoOptionsFromEngineOptions(options).enableDebug ? llvm::CodeGenOptLevel::Less
	                                                              : llvm::CodeGenOptLevel::Aggressive;
}

namespace {

int64_t countInstructions(const llvm::Module& module) {
	int64_t count = 0;
	for (const auto& func : module) {
		count += static_cast<int64_t>(func.getInstructionCount());
	}
	return count;
}

int64_t countBasicBlocks(const llvm::Module& module) {
	int64_t count = 0;
	for (const auto& func : module) {
		count += static_cast<int64_t>(func.size());
	}
	return count;
}

llvm::OptimizationLevel toOptimizationLevel(int level) {
	switch (level) {
	case 0:
		return llvm::OptimizationLevel::O0;
	case 1:
		return llvm::OptimizationLevel::O1;
	case 2:
		return llvm::OptimizationLevel::O2;
	default:
		return llvm::OptimizationLevel::O3;
	}
}

/// Pass-manager state for one run, configured like mlir::makeOptimizingTransformer
/// so a textual pipeline is directly comparable with the `optimizationLevel`
/// one. With a non-null @p instrumentation, the PassBuilder registers every
/// pass's class-to-name mapping in it, which printPipeline() needs.
struct PipelineContext {
	llvm::LoopAnalysisManager lam;
	llvm::FunctionAnalysisManager fam;
	llvm::CGSCCAnalysisManager cgam;
	llvm::ModuleAnalysisManager mam;
	llvm::PassBuilder pb;

	static llvm::PipelineTuningOptions tuningOptions() {
		llvm::PipelineTuningOptions options;
		options.LoopUnrolling = true;
		options.LoopInterleaving = true;
		options.LoopVectorization = true;
		options.SLPVectorization = true;
		return options;
	}

	PipelineContext(llvm::TargetMachine* targetMachine, llvm::PassInstrumentationCallbacks* instrumentation)
	    : pb(targetMachine, tuningOptions(), std::nullopt, instrumentation) {
		pb.registerModuleAnalyses(mam);
		pb.registerCGSCCAnalyses(cgam);
		pb.registerFunctionAnalyses(fam);
		pb.registerLoopAnalyses(lam);
		pb.crossRegisterProxies(lam, fam, cgam, mam);
	}
};

/// Runs the textual new-pass-manager pipeline @p pipeline (the `opt -passes=`
/// syntax, e.g. `default<O2>` or `function(sroa,instcombine)`) over @p module.
llvm::Error runTextualPipeline(llvm::Module& module, llvm::TargetMachine* targetMachine, llvm::StringRef pipeline) {
	PipelineContext context(targetMachine, nullptr);
	llvm::ModulePassManager mpm;
	if (auto err = context.pb.parsePassPipeline(mpm, pipeline)) {
		return llvm::make_error<llvm::StringError>("invalid mlir.llvmPipeline '" + pipeline.str() +
		                                               "': " + llvm::toString(std::move(err)),
		                                           llvm::inconvertibleErrorCode());
	}
	mpm.run(module, context.mam);
	return llvm::Error::success();
}

/// The fully expanded textual form of the pipeline a compile runs: @p pipeline
/// itself when set, otherwise the default pipeline for @p level. Feeding the
/// result back through `mlir.llvmPipeline` reproduces that pipeline pass for
/// pass, which is what lets a tool ablate individual passes out of it.
std::string printPipeline(llvm::TargetMachine* targetMachine, int level, llvm::StringRef pipeline) {
	llvm::PassInstrumentationCallbacks instrumentation;
	PipelineContext context(targetMachine, &instrumentation);
	llvm::ModulePassManager mpm;
	if (!pipeline.empty()) {
		if (auto err = context.pb.parsePassPipeline(mpm, pipeline)) {
			llvm::consumeError(std::move(err));
			return pipeline.str();
		}
	} else if (level == 0) {
		mpm.addPass(context.pb.buildO0DefaultPipeline(llvm::OptimizationLevel::O0));
	} else {
		mpm.addPass(context.pb.buildPerModuleDefaultPipeline(toOptimizationLevel(level)));
	}
	std::string text;
	llvm::raw_string_ostream stream(text);
	mpm.printPipeline(stream, [&](llvm::StringRef className) {
		auto passName = instrumentation.getPassNameForClassName(className);
		return passName.empty() ? className : passName;
	});
	return text;
}

} // namespace

LLVMIROptimizer::LLVMIROptimizer() = default;
LLVMIROptimizer::~LLVMIROptimizer() = default;

std::function<llvm::Error(llvm::Module*)> LLVMIROptimizer::getLLVMOptimizerPipeline(const engine::Options& options,
                                                                                    const DumpHandler& handler,
                                                                                    CompilationStatistics* statistics) {
	// Return LLVM optimizer pipeline.
	return [options, &handler, statistics](llvm::Module* llvmIRModule) {
		// Currently, we do not increase the sizeLevel requirement of the
		// optimizingTransformer beyond 0.
		constexpr int SIZE_LEVEL = 0;
		// Create A target-specific target machine for the host. The `mlir.targetCpu`
		// option pins the CPU (and drops the host feature set) so IR-generation and
		// optimization decisions — most visibly the LLVM loop vectorizer's width —
		// are deterministic across machines. This is what the LLVM IR reference
		// tests rely on; without the pin, a runner with AVX-512 vectorizes wider
		// than one without, so the generated IR differs run to run.
		auto tmBuilderOrError = llvm::orc::JITTargetMachineBuilder::detectHost();
		const std::string pinnedCpu = options.getOptionOrDefault<std::string>("mlir.targetCpu", "");
		if (!pinnedCpu.empty()) {
			tmBuilderOrError->setCPU(pinnedCpu);
			// Pin an explicit baseline feature set (rather than leaving the
			// host's) so TTI-driven decisions are deterministic. A non-empty
			// feature string also keeps the reference-IR normalizer's
			// target-features stripping effective.
			tmBuilderOrError->setFeatures("+64bit,+sse2");
		}
		auto targetMachine = tmBuilderOrError->createTargetMachine();
		llvm::TargetMachine* targetMachinePtr = targetMachine->get();
		// With debug info active, lower the codegen opt level to `Less`
		// (-O1 equivalent) so the register allocator does not collapse
		// separate SSA values into the same physical register.
		// `None` triggers fast-regalloc which spills everything to stack
		// and breaks LLVM's DWARF emission for dbg.value expressions.
		// Keyed on `enableDebug` alone: perf-only mode keeps the codegen
		// level the IR optimizer above chose.
		// `mlir.codegenOptLevel` overrides either.
		const auto debugInfoForCodegen = debugInfoOptionsFromEngineOptions(options);
		targetMachinePtr->setOptLevel(getCodeGenOptLevel(options));

		// Add target-specific attributes to all non-declaration functions in the module.
		for (auto& func : *llvmIRModule) {
			if (func.isDeclaration()) {
				continue;
			}
			func.addAttributeAtIndex(
			    ~0, llvm::Attribute::get(llvmIRModule->getContext(), "target-cpu", targetMachinePtr->getTargetCPU()));
			func.addAttributeAtIndex(~0, llvm::Attribute::get(llvmIRModule->getContext(), "target-features",
			                                                  targetMachinePtr->getTargetFeatureString()));
			func.addAttributeAtIndex(
			    ~0, llvm::Attribute::get(llvmIRModule->getContext(), "tune-cpu", targetMachinePtr->getTargetCPU()));
			// At -O3 LLVM omits frame pointers by default, so a
			// frame-pointer-based unwinder cannot walk out of a JIT frame into
			// the host application's stack. That is true of `perf record -g`
			// out of process and of an in-process sampler's callchain alike,
			// so both `perf` and `perf.sample` ask for this. Cheap either way.
			if (debugInfoForCodegen.emitPerfMetadata() && debugInfoForCodegen.perfFramePointers) {
				func.addAttributeAtIndex(~0, llvm::Attribute::get(llvmIRModule->getContext(), "frame-pointer", "all"));
			}
		}

		// When debug info was requested during MLIR lowering, the translated
		// LLVM module needs the `Debug Info Version` and `Dwarf Version` module
		// flags so LLVM's codegen emits the DWARF sections that GDB's JIT
		// interface (and PerfSupportPlugin's DWARFContext) read.  MLIR's
		// DebugTranslation only sets these when a DICompileUnit is present in
		// the module; we defensively add them regardless so that a bare
		// function with only FileLineColLocs still yields a valid line table.
		// `emitDebugInfo()`: a perf-only compile needs these flags just as
		// much as a debug one to get a DWARF line table at all.
		const auto debugInfo = debugInfoOptionsFromEngineOptions(options);
		if (debugInfo.emitDebugInfo()) {
			if (!llvmIRModule->getModuleFlag("Debug Info Version")) {
				llvmIRModule->addModuleFlag(llvm::Module::Warning, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);
			}
			if (!llvmIRModule->getModuleFlag("Dwarf Version")) {
				llvmIRModule->addModuleFlag(llvm::Module::Warning, "Dwarf Version", debugInfo.dwarfVersion);
			}
		}

		// Apply optional pre-optimization module transforms installed by plugins
		// (e.g. the inlining plugin's JIT-time LLVM inliner).
		if (options.getOptionOrDefault("mlir.inline_invoke_calls", false)) {
			if (const auto& hook = getLLVMBackendHooks().preOptModuleTransform) {
				hook(*llvmIRModule);
			}
		}

		// Dump LLVM IR BEFORE optimization so tests / developers can see
		// which debug intrinsics survived MLIR -> LLVM translation but
		// before LLVM's opt pipeline potentially strips them.
		handler.dump("before_llvm_optimization", "ll", [&]() {
			std::string llvmIRString;
			llvm::raw_string_ostream llvmStringStream(llvmIRString);
			llvmIRModule->print(llvmStringStream, nullptr);
			return llvmIRString;
		});

		// `mlir.llvmPipeline` replaces the `optimizationLevel` pipeline with an
		// arbitrary textual one, so individual LLVM passes can be measured.
		const auto optimizationLevel = getOptimizationLevel(options);
		const auto customPipeline = options.getOptionOrDefault<std::string>("mlir.llvmPipeline", "");
		if (statistics != nullptr) {
			statistics->set("llvm.ir.instructions.before", countInstructions(*llvmIRModule));
			if (options.getOptionOrDefault("mlir.recordLLVMPipeline", false)) {
				statistics->set("llvm.pipeline", printPipeline(targetMachinePtr, optimizationLevel, customPipeline));
			}
		}
		const auto optimizeStart = std::chrono::steady_clock::now();
		auto optimizedModule =
		    customPipeline.empty()
		        ? ::mlir::makeOptimizingTransformer(optimizationLevel, SIZE_LEVEL, targetMachinePtr)(llvmIRModule)
		        : runTextualPipeline(*llvmIRModule, targetMachinePtr, customPipeline);
		if (statistics != nullptr) {
			statistics->recordTimingMs("llvm.optimize.ms", optimizeStart);
			statistics->set("llvm.ir.instructions.after", countInstructions(*llvmIRModule));
			statistics->set("llvm.ir.basicBlocks.after", countBasicBlocks(*llvmIRModule));
		}

		handler.dump("after_llvm_generation", "ll", [&]() {
			std::string llvmIRString;
			llvm::raw_string_ostream llvmStringStream(llvmIRString);
			llvmIRModule->print(llvmStringStream, nullptr);
			return llvmIRString;
		});

		return optimizedModule;
	};
}
} // namespace nautilus::compiler::mlir
