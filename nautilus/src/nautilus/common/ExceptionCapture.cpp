#include "nautilus/common/ExceptionCapture.hpp"
#include <utility>

namespace nautilus {

namespace {
thread_local ExceptionFrame* activeExceptionFrame = nullptr;
}

ExceptionFrameGuard::ExceptionFrameGuard() noexcept {
	frame_.parent = activeExceptionFrame;
	activeExceptionFrame = &frame_;
}

ExceptionFrameGuard::~ExceptionFrameGuard() {
	if (activeExceptionFrame != &frame_) {
		std::terminate();
	}
	activeExceptionFrame = frame_.parent;
}

ExceptionFrame* ExceptionFrameGuard::get() noexcept {
	return &frame_;
}

void ExceptionFrameGuard::rethrowPending() {
	if (frame_.pending) {
		auto exception = std::exchange(frame_.pending, {});
		std::rethrow_exception(exception);
	}
}

ExceptionFrame* currentExceptionFrame() noexcept {
	return activeExceptionFrame;
}

bool hasPendingException(const ExceptionFrame* frame) noexcept {
	return frame != nullptr && static_cast<bool>(frame->pending);
}

} // namespace nautilus
