#include "nautilus/profiling/counters.hpp"
#include "CounterConversion.hpp"
#include "ThreadSlots.hpp"
#include <algorithm>
#include <atomic>
#include <exception>
#include <mutex>
#include <optional>
#include <perfcpp/counter_definition.hpp>
#include <perfcpp/event_counter.hpp>
#include <thread>
#include <utility>

// perf-cpp lives in the global namespace `perf`. Reaching it through an alias
// rather than unqualified keeps the vendored library visibly distinct from this
// plugin's own `nautilus::profiling` names at every use site.
namespace pc = ::perf;

namespace nautilus::profiling {

namespace {

/// Appends @p text to @p reason as one more sentence, so several independent
/// degradations (an unknown event, then a thread with no free slot) accumulate
/// into one message instead of overwriting each other.
void addReason(std::string& reason, const std::string& text) {
	if (!reason.empty()) {
		reason += "; ";
	}
	reason += text;
}

/// Default slot capacity: enough for a pool sized to the machine.
uint16_t defaultCapacity() {
	const auto hardware = std::thread::hardware_concurrency();
	if (hardware == 0) {
		// hardware_concurrency is allowed to answer "I don't know".
		return 8;
	}
	return static_cast<uint16_t>(std::min<unsigned>(hardware, UINT16_MAX - 1));
}

} // namespace

struct CounterSet::Impl {
	std::vector<std::string> requested;
	/// What was actually programmed; a subset of `requested` when the host did
	/// not know an event or the PMU ran out of counters.
	std::vector<std::string> programmed;

	bool usable = false;
	std::string reason;
	/// Guards `reason`, which any measured thread may append to.
	mutable std::mutex reasonMutex;

	/// Always the multi-threaded counter, even for a single thread: a slot costs
	/// one small object and opens no file descriptor until it is started, so
	/// there is nothing to gain from a separate single-threaded path -- and a
	/// separate path is what used to make "measure a pool" a different type with
	/// a different API.
	std::optional<pc::MultiThreadEventCounter> counter;
	detail::ThreadSlots slots;

	/// Per slot, written by the thread that owns it and read by stopAll() from
	/// another. Atomic rather than `std::vector<bool>`, whose bit-packing makes
	/// two threads writing "different" elements a genuine data race.
	std::vector<std::atomic<bool>> running;
	/// Per slot, set once that slot has been stopped and its result latched.
	std::vector<std::atomic<bool>> latched;

	Measurement aggregate;
	std::vector<Measurement> perThread;
	mutable std::mutex resultMutex;

	explicit Impl(uint16_t capacity) : slots(capacity), running(capacity), latched(capacity), perThread(capacity) {
		for (uint16_t slot = 0; slot < capacity; ++slot) {
			running[slot].store(false, std::memory_order_relaxed);
			latched[slot].store(false, std::memory_order_relaxed);
		}
	}

	void note(const std::string& text) {
		const std::lock_guard lock(reasonMutex);
		addReason(reason, text);
	}

	/// Latches one slot's result and refreshes the aggregate. Called after that
	/// slot has stopped.
	void latch(uint16_t slot) {
		const std::lock_guard lock(resultMutex);
		try {
			perThread[slot] = Measurement(detail::toEntries(counter->result_of_thread(slot)));
			latched[slot].store(true, std::memory_order_release);
		} catch (const std::exception&) {
			// A slot that never opened has no result; leaving it empty is the
			// honest answer and must not take the other slots down with it.
		}
		try {
			// perf-cpp sums the raw events and *recomputes* derived metrics from
			// that sum, which is why the aggregate comes from it rather than from
			// adding up `perThread` here -- averaging a ratio like
			// cycles-per-instruction across threads would be meaningless.
			aggregate = Measurement(detail::toEntries(counter->result()));
		} catch (const std::exception&) {
		}
	}
};

std::vector<std::string> CounterSet::defaultEvents() {
	return {"seconds", "cycles", "instructions", "cache-misses", "branch-misses"};
}

CounterSet::CounterSet(std::vector<std::string> events, uint16_t maxThreads)
    : impl_(std::make_unique<Impl>(maxThreads == 0 ? defaultCapacity() : maxThreads)) {
	impl_->requested = std::move(events);

	const auto& hostSupport = support();
	if (!hostSupport) {
		impl_->reason = hostSupport.reason;
		return;
	}

	try {
		impl_->counter.emplace(impl_->slots.capacity());
	} catch (const std::exception& error) {
		impl_->reason = std::string("could not create a perf event counter: ") + error.what();
		return;
	}

	// Added one at a time rather than as a batch: perf-cpp throws on the first
	// name it cannot resolve, which would drop every event after it. Per-event
	// handling lets an unknown or unschedulable event be skipped while the rest
	// of the set still counts.
	for (const auto& event : impl_->requested) {
		try {
			impl_->counter->add(std::string(event));
			impl_->programmed.push_back(event);
		} catch (const std::exception& error) {
			addReason(impl_->reason, "event '" + event + "' dropped: " + error.what());
		}
	}

	if (impl_->programmed.empty()) {
		addReason(impl_->reason, "no requested event could be programmed");
		impl_->counter.reset();
		return;
	}
	impl_->usable = true;
}

CounterSet::~CounterSet() = default;
CounterSet::CounterSet(CounterSet&&) noexcept = default;
CounterSet& CounterSet::operator=(CounterSet&&) noexcept = default;

bool CounterSet::available() const noexcept {
	return impl_->usable;
}

std::string_view CounterSet::unavailableReason() const noexcept {
	const std::lock_guard lock(impl_->reasonMutex);
	return impl_->reason;
}

const std::vector<std::string>& CounterSet::events() const noexcept {
	return impl_->programmed;
}

uint16_t CounterSet::maxThreads() const noexcept {
	return impl_->slots.capacity();
}

bool CounterSet::running() const noexcept {
	const auto slot = impl_->slots.current();
	if (slot == detail::ThreadSlots::INVALID) {
		return false;
	}
	return impl_->running[slot].load(std::memory_order_acquire);
}

void CounterSet::start() {
	if (!impl_->usable) {
		return;
	}
	const auto slot = impl_->slots.acquire();
	if (slot == detail::ThreadSlots::INVALID) {
		impl_->note("a thread found no free counter slot (maxThreads = " + std::to_string(impl_->slots.capacity()) +
		            "); its work is not measured");
		return;
	}
	if (impl_->running[slot].exchange(true, std::memory_order_acq_rel)) {
		return;
	}
	try {
		// Opens against the *calling* thread, which is the whole reason the slot
		// is derived from the caller rather than passed in.
		impl_->counter->start(slot);
	} catch (const std::exception& error) {
		impl_->running[slot].store(false, std::memory_order_release);
		impl_->note(std::string("could not start counters on a thread: ") + error.what());
	}
}

void CounterSet::stop() {
	if (!impl_->usable) {
		return;
	}
	const auto slot = impl_->slots.current();
	if (slot == detail::ThreadSlots::INVALID) {
		return;
	}
	if (!impl_->running[slot].exchange(false, std::memory_order_acq_rel)) {
		return;
	}
	try {
		impl_->counter->stop(slot);
	} catch (const std::exception& error) {
		impl_->note(std::string("could not stop counters on a thread: ") + error.what());
		return;
	}
	impl_->latch(slot);
}

void CounterSet::stopAll() {
	if (!impl_->usable) {
		return;
	}
	for (const auto slot : impl_->slots.assigned()) {
		if (impl_->running[slot].exchange(false, std::memory_order_acq_rel)) {
			try {
				impl_->counter->stop(slot);
			} catch (const std::exception& error) {
				impl_->note(std::string("could not stop counters on a thread: ") + error.what());
				continue;
			}
		}
		impl_->latch(slot);
	}
}

const Measurement& CounterSet::result() const noexcept {
	return impl_->aggregate;
}

std::vector<Measurement> CounterSet::perThreadResults() const {
	const std::lock_guard lock(impl_->resultMutex);
	std::vector<Measurement> results;
	for (const auto slot : impl_->slots.assigned()) {
		if (impl_->latched[slot].load(std::memory_order_acquire)) {
			results.push_back(impl_->perThread[slot]);
		}
	}
	return results;
}

void CounterSet::reset() {
	if (impl_->usable) {
		try {
			// Closes every slot's file descriptors, so a slot handed to a
			// different thread afterwards re-opens against that thread rather
			// than counting on a dead one's behalf.
			impl_->counter->close();
		} catch (const std::exception&) {
		}
	}
	for (const auto slot : impl_->slots.assigned()) {
		impl_->running[slot].store(false, std::memory_order_release);
		impl_->latched[slot].store(false, std::memory_order_release);
	}
	impl_->slots.clear();
	const std::lock_guard lock(impl_->resultMutex);
	impl_->aggregate = Measurement();
	for (auto& measurement : impl_->perThread) {
		measurement = Measurement();
	}
}

} // namespace nautilus::profiling
