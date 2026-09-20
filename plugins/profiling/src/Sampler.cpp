#include "nautilus/profiling/sampler.hpp"
#include "SampleAggregation.hpp"
#include "ThreadSlots.hpp"
#include <algorithm>
#include <atomic>
#include <exception>
#include <mutex>
#include <optional>
#include <perfcpp/sampler.hpp>
#include <thread>
#include <utility>

// perf-cpp lives in the global namespace `perf`. Reaching it through an alias
// rather than unqualified keeps the vendored library visibly distinct from this
// plugin's own `nautilus::profiling` names at every use site.
namespace pc = ::perf;

namespace nautilus::profiling {

namespace {

uint16_t defaultCapacity() {
	const auto hardware = std::thread::hardware_concurrency();
	if (hardware == 0) {
		return 8;
	}
	return static_cast<uint16_t>(std::min<unsigned>(hardware, UINT16_MAX - 1));
}

} // namespace

struct Sampler::Impl {
	bool usable = false;
	std::string reason;
	mutable std::mutex reasonMutex;

	/// Always the multi-threaded sampler, even for one thread: a slot only
	/// costs a ring buffer once it is started, so a single-threaded path would
	/// buy nothing but a second API.
	std::optional<pc::MultiThreadSampler> sampler;
	detail::ThreadSlots slots;

	std::vector<std::atomic<bool>> running;
	/// Set once stopAll() has drained the ring buffers; draining twice would
	/// come back empty and clobber the report.
	bool drained = false;

	SampleReport report;

	explicit Impl(uint16_t capacity) : slots(capacity), running(capacity) {
		for (uint16_t slot = 0; slot < capacity; ++slot) {
			running[slot].store(false, std::memory_order_relaxed);
		}
	}

	void note(const std::string& text) {
		const std::lock_guard lock(reasonMutex);
		if (!reason.empty()) {
			reason += "; ";
		}
		reason += text;
	}
};

Sampler::Sampler() : Sampler(Options {}) {
}

Sampler::Sampler(Options options)
    : impl_(std::make_unique<Impl>(options.maxThreads == 0 ? defaultCapacity() : options.maxThreads)) {
	const auto& hostSupport = support();
	if (!hostSupport) {
		impl_->reason = hostSupport.reason;
		return;
	}

	try {
		pc::SampleConfig config;
		config.buffer_pages(static_cast<uint16_t>(options.bufferPages));
		impl_->sampler.emplace(impl_->slots.capacity(), config);

		impl_->sampler->trigger(std::string(options.event), pc::Period {options.period});
		// The logical instruction pointer is the whole point: it is what gets
		// resolved against the JIT symbol registry.
		impl_->sampler->values().logical_instruction_pointer(true);
		if (options.callchain) {
			impl_->sampler->values().callchain(true);
		}
	} catch (const std::exception& error) {
		impl_->reason = std::string("could not configure the sampler: ") + error.what();
		impl_->sampler.reset();
		return;
	}
	impl_->usable = true;
}

Sampler::~Sampler() = default;
Sampler::Sampler(Sampler&&) noexcept = default;
Sampler& Sampler::operator=(Sampler&&) noexcept = default;

bool Sampler::available() const noexcept {
	return impl_->usable;
}

std::string_view Sampler::unavailableReason() const noexcept {
	const std::lock_guard lock(impl_->reasonMutex);
	return impl_->reason;
}

uint16_t Sampler::maxThreads() const noexcept {
	return impl_->slots.capacity();
}

bool Sampler::running() const noexcept {
	const auto slot = impl_->slots.current();
	if (slot == detail::ThreadSlots::INVALID) {
		return false;
	}
	return impl_->running[slot].load(std::memory_order_acquire);
}

void Sampler::start() {
	if (!impl_->usable || impl_->drained) {
		return;
	}
	const auto slot = impl_->slots.acquire();
	if (slot == detail::ThreadSlots::INVALID) {
		impl_->note("a thread found no free sampler slot (maxThreads = " + std::to_string(impl_->slots.capacity()) +
		            "); its work is not sampled");
		return;
	}
	if (impl_->running[slot].exchange(true, std::memory_order_acq_rel)) {
		return;
	}
	try {
		// Opens against the *calling* thread, which is why the slot is derived
		// from the caller rather than passed in.
		impl_->sampler->start(slot);
	} catch (const std::exception& error) {
		impl_->running[slot].store(false, std::memory_order_release);
		// Sampling needs more than counting does -- a ring buffer mapping, and
		// `perf_event_paranoid <= 2` even for own-thread sampling on some
		// kernels -- so this can fail where a CounterSet succeeded.
		impl_->note(std::string("could not start sampling on a thread: ") + error.what());
	}
}

void Sampler::stop() {
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
		impl_->sampler->stop(slot);
	} catch (const std::exception&) {
		// A slot that will not stop still has its samples drained by stopAll().
	}
}

void Sampler::stopAll() {
	if (!impl_->usable || impl_->drained) {
		return;
	}
	for (const auto slot : impl_->slots.assigned()) {
		if (!impl_->running[slot].exchange(false, std::memory_order_acq_rel)) {
			continue;
		}
		try {
			impl_->sampler->stop(slot);
		} catch (const std::exception&) {
		}
	}

	try {
		// result() merges every slot's samples, and has to run before close()
		// unmaps the ring buffers and discards what is in them. Draining happens
		// once, here, rather than per stop(): a buffer read twice comes back
		// empty the second time.
		impl_->report = detail::aggregate(impl_->sampler->result());
		impl_->sampler->close();
	} catch (const std::exception& error) {
		impl_->note(std::string("could not read samples: ") + error.what());
	}
	impl_->drained = true;
}

const SampleReport& Sampler::report() const noexcept {
	return impl_->report;
}

void Sampler::reset() {
	for (const auto slot : impl_->slots.assigned()) {
		impl_->running[slot].store(false, std::memory_order_release);
	}
	impl_->slots.clear();
	impl_->report = SampleReport();
	impl_->drained = false;
}

} // namespace nautilus::profiling
