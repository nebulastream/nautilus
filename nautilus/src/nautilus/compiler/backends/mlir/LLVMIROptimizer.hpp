#pragma once
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/options.hpp"
#include <llvm/Support/CodeGen.h>
#include <map>
#include <string>
namespace NES {
class DumpHelper;
namespace Nautilus {
class CompilationOptions;
}
} // namespace NES

#include <llvm/IR/Module.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/Pass/Pass.h>
#include <vector>

namespace nautilus::compiler {
class CompilationStatistics;
}

namespace nautilus::compiler::mlir {

/**
 * @brief The machine-code generation level for the JIT: `mlir.codegenOptLevel`
 * (0-3) when set, otherwise `Less` with debug info active and `Aggressive`
 * without.
 */
llvm::CodeGenOptLevel getCodeGenOptLevel(const engine::Options& options);

/// Makes LLVM's legacy pass manager, which generates the machine code, time
/// every pass, and zeroes every timer. Process-wide (the timers are LLVM
/// globals) and never turned off again: `mlir.recordPassTimings` is a
/// diagnostic option, and concurrent compilations would share the timers.
void resetCodegenPassTimers();

/// The wall time in milliseconds of every machine-code generation pass timed
/// since resetCodegenPassTimers(), by pass argument (e.g. `x86-isel`, `greedy`).
std::map<std::string, double> snapshotCodegenPassTimers();

/**
 * @brief The LLVMIROptimizer takes a generated MLIR module,
 * and applies configured lowering & optimization passes to it.
 */
class LLVMIROptimizer {
public:
	LLVMIROptimizer();  // Disable default constructor
	~LLVMIROptimizer(); // Disable default destructor

	/**
	 * @brief Builds the transformer the JIT runs over the translated LLVM module.
	 *
	 * The transformer runs synchronously while the JIT is created, so
	 * @p statistics (nullable) only has to outlive that call. It receives
	 * `llvm.optimize.ms`, the module's instruction counts before and after the
	 * pipeline and, with `mlir.recordLLVMPipeline`, the textual pipeline run.
	 */
	static std::function<llvm::Error(llvm::Module*)>
	getLLVMOptimizerPipeline(const engine::Options& options, const DumpHandler& handler,
	                         CompilationStatistics* statistics = nullptr);
};
} // namespace nautilus::compiler::mlir
