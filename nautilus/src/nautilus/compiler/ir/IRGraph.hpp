
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

	const FunctionOperation& getRootOperation() const;
	std::string toString() const;

	[[nodiscard]] const CompilationUnitID& getId() const;

private:
	std::unique_ptr<FunctionOperation> rootOperation;
	const CompilationUnitID id;
};

} // namespace nautilus::compiler::ir
