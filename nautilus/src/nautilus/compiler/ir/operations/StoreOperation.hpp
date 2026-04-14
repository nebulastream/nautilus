#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <vector>

namespace nautilus::compiler::ir {
class StoreOperation : public Operation {
public:
	explicit StoreOperation(Operation* value, Operation* address);

	~StoreOperation() override = default;

	Operation* getValue() const;

	Operation* getAddress() const;

	void setValue(Operation* newValue);

	void setAddress(Operation* newAddress);

	static bool classof(const Operation* op);
};
} // namespace nautilus::compiler::ir
