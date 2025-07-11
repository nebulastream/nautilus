#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>

namespace nautilus {

enum Color { BLUE, GREEN };
enum class LogLevel : uint8_t {
	LOG_NONE = 1,
	LOG_FATAL_ERROR = 2,
	LOG_ERROR = 3,
	LOG_WARNING = 4,
	LOG_INFO = 5,
	LOG_DEBUG = 6,
	LOG_TRACE = 7
};

val<bool> handleEnum(val<Color> enumVal) {
	return enumVal == Color::BLUE;
}

val<bool> handleEnumLogLevel(val<LogLevel> enumVal) {
	return enumVal == LogLevel::LOG_DEBUG || enumVal == LogLevel::LOG_INFO;
}

val<int32_t> isEnum(val<Color> enumVal) {
	if (enumVal == Color::BLUE) {
		return 1;
	} else if (enumVal == GREEN) {
		return 2;
	} else {
		return 42;
	}
}

val<Color> getEnum() {
	return Color::BLUE;
}

int32_t enumFunction(Color value) {
	switch (value) {
	case BLUE:
		return 42;
	default:
		return 1;
	}
}

auto enumClassFunction(LogLevel level) {
	switch (level) {
	case LogLevel::LOG_NONE:
		return 1;
	case LogLevel::LOG_FATAL_ERROR:
		return 2;
	case LogLevel::LOG_ERROR:
		return 3;
	case LogLevel::LOG_WARNING:
		return 4;
	case LogLevel::LOG_INFO:
		return 5;
	case LogLevel::LOG_DEBUG:
		return 6;
	case LogLevel::LOG_TRACE:
		return 7;
	}

	return 43;
}

val<int32_t> callEnumFunction(val<Color> enumVal) {
	return invoke(enumFunction, enumVal);
}

val<int32_t> callEnumClassFunction(val<LogLevel> enumClassVal) {
	return invoke(enumClassFunction, enumClassVal);
}
} // namespace nautilus
