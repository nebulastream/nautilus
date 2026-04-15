
#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class ConstBooleanOperation : public Operation {
public:
	explicit ConstBooleanOperation(common::Arena& arena, OperationIdentifier identifier, bool value);

	~ConstBooleanOperation() = default;

	bool getValue() const;

	static bool classof(const Operation* Op);

private:
	bool constantValue; // Can also hold uInts
};

} // namespace nautilus::compiler::ir
