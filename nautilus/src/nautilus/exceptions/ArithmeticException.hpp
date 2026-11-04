#pragma once

#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus {

/// Thrown by a checked integer `div` or `rem` whose divisor is 0, or whose dividend is the type's minimum and whose
/// divisor is -1 (signed); D10 of docs/plans/nautilus-0.4/13-nir-spec.md §5.10.
class ArithmeticException : public RuntimeException {
public:
	ArithmeticException(std::string msg);

	~ArithmeticException() noexcept override;
};

} // namespace nautilus
