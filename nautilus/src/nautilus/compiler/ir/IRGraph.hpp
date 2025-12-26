
#pragma once

#include "nautilus/JITCompiler.hpp"
#include <memory>
#include <string>

namespace nautilus::compiler::ir {

class FunctionOperation;

/**
 * @brief The IRGraph represents a fragment of nautilus ir.
 */
class IRGraph {
public:
	IRGraph(const CompilationUnitID& id);

	~IRGraph() = default;

	std::unique_ptr<FunctionOperation>& addRootOperation(std::unique_ptr<FunctionOperation> rootOperation);

	/**
	 * @brief Adds a function operation to the IR graph
	 * @param functionOperation The function operation to add
	 * @return Reference to the added function operation
	 */
	std::unique_ptr<FunctionOperation>& addFunctionOperation(std::unique_ptr<FunctionOperation> functionOperation);

	/**
	 * @brief Gets all function operations in the IR graph
	 * @return Vector of function operations
	 */
	const std::vector<std::unique_ptr<FunctionOperation>>& getFunctionOperations() const;

	/**
	 * @brief Gets a specific function operation by name
	 * @param name The name of the function
	 * @return Pointer to the function operation if found, nullptr otherwise
	 */
	const FunctionOperation* getFunctionOperation(const std::string& name) const;

	std::string toString() const;

	[[nodiscard]] const CompilationUnitID& getId() const;

private:
	std::unique_ptr<FunctionOperation> rootOperation;
	std::vector<std::unique_ptr<FunctionOperation>> functionOperations;
	const CompilationUnitID id;
};

} // namespace nautilus::compiler::ir
