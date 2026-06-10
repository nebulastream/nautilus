#include <nautilus/tracing/exceptions/TraceDivergenceException.hpp>

namespace nautilus {

TraceDivergenceException::TraceDivergenceException(const std::string& message) : RuntimeException(message) {
}
TraceDivergenceException::~TraceDivergenceException() noexcept = default;

} // namespace nautilus
