#pragma once

#include <nautilus/Engine.hpp>

namespace nautilus {

enum Color { BLUE, GREEN };

val<bool> handleEnum(val<Color> enumVal) {
	return enumVal == Color::BLUE;
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

val<int32_t> callEnumFunction(val<Color> enumVal) {
	return invoke(enumFunction, enumVal);
}
} // namespace nautilus