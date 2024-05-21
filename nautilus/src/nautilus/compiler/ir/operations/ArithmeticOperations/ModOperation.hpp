
#pragma once

#include <nautilus/compiler/ir/operations/Operation.hpp>

namespace nautilus::compiler::ir {

class ModOperation final : public BinaryOperation {
public:
	ModOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput);

	~ModOperation() override = default;

	std::string toString() override;

	bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir
