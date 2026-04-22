// In-process SIGPROF-based stack sampler. Per-thread SPSC ring buffers are
// filled by a signal handler and drained by a consumer. dladdr plus the
// profile JIT symbol map resolve frames back to names.
#include "nautilus/profile/Sampler.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/profile/JitSymbols.hpp"
#include "nautilus/profile/Recorder.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <mutex>
#include <pthread.h>
#include <signal.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#if defined(__linux__)
#include <sys/syscall.h>
#endif

namespace nautilus::profile {

// Defined in profile_runtime.cpp. Declared here so the consumer in this TU
// can reach the shared recorder buffer without duplicating its mutex.
void recordSampleEvent(Event ev);

// Also defined in profile_runtime.cpp. Returns raw cycle-counter ticks
// since the recorder's session start — the same timebase region events
// use, so samples and regions correlate on a shared timeline.
uint64_t sessionNowTicks();

namespace {

// Stored per registered thread. Signal handler writes to `buffer` through
// `head` (producer); consumer advances `tail`. SPSC, so plain atomics
// suffice for ordering.
struct Sample {
	uint64_t ts_ticks;
	uint32_t depth;
	void* frames[64];
};

struct ThreadSlot {
	pthread_t pthread_id;
	uint64_t tid;
	std::atomic<uint64_t> head {0};
	std::atomic<uint64_t> tail {0};
	std::atomic<uint64_t> overruns {0};
	std::vector<Sample> buffer;
	// mask = buffer.size() - 1 (power-of-two for cheap wrap)
	size_t mask;
};

struct State {
	std::mutex mu;
	std::vector<ThreadSlot*> slots;
	std::atomic<bool> sampling {false};
	std::atomic<bool> stop {false};
	std::thread sampler_thread;
	SamplerConfig cfg;
	std::atomic<size_t> total_overruns {0};
};

static State& state() {
	static State s;
	return s;
}

static thread_local ThreadSlot* tls_slot = nullptr;

static uint64_t currentTid() {
#if defined(__linux__)
	return static_cast<uint64_t>(::syscall(SYS_gettid));
#elif defined(__APPLE__)
	uint64_t t = 0;
	pthread_threadid_np(nullptr, &t);
	return t;
#else
	return reinterpret_cast<uint64_t>(pthread_self());
#endif
}

static uint64_t monotonicTicks() {
	// Share the timebase with the region recorder so a sample's timestamp
	// can be compared directly against B/E timestamps and correlated to
	// the region that was open at the moment the signal fired.
	return sessionNowTicks();
}

static size_t roundUpPow2(size_t v) {
	if (v < 2) {
		return 2;
	}
	size_t p = 1;
	while (p < v) {
		p <<= 1;
	}
	return p;
}

// Signal handler. MUST be async-signal-safe: only atomic ops, backtrace(),
// clock_gettime(). No malloc, no locks, no logging. The buffer is
// preallocated and power-of-two sized.
extern "C" void sigprof_handler(int /*sig*/, siginfo_t* /*info*/, void* /*ctx*/) {
	ThreadSlot* slot = tls_slot;
	if (slot == nullptr) {
		return;
	}
	if (!state().sampling.load(std::memory_order_relaxed)) {
		return;
	}

	uint64_t head = slot->head.load(std::memory_order_relaxed);
	uint64_t tail = slot->tail.load(std::memory_order_acquire);
	if (head - tail >= slot->buffer.size()) {
		slot->overruns.fetch_add(1, std::memory_order_relaxed);
		return;
	}

	Sample& s = slot->buffer[head & slot->mask];
	s.ts_ticks = monotonicTicks();
	int depth = backtrace(s.frames, static_cast<int>(sizeof(s.frames) / sizeof(void*)));
	s.depth = depth > 0 ? static_cast<uint32_t>(depth) : 0;

	slot->head.store(head + 1, std::memory_order_release);
}

static void installSignalHandler() {
	struct sigaction sa {};
	sa.sa_sigaction = &sigprof_handler;
	sa.sa_flags = SA_SIGINFO | SA_RESTART;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGPROF, &sa, nullptr) != 0) {
		nautilus::log::error("profile.sampler: sigaction failed");
	}

	// Prime backtrace() so its lazy-init doesn't happen in signal context.
	void* dummy[8];
	(void) backtrace(dummy, 8);
}

static std::string demangleOrRaw(const char* mangled) {
	if (mangled == nullptr) {
		return "<unknown>";
	}
	int status = 0;
	char* out = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);
	if (status == 0 && out != nullptr) {
		std::string result(out);
		std::free(out);
		return result;
	}
	return mangled;
}

// Resolves one frame to a human-readable name. Prefers the JIT symbol table
// (so frames inside MLIR/AsmJIT output show their Nautilus name), then
// falls back to dladdr for anything linked normally.
static std::string resolveFrame(void* ip) {
	if (auto jit = lookupJitSymbol(reinterpret_cast<uintptr_t>(ip))) {
		return jit->name + "[" + jit->module + "]";
	}
	Dl_info info {};
	if (dladdr(ip, &info) != 0 && info.dli_sname != nullptr) {
		return demangleOrRaw(info.dli_sname);
	}
	char buf[32];
	std::snprintf(buf, sizeof(buf), "0x%lx", reinterpret_cast<unsigned long>(ip));
	return buf;
}

static size_t drainSlot(ThreadSlot* slot) {
	size_t drained = 0;
	uint64_t head = slot->head.load(std::memory_order_acquire);
	uint64_t tail = slot->tail.load(std::memory_order_relaxed);
	while (tail != head) {
		const Sample& s = slot->buffer[tail & slot->mask];
		Event ev;
		ev.kind = Event::Kind::Sample;
		ev.name = "sample";
		ev.value = 0;
		ev.timestamp_ticks = s.ts_ticks;
		ev.tid = slot->tid;
		ev.module.clear();
		ev.stack.reserve(s.depth);
		for (uint32_t i = 0; i < s.depth; ++i) {
			ev.stack.push_back(resolveFrame(s.frames[i]));
		}

		// recordSampleEvent lives in profile_runtime.cpp so the recorder
		// state (mutex + buffer) stays encapsulated in one TU. Declared
		// at namespace scope below to give it external linkage.
		recordSampleEvent(std::move(ev));

		++tail;
		++drained;
	}
	slot->tail.store(tail, std::memory_order_release);
	return drained;
}

static void samplerThreadMain() {
	while (!state().stop.load(std::memory_order_relaxed)) {
		{
			std::lock_guard<std::mutex> lock(state().mu);
			for (auto* slot : state().slots) {
				pthread_kill(slot->pthread_id, SIGPROF);
			}
		}
		std::this_thread::sleep_for(state().cfg.interval);
	}
}

} // namespace

void startSampler(SamplerConfig cfg) {
	State& s = state();
	std::lock_guard<std::mutex> lock(s.mu);
	if (s.sampling.load()) {
		// Already running: update config and return.
		s.cfg = cfg;
		return;
	}
	s.cfg = cfg;
	installSignalHandler();
	s.stop.store(false);
	s.sampling.store(true, std::memory_order_release);
	s.sampler_thread = std::thread(samplerThreadMain);
}

size_t stopSampler() {
	State& s = state();
	{
		std::lock_guard<std::mutex> lock(s.mu);
		if (!s.sampling.load()) {
			return 0;
		}
		s.stop.store(true, std::memory_order_relaxed);
	}
	if (s.sampler_thread.joinable()) {
		s.sampler_thread.join();
	}
	s.sampling.store(false, std::memory_order_release);

	// Drain every slot. Do this without holding the sampling mutex for the
	// whole time — grab the slot list, release, then drain without
	// contention.
	std::vector<ThreadSlot*> slots;
	{
		std::lock_guard<std::mutex> lock(s.mu);
		slots = s.slots;
	}
	size_t total = 0;
	for (auto* slot : slots) {
		total += drainSlot(slot);
		s.total_overruns.fetch_add(slot->overruns.load(), std::memory_order_relaxed);
	}
	return total;
}

void registerThisThread() {
	if (tls_slot != nullptr) {
		return;
	}
	auto* slot = new ThreadSlot();
	slot->pthread_id = pthread_self();
	slot->tid = currentTid();
	slot->buffer.resize(roundUpPow2(state().cfg.ring_slots));
	slot->mask = slot->buffer.size() - 1;
	tls_slot = slot;

	// Ensure SIGPROF is deliverable on this thread. Some runtimes block it
	// by default (notably when ITIMER_PROF-based timers are enabled elsewhere
	// in the process); without this, pthread_kill arrives but the handler
	// never runs until the mask is lifted.
	sigset_t unblock;
	sigemptyset(&unblock);
	sigaddset(&unblock, SIGPROF);
	pthread_sigmask(SIG_UNBLOCK, &unblock, nullptr);

	std::lock_guard<std::mutex> lock(state().mu);
	state().slots.push_back(slot);
}

void unregisterThisThread() {
	auto* slot = tls_slot;
	if (slot == nullptr) {
		return;
	}
	tls_slot = nullptr;

	// Drain before destroy so outstanding samples are not lost.
	drainSlot(slot);

	std::lock_guard<std::mutex> lock(state().mu);
	auto& v = state().slots;
	v.erase(std::remove(v.begin(), v.end(), slot), v.end());
	delete slot;
}

} // namespace nautilus::profile
