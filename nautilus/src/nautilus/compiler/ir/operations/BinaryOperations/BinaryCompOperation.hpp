

#pragma once

#include <nautilus/compiler/ir/operations/Operation.hpp>

namespace nautilus::compiler::ir {

class BinaryCompOperation final : public BinaryOperation {
public:
	enum Type { BAND, BOR, XOR };

	BinaryCompOperation(common::Arena& arena, OperationIdentifier identifier, Operation* leftInput,
	                    Operation* rightInput, Type type);

	~BinaryCompOperation() = default;

	static bool classof(const Operation* Op);

	Type getType() const;

private:
	Type type;
};
} // namespace nautilus::compiler::ir
