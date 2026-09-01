#pragma once

#include <cassert>
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
/// Pops back to @p frame's parent. Unlike the argument-less overload this
/// cannot leave the thread-local stack pointing into a destroyed frame when
/// pushes and pops got out of balance.
void popExceptionFrame(ExceptionFrame* frame);
bool hasPendingException();

/// Stores the in-flight exception (i.e. the one currently being handled) in
/// the innermost frame, if there is one and it is still empty.
///
/// Returns false when there is no frame to store it in. Callers running in a
/// frame that can unwind must then rethrow rather than drop the exception;
/// callers that cannot unwind (the capture thunk below, invoked from
/// generated code without unwind tables) have no such option.
[[nodiscard]] bool captureCurrentException() noexcept;

/// RAII owner of an ExceptionFrame: pushes on construction and pops on
/// destruction, so an exception escaping the guarded call cannot leave the
/// thread-local stack pointing at a destroyed frame.
class ExceptionFrameScope {
public:
	explicit ExceptionFrameScope(bool enabled) : enabled_(enabled) {
		if (enabled_) {
			pushExceptionFrame(&frame_);
		}
	}

	~ExceptionFrameScope() {
		if (enabled_) {
			popExceptionFrame(&frame_);
		}
	}

	ExceptionFrameScope(const ExceptionFrameScope&) = delete;
	ExceptionFrameScope& operator=(const ExceptionFrameScope&) = delete;

	/// The exception captured during the guarded call, or null.
	[[nodiscard]] const std::exception_ptr& pending() const {
		return frame_.pending;
	}

private:
	ExceptionFrame frame_;
	bool enabled_;
};

template <typename R, typename... Args>
R captureThrowingCall(R (*target)(Args...), Args... args) noexcept {
	// This thunk is invoked from generated/JIT-compiled frames that have no
	// unwind tables, so it must not let an exception escape -- hence noexcept
	// and the unconditional catch. Reaching it without a frame to capture into
	// means the lowering emitted a capture site without pushing a frame, which
	// is a bug on our side: assert loudly, and in release drop the exception
	// rather than unwind through a frame that cannot be unwound.
	if constexpr (std::is_void_v<R>) {
		try {
			target(std::forward<Args>(args)...);
		} catch (...) {
			[[maybe_unused]] const bool captured = captureCurrentException();
			assert(captured && "captureThrowingCall invoked without an active ExceptionFrame");
		}
	} else {
		try {
			return target(std::forward<Args>(args)...);
		} catch (...) {
			[[maybe_unused]] const bool captured = captureCurrentException();
			assert(captured && "captureThrowingCall invoked without an active ExceptionFrame");
			return R {};
		}
	}
}

} // namespace nautilus::compiler
