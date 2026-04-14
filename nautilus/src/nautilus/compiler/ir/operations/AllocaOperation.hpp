#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstddef>

namespace nautilus::compiler::ir {
class AllocaOperation : public Operation {
public:
	AllocaOperation(OperationIdentifier id, size_t allocationSize);

	~AllocaOperation() override = default;

	size_t getSize() const;

	static bool classof(const Operation* op);

private:
	size_t allocationSize;
};
} // namespace nautilus::compiler::ir
