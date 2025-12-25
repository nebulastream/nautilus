#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

/**
 * @brief SelectOperation represents a conditional select operation (like LLVM's select or MLIR's arith.select).
 *
 * Selects between two values based on a boolean condition:
 * result = condition ? trueValue : falseValue
 *
 * This is a ternary operation with three inputs:
 * - condition: boolean value (Type::b)
 * - trueValue: value to return if condition is true
 * - falseValue: value to return if condition is false
 *
 * Both trueValue and falseValue must have the same type, which becomes the result type.
 */
class SelectOperation : public Operation {
public:
	explicit SelectOperation(OperationIdentifier identifier, Operation* condition, Operation* trueValue,
	                         Operation* falseValue, Type resultStamp);

	~SelectOperation() override = default;

	Operation* getCondition() const;
	Operation* getTrueValue() const;
	Operation* getFalseValue() const;

	void setCondition(Operation* newCondition);
	void setTrueValue(Operation* newTrueValue);
	void setFalseValue(Operation* newFalseValue);
};

} // namespace nautilus::compiler::ir
