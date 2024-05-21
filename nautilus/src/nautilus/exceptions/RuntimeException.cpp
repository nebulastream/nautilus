
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/exceptions/Stacktrace.hpp"

namespace nautilus {

RuntimeException::RuntimeException(std::string msg) : errorMessage(std::move(msg)) {
	auto stackStrace = printAndCollectCurrentStacktrace();
	errorMessage.append(stackStrace);
}

RuntimeException::~RuntimeException() noexcept = default;

const char* RuntimeException::what() const noexcept {
	return errorMessage.c_str();
}

} // namespace nautilus
