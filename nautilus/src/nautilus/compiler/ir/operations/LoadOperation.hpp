#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {
class LoadOperation : public Operation {
public:
	explicit LoadOperation(common::Arena& arena, OperationIdentifier identifier, Operation* address, Type stamp);

	~LoadOperation() = default;

	const Operation* getAddress() const;

	static bool classof(const Operation* op);
};
} // namespace nautilus::compiler::ir
