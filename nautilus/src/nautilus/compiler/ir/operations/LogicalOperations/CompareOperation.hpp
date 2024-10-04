#pragma once
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {
class CompareOperation : public BinaryOperation {
public:
	enum Comparator {
		EQ = 0,
		NE = 1,
		LT = 2,
		LE = 3,
		GT = 4,
		GE = 5,
	};

	CompareOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput,
	                 Comparator comparator);

	~CompareOperation() override = default;

	Comparator getComparator() const;

	bool isLessThan();

	bool isLessEqual();

	bool isGreaterThan();

	bool isGreaterEqual();

	bool isEquals();

	bool isLessThanOrGreaterThan();

	bool isLess();

	bool isGreater();

	std::string getComparatorAsString();

private:
	Comparator comparator;
};
} // namespace nautilus::compiler::ir
