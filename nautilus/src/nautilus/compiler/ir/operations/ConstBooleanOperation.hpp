
#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class ConstBooleanOperation : public Operation {
public:
	explicit ConstBooleanOperation(common::Arena& arena, OperationIdentifier identifier, bool value,
	                               ConstantOrigin origin = ConstantOrigin::Unspecified);

	~ConstBooleanOperation() = default;

	bool getValue() const;

	ConstantOrigin getConstantOrigin() const;

	static bool classof(const Operation* Op);

private:
	bool constantValue; // Can also hold uInts
	ConstantOrigin constantOrigin;
};

} // namespace nautilus::compiler::ir
