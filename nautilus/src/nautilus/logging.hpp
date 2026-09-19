#pragma once

#include <chrono>
#include <nautilus/config.hpp>
#include <string>
#ifdef ENABLE_LOGGING
#include <spdlog/spdlog.h>
#endif

namespace nautilus::log {
template <typename... Args>
void info([[maybe_unused]] const char* fmt, [[maybe_unused]] Args&&... args) {
#ifdef ENABLE_LOGGING
	spdlog::info("{}", fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...));
#endif
}

template <typename... Args>
void debug([[maybe_unused]] const char* fmt, [[maybe_unused]] Args&&... args) {
#ifdef ENABLE_LOGGING
	if (spdlog::should_log(spdlog::level::debug)) {
		spdlog::debug("{}", fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...));
	}
#endif
}

template <typename... Args>
void trace([[maybe_unused]] const char* fmt, [[maybe_unused]] Args&&... args) {
#ifdef ENABLE_LOGGING
	if (spdlog::should_log(spdlog::level::trace)) {
		spdlog::trace("{}", fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...));
	}
#endif
}

template <typename... Args>
void warn([[maybe_unused]] const char* fmt, [[maybe_unused]] Args&&... args) {
#ifdef ENABLE_LOGGING
	spdlog::warn("{}", fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...));
#endif
}

template <typename... Args>
void error([[maybe_unused]] const char* fmt, [[maybe_unused]] Args&&... args) {
#ifdef ENABLE_LOGGING
	spdlog::error("{}", fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...));
#endif
}

inline std::chrono::steady_clock::time_point now() {
#ifdef ENABLE_LOGGING
	return std::chrono::steady_clock::now();
#else
	return {};
#endif
}

namespace options {

bool getLogAddresses();
void setLogAddresses(bool);

bool getLogStaticVars();
void setLogStaticVars(bool);

/// Whether a dump spells out the source location a region() call site was written at
/// (docs/region.md). On by default: the location is what relates traced code back to the
/// source. Turn it off for a dump that has to be stable across machines and compilers --
/// the path is wherever the build compiled from, and the column is the compiler's own
/// choice (for one and the same call GCC reports the callee's closing position and Clang
/// the start of the expression). The region's name and, in the IR, its id are printed
/// either way, so a dump without locations still says which region each block and
/// operation belongs to.
bool getLogSourceLocations();
void setLogSourceLocations(bool);

} // namespace options
} // namespace nautilus::log
