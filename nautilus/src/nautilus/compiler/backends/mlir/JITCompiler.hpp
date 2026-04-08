#pragma once

#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include <llvm/IR/Module.h>
#include <mlir/ExecutionEngine/ExecutionEngine.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/Pass/Pass.h>
#include <vector>

namespace nautilus::compiler::mlir {

/**
 * @brief The JITCompiler takes a generated MLIR module,
 * and applies configured lowering & optimization passes to it.
 */
class JITCompiler {
public:
	JITCompiler() = delete;  // Disable default constructor
	~JITCompiler() = delete; // Disable default destructor

	static std::unique_ptr<::mlir::ExecutionEngine>
	jitCompileModule(::mlir::OwningOpRef<::mlir::ModuleOp>& mlirModule,
	                 const llvm::function_ref<llvm::Error(llvm::Module*)> optPipeline,
	                 const std::vector<std::string>& jitProxyFunctionSymbols,
	                 const std::vector<void*>& jitProxyFunctionTargetAddresses);
};
} // namespace nautilus::compiler::mlir
