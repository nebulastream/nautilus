#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace nautilus::engine {
class NautilusEngine;
}

namespace nautilus::profiling {

/// Hardware performance counters available for collection (Linux only).
enum class HwCounter : uint8_t {
	CPU_CYCLES = 0,
	INSTRUCTIONS = 1,
	CACHE_MISSES = 2,
	BRANCH_MISSES = 3,
};

static constexpr int MAX_HW_COUNTERS = 4;

/// A recorded [start, stop] interval for one invocation of a ProfileRegion.
struct RegionInterval {
	uint64_t start_ns;
	uint64_t stop_ns;
};

class Profiler;
class ProfilingIRPass;

/// A named profiling region that collects timing data for a compiled function.
///
/// Regions are created automatically by the ProfilingIRPass during compilation.
/// Users interact with regions only through the Profiler API and read-only
/// accessors on individual regions returned by Profiler::getRegions().
///
/// Thread-safe: multiple threads may execute the same ProfileRegion concurrently.
class ProfileRegion {
	friend class Profiler;
	friend class ProfilingIRPass;

public:
	~ProfileRegion();

	const std::string& name() const noexcept;
	uint64_t callCount() const;
	uint64_t totalDurationNs() const;
	uint64_t minDurationNs() const;
	uint64_t maxDurationNs() const;
	double avgDurationNs() const;

	uint64_t counterTotal(HwCounter counter) const;
	double counterAvg(HwCounter counter) const;

	/// Runtime callbacks invoked by compiled code. Public because their
	/// addresses are embedded as function pointers in the generated IR.
	static void runtimeStart(ProfileRegion* region);
	static void runtimeStop(ProfileRegion* region);

private:
	explicit ProfileRegion(std::string name);

	void enableCounter(HwCounter counter);
	void enableIntervalRecording(size_t max_intervals = 1000000);
	void reset();
	void setActive(bool active);
	bool isActive() const;
	const std::vector<RegionInterval>& getIntervals() const;
	void report() const;
	void openCounters();

	std::string name_;

	/// When false, runtimeStart/runtimeStop are no-ops.
	std::atomic<bool> active_ {false};

	// Accumulated statistics (thread-safe via atomics).
	std::atomic<uint64_t> call_count_ {0};
	std::atomic<uint64_t> total_ns_ {0};
	std::atomic<uint64_t> min_ns_ {UINT64_MAX};
	std::atomic<uint64_t> max_ns_ {0};

	// Hardware counters (Linux perf_event_open).
	bool counters_enabled_[MAX_HW_COUNTERS] = {};
	int counter_fds_[MAX_HW_COUNTERS] = {-1, -1, -1, -1};
	std::atomic<uint64_t> total_counter_[MAX_HW_COUNTERS] = {};
	bool counters_opened_ = false;

	// Per-invocation interval recording for perf correlation.
	bool record_intervals_ = false;
	size_t max_intervals_ = 0;
	std::mutex intervals_mutex_;
	std::vector<RegionInterval> intervals_;
};

/// Manages profiling sessions with optional system profiler integration.
///
/// The primary entry point is the static enableForEngine() method, which
/// installs an IR pass on the engine that instruments every function the
/// engine subsequently compiles, and returns a shared handle to the
/// Profiler that will collect timing data. Keep the handle alive for as
/// long as any compiled module is used: the compiled code calls back into
/// this Profiler on every profiled-function entry/exit, so destroying it
/// while an executable is still callable is a use-after-free.
///
/// On Linux, automatically launches `perf record` for native call-stack
/// sampling. On macOS, uses the `sample` command. The exported Chrome Trace
/// combines region timing with native call-stack attribution.
///
/// Limitations:
///   - Instrumentation inserts start/stop calls only around ReturnOps. A
///     profiled function that never returns normally (infinite loop, or a
///     C++ exception escaping an `invoke()`d native call) will fire
///     `runtimeStart` but no matching `runtimeStop`, leaking its per-thread
///     entry timestamp and dropping that invocation from the aggregates.
///   - Each thread accumulates a per-region entry slot on first use that
///     is not released until the thread exits; short-lived regions
///     combined with long-lived threads slowly grow this per-thread state.
///
/// @code
///   auto profiler = Profiler::enableForEngine(engine);
///   auto module = engine.createModule();
///   module.registerFunction<...>("scale", scale);
///   module.registerFunction<...>("reduce", reduce);
///   auto compiled = module.compile();
///
///   profiler->start();
///   scaleFn(data, N);
///   reduceFn(data, N);
///   profiler->stop();
///
///   profiler->report();
///   profiler->exportChromeTrace("out.json");
/// @endcode
class Profiler {
	friend class ProfilingIRPass;

public:
	enum class State { IDLE, RUNNING, PAUSED };

	Profiler() = default;
	~Profiler();

	Profiler(const Profiler&) = delete;
	Profiler& operator=(const Profiler&) = delete;
	Profiler(Profiler&&) = delete;
	Profiler& operator=(Profiler&&) = delete;

	/// Enable automatic profiling for every function the engine compiles.
	/// Registers a ProfilingIRPass on the engine and returns a shared handle
	/// to the Profiler that will collect timing data.
	static std::shared_ptr<Profiler> enableForEngine(const engine::NautilusEngine& engine);

	void start();
	void pause();
	void stop();

	/// Print a summary table of all regions to stdout.
	void report() const;

	/// Export a Chrome Trace JSON combining region timing (level 1) with
	/// native call-stack samples from the system profiler (level 2).
	void exportChromeTrace(const std::string& path) const;

	const std::vector<ProfileRegion*>& getRegions() const;
	State getState() const;

private:
	ProfileRegion* getOrCreateRegion(const std::string& name);
	void correlateWithPerf(const std::string& perfScriptPath, const std::string& outputPath);

	std::vector<ProfileRegion*> regions_;
	State state_ = State::IDLE;
	mutable std::mutex mutex_;

	std::vector<std::unique_ptr<ProfileRegion>> ownedRegions_;

	int profilerPid_ = -1;
	std::string profilerDataDir_;
	std::string combinedStacks_;
};

} // namespace nautilus::profiling
