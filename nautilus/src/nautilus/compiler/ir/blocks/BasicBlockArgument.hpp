

#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <ostream>

namespace nautilus::compiler::ir {

class BasicBlockArgument : public Operation {
public:
	explicit BasicBlockArgument(const OperationIdentifier identifier, Type stamp);

	~BasicBlockArgument() override = default;

	friend std::ostream& operator<<(std::ostream& os, const BasicBlockArgument& argument);

	std::string toString() override;
};

} // namespace nautilus::compiler::ir
