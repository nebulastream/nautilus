#pragma once

#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/Location.h>
#include <mlir/IR/Operation.h>
#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/BuiltinOps.h>
#include <string>

namespace nautilus::compiler::mlir {

/**
 * @brief Utility class for building debug information in MLIR.
 *
 * This class provides utilities to annotate MLIR operations with semantic names and location
 * information that will be preserved during lowering to LLVM IR, enabling debuggers to show
 * meaningful variable names and source locations.
 *
 * When operations are annotated with NameLoc and FileLineColLoc, these locations are preserved
 * through the MLIR->LLVM IR lowering process and can be used by debuggers to display
 * source-level information.
 */
class DebugInfoBuilder {
public:
	/**
	 * @brief Initializes the debug info builder with an MLIR context and builder.
	 *
	 * @param context: MLIR context for creating attributes and types
	 * @param builder: MLIR OpBuilder for creating operations
	 */
	explicit DebugInfoBuilder(::mlir::MLIRContext& context, ::mlir::OpBuilder& builder);

	/**
	 * @brief Creates debug info infrastructure for a module.
	 *
	 * Currently a no-op but reserved for future debug metadata generation.
	 *
	 * @param moduleOp: The MLIR module operation
	 * @param sourceFileName: Source file name (e.g., "Query_1")
	 */
	void createModuleDebugInfo(::mlir::ModuleOp moduleOp, const std::string& sourceFileName);

	/**
	 * @brief Annotates an MLIR operation with debug location information.
	 *
	 * Attaches location and semantic name information to an operation so that the
	 * information is preserved through lowering and can be used by debuggers.
	 *
	 * @param op: The operation to annotate
	 * @param semanticName: Meaningful name for the value (e.g., "sum", "temp1")
	 * @param fileName: Source file name
	 * @param line: Source line number
	 * @param column: Source column number
	 */
	void annotateOperationWithDebugInfo(::mlir::Operation* op, const std::string& semanticName,
	                                     const std::string& fileName, unsigned line, unsigned column);

private:
	::mlir::MLIRContext* context;
	::mlir::OpBuilder* builder;
	bool moduleDebugInfoCreated = false;
};

} // namespace nautilus::compiler::mlir
