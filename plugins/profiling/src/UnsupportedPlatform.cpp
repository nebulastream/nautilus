// Stub implementations for hosts without a perf subsystem.
//
// perf_event_open(2) is Linux-only, so perf-cpp is not even compiled elsewhere
// (see plugins/profiling/CMakeLists.txt). The public API still exists and still
// links, reporting itself permanently unavailable -- the same
// warning-and-continue contract the `perf` engine option honours, so an
// application that measures itself on Linux still builds and runs on macOS.

#include "nautilus/profiling/counters.hpp"
#include "nautilus/profiling/sampler.hpp"
#include <memory>
#include <utility>

namespace nautilus::profiling {

namespace {
constexpr auto UNSUPPORTED_REASON = "hardware performance counters require Linux perf_event_open(2)";
}

const Support& support() noexcept {
	static const Support cached {Availability::UnsupportedPlatform, UNSUPPORTED_REASON};
	return cached;
}

struct CounterSet::Impl {
	std::vector<std::string> programmed;
	std::string reason {UNSUPPORTED_REASON};
	Measurement result;
};

std::vector<std::string> CounterSet::defaultEvents() {
	return {"seconds", "cycles", "instructions", "cache-misses", "branch-misses"};
}

CounterSet::CounterSet(std::vector<std::string>, uint16_t) : impl_(std::make_unique<Impl>()) {
}

CounterSet::~CounterSet() = default;
CounterSet::CounterSet(CounterSet&&) noexcept = default;
CounterSet& CounterSet::operator=(CounterSet&&) noexcept = default;

bool CounterSet::available() const noexcept {
	return false;
}

std::string_view CounterSet::unavailableReason() const noexcept {
	return impl_->reason;
}

const std::vector<std::string>& CounterSet::events() const noexcept {
	return impl_->programmed;
}

uint16_t CounterSet::maxThreads() const noexcept {
	return 0;
}

void CounterSet::start() {
}

void CounterSet::stop() {
}

void CounterSet::stopAll() {
}

bool CounterSet::running() const noexcept {
	return false;
}

const Measurement& CounterSet::result() const noexcept {
	return impl_->result;
}

std::vector<Measurement> CounterSet::perThreadResults() const {
	return {};
}

void CounterSet::reset() {
}

struct Sampler::Impl {
	std::string reason {UNSUPPORTED_REASON};
	SampleReport report;
};

Sampler::Sampler() : Sampler(Options {}) {
}

Sampler::Sampler(Options) : impl_(std::make_unique<Impl>()) {
}

Sampler::~Sampler() = default;
Sampler::Sampler(Sampler&&) noexcept = default;
Sampler& Sampler::operator=(Sampler&&) noexcept = default;

bool Sampler::available() const noexcept {
	return false;
}

std::string_view Sampler::unavailableReason() const noexcept {
	return impl_->reason;
}

uint16_t Sampler::maxThreads() const noexcept {
	return 0;
}

void Sampler::start() {
}

void Sampler::stop() {
}

void Sampler::stopAll() {
}

bool Sampler::running() const noexcept {
	return false;
}

const SampleReport& Sampler::report() const noexcept {
	return impl_->report;
}

void Sampler::reset() {
}

} // namespace nautilus::profiling
