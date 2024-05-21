
#pragma once

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
	IRGraph();

	~IRGraph() = default;

	std::unique_ptr<FunctionOperation>& addRootOperation(std::unique_ptr<FunctionOperation> rootOperation);

	FunctionOperation* getRootOperation();

	[[nodiscard]] std::string toString() const;

	friend std::ostream& operator<<(std::ostream& os, const IRGraph& graph);

private:
	std::unique_ptr<FunctionOperation> rootOperation;
};

} // namespace nautilus::compiler::ir
