
#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

class CastOperation : public Operation {
public:
	explicit CastOperation(OperationIdentifier identifier, Operation* input, Type targetStamp);

	~CastOperation() override = default;

	std::string toString() override;

	Operation* getInput();

	void setInput(Operation* newInput);
};

} // namespace nautilus::compiler::ir
