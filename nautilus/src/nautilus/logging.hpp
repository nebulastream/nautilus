#pragma once

#include <spdlog/spdlog.h>

namespace nautilus::log {
template <typename... Args>
void info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
	spdlog::info(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void debug(spdlog::format_string_t<Args...> fmt, Args&&... args) {
	spdlog::debug(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void trace(spdlog::format_string_t<Args...> fmt, Args&&... args) {
	spdlog::trace(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
	spdlog::debug(fmt, std::forward<Args>(args)...);
}
} // namespace nautilus::log
