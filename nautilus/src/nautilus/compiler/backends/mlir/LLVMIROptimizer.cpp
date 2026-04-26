

#include "nautilus/compiler/backends/mlir/LLVMIROptimizer.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/compiler/backends/mlir/debug/DebugInfoOptions.hpp"
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
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
	// explicit `mlir.optimizationLevel` option still takes precedence so
	// a caller can override this for diagnostic purposes.
	const int defaultLevel = debugInfoOptionsFromEngineOptions(options).enable ? 0 : 3;
	return options.getOptionOrDefault("mlir.optimizationLevel", defaultLevel);
}

LLVMIROptimizer::LLVMIROptimizer() = default;
LLVMIROptimizer::~LLVMIROptimizer() = default;

std::function<llvm::Error(llvm::Module*)> LLVMIROptimizer::getLLVMOptimizerPipeline(const engine::Options& options,
                                                                                    const DumpHandler& handler) {
	// Return LLVM optimizer pipeline.
	return [options, &handler](llvm::Module* llvmIRModule) {
		// Currently, we do not increase the sizeLevel requirement of the
		// optimizingTransformer beyond 0.
		constexpr int SIZE_LEVEL = 0;
		// Create A target-specific target machine for the host
		auto tmBuilderOrError = llvm::orc::JITTargetMachineBuilder::detectHost();
		auto targetMachine = tmBuilderOrError->createTargetMachine();
		llvm::TargetMachine* targetMachinePtr = targetMachine->get();
		// With debug info active, lower the codegen opt level to `Less`
		// (-O1 equivalent) so the register allocator does not collapse
		// separate SSA values into the same physical register.
		// `None` triggers fast-regalloc which spills everything to stack
		// and breaks LLVM's DWARF emission for dbg.value expressions.
		const auto debugInfoForCodegen = debugInfoOptionsFromEngineOptions(options);
		targetMachinePtr->setOptLevel(debugInfoForCodegen.enable ? llvm::CodeGenOptLevel::Less
		                                                         : llvm::CodeGenOptLevel::Aggressive);

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
		}

		// When debug info was requested during MLIR lowering, the translated
		// LLVM module needs the `Debug Info Version` and `Dwarf Version` module
		// flags so LLVM's codegen emits the DWARF sections that GDB's JIT
		// interface reads.  MLIR's DebugTranslation only sets these when a
		// DICompileUnit is present in the module; we defensively add them
		// regardless so that a bare function with only FileLineColLocs still
		// yields a valid line table.
		const auto debugInfo = debugInfoOptionsFromEngineOptions(options);
		if (debugInfo.enable) {
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

		auto optPipeline =
		    ::mlir::makeOptimizingTransformer(getOptimizationLevel(options), SIZE_LEVEL, targetMachinePtr);
		auto optimizedModule = optPipeline(llvmIRModule);

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
