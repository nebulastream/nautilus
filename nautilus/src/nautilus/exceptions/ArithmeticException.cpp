#include "nautilus/exceptions/ArithmeticException.hpp"

namespace nautilus {

ArithmeticException::ArithmeticException(std::string msg) : RuntimeException(std::move(msg)) {
}

ArithmeticException::~ArithmeticException() noexcept = default;

} // namespace nautilus
