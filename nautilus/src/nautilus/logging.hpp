#pragma once

#include <nautilus/config.hpp>
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
	spdlog::debug("{}", fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...));
#endif
}

template <typename... Args>
void trace([[maybe_unused]] const char* fmt, [[maybe_unused]] Args&&... args) {
#ifdef ENABLE_LOGGING
	spdlog::trace("{}", fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...));
#endif
}

template <typename... Args>
void error([[maybe_unused]] const char* fmt, [[maybe_unused]] Args&&... args) {
#ifdef ENABLE_LOGGING
	spdlog::error("{}", fmt::format(fmt::runtime(fmt), std::forward<Args>(args)...));
#endif
}

namespace options {

bool getLogAddresses();
void setLogAddresses(bool);

} // namespace options
} // namespace nautilus::log
