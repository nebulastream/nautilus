#pragma once

#include "nautilus/profiling/availability.hpp"
#include "nautilus/profiling/sample_report.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace nautilus::profiling {

/// A sampling profiler over an explicit measurement window, which understands
/// nautilus JIT-compiled code.
///
/// Same shape as `CounterSet`, deliberately: `start()`, arbitrary work,
/// `stop()`. The two compose over one window and answer different halves of the
/// question -- counters give the window's exact totals, sampling gives the
/// breakdown within it.
///
/// ```cpp
/// profiling::Sampler sampler;
/// sampler.start();
/// pipeline(input);          // a compiled nautilus function
/// sampler.stop();
/// std::cout << sampler.report().table();
/// // execute::query::aggregate::mix   72.48%
/// // execute::query::scan              4.17%
/// ```
///
/// Symbolization is what makes this different from pointing a general-purpose
/// in-process sampler at the program. Generated code lives in anonymous memory
/// that no ELF file describes, so a symbolizer reading `/proc/self/maps` cannot
/// name it. This resolves against nautilus's own JIT symbol registry first --
/// giving the same region-qualified names `perf report` shows after a
/// `perf inject --jit` pass -- and falls back to ELF symbols for host frames.
///
/// **The compile has to have been asked for it.** Deriving a region's qualified
/// name needs DWARF the backend does not otherwise emit, so the engine wants
/// `perf.sample` set and the MLIR backend pinned:
///
/// ```cpp
/// options.setOption("perf.sample", true);
/// options.setOption("engine.backend", std::string("mlir"));
/// ```
///
/// Without `perf.sample` the profile still works, but JIT frames come back
/// unresolved; without the backend pinned, tiered compilation starts on an
/// interpreting tier that registers no symbols at all. `report()` says how much
/// of the profile it could not resolve, so neither mistake is silent.
///
/// **`start()` and `stop()` always act on the calling thread**, because that is
/// what the perf subsystem can do. Sampling work that runs on a pool is the
/// same two calls, made from each worker, with no thread indices to assign:
///
/// ```cpp
/// profiling::Sampler sampler;
///
/// // in each worker:
/// {
///     profiling::Sampler::Scope scope {sampler};
///     runPipelineSlice();
/// }
///
/// sampler.stopAll();                      // once the pool has joined
/// std::cout << sampler.report().table();  // every thread's samples, merged
/// ```
///
/// Nothing here throws on an unusable perf subsystem; see `availability.hpp`.
class Sampler {
public:
	struct Options {
		/// The event whose overflow triggers a sample. `cycles` profiles time;
		/// `cache-misses`, `branch-misses` and the rest turn the same report
		/// into a per-region breakdown of that event instead.
		std::string event = "cycles";

		/// Sample every Nth event. Lower means more samples and more overhead;
		/// the default is perf's own rough order of magnitude for a profile
		/// that resolves individual functions without dominating the run. A
		/// short window wants a shorter period.
		uint64_t period = 100000;

		/// Record the call stack of each sample as well as its leaf address, so
		/// a JIT frame's host caller appears in the folded stacks. Needs frame
		/// pointers, which a `perf.sample` compile asks for; host code built
		/// with `-fomit-frame-pointer` may still truncate a walk.
		bool callchain = false;

		/// Size of the ring buffer perf writes samples into, in pages, **per
		/// measured thread**. Samples overflowing it are dropped by the kernel;
		/// raise this for a long window or a short period.
		uint64_t bufferPages = 64;

		/// How many threads may be sampled at once. 0 means
		/// `std::thread::hardware_concurrency()`. Unlike counters a slot here
		/// does cost a ring buffer once started, so an oversubscribed pool is
		/// worth sizing deliberately.
		uint16_t maxThreads = 0;
	};

	/// Samples `cycles` at the default period.
	Sampler();

	/// A separate overload rather than a defaulted parameter: `Options` is
	/// nested, and a default argument of `{}` would need its default member
	/// initializers before the enclosing class is complete.
	explicit Sampler(Options options);
	~Sampler();

	Sampler(Sampler&&) noexcept;
	Sampler& operator=(Sampler&&) noexcept;
	Sampler(const Sampler&) = delete;
	Sampler& operator=(const Sampler&) = delete;

	/// Whether the sampler could actually be opened.
	[[nodiscard]] bool available() const noexcept;

	/// Why `available()` is false, or what was degraded along the way. Empty
	/// when all is well.
	[[nodiscard]] std::string_view unavailableReason() const noexcept;

	/// How many threads may be sampled at once.
	[[nodiscard]] uint16_t maxThreads() const noexcept;

	/// Begins sampling the calling thread. A no-op if it is already sampling.
	void start();

	/// Stops sampling the calling thread. The report is not built until
	/// `stopAll()`, because a ring buffer can only be drained once and the
	/// samples are merged across threads.
	void stop();

	/// Stops every thread still sampling, drains every ring buffer and builds
	/// the merged report. Call it once, after any pool has joined.
	void stopAll();

	/// Whether the calling thread is currently sampling.
	[[nodiscard]] bool running() const noexcept;

	/// The merged profile of every measured thread, available after
	/// `stopAll()`. A region's share is its share of all measured work, which
	/// for a pool is the question usually worth asking.
	[[nodiscard]] const SampleReport& report() const noexcept;

	/// Discards the latched report and forgets which threads were measured.
	/// See `CounterSet::reset()` for when this matters.
	void reset();

	/// Starts sampling the calling thread on construction and stops it on
	/// destruction. Needs no thread index: whichever thread the object lives on
	/// is the one sampled.
	class Scope {
	public:
		explicit Scope(Sampler& sampler) : sampler_(sampler) {
			sampler_.start();
		}
		~Scope() {
			sampler_.stop();
		}
		Scope(const Scope&) = delete;
		Scope& operator=(const Scope&) = delete;

	private:
		Sampler& sampler_;
	};

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};

} // namespace nautilus::profiling
