#pragma once

#include "nautilus/profiling/availability.hpp"
#include "nautilus/profiling/measurement.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace nautilus::profiling {

/// Hardware performance counters over an explicit measurement window.
///
/// This is `perf stat` scoped to exactly the code you bracket, read from inside
/// the process. It is deliberately *not* tied to a compiled function: anything
/// may sit between `start()` and `stop()` -- one compiled nautilus pipeline,
/// several of them, `invoke()`d runtime calls, host code in between, work from
/// more than one engine:
///
/// ```cpp
/// profiling::CounterSet counters {{"cycles", "instructions", "cache-misses"}};
/// counters.start();
/// auto partial = scanPipeline(input);      // a compiled nautilus function
/// hostSideMerge(partial);                  // ordinary host code
/// auto out = aggregatePipeline(partial);   // another compiled function
/// counters.stop();
/// std::cout << counters.result().table();
/// ```
///
/// **`start()` and `stop()` always act on the calling thread**, because that is
/// what the perf subsystem can do: a counter is opened against the thread that
/// opens it and does not follow threads spawned later. Measuring work that runs
/// on a pool is therefore the same two calls, made from each worker:
///
/// ```cpp
/// profiling::CounterSet counters;
///
/// // in each worker:
/// {
///     profiling::CounterSet::Scope scope {counters};
///     runPipelineSlice();
/// }
///
/// counters.stopAll();                      // once the pool has joined
/// std::cout << counters.result().table();  // summed over every thread
/// ```
///
/// There are no thread indices to assign and no separate type for the parallel
/// case: which counters a call touches follows from which thread is asking.
/// A thread keeps its counters across windows, so a pool measuring one window
/// after another costs nothing extra.
///
/// Counting does not perturb what it measures. The counters are programmed
/// before the window and read after it, so the code in between is the code you
/// would have shipped, unmodified and unobserved. That is the difference from
/// instrumenting the compiled code itself, and the reason this composes with
/// the sampling profiler over the same window -- counters give the window's
/// exact totals, sampling gives the breakdown within it.
///
/// Nothing here throws on an unusable perf subsystem. If `available()` is false
/// every method is a no-op and `result()` is empty; see `availability.hpp`.
class CounterSet {
public:
	/// cycles, instructions, cache-misses, branch-misses, and wall-clock seconds.
	///
	/// Four hardware events plus a software timer is a set that fits the generic
	/// PMU slots of every supported microarchitecture without multiplexing, so
	/// the values come back exact rather than extrapolated.
	[[nodiscard]] static std::vector<std::string> defaultEvents();

	/// @param events perf event names (as `perf list` spells them) and/or derived
	///               metric names such as `cycles-per-instruction`. Names the host
	///               does not know are dropped, which is reported through
	///               `unavailableReason()` rather than thrown.
	/// @param maxThreads how many threads may be measured at once. 0 means
	///               `std::thread::hardware_concurrency()`, which covers any
	///               pool sized for the CPU; raise it for one deliberately
	///               oversubscribed, since a thread that finds no free slot is
	///               not measured and says so through `unavailableReason()`.
	///               Unused capacity is nearly free -- a slot costs one small
	///               object and opens no file descriptor until it is started.
	explicit CounterSet(std::vector<std::string> events = defaultEvents(), uint16_t maxThreads = 0);
	~CounterSet();

	CounterSet(CounterSet&&) noexcept;
	CounterSet& operator=(CounterSet&&) noexcept;
	CounterSet(const CounterSet&) = delete;
	CounterSet& operator=(const CounterSet&) = delete;

	/// Whether counters could actually be programmed. False on a non-Linux host,
	/// under a restrictive `perf_event_paranoid`, in a container that filters the
	/// syscall, or on hardware without the requested events.
	[[nodiscard]] bool available() const noexcept;

	/// Why `available()` is false, or what was degraded along the way -- a
	/// dropped event, a thread that found no free slot. Empty when all is well.
	[[nodiscard]] std::string_view unavailableReason() const noexcept;

	/// The event names actually programmed, which may be a subset of those
	/// requested if the host did not know some of them.
	[[nodiscard]] const std::vector<std::string>& events() const noexcept;

	/// How many threads may be measured at once.
	[[nodiscard]] uint16_t maxThreads() const noexcept;

	/// Opens the calling thread's counters if needed and begins its window.
	/// Calling it twice from one thread without stopping is a no-op.
	void start();

	/// Ends the calling thread's window and latches its result.
	void stop();

	/// Ends every thread's window that is still open and latches every result.
	/// Safe from any thread, and the usual way to close a window once a pool has
	/// joined.
	void stopAll();

	/// Whether the calling thread currently has a window open.
	[[nodiscard]] bool running() const noexcept;

	/// Every measured thread's counters, summed.
	///
	/// A time event such as `seconds` sums too, so across N busy threads it
	/// reports roughly N times wall clock -- it is thread-seconds, not elapsed
	/// time. Derived metrics are recomputed from the summed events rather than
	/// averaged, so `cycles-per-instruction` stays meaningful.
	///
	/// Empty until something has been stopped, and always empty when
	/// `available()` is false.
	[[nodiscard]] const Measurement& result() const noexcept;

	/// One entry per measured thread, in the order the threads first started,
	/// for finding the worker that behaved differently from the rest.
	[[nodiscard]] std::vector<Measurement> perThreadResults() const;

	/// Discards latched results and forgets which threads were measured, closing
	/// their counters.
	///
	/// Worth calling when the set of measured threads changes -- a pool being
	/// torn down and replaced. A thread keeps its counters across windows, and
	/// `std::thread::id` may be recycled once a thread exits, so without a reset
	/// a brand-new thread that inherits a dead one's id would inherit counters
	/// bound to a thread that no longer exists and silently measure nothing.
	/// Steady pools, and anything single-threaded, never have to think about it.
	void reset();

	/// Starts the calling thread's window on construction and stops it on
	/// destruction. Needs no thread index: whichever thread the object lives on
	/// is the one measured.
	class Scope {
	public:
		explicit Scope(CounterSet& counters) : counters_(counters) {
			counters_.start();
		}
		~Scope() {
			counters_.stop();
		}
		Scope(const Scope&) = delete;
		Scope& operator=(const Scope&) = delete;

	private:
		CounterSet& counters_;
	};

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};

} // namespace nautilus::profiling
