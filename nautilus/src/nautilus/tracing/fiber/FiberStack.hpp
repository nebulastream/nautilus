
#pragma once

#include <cstddef>
#include <cstdint>

namespace nautilus::tracing::detail {

/**
 * @brief An mmap-allocated side stack with a guard page at its low end.
 *
 * The stack-copy tracer runs traced functions on this dedicated stack so that
 * (a) the live stack region of a branch snapshot is bounded by a stable top
 * address and (b) overflowing traced code faults on the guard page instead of
 * silently corrupting adjacent memory.
 */
class FiberStack {
public:
	/// Allocates @p size bytes of stack (rounded up to the page size) plus one guard page.
	explicit FiberStack(size_t size);
	~FiberStack();

	FiberStack(const FiberStack&) = delete;
	FiberStack& operator=(const FiberStack&) = delete;
	FiberStack(FiberStack&&) = delete;
	FiberStack& operator=(FiberStack&&) = delete;

	/// Highest usable address (exclusive); execution starts here and grows downwards.
	std::byte* top() const {
		return top_;
	}

	/// Lowest usable address (just above the guard page).
	std::byte* bottom() const {
		return bottom_;
	}

	size_t usableSize() const {
		return static_cast<size_t>(top_ - bottom_);
	}

private:
	void* mapping_ = nullptr;
	size_t mappingSize_ = 0;
	std::byte* bottom_ = nullptr;
	std::byte* top_ = nullptr;
};

} // namespace nautilus::tracing::detail
