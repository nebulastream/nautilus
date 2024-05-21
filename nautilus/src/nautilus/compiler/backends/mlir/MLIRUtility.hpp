#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp>
#include <mlir/ExecutionEngine/ExecutionEngine.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/OwningOpRef.h>
#include <string>

namespace nautilus::compiler::mlir {

/**
 * @brief Provides utility functions for the MLIR backend.
 */
class MLIRUtility {
public:
	MLIRUtility();
	~MLIRUtility();

	/**
	 * @brief Writes an MLIR module to a file. A module that is loaded from file allows for step-through debugging.
	 * @param mlirModule: The module to write.
	 * @param mlirFilepath: Target write path.
	 */
	static void writeMLIRModuleToFile(mlir::OwningOpRef<mlir::ModuleOp>& mlirModule, std::string mlirFilepath);

	/**
	 * @brief Takes an MLIR module in string representation,  and lowers, compiles, and executes it.
	 * @param moduleString: String that should represent a valid MLIR module.
	 * @param rootFunctionName: Name of the function that is called to initiate execution.
	 * @return int: 0 if success, 1 otherwise.
	 */
	static int loadAndExecuteModuleFromString(const std::string& moduleString, const std::string& rootFunctionName);

	/**
	 * @brief Takes NESIR, lowers it to MLIR, optimizes it, and returns an MLIR ExecutionEngine.
	 * @param ir: NESIR that is lowered to MLIR, and JIT compiled.
	 * @return std::unique_ptr<mlir::ExecutionEngine> Provides a function pointer to executable function.
	 */
	static std::unique_ptr<mlir::ExecutionEngine>
	compileNESIRToMachineCode(std::shared_ptr<NES::Nautilus::IR::IRGraph> ir);
};

} // namespace nautilus::compiler::mlir
