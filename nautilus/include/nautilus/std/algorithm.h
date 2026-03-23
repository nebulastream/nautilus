#pragma once

#include "nautilus/function.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <algorithm>

namespace nautilus {

// --- min / max / clamp ---

template <typename T>
    requires std::is_arithmetic_v<T>
val<T> min(val<T> a, val<T> b) {
	return invoke(+[](T a, T b) -> T { return std::min(a, b); }, a, b);
}

template <typename T>
    requires std::is_arithmetic_v<T>
val<T> max(val<T> a, val<T> b) {
	return invoke(+[](T a, T b) -> T { return std::max(a, b); }, a, b);
}

template <typename T>
    requires std::is_arithmetic_v<T>
val<T> clamp(val<T> v, val<T> lo, val<T> hi) {
	return invoke(+[](T v, T lo, T hi) -> T { return std::clamp(v, lo, hi); }, v, lo, hi);
}

// --- swap ---

template <typename T>
    requires std::is_arithmetic_v<T>
void swap(val<T>& a, val<T>& b) {
	auto tmp = a;
	a = b;
	b = tmp;
}

// --- Pointer-range algorithms for buffer operations ---

// copy: copies elements from [src, src + count) to dst
template <typename T>
    requires std::is_arithmetic_v<T>
val<T*> copy(val<const T*> src, val<size_t> count, val<T*> dst) {
	return invoke(
	    +[](const T* src, size_t count, T* dst) -> T* { return std::copy(src, src + count, dst); }, src, count, dst);
}

// fill: fills [dst, dst + count) with value
template <typename T>
    requires std::is_arithmetic_v<T>
void fill(val<T*> dst, val<size_t> count, val<T> value) {
	invoke(+[](T* dst, size_t count, T value) -> void { std::fill(dst, dst + count, value); }, dst, count, value);
}

// find: returns pointer to first element equal to value, or first + count if not found
template <typename T>
    requires std::is_arithmetic_v<T>
val<const T*> find(val<const T*> first, val<size_t> count, val<T> value) {
	return invoke(
	    +[](const T* first, size_t count, T value) -> const T* { return std::find(first, first + count, value); },
	    first, count, value);
}

// count: counts elements equal to value in [first, first + count)
template <typename T>
    requires std::is_arithmetic_v<T>
val<size_t> count(val<const T*> first, val<size_t> n, val<T> value) {
	return invoke(
	    +[](const T* first, size_t n, T value) -> size_t {
		    return static_cast<size_t>(std::count(first, first + n, value));
	    },
	    first, n, value);
}

// equal: compares two ranges for equality
template <typename T>
    requires std::is_arithmetic_v<T>
val<bool> equal(val<const T*> first1, val<size_t> count, val<const T*> first2) {
	return invoke(
	    +[](const T* first1, size_t count, const T* first2) -> bool {
		    return std::equal(first1, first1 + count, first2);
	    },
	    first1, count, first2);
}

// reverse: reverses elements in [first, first + count)
template <typename T>
    requires std::is_arithmetic_v<T>
void reverse(val<T*> first, val<size_t> count) {
	invoke(+[](T* first, size_t count) -> void { std::reverse(first, first + count); }, first, count);
}

// sort: sorts elements in [first, first + count)
template <typename T>
    requires std::is_arithmetic_v<T>
void sort(val<T*> first, val<size_t> count) {
	invoke(+[](T* first, size_t count) -> void { std::sort(first, first + count); }, first, count);
}

// lower_bound: returns pointer to first element not less than value
template <typename T>
    requires std::is_arithmetic_v<T>
val<const T*> lower_bound(val<const T*> first, val<size_t> count, val<T> value) {
	return invoke(
	    +[](const T* first, size_t count, T value) -> const T* {
		    return std::lower_bound(first, first + count, value);
	    },
	    first, count, value);
}

// upper_bound: returns pointer to first element greater than value
template <typename T>
    requires std::is_arithmetic_v<T>
val<const T*> upper_bound(val<const T*> first, val<size_t> count, val<T> value) {
	return invoke(
	    +[](const T* first, size_t count, T value) -> const T* {
		    return std::upper_bound(first, first + count, value);
	    },
	    first, count, value);
}

// binary_search: checks if value exists in sorted range
template <typename T>
    requires std::is_arithmetic_v<T>
val<bool> binary_search(val<const T*> first, val<size_t> count, val<T> value) {
	return invoke(
	    +[](const T* first, size_t count, T value) -> bool {
		    return std::binary_search(first, first + count, value);
	    },
	    first, count, value);
}

// min_element: returns pointer to the smallest element
template <typename T>
    requires std::is_arithmetic_v<T>
val<const T*> min_element(val<const T*> first, val<size_t> count) {
	return invoke(
	    +[](const T* first, size_t count) -> const T* { return std::min_element(first, first + count); }, first, count);
}

// max_element: returns pointer to the largest element
template <typename T>
    requires std::is_arithmetic_v<T>
val<const T*> max_element(val<const T*> first, val<size_t> count) {
	return invoke(
	    +[](const T* first, size_t count) -> const T* { return std::max_element(first, first + count); }, first, count);
}

} // namespace nautilus
