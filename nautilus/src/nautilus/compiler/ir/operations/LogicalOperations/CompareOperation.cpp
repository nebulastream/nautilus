

#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"

namespace nautilus::compiler::ir {
CompareOperation::CompareOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput,
                                   Comparator comparator)
    : BinaryOperation(Operation::OperationType::CompareOp, identifier, Type::b, leftInput, rightInput),
      comparator(comparator) {
}

CompareOperation::Comparator CompareOperation::getComparator() const {
	return comparator;
}

bool CompareOperation::isLessThan() {
	return (comparator == LT);
}

bool CompareOperation::isLessEqual() {
	return (comparator == LE);
}

bool CompareOperation::isGreaterThan() {
	return (comparator == GT);
}

bool CompareOperation::isGreaterEqual() {
	return (comparator == GE);
}

bool CompareOperation::isEquals() {
	return (comparator == EQ);
}

bool CompareOperation::isLessThanOrGreaterThan() {
	return isLessThan() || isGreaterThan();
}

bool CompareOperation::isLess() {
	return isLessThan() || isLessEqual();
}

bool CompareOperation::isGreater() {
	return isGreaterThan() || isGreaterEqual();
}

std::string CompareOperation::getComparatorAsString() {
	switch (comparator) {
	case EQ:
		return "==";
	case NE:
		return "!=";
	case LT:
		return "<";
	case LE:
		return "<=";
	case GT:
		return ">";
	case GE:
		return ">=";
	}
	return "";
}

} // namespace nautilus::compiler::ir
