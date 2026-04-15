#pragma once
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class OrOperation : public BinaryOperation {
public:
	OrOperation(common::Arena& arena, OperationIdentifier identifier, Operation* leftInput, Operation* rightInput);

	~OrOperation() = default;

	static bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir
