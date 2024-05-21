
#include "nautilus/exceptions/NotImplementedException.hpp"

namespace nautilus {

NotImplementedException::NotImplementedException(std::string msg) : RuntimeException(msg) {
}

NotImplementedException::~NotImplementedException() noexcept = default;

} // namespace nautilus
