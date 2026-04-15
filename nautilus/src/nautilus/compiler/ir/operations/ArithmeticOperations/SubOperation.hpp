
#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class SubOperation final : public BinaryOperation {
public:
	SubOperation(common::Arena& arena, OperationIdentifier identifier, Operation* leftInput, Operation* rightInput);

	~SubOperation() = default;

	static bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir
