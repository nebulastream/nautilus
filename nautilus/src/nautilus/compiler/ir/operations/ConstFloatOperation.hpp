
#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class ConstFloatOperation : public Operation {
public:
	explicit ConstFloatOperation(common::Arena& arena, OperationIdentifier identifier, double constantValue,
	                             Type stamp);

	~ConstFloatOperation() = default;

	double getValue() const;

	template <class T>
	T getFloatViaType();

	static bool classof(const Operation* Op);

private:
	double constantValue;
};

} // namespace nautilus::compiler::ir
