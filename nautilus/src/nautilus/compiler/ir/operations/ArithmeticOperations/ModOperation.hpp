
#pragma once

#include <nautilus/compiler/ir/operations/Operation.hpp>

namespace nautilus::compiler::ir {

class ModOperation final : public BinaryOperation {
public:
	ModOperation(common::Arena& arena, OperationIdentifier identifier, Operation* leftInput, Operation* rightInput);

	~ModOperation() = default;

	static bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir
