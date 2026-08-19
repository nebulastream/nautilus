#pragma once

#include <exception>
#include <type_traits>
#include <utility>

namespace nautilus::compiler {

struct ExceptionFrame {
	std::exception_ptr pending;
	ExceptionFrame* parent = nullptr;
};

ExceptionFrame* currentExceptionFrame();
void pushExceptionFrame(ExceptionFrame* frame);
void popExceptionFrame();
bool hasPendingException();

template <typename R, typename... Args>
R captureThrowingCall(R (*target)(Args...), Args... args) noexcept {
	if constexpr (std::is_void_v<R>) {
		try {
			target(std::forward<Args>(args)...);
		} catch (...) {
			auto* frame = currentExceptionFrame();
			if (frame && !frame->pending) {
				frame->pending = std::current_exception();
			}
		}
	} else {
		try {
			return target(std::forward<Args>(args)...);
		} catch (...) {
			auto* frame = currentExceptionFrame();
			if (frame && !frame->pending) {
				frame->pending = std::current_exception();
			}
			return R {};
		}
	}
}

} // namespace nautilus::compiler
