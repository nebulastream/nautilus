#pragma once

#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>

namespace nautilus::profiling::detail {

/// Maps calling threads onto the fixed slot vector that perf-cpp's
/// multi-threaded counters and samplers allocate at construction.
///
/// This is what keeps thread indices out of the public API. perf-cpp needs a
/// dense `[0, n)` slot id for every thread it measures, and requires that slot
/// to be started on the thread it belongs to; assigning one here, keyed on
/// `std::this_thread::get_id()`, means a caller only ever says "start" and the
/// right slot is implied by which thread is asking.
///
/// The slot vector cannot grow -- perf-cpp sizes it once -- which is why
/// `acquire()` can fail, and why the capacity is worth choosing deliberately.
class ThreadSlots {
public:
	static constexpr uint16_t INVALID = UINT16_MAX;

	explicit ThreadSlots(uint16_t capacity);

	/// The calling thread's slot, assigning the next free one on first use.
	/// Returns INVALID when every slot is already spoken for.
	///
	/// A thread keeps its slot across windows, so a worker pool measuring one
	/// window after another reuses the same counters rather than exhausting the
	/// capacity. See the note on thread-id reuse in counters.hpp.
	uint16_t acquire();

	/// The calling thread's slot if it has one, else INVALID. Never assigns.
	[[nodiscard]] uint16_t current() const;

	/// Every assigned slot, in assignment order.
	[[nodiscard]] std::vector<uint16_t> assigned() const;

	[[nodiscard]] uint16_t capacity() const noexcept {
		return capacity_;
	}

	/// Forgets every assignment, so the next `acquire()` starts from slot 0.
	void clear();

private:
	mutable std::mutex mutex_;
	uint16_t capacity_;
	/// Indexed by slot; only the first `assignedCount_` entries are meaningful.
	std::vector<std::thread::id> owners_;
	uint16_t assignedCount_ = 0;
};

} // namespace nautilus::profiling::detail
