#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstdint>

namespace nautilus::compiler::ir {
class ReturnOperation : public Operation {
public:
	ReturnOperation();

	ReturnOperation(Operation* returnValue);

	~ReturnOperation() override = default;

	Operation* getReturnValue();

	void setReturnValue(Operation* newReturnValue);

	bool hasReturnValue();

	std::string toString() override;
};
} // namespace nautilus::compiler::ir
