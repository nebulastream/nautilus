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

namespace nautilus::profile::detail {

static std::mutex& recorderMutex() {
	static std::mutex m;
	return m;
}

static std::vector<Event>& recorderBuffer() {
	static std::vector<Event> buf;
	return buf;
}

// Session start time; every event ts is (now - sessionStart) in microseconds.
// Reset by clearRecordedEvents().
static std::atomic<std::chrono::steady_clock::time_point>& sessionStart() {
	static std::atomic<std::chrono::steady_clock::time_point> t {std::chrono::steady_clock::now()};
	return t;
}

static uint64_t nowMicros() {
	using namespace std::chrono;
	auto start = sessionStart().load(std::memory_order_relaxed);
	return duration_cast<microseconds>(steady_clock::now() - start).count();
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

static Event makeEvent(Event::Kind kind, const char* name, int64_t value) {
	Event ev;
	ev.kind = kind;
	ev.name = name != nullptr ? name : "<null>";
	ev.value = value;
	ev.timestamp_us = nowMicros();
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
	detail::sessionStart().store(std::chrono::steady_clock::now(), std::memory_order_relaxed);
}

// Bridge used by the sampler consumer. Defined here so the recorder state
// (mutex + buffer) stays encapsulated in this TU.
void recordSampleEvent(Event ev) {
	std::lock_guard<std::mutex> lock(detail::recorderMutex());
	detail::recorderBuffer().push_back(std::move(ev));
}

// Exposes the monotonic session clock so the sampler (a separate TU) can
// stamp samples in the same timebase as regions. Signal-safe: a relaxed
// atomic load plus one steady_clock::now() call.
uint64_t sessionNowMicros() {
	return detail::nowMicros();
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
		buf.append(std::to_string(ev.timestamp_us));

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

void __nautilus_profile_begin(const char* name) {
	nautilus::log::debug("profile.begin {}", name != nullptr ? name : "<null>");
	nautilus::profile::detail::record(
	    nautilus::profile::detail::makeEvent(nautilus::profile::Event::Kind::Begin, name, 0));
}

void __nautilus_profile_end(const char* name) {
	nautilus::log::debug("profile.end {}", name != nullptr ? name : "<null>");
	nautilus::profile::detail::record(
	    nautilus::profile::detail::makeEvent(nautilus::profile::Event::Kind::End, name, 0));
}

void __nautilus_profile_counter_i64(const char* name, int64_t value) {
	nautilus::log::debug("profile.counter {} {}", name != nullptr ? name : "<null>", value);
	nautilus::profile::detail::record(
	    nautilus::profile::detail::makeEvent(nautilus::profile::Event::Kind::CounterI64, name, value));
}

} // extern "C"
