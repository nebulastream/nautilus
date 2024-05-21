
#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class SubOperation final : public BinaryOperation {
public:
	SubOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput);

	~SubOperation() override = default;

	std::string toString() override;

	bool classof(const Operation* Op);
};
} // namespace nautilus::compiler::ir
