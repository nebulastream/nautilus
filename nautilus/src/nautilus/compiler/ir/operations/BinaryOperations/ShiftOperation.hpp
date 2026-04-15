
#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class ShiftOperation final : public BinaryOperation {
public:
	enum ShiftType { LS, RS };

	ShiftOperation(common::Arena& arena, OperationIdentifier identifier, Operation* leftInput, Operation* rightInput,
	               ShiftType type);

	~ShiftOperation() = default;

	static bool classof(const Operation* Op);

	ShiftType getType() const;

private:
	ShiftType type;
};
} // namespace nautilus::compiler::ir
