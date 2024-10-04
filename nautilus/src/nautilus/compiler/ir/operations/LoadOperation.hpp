#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {
class LoadOperation : public Operation {
public:
	explicit LoadOperation(const OperationIdentifier& identifier, Operation* address, Type stamp);

	~LoadOperation() override = default;

	const Operation* getAddress() const;
};
} // namespace nautilus::compiler::ir
