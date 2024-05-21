
#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class ConstFloatOperation : public Operation {
public:
	explicit ConstFloatOperation(OperationIdentifier identifier, double constantValue, Type stamp);

	~ConstFloatOperation() override = default;

	double getValue();

	template <class T>
	T getFloatViaType();

	std::string toString() override;

	static bool classof(const Operation* Op);

private:
	double constantValue;
};

} // namespace nautilus::compiler::ir
