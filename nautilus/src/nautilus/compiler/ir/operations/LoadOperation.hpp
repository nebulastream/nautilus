#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {
class LoadOperation : public Operation {
public:
	explicit LoadOperation(OperationIdentifier identifier, Operation* address, Type stamp);

	~LoadOperation() override = default;

	const Operation* getAddress() const;

	static bool classof(const Operation* op);
};
} // namespace nautilus::compiler::ir
