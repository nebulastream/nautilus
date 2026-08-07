#pragma once

#include <exception>
#include <type_traits>

namespace nautilus {

struct ExceptionFrame {
	std::exception_ptr pending;
	ExceptionFrame* parent = nullptr;
};

/// Installs an isolated exception frame for one typed host invocation.
class ExceptionFrameGuard {
public:
	ExceptionFrameGuard() noexcept;
	~ExceptionFrameGuard();

	ExceptionFrameGuard(const ExceptionFrameGuard&) = delete;
	ExceptionFrameGuard& operator=(const ExceptionFrameGuard&) = delete;

	[[nodiscard]] ExceptionFrame* get() noexcept;
	void rethrowPending();

private:
	ExceptionFrame frame_;
};

[[nodiscard]] ExceptionFrame* currentExceptionFrame() noexcept;
[[nodiscard]] bool hasPendingException(const ExceptionFrame* frame) noexcept;

template <typename R, typename... Args>
R invokeCatching(ExceptionFrame* frame, void* target, Args... args) noexcept {
	try {
		auto function = reinterpret_cast<R (*)(Args...)>(target);
		if constexpr (std::is_void_v<R>) {
			function(args...);
			return;
		} else {
			return function(args...);
		}
	} catch (...) {
		if (frame == nullptr) {
			std::terminate();
		}
		if (!frame->pending) {
			frame->pending = std::current_exception();
		}
		if constexpr (!std::is_void_v<R>) {
			static_assert(std::is_fundamental_v<R> || std::is_pointer_v<R> || std::is_enum_v<R>);
			return R {};
		}
	}
}

template <typename R, typename... Args>
constexpr auto exceptionCaptureFunction() noexcept {
	return &invokeCatching<R, Args...>;
}

} // namespace nautilus
