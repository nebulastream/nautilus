#pragma once
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class AndOperation : public BinaryOperation {
public:
	AndOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput);

	~AndOperation() override = default;

	std::string toString() override;

	bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir
