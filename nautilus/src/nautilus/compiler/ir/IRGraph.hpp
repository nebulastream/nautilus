
#pragma once

#include "nautilus/JITCompiler.hpp"
#include <memory>
#include <ostream>
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

	FunctionOperation* getRootOperation();

	[[nodiscard]] std::string toString() const;
	[[nodiscard]] const CompilationUnitID& getId() const;

	friend std::ostream& operator<<(std::ostream& os, const IRGraph& graph);

private:
	std::unique_ptr<FunctionOperation> rootOperation;
	const CompilationUnitID id;
};

} // namespace nautilus::compiler::ir
