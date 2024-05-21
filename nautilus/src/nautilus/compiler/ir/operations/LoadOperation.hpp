#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {
class LoadOperation : public Operation {
public:
	explicit LoadOperation(const OperationIdentifier& identifier, Operation* address, Type stamp);

	~LoadOperation() override = default;

	const Operation* getAddress();

	std::string toString() override;
};
} // namespace nautilus::compiler::ir
