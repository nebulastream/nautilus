#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class MulOperation final : public BinaryOperation {
public:
	MulOperation(common::Arena& arena, OperationIdentifier identifier, Operation* leftInput, Operation* rightInput);

	~MulOperation() = default;

	static bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir
