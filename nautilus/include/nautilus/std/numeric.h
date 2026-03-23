#pragma once

#include "nautilus/function.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <numeric>

namespace nautilus {

// accumulate: sums elements in [first, first + count) starting from init
template <typename T>
    requires std::is_arithmetic_v<T>
val<T> accumulate(val<const T*> first, val<size_t> count, val<T> init) {
	return invoke(
	    +[](const T* first, size_t count, T init) -> T { return std::accumulate(first, first + count, init); }, first,
	    count, init);
}

// inner_product: computes inner product of two ranges
template <typename T>
    requires std::is_arithmetic_v<T>
val<T> inner_product(val<const T*> first1, val<size_t> count, val<const T*> first2, val<T> init) {
	return invoke(
	    +[](const T* first1, size_t count, const T* first2, T init) -> T {
		    return std::inner_product(first1, first1 + count, first2, init);
	    },
	    first1, count, first2, init);
}

// iota: fills range [first, first + count) with sequentially increasing values starting from value
template <typename T>
    requires std::is_arithmetic_v<T>
void iota(val<T*> first, val<size_t> count, val<T> value) {
	invoke(+[](T* first, size_t count, T value) -> void { std::iota(first, first + count, value); }, first, count,
	       value);
}

// partial_sum: computes partial sums of [first, first + count) into d_first
template <typename T>
    requires std::is_arithmetic_v<T>
val<T*> partial_sum(val<const T*> first, val<size_t> count, val<T*> d_first) {
	return invoke(
	    +[](const T* first, size_t count, T* d_first) -> T* {
		    return std::partial_sum(first, first + count, d_first);
	    },
	    first, count, d_first);
}

// adjacent_difference: computes differences between adjacent elements
template <typename T>
    requires std::is_arithmetic_v<T>
val<T*> adjacent_difference(val<const T*> first, val<size_t> count, val<T*> d_first) {
	return invoke(
	    +[](const T* first, size_t count, T* d_first) -> T* {
		    return std::adjacent_difference(first, first + count, d_first);
	    },
	    first, count, d_first);
}

} // namespace nautilus
