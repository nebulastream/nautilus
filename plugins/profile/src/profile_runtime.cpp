#include "nautilus/profile/profile_runtime.h"
#include "PerfettoTraceWriter.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/profile/Recorder.hpp"
#include "nautilus/profile/TraceWriter.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#if defined(__linux__)
#include <sys/syscall.h>
#include <unistd.h>
#endif

#if defined(__x86_64__) || defined(__i386__)
#include <cpuid.h>
#endif

namespace nautilus::profile::detail {

// ---- Session timebase ------------------------------------------------------
//
// Event timestamps are raw "ticks" from a monotonic cycle counter (invariant
// TSC on x86, CNTVCT on arm64) where available. The JIT'd path emits
// `llvm.readcyclecounter()` inline before calling the profiler runtime, so
// the cost of a region event drops from a vDSO `clock_gettime` (~20-30 ns)
// to a single `rdtsc` (~5-10 ns). On hardware without an invariant cycle
// counter, we fall back to `steady_clock::now()` expressed in nanoseconds
// and the calibration degenerates to 1000 ticks/µs.
//
// Conversion to microseconds happens lazily at flush time so the hot path
// never divides.

struct Timebase {
	bool useTsc = false;
	uint64_t baseline = 0;
	double ticks_per_us = 1;
};

static bool hostHasInvariantCycleCounter() {
#if defined(__aarch64__)
	return true;
#elif defined(__x86_64__) || defined(__i386__)
	unsigned int max_ext = __get_cpuid_max(0x80000000u, nullptr);
	if (max_ext < 0x80000007u) {
		return false;
	}
	unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;
	if (!__get_cpuid(0x80000007u, &eax, &ebx, &ecx, &edx)) {
		return false;
	}
	return ((edx >> 8) & 1u) != 0u;
#else
	return false;
#endif
}

static inline uint64_t rawCycles() {
#if defined(__x86_64__) || defined(__i386__) || defined(__aarch64__)
	return __builtin_readcyclecounter();
#else
	auto d = std::chrono::steady_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
#endif
}

static inline uint64_t steadyNanos() {
	auto d = std::chrono::steady_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
}

static Timebase calibrateTimebase() {
	Timebase tb;
	tb.useTsc = hostHasInvariantCycleCounter();
	if (tb.useTsc) {
		uint64_t t0 = rawCycles();
		auto w0 = std::chrono::steady_clock::now();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		uint64_t t1 = rawCycles();
		auto w1 = std::chrono::steady_clock::now();
		auto us = std::chrono::duration_cast<std::chrono::microseconds>(w1 - w0).count();
		if (us > 0 && t1 > t0) {
			tb.ticks_per_us = static_cast<double>(t1 - t0) / static_cast<double>(us);
		}
		tb.baseline = t0;
	} else {
		tb.ticks_per_us = 1000.0;
		tb.baseline = steadyNanos();
	}
	return tb;
}

static Timebase& timebase() {
	static Timebase tb = calibrateTimebase();
	return tb;
}

static uint64_t sessionTicks() {
	const Timebase& tb = timebase();
	if (tb.useTsc) {
		return rawCycles() - tb.baseline;
	}
	return steadyNanos() - tb.baseline;
}

uint64_t ticksToMicros(uint64_t ticks) {
	return static_cast<uint64_t>(static_cast<double>(ticks) / timebase().ticks_per_us);
}

static uint64_t currentTid() {
#if defined(__linux__)
	return static_cast<uint64_t>(::syscall(SYS_gettid));
#else
	return std::hash<std::thread::id> {}(std::this_thread::get_id());
#endif
}

// Per-thread stack of open module names. Stored as raw const char* pointers;
// the caller guarantees these point to static storage (per the design doc).
static thread_local std::vector<const char*> tlsModuleStack;

static const char* currentModuleRaw() {
	return tlsModuleStack.empty() ? nullptr : tlsModuleStack.back();
}

// ---- Per-thread region-event SPSC ring -------------------------------------
//
// Trivially copyable payload, 40 bytes. Writer (profiler hot path) does two
// atomic loads + a small number of stores + one atomic store with release.
// No mutex, no allocation.

struct RegionEvent {
	uint64_t ticks;
	const char* name;   // interned literal
	const char* module; // interned literal or nullptr
	int64_t value;      // counter value, else 0
	uint32_t kind;      // static_cast<uint32_t>(Event::Kind)
	uint32_t pad;       // explicit padding — pointer-aligned
};
static_assert(sizeof(RegionEvent) == 40, "RegionEvent layout changed unexpectedly");

struct RegionSlot {
	std::atomic<uint64_t> head {0};
	std::atomic<uint64_t> tail {0};
	std::atomic<uint64_t> overruns {0};
	size_t mask = 0;
	std::vector<RegionEvent> buffer;
	uint64_t tid = 0;
};

static std::mutex& regionSlotsMutex() {
	static std::mutex m;
	return m;
}
static std::vector<RegionSlot*>& regionSlots() {
	static std::vector<RegionSlot*> v;
	return v;
}
static thread_local RegionSlot* tls_region_slot = nullptr;

// Ring capacity. Large enough that typical workloads never drop events but
// small enough that per-thread overhead stays reasonable (64 KiB each).
static constexpr size_t kRegionRingSlots = 16 * 1024;

static RegionSlot* getOrCreateRegionSlot() {
	if (tls_region_slot != nullptr) {
		return tls_region_slot;
	}
	auto* slot = new RegionSlot();
	slot->buffer.resize(kRegionRingSlots);
	slot->mask = kRegionRingSlots - 1;
	slot->tid = currentTid();
	tls_region_slot = slot;
	{
		std::lock_guard<std::mutex> lock(regionSlotsMutex());
		regionSlots().push_back(slot);
	}
	return slot;
}

static inline void pushRegionEvent(Event::Kind kind, const char* name, int64_t value, uint64_t ticks) {
	RegionSlot* slot = getOrCreateRegionSlot();
	const uint64_t head = slot->head.load(std::memory_order_relaxed);
	const uint64_t tail = slot->tail.load(std::memory_order_acquire);
	if (head - tail >= slot->buffer.size()) {
		slot->overruns.fetch_add(1, std::memory_order_relaxed);
		return;
	}
	RegionEvent& e = slot->buffer[head & slot->mask];
	e.ticks = ticks;
	e.name = name != nullptr ? name : "<null>";
	e.module = currentModuleRaw();
	e.value = value;
	e.kind = static_cast<uint32_t>(kind);
	e.pad = 0;
	slot->head.store(head + 1, std::memory_order_release);
}

// ---- Sample-event buffer ---------------------------------------------------
//
// Samples are low-frequency (~2 kHz) and already land on the consumer thread
// via the Sampler's own per-thread rings. Keep them in a simple global
// mutex-protected vector here — the hot path for these is already off the
// profiler's critical section.

static std::mutex& sampleMutex() {
	static std::mutex m;
	return m;
}
static std::vector<Event>& sampleBuffer() {
	static std::vector<Event> v;
	return v;
}

// ---- Drain: materialize a merged, time-sorted std::vector<Event> -----------

static std::vector<Event> drainAll() {
	std::vector<Event> out;

	// Samples: swap the global buffer out.
	{
		std::lock_guard<std::mutex> lock(sampleMutex());
		out.swap(sampleBuffer());
	}

	// Regions: walk every registered slot under a snapshot of the slot list,
	// drain each ring SPSC, and materialize Event objects with strings here
	// (off the hot path).
	std::vector<RegionSlot*> slots;
	{
		std::lock_guard<std::mutex> lock(regionSlotsMutex());
		slots = regionSlots();
	}
	for (RegionSlot* slot : slots) {
		uint64_t head = slot->head.load(std::memory_order_acquire);
		uint64_t tail = slot->tail.load(std::memory_order_relaxed);
		out.reserve(out.size() + static_cast<size_t>(head - tail));
		while (tail != head) {
			const RegionEvent& r = slot->buffer[tail & slot->mask];
			Event ev;
			ev.kind = static_cast<Event::Kind>(r.kind);
			ev.name = r.name != nullptr ? std::string(r.name) : std::string("<null>");
			ev.value = r.value;
			ev.timestamp_ticks = r.ticks;
			ev.tid = slot->tid;
			ev.module = r.module != nullptr ? std::string(r.module) : std::string();
			out.push_back(std::move(ev));
			++tail;
		}
		slot->tail.store(tail, std::memory_order_release);
	}

	// Merge by timestamp so the output is a single monotonic stream. This is
	// a post-drain sort — flush-time only, never on the hot path.
	std::sort(out.begin(), out.end(),
	          [](const Event& a, const Event& b) { return a.timestamp_ticks < b.timestamp_ticks; });
	return out;
}

} // namespace nautilus::profile::detail

namespace nautilus::profile {

std::vector<Event> takeRecordedEvents() {
	return detail::drainAll();
}

void clearRecordedEvents() {
	// Discard everything and reset the timebase baseline. Subsequent sessions
	// start at ticks=0 again; ticks_per_us stays as calibrated.
	(void) detail::drainAll();
	auto& tb = detail::timebase();
	tb.baseline = tb.useTsc ? detail::rawCycles() : detail::steadyNanos();
}

// Bridge used by the SIGPROF sampler consumer. Samples are rare; the
// mutex-protected global buffer is fine for them.
void recordSampleEvent(Event ev) {
	std::lock_guard<std::mutex> lock(detail::sampleMutex());
	detail::sampleBuffer().push_back(std::move(ev));
}

uint64_t sessionNowTicks() {
	return detail::sessionTicks();
}

void openModule(const char* name) {
	detail::tlsModuleStack.push_back(name != nullptr ? name : "<null>");
}

void closeModule(const char* /*name*/) {
	if (!detail::tlsModuleStack.empty()) {
		detail::tlsModuleStack.pop_back();
	}
}

namespace {

void appendEscaped(std::string& out, const std::string& s) {
	for (char c : s) {
		switch (c) {
		case '"':
			out.append("\\\"");
			break;
		case '\\':
			out.append("\\\\");
			break;
		case '\n':
			out.append("\\n");
			break;
		case '\r':
			out.append("\\r");
			break;
		case '\t':
			out.append("\\t");
			break;
		default:
			if (static_cast<unsigned char>(c) < 0x20) {
				char buf[8];
				std::snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned>(c));
				out.append(buf);
			} else {
				out.push_back(c);
			}
		}
	}
}

class VirtualTidTable {
public:
	uint64_t assign(uint64_t real_tid, const std::string& module) {
		auto key = std::make_pair(real_tid, module);
		auto it = map_.find(key);
		if (it != map_.end()) {
			return it->second;
		}
		uint64_t vid = next_++;
		map_.emplace(std::move(key), vid);
		return vid;
	}

private:
	struct PairHash {
		size_t operator()(const std::pair<uint64_t, std::string>& p) const noexcept {
			return std::hash<uint64_t> {}(p.first) ^ (std::hash<std::string> {}(p.second) << 1);
		}
	};
	std::unordered_map<std::pair<uint64_t, std::string>, uint64_t, PairHash> map_;
	uint64_t next_ = 1;
};

} // namespace

bool flushTrace(const std::string& path) {
	std::vector<Event> events = detail::drainAll();

	std::ofstream out(path);
	if (!out) {
		nautilus::log::error("profile.flushTrace: cannot open {}", path);
		return false;
	}

	VirtualTidTable tids;
	const uint32_t pid = 1;

	out << "{\"traceEvents\":[\n";
	std::string buf;
	bool first = true;

	std::vector<std::pair<uint64_t, std::string>> threadLabels;
	std::unordered_map<uint64_t, bool> labelled;

	auto flushComma = [&](std::ofstream& o) {
		if (!first) {
			o << ",\n";
		}
		first = false;
	};

	for (const auto& ev : events) {
		uint64_t vtid = tids.assign(ev.tid, ev.module);
		if (!labelled[vtid]) {
			threadLabels.emplace_back(vtid, ev.module.empty() ? "<default>" : ev.module);
			labelled[vtid] = true;
		}

		buf.clear();
		buf.append("  {\"cat\":\"nautilus\",\"pid\":");
		buf.append(std::to_string(pid));
		buf.append(",\"tid\":");
		buf.append(std::to_string(vtid));
		buf.append(",\"ts\":");
		buf.append(std::to_string(detail::ticksToMicros(ev.timestamp_ticks)));

		switch (ev.kind) {
		case Event::Kind::Begin:
			buf.append(",\"ph\":\"B\",\"name\":\"");
			appendEscaped(buf, ev.name);
			buf.append("\"");
			break;
		case Event::Kind::End:
			buf.append(",\"ph\":\"E\",\"name\":\"");
			appendEscaped(buf, ev.name);
			buf.append("\"");
			break;
		case Event::Kind::CounterI64:
			buf.append(",\"ph\":\"C\",\"name\":\"");
			appendEscaped(buf, ev.name);
			buf.append("\",\"args\":{\"value\":");
			buf.append(std::to_string(ev.value));
			buf.append("}");
			break;
		case Event::Kind::Sample:
			buf.append(",\"ph\":\"i\",\"s\":\"t\",\"name\":\"");
			appendEscaped(buf, ev.name.empty() ? "sample" : ev.name);
			buf.append("\",\"args\":{\"frames\":[");
			for (size_t i = 0; i < ev.stack.size(); ++i) {
				if (i != 0) {
					buf.append(",");
				}
				buf.append("\"");
				appendEscaped(buf, ev.stack[i]);
				buf.append("\"");
			}
			buf.append("]}");
			break;
		}
		buf.append("}");

		flushComma(out);
		out << buf;
	}

	for (const auto& [vtid, label] : threadLabels) {
		buf.clear();
		buf.append("  {\"cat\":\"__metadata\",\"ph\":\"M\",\"name\":\"thread_name\",\"pid\":");
		buf.append(std::to_string(pid));
		buf.append(",\"tid\":");
		buf.append(std::to_string(vtid));
		buf.append(",\"args\":{\"name\":\"");
		appendEscaped(buf, label);
		buf.append("\"}}");
		flushComma(out);
		out << buf;
	}

	out << "\n]}\n";
	if (!out) {
		nautilus::log::error("profile.flushTrace: write failed for {}", path);
		return false;
	}
	return true;
}

bool flushPerfettoTrace(const std::string& path) {
	std::vector<Event> events = detail::drainAll();
	if (!detail::writePerfettoTrace(path, events)) {
		nautilus::log::error("profile.flushPerfettoTrace: write failed for {}", path);
		return false;
	}
	return true;
}

} // namespace nautilus::profile

extern "C" {

// Default entry points — read the cycle counter internally. Used by the
// interpreter and by the CPP/BC backends that call invoke() directly.

void __nautilus_profile_begin(const char* name) {
	nautilus::profile::detail::pushRegionEvent(nautilus::profile::Event::Kind::Begin, name, 0,
	                                           nautilus::profile::detail::sessionTicks());
}

void __nautilus_profile_end(const char* name) {
	nautilus::profile::detail::pushRegionEvent(nautilus::profile::Event::Kind::End, name, 0,
	                                           nautilus::profile::detail::sessionTicks());
}

void __nautilus_profile_counter_i64(const char* name, int64_t value) {
	nautilus::profile::detail::pushRegionEvent(nautilus::profile::Event::Kind::CounterI64, name, value,
	                                           nautilus::profile::detail::sessionTicks());
}

// Pre-timed entry points — the caller (currently only the MLIR intrinsic
// lowering) emits llvm.readcyclecounter() inline and passes the raw tick
// value. Saves one function call and the cost of reading the counter
// through the plain entry point.

void __nautilus_profile_begin_ticks(const char* name, uint64_t ticks) {
	nautilus::profile::detail::pushRegionEvent(nautilus::profile::Event::Kind::Begin, name, 0,
	                                           ticks - nautilus::profile::detail::timebase().baseline);
}

void __nautilus_profile_end_ticks(const char* name, uint64_t ticks) {
	nautilus::profile::detail::pushRegionEvent(nautilus::profile::Event::Kind::End, name, 0,
	                                           ticks - nautilus::profile::detail::timebase().baseline);
}

void __nautilus_profile_counter_i64_ticks(const char* name, int64_t value, uint64_t ticks) {
	nautilus::profile::detail::pushRegionEvent(nautilus::profile::Event::Kind::CounterI64, name, value,
	                                           ticks - nautilus::profile::detail::timebase().baseline);
}

} // extern "C"
