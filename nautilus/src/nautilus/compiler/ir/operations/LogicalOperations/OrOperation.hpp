#pragma once
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class OrOperation : public BinaryOperation {
public:
	OrOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput);

	~OrOperation() override = default;

	bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir
