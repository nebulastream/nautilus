#include <nautilus/tracing/exceptions/TraceIncompleteException.hpp>

namespace nautilus {

TraceIncompleteException::TraceIncompleteException(const std::string& message) : RuntimeException(message) {
}
TraceIncompleteException::~TraceIncompleteException() noexcept = default;

} // namespace nautilus
