#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstddef>

namespace nautilus::compiler::ir {
class AllocaOperation : public Operation {
public:
	explicit AllocaOperation(size_t allocationSize);

	~AllocaOperation() override = default;

	size_t getSize() const;

private:
	size_t allocationSize;
};
} // namespace nautilus::compiler::ir
