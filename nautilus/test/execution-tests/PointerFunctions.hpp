#pragma once

#include <nautilus/Engine.hpp>

namespace nautilus::engine {

val<int32_t> load(val<int32_t*> array, val<int32_t> index) {
	val<int32_t> value = array[index];
	return value;
}

val<int32_t> sumArray(val<int32_t*> array, val<int32_t> length) {
	val<int32_t> sum = val<int32_t>(0);
	for (val<int32_t> i = 0; i < length; i = i + 1) {
		val<int32_t> value = array[i];
		sum = sum + value;
	}
	return sum;
}

template <typename T>
void addArray(val<T*> array, val<T*> array2, val<T> length) {
	for (val<T> i = 0; i < length; i = i + 1) {
		val<T> left = array[i];
		val<T> right = array2[i];
		array[i] = left + right;
	}
}
} // namespace nautilus::engine
