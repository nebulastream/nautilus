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

class CompilationStatsLogger {
public:
	CompilationStatsLogger(bool enabled, const std::string& compilationId)
	    : enabled_(enabled), compilationId_(compilationId) {
	}

	template <typename... Args>
	void logTiming([[maybe_unused]] std::chrono::steady_clock::time_point start, [[maybe_unused]] const char* fmt,
	               [[maybe_unused]] Args&&... args) const {
#ifdef ENABLE_LOGGING
		if (enabled_ && spdlog::should_log(spdlog::level::info)) {
			auto end = std::chrono::steady_clock::now();
			auto ms = std::chrono::duration<double, std::milli>(end - start).count();
			auto msg = fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...);
			spdlog::info("[nautilus] [{}] {} in {:.2f} ms", compilationId_, msg, ms);
		}
#endif
	}

private:
	[[maybe_unused]] bool enabled_;
	[[maybe_unused]] const std::string& compilationId_;
};

namespace options {

bool getLogAddresses();
void setLogAddresses(bool);

bool getLogStaticVars();
void setLogStaticVars(bool);

} // namespace options
} // namespace nautilus::log
