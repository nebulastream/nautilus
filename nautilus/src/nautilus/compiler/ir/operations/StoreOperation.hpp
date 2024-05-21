#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <vector>

namespace nautilus::compiler::ir {
class StoreOperation : public Operation {
public:
	explicit StoreOperation(Operation* value, Operation* address);

	~StoreOperation() override = default;

	Operation* getValue();

	Operation* getAddress();

	void setValue(Operation* newValue);

	void setAddress(Operation* newAddress);

	std::string toString() override;
};
} // namespace nautilus::compiler::ir
