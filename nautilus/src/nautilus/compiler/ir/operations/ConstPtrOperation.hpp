
#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class ConstPtrOperation : public Operation {
public:
	explicit ConstPtrOperation(OperationIdentifier identifier, void* value);

	~ConstPtrOperation() override = default;

	void* getValue();

	std::string toString() override;

	static bool classof(const Operation* Op);

private:
	void* constantValue; // Can also hold uInts
};

} // namespace nautilus::compiler::ir
