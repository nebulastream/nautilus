

#pragma once

#include <nautilus/compiler/ir/operations/Operation.hpp>

namespace nautilus::compiler::ir {

class BinaryCompOperation final : public BinaryOperation {
public:
	enum Type { BAND, BOR, XOR };

	BinaryCompOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput, Type type);

	~BinaryCompOperation() override = default;

	std::string toString() override;

	bool classof(const Operation* Op);

	Type getType() const;

private:
	Type type;
};
} // namespace nautilus::compiler::ir
