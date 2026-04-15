#pragma once
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class AndOperation : public BinaryOperation {
public:
	AndOperation(common::Arena& arena, OperationIdentifier identifier, Operation* leftInput, Operation* rightInput);

	~AndOperation() = default;

	static bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir
