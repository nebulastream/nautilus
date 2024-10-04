#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class MulOperation final : public BinaryOperation {
public:
	MulOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput);

	~MulOperation() override = default;

	bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir
