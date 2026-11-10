#include "nautilus/config.hpp"

#ifdef ENABLE_COMPILER

#include "nautilus/compiler/PromotionService.hpp"
#include "nautilus/logging.hpp"
#include <algorithm>
#include <cxxabi.h>
#include <system_error>

#if defined(__linux__)
#include <sys/resource.h>
#include <sys/syscall.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <pthread/qos.h>
#endif

namespace nautilus::compiler {

std::size_t defaultPromotionThreads() {
	return std::max<std::size_t>(1, std::thread::hardware_concurrency() / 4);
}

namespace {

/// Lowers the priority of the calling thread, where the platform allows it: `nice` on Linux (per thread there), the
/// background QoS class on macOS. A failure is ignored: promotions then compete at normal priority.
void lowerThreadPriority() {
#if defined(__linux__)
	const auto tid = static_cast<id_t>(::syscall(SYS_gettid));
	(void) ::setpriority(PRIO_PROCESS, tid, 10);
#elif defined(__APPLE__)
	(void) ::pthread_set_qos_class_self_np(QOS_CLASS_BACKGROUND, 0);
#endif
}

} // namespace

PromotionService::PromotionService(PromotionServiceConfig config)
    : config_([&] {
	      config.threads = std::max<std::size_t>(1, config.threads);
	      return config;
      }()) {
}

PromotionService::~PromotionService() {
	shutdown();
}

bool PromotionService::submit(const std::shared_ptr<PromotionJob>& job) {
	auto expected = PromotionJob::State::parked;
	if (!job->state_.compare_exchange_strong(expected, PromotionJob::State::queued, std::memory_order_acq_rel)) {
		return false; // requested before
	}
	if (job->token_.cancelled()) {
		finish(job, PromotionJob::State::cancelled);
		return false;
	}
	job->submitted_ = std::chrono::steady_clock::now();
	if (auto module = job->module_.lock()) {
		module->setPromotion(engine::PromotionState::queued);
	}

	bool admitted = false;
	bool stopping = false;
	std::vector<std::shared_ptr<PromotionJob>> dropped;
	{
		std::unique_lock<std::mutex> lock(mutex_);
		stopping = stopping_;
		if (!stopping_) {
			if (config_.bounded && queue_.size() >= config_.maxQueued) {
				// jobs of dropped modules do not count against the cap: make room before refusing this one
				for (auto it = queue_.begin(); it != queue_.end();) {
					if ((*it)->token_.cancelled()) {
						dropped.push_back(std::move(*it));
						it = queue_.erase(it);
					} else {
						++it;
					}
				}
			}
			if (!config_.bounded || queue_.size() < config_.maxQueued) {
				queue_.push_back(job);
				admitted = true;
				// a worker is created when a job waits and none is idle, up to the configured number
				if (workers_.size() < config_.threads && queue_.size() > idleWorkers_) {
					try {
						workers_.emplace_back([this] { workerMain(); });
					} catch (const std::system_error& e) {
						log::warn("Could not start a promotion worker: {}", e.what());
						if (workers_.empty()) {
							// nobody would ever run the job
							queue_.pop_back();
							admitted = false;
						}
					}
				}
				if (admitted) {
					wake_.notify_one();
				}
			}
		}
	}
	for (auto& old : dropped) {
		finish(old, PromotionJob::State::cancelled);
	}
	if (!admitted) {
		finish(job, stopping ? PromotionJob::State::cancelled : PromotionJob::State::skipped);
		return false;
	}
	return true;
}

void PromotionService::park(const std::shared_ptr<PromotionJob>& job) {
	bool stopping = false;
	{
		const std::lock_guard<std::mutex> lock(mutex_);
		stopping = stopping_;
		if (!stopping) {
			// amortized: drop the entries of dead jobs whenever the list has doubled since the last time
			if (parked_.size() >= parkedPruneAt_) {
				std::erase_if(parked_, [](const std::weak_ptr<PromotionJob>& entry) { return entry.expired(); });
				parkedPruneAt_ = std::max<std::size_t>(64, 2 * parked_.size());
			}
			parked_.push_back(job);
		}
	}
	if (stopping) {
		auto expected = PromotionJob::State::parked;
		if (job->state_.compare_exchange_strong(expected, PromotionJob::State::cancelled, std::memory_order_acq_rel)) {
			finish(job, PromotionJob::State::cancelled);
		}
	}
}

bool PromotionService::idle() const {
	const std::lock_guard<std::mutex> lock(mutex_);
	return queue_.empty() && active_.empty();
}

bool PromotionService::waitIdle(std::chrono::milliseconds timeout) {
	std::unique_lock<std::mutex> lock(mutex_);
	const auto settled = [this] {
		return queue_.empty() && active_.empty();
	};
	if (timeout == std::chrono::milliseconds::max()) {
		idle_.wait(lock, settled);
		return true;
	}
	return idle_.wait_for(lock, timeout, settled);
}

std::size_t PromotionService::workerCount() const {
	const std::lock_guard<std::mutex> lock(mutex_);
	return workers_.size();
}

std::size_t PromotionService::queuedCount() const {
	const std::lock_guard<std::mutex> lock(mutex_);
	return queue_.size();
}

void PromotionService::shutdown() {
	std::vector<std::shared_ptr<PromotionJob>> queued;
	std::vector<std::shared_ptr<PromotionJob>> running;
	std::vector<std::weak_ptr<PromotionJob>> parked;
	std::vector<std::thread> workers;
	{
		const std::lock_guard<std::mutex> lock(mutex_);
		if (stopping_) {
			return;
		}
		stopping_ = true;
		queued.assign(std::make_move_iterator(queue_.begin()), std::make_move_iterator(queue_.end()));
		queue_.clear();
		running = active_;
		parked = std::move(parked_);
		parked_.clear();
		workers = std::move(workers_);
		workers_.clear();
	}
	// everything below runs without the service mutex
	for (auto& job : running) {
		job->cancel(); // ends at its next stage boundary
	}
	wake_.notify_all();
	for (auto& job : queued) {
		job->cancel();
		finish(job, PromotionJob::State::cancelled);
	}
	for (auto& entry : parked) {
		if (auto job = entry.lock()) {
			auto expected = PromotionJob::State::parked;
			if (job->state_.compare_exchange_strong(expected, PromotionJob::State::cancelled,
			                                        std::memory_order_acq_rel)) {
				job->cancel();
				finish(job, PromotionJob::State::cancelled);
			}
		}
	}
	for (auto& worker : workers) {
		if (worker.joinable()) {
			worker.join();
		}
	}
	idle_.notify_all();
}

void PromotionService::workerMain() {
	lowerThreadPriority();
	std::unique_lock<std::mutex> lock(mutex_);
	for (;;) {
		++idleWorkers_;
		wake_.wait(lock, [this] { return stopping_ || !queue_.empty(); });
		--idleWorkers_;
		if (stopping_) {
			return;
		}
		auto job = std::move(queue_.front());
		queue_.pop_front();
		active_.push_back(job);
		lock.unlock();

		runJob(job);

		lock.lock();
		std::erase(active_, job);
		const bool nowIdle = queue_.empty() && active_.empty();
		// the job (and with it the last reference to its module's control, if the module is gone) is released
		// without the service mutex
		lock.unlock();
		job.reset();
		if (nowIdle) {
			idle_.notify_all();
		}
		lock.lock();
	}
}

void PromotionService::runJob(const std::shared_ptr<PromotionJob>& job) {
	auto expected = PromotionJob::State::queued;
	if (job->token_.cancelled() ||
	    !job->state_.compare_exchange_strong(expected, PromotionJob::State::running, std::memory_order_acq_rel)) {
		finish(job, PromotionJob::State::cancelled); // the module is gone
		return;
	}
	{
		// not held during the compile: a module whose last reference is dropped meanwhile cancels the job
		auto module = job->module_.lock();
		if (module == nullptr) {
			finish(job, PromotionJob::State::cancelled);
			return;
		}
		module->setPromotion(engine::PromotionState::running);
	}

	const PromotionRun run {job->token_, job->submitted_, std::chrono::steady_clock::now()};
	std::unique_ptr<Executable> executable;
	std::string error;
	bool failed = false;
	try {
		executable = job->compile_(run);
	} catch (const std::exception& e) {
		failed = true;
		error = e.what();
	} catch (...) {
		failed = true;
		error = "an exception that is not derived from std::exception";
#if defined(__GLIBCXX__) || defined(_LIBCPP_VERSION)
		if (const auto* type = abi::__cxa_current_exception_type()) {
			error += std::string(" (type ") + type->name() + ")";
		}
#endif
	}
	if (failed) {
		log::error("Tier 1 promotion failed, the module stays on tier 0: {}", error);
		finish(job, PromotionJob::State::failed, std::move(error));
		return;
	}
	if (executable == nullptr || job->token_.cancelled()) {
		finish(job, PromotionJob::State::cancelled);
		return;
	}

	std::shared_ptr<Executable> superseded;
	{
		auto module = job->module_.lock();
		if (module == nullptr) {
			finish(job, PromotionJob::State::cancelled);
			return;
		}
		// whether or not tier 0 is installed yet: the caller installs tier 0 only while the version is 0, under this
		// same lock, so the two never overwrite each other in the wrong order (11 §4.5)
		{
			const std::unique_lock<std::shared_mutex> lock(module->mutex);
			superseded = std::move(module->executable);
			module->executable = std::move(executable);
			module->tier.store(engine::Tier::tier1, std::memory_order_relaxed);
			module->version.fetch_add(1, std::memory_order_release);
		}
		// `module` and `superseded` are released after the lock: freeing tier 0 can take a while
	}
	superseded.reset();
	finish(job, PromotionJob::State::done);
}

void PromotionService::finish(const std::shared_ptr<PromotionJob>& job, PromotionJob::State state, std::string error) {
	// first the IR and the options the compile function holds, then the status: whoever waits for the status sees the
	// memory released
	job->compile_ = nullptr;
	job->state_.store(state, std::memory_order_release);
	auto module = job->module_.lock();
	if (module == nullptr) {
		return;
	}
	engine::PromotionState published = engine::PromotionState::cancelled;
	switch (state) {
	case PromotionJob::State::done:
		published = engine::PromotionState::done;
		break;
	case PromotionJob::State::failed:
		published = engine::PromotionState::failed;
		break;
	case PromotionJob::State::skipped:
		published = engine::PromotionState::skipped;
		break;
	default:
		break;
	}
	module->setPromotion(published, std::move(error));
}

} // namespace nautilus::compiler

#endif
