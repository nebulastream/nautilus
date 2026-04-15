
#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class CastOperation : public Operation {
public:
	explicit CastOperation(common::Arena& arena, OperationIdentifier identifier, Operation* input, Type targetStamp);

	~CastOperation() = default;

	Operation* getInput() const;

	void setInput(Operation* newInput);

	static bool classof(const Operation* op);
};

} // namespace nautilus::compiler::ir
