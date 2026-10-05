#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class ConstIntOperation : public Operation {
public:
	explicit ConstIntOperation(common::Arena& arena, OperationIdentifier identifier, int64_t constantValue, Type stamp,
	                           ConstantOrigin origin = ConstantOrigin::Unspecified);

	~ConstIntOperation() = default;

	int64_t getValue() const;

	ConstantOrigin getConstantOrigin() const;

	template <class T>
	T getIntegerViaType();

	static bool classof(const Operation* Op);

private:
	int64_t constantValue; // Can also hold uInts
	ConstantOrigin constantOrigin;
};

} // namespace nautilus::compiler::ir
