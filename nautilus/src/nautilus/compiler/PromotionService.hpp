#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/Module.hpp"
#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace nautilus::compiler {

/// What a promotion's compile function learns about its run: the token to hand to the backend, and when the job was
/// queued and picked up (11-tiered-compilation.md §4.3: `promotion.queueMs`, `promotion.timeToTier1Ms`).
struct PromotionRun {
	const nirbackend::CancellationToken& cancellation;
	std::chrono::steady_clock::time_point submitted;
	std::chrono::steady_clock::time_point started;
};

/// One module's promotion to tier 1 (11 §4.2). It lives from the tier-0 compile of its module until the module is
/// gone or the promotion has ended, and moves through
///
///   parked -> queued -> running -> done | failed | cancelled      (skipped: refused on admission)
///
/// `parked` is a job of the policy `manual` that waits for `promote()`; an `eager` job is submitted at once. The
/// transitions are compare-and-swaps, so a job is queued once however many threads ask. The module is held weakly:
/// the job never keeps a dropped module alive, and a module that is dropped cancels its job (`cancel`).
class PromotionJob {
public:
	enum class State : uint8_t { parked, queued, running, done, failed, cancelled, skipped };

	/// Compiles the tier-1 executable. Returns null if it was cancelled. May throw: the service records the
	/// message and marks the job `failed`.
	using CompileFunction = std::function<std::unique_ptr<Executable>(const PromotionRun&)>;

	PromotionJob(std::weak_ptr<engine::details::ModuleState> module, CompileFunction compile)
	    : module_(std::move(module)), compile_(std::move(compile)) {
	}

	PromotionJob(const PromotionJob&) = delete;
	PromotionJob& operator=(const PromotionJob&) = delete;

	/// Asks a queued or running job to end at its next stage boundary, and a job that was not queued yet never to
	/// run. Safe from any thread and from the destructor of the module's state; takes no lock.
	void cancel() noexcept {
		token_.cancel();
	}

	[[nodiscard]] State state() const noexcept {
		return state_.load(std::memory_order_acquire);
	}

private:
	friend class PromotionService;

	std::weak_ptr<engine::details::ModuleState> module_;
	CompileFunction compile_; // the IR graph and options live in it; dropped when the job ends
	nirbackend::CancellationToken token_;
	std::atomic<State> state_ {State::parked};
	std::chrono::steady_clock::time_point submitted_ {};
};

/// The settings of a service (`engine.tiered.*`).
struct PromotionServiceConfig {
	/// Number of workers, at least 1.
	std::size_t threads = 1;
	/// Jobs that may wait for a worker; a job beyond it is skipped. 0 together with `bounded` skips every job.
	std::size_t maxQueued = 0;
	bool bounded = false;
	/// Workers run at a lower OS priority, so promotions do not slow the caller's tier-0 compile or the query.
	bool lowPriority = true;
};

/// The message of the exception that is being handled; call it inside a `catch (...)`. A `std::exception` gives its
/// `what()`, anything else a description naming its type.
std::string describeCurrentException();

/// The default of `engine.tiered.promotionThreads`: a quarter of the hardware threads, at least one.
std::size_t defaultPromotionThreads();

/**
 * @brief The promotion service of an engine (11-tiered-compilation.md §4.1, §4.2).
 *
 * A fixed pool of at most `engine.tiered.promotionThreads` worker threads, created on the first job and kept for the
 * engine's lifetime, runs the tier-1 compiles of all of the engine's modules, whatever their number: the thread count
 * is bounded, and the thread-local state of a backend (the `MLIRContext` and target machines of #561) is reused by
 * every promotion after a worker's first. Workers own no further threads; the compile function must not give a
 * context one (the tiered compiler turns `mlir.enableMultithreading` off for promotions).
 *
 * Locking: one mutex guards the queue and the counters. It is never held while a job compiles, while a module's
 * status is published, while a thread is joined or while a wait is in progress, so a `compileModule` on another
 * thread never blocks on a wait or on a running promotion.
 */
class PromotionService {
public:
	explicit PromotionService(PromotionServiceConfig config);
	/// Calls `shutdown`.
	~PromotionService();

	PromotionService(const PromotionService&) = delete;
	PromotionService& operator=(const PromotionService&) = delete;

	/**
	 * @brief Queues @p job unless it was queued before.
	 *
	 * Publishes `queued` to the module, or `skipped` if the admission cap is reached or the service is shutting
	 * down. Returns true if this call queued the job.
	 */
	bool submit(const std::shared_ptr<PromotionJob>& job);

	/**
	 * @brief Tracks a job of policy `manual` that waits for `promote()`.
	 *
	 * At shutdown such a job is cancelled and its compile function (and the IR it holds) dropped, as a module can
	 * outlive the engine that owns the memory that IR draws from.
	 */
	void park(const std::shared_ptr<PromotionJob>& job);

	/// Blocks until no job is queued or running (a parked job is not pending). Returns false on timeout.
	bool waitIdle(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());

	[[nodiscard]] bool idle() const;

	/// Cancels the queued jobs and the parked ones, asks the running ones to stop and joins the workers: at most one
	/// stage of one compile per worker. No lock is held while joining. Idempotent.
	void shutdown();

	/// Workers created so far (never more than the configured number).
	[[nodiscard]] std::size_t workerCount() const;
	[[nodiscard]] std::size_t queuedCount() const;
	[[nodiscard]] const PromotionServiceConfig& config() const {
		return config_;
	}

private:
	void workerMain();
	void runJob(const std::shared_ptr<PromotionJob>& job);
	/// Ends @p job in the terminal state @p state: publishes it to the module (if it is alive) and drops the compile
	/// function. Called without the service mutex.
	void finish(const std::shared_ptr<PromotionJob>& job, PromotionJob::State state, std::string error = {});

	const PromotionServiceConfig config_;

	mutable std::mutex mutex_;
	std::condition_variable wake_; // a job was queued, or the service stops
	std::condition_variable idle_; // the last running job ended
	std::deque<std::shared_ptr<PromotionJob>> queue_;
	std::vector<std::weak_ptr<PromotionJob>> parked_;
	std::size_t parkedPruneAt_ = 64;
	std::vector<std::shared_ptr<PromotionJob>> active_; // the running jobs
	std::size_t idleWorkers_ = 0;
	bool stopping_ = false;
	std::vector<std::thread> workers_;
};

} // namespace nautilus::compiler
