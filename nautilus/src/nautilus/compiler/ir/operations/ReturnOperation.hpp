#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstdint>

namespace nautilus::compiler::ir {
class ReturnOperation : public Operation {
public:
	/// Void return: no SSA value to thread back, no inputs to allocate, so
	/// the no-input Operation constructor is used and no Arena is required.
	ReturnOperation();

	/// Value return: the returned value is stored as the single input,
	/// allocated from @p arena.
	ReturnOperation(common::Arena& arena, Operation* returnValue);

	~ReturnOperation() = default;

	Operation* getReturnValue() const;

	void setReturnValue(Operation* newReturnValue);

	bool hasReturnValue() const;

	static bool classof(const Operation* op);
};
} // namespace nautilus::compiler::ir
