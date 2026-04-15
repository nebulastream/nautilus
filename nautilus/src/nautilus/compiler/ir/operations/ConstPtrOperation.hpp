
#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class ConstPtrOperation : public Operation {
public:
	explicit ConstPtrOperation(common::Arena& arena, OperationIdentifier identifier, void* value);

	~ConstPtrOperation() = default;

	void* getValue() const;

	static bool classof(const Operation* Op);

private:
	void* constantValue; // Can also hold uInts
};

} // namespace nautilus::compiler::ir
