#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class ConstIntOperation : public Operation {
public:
	explicit ConstIntOperation(OperationIdentifier identifier, int64_t constantValue, Type stamp);

	~ConstIntOperation() override = default;

	int64_t getValue();

	template <class T>
	T getIntegerViaType();

	std::string toString() override;

	static bool classof(const Operation* Op);

private:
	int64_t constantValue; // Can also hold uInts
};

} // namespace nautilus::compiler::ir
