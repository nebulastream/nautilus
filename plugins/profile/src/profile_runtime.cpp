#include "nautilus/profile/profile_runtime.h"
#include "nautilus/logging.hpp"
#include "nautilus/profile/Recorder.hpp"
#include "nautilus/profile/TraceWriter.hpp"
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

static std::mutex& recorderMutex() {
	static std::mutex m;
	return m;
}

static std::vector<Event>& recorderBuffer() {
	static std::vector<Event> buf;
	return buf;
}

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
	bool useTsc = false;     // true if the raw tick source is rdtsc/cntvct
	uint64_t baseline = 0;   // subtracted from raw ticks to produce session-relative
	double ticks_per_us = 1; // divisor applied at flush time
};

static bool hostHasInvariantCycleCounter() {
#if defined(__aarch64__)
	return true; // arm64 cntvct is architecturally invariant
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
		// Measure ticks/µs by sleeping briefly. 5 ms is enough to dwarf
		// clock-read jitter without noticeably delaying startup.
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
		// Fallback: store steady_clock nanoseconds; 1000 ticks per µs.
		tb.ticks_per_us = 1000.0;
		tb.baseline = steadyNanos();
	}
	return tb;
}

static Timebase& timebase() {
	// Meyers singleton with first-use calibration. Access from the profiler
	// hot path is a pair of relaxed atomic loads (ticks_per_us + baseline).
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
	// macOS and generic fallback: hash the std::thread::id. Stable for the
	// lifetime of the thread and unique across live threads, which is all
	// the trace writer needs.
	return std::hash<std::thread::id> {}(std::this_thread::get_id());
#endif
}

// Per-thread stack of open module names. "module" here means a logical
// region-grouping opened via profile::openModule, not an MLIR/Nautilus
// compilation module.
static thread_local std::vector<const char*> tlsModuleStack;

static std::string currentModule() {
	if (tlsModuleStack.empty()) {
		return {};
	}
	return tlsModuleStack.back();
}

static void record(Event ev) {
	std::lock_guard<std::mutex> lock(recorderMutex());
	recorderBuffer().push_back(std::move(ev));
}

static Event makeEvent(Event::Kind kind, const char* name, int64_t value, uint64_t ticks) {
	Event ev;
	ev.kind = kind;
	ev.name = name != nullptr ? name : "<null>";
	ev.value = value;
	ev.timestamp_ticks = ticks;
	ev.tid = currentTid();
	ev.module = currentModule();
	return ev;
}

} // namespace nautilus::profile::detail

namespace nautilus::profile {

std::vector<Event> takeRecordedEvents() {
	std::lock_guard<std::mutex> lock(detail::recorderMutex());
	std::vector<Event> out;
	out.swap(detail::recorderBuffer());
	return out;
}

void clearRecordedEvents() {
	std::lock_guard<std::mutex> lock(detail::recorderMutex());
	detail::recorderBuffer().clear();
	// Reset the timebase baseline so the next session's events start at
	// ticks=0 again. ticks_per_us and useTsc stay as calibrated.
	auto& tb = detail::timebase();
	tb.baseline = tb.useTsc ? detail::rawCycles() : detail::steadyNanos();
}

// Bridge used by the sampler consumer. Defined here so the recorder state
// (mutex + buffer) stays encapsulated in this TU.
void recordSampleEvent(Event ev) {
	std::lock_guard<std::mutex> lock(detail::recorderMutex());
	detail::recorderBuffer().push_back(std::move(ev));
}

// Exposes the monotonic session clock so the sampler (a separate TU) can
// stamp samples in the same timebase as regions. Returns raw ticks; the
// sampler records them directly into Event::timestamp_ticks. Signal-safe:
// just a couple of atomic loads and a rdtsc/cntvct read on TSC-capable
// hardware, or steady_clock::now() on the fallback path.
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

// JSON escape of a region/module name. Chrome-trace names are unlikely to
// contain unusual characters but we still handle the common cases.
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

// Assigns a stable "virtual tid" per (real_tid, module_name) pair so that
// Perfetto UI renders one track per logical module on each thread.
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
	// Drain events under the same lock so we don't race with in-flight
	// emits. Then release the lock before touching the filesystem.
	std::vector<Event> events;
	{
		std::lock_guard<std::mutex> lock(detail::recorderMutex());
		events.swap(detail::recorderBuffer());
	}

	std::ofstream out(path);
	if (!out) {
		nautilus::log::error("profile.flushTrace: cannot open {}", path);
		return false;
	}

	VirtualTidTable tids;
	const uint32_t pid = 1; // Chrome-trace pid is a logical grouping; anything nonzero works.

	out << "{\"traceEvents\":[\n";
	std::string buf;
	bool first = true;

	// Metadata events to label each virtual tid with its module name.
	// We must buffer these until we know all module names, so emit after
	// the main event loop.
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
			// Chrome-trace "sample" point event; frames are carried in args.
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

} // namespace nautilus::profile

extern "C" {

// Default entry points — read the cycle counter internally. Used by the
// interpreter and by the CPP/BC backends that call invoke() directly.

void __nautilus_profile_begin(const char* name) {
	const uint64_t ticks = nautilus::profile::detail::sessionTicks();
	nautilus::profile::detail::record(
	    nautilus::profile::detail::makeEvent(nautilus::profile::Event::Kind::Begin, name, 0, ticks));
}

void __nautilus_profile_end(const char* name) {
	const uint64_t ticks = nautilus::profile::detail::sessionTicks();
	nautilus::profile::detail::record(
	    nautilus::profile::detail::makeEvent(nautilus::profile::Event::Kind::End, name, 0, ticks));
}

void __nautilus_profile_counter_i64(const char* name, int64_t value) {
	const uint64_t ticks = nautilus::profile::detail::sessionTicks();
	nautilus::profile::detail::record(
	    nautilus::profile::detail::makeEvent(nautilus::profile::Event::Kind::CounterI64, name, value, ticks));
}

// Pre-timed entry points — the caller (currently only the MLIR intrinsic
// lowering) emits llvm.readcyclecounter() inline and passes the raw tick
// value. Saves one function call and the cost of reading the counter
// through the plain entry point.

void __nautilus_profile_begin_ticks(const char* name, uint64_t ticks) {
	nautilus::profile::detail::record(nautilus::profile::detail::makeEvent(
	    nautilus::profile::Event::Kind::Begin, name, 0, ticks - nautilus::profile::detail::timebase().baseline));
}

void __nautilus_profile_end_ticks(const char* name, uint64_t ticks) {
	nautilus::profile::detail::record(nautilus::profile::detail::makeEvent(
	    nautilus::profile::Event::Kind::End, name, 0, ticks - nautilus::profile::detail::timebase().baseline));
}

void __nautilus_profile_counter_i64_ticks(const char* name, int64_t value, uint64_t ticks) {
	nautilus::profile::detail::record(
	    nautilus::profile::detail::makeEvent(nautilus::profile::Event::Kind::CounterI64, name, value,
	                                         ticks - nautilus::profile::detail::timebase().baseline));
}

} // extern "C"
