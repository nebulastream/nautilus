#pragma once
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class DivOperation final : public BinaryOperation {
public:
	DivOperation(common::Arena& arena, OperationIdentifier identifier, Operation* leftInput, Operation* rightInput);

	~DivOperation() = default;

	static bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir
