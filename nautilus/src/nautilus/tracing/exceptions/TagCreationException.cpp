
#include <nautilus/tracing/exceptions/TagCreationException.hpp>

namespace nautilus {

TagCreationException::TagCreationException(const std::string& message) : RuntimeException(message) {
}
TagCreationException::TagCreationException::~TagCreationException() noexcept = default;

} // namespace nautilus
