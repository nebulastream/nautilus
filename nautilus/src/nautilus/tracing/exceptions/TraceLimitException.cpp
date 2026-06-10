#include <nautilus/tracing/exceptions/TraceLimitException.hpp>

namespace nautilus {

TraceLimitException::TraceLimitException(const std::string& message) : RuntimeException(message) {
}
TraceLimitException::~TraceLimitException() noexcept = default;

} // namespace nautilus
