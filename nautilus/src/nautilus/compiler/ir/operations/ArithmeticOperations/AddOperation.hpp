#pragma once
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class AddOperation final : public BinaryOperation {
public:
	AddOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput);

	~AddOperation() override = default;

	bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir
