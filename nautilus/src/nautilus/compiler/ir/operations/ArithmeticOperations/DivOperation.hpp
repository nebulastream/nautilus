#pragma once
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class DivOperation final : public BinaryOperation {
public:
	DivOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput);

	~DivOperation() override = default;

	bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir
