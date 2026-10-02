#pragma once
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/options.hpp"
#include <llvm/Support/CodeGen.h>
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
