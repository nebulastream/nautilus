#include "nautilus/profile/profile_runtime.h"
#include "nautilus/logging.hpp"
#include "nautilus/profile/Recorder.hpp"
#include <mutex>
#include <vector>

namespace nautilus::profile::detail {

// Step 1 stub: a process-global, mutex-protected event buffer.
// Replaced by a Perfetto session in step 3. The global is intentional so
// the C entry points below don't need to carry any state.
static std::mutex& recorderMutex() {
	static std::mutex m;
	return m;
}

static std::vector<Event>& recorderBuffer() {
	static std::vector<Event> buf;
	return buf;
}

static void record(Event ev) {
	std::lock_guard<std::mutex> lock(recorderMutex());
	recorderBuffer().push_back(std::move(ev));
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
}

} // namespace nautilus::profile

extern "C" {

void __nautilus_profile_begin(const char* name) {
	const char* safe = name != nullptr ? name : "<null>";
	nautilus::log::debug("profile.begin {}", safe);
	nautilus::profile::detail::record({nautilus::profile::Event::Kind::Begin, safe, 0});
}

void __nautilus_profile_end(const char* name) {
	const char* safe = name != nullptr ? name : "<null>";
	nautilus::log::debug("profile.end {}", safe);
	nautilus::profile::detail::record({nautilus::profile::Event::Kind::End, safe, 0});
}

void __nautilus_profile_counter_i64(const char* name, int64_t value) {
	const char* safe = name != nullptr ? name : "<null>";
	nautilus::log::debug("profile.counter {} {}", safe, value);
	nautilus::profile::detail::record({nautilus::profile::Event::Kind::CounterI64, safe, value});
}

} // extern "C"
