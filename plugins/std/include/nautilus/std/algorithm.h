#pragma once

#include <algorithm>
#include <functional>
#include <nautilus/function.hpp>
#include <nautilus/val_ptr.hpp>
#include <type_traits>

namespace nautilus {

/*
 * Traced wrappers for the sorting and selection algorithms of <algorithm>.
 *
 * Ranges are passed as `val<T*>` pointers (i.e., contiguous iterators). Every
 * wrapper forwards to the corresponding std:: algorithm through `invoke(...)`,
 * so the whole algorithm executes as a single runtime call in the generated
 * code.
 *
 * A custom ordering can be supplied as the `Compare` template argument. Since
 * the comparator is instantiated inside the runtime function, it must be a
 * stateless, default-constructible function object type, e.g.
 * `std::greater<>` or a user-defined struct with a `bool operator()`.
 *
 *   nautilus::sort<std::greater<>>(first, last);
 *
 * The runtime functions are annotated as inlining candidates for the
 * `nautilus-inlining` plugin. If a translation unit that uses these wrappers
 * is compiled with `nautilus_inline(<target>)`, includes
 * `<nautilus/inline.hpp>` and links `nautilus-inlining`, the MLIR backend
 * inlines the algorithm into the generated code when
 * `mlir.inline_invoke_calls` is enabled. Otherwise, it emits a regular call.
 */

// Marks a function as an inlining candidate for the `nautilus-inlining` plugin
// without making the std plugin depend on it. The annotation string must match
// `NAUTILUS_INLINE` in `plugins/inlining/include/nautilus/inline.hpp`.
#if defined(__clang__)
#define NAUTILUS_STD_INLINE __attribute__((annotate("nautilus_inline_v0001")))
#else
#define NAUTILUS_STD_INLINE
#endif

namespace detail {

template <typename Compare>
concept stateless_comparator = std::is_empty_v<Compare> && std::is_default_constructible_v<Compare>;

/// True for the standard ordering function objects (`std::less<>`,
/// `std::greater<int>`, ...). Their typed specializations are not declared
/// `noexcept`, although comparing arithmetic values or pointers never throws.
template <typename Compare>
inline constexpr bool is_std_ordering_v = false;
template <typename U>
inline constexpr bool is_std_ordering_v<std::less<U>> = true;
template <typename U>
inline constexpr bool is_std_ordering_v<std::greater<U>> = true;
template <typename U>
inline constexpr bool is_std_ordering_v<std::less_equal<U>> = true;
template <typename U>
inline constexpr bool is_std_ordering_v<std::greater_equal<U>> = true;

template <typename T, typename Compare>
inline constexpr bool nothrow_compare_v =
    std::is_nothrow_invocable_r_v<bool, Compare&, T&, T&> ||
    ((std::is_arithmetic_v<T> || std::is_pointer_v<T>) && is_std_ordering_v<Compare>);

/// The runtime functions are `noexcept` (and the call gets the `nounwind`
/// attribute) only if neither the comparator nor moving elements can throw.
/// The algorithms themselves do not throw: `std::stable_sort` falls back to an
/// in-place merge sort if it cannot allocate a temporary buffer.
template <typename T, typename Compare>
inline constexpr bool nothrow_ordering_v =
    nothrow_compare_v<T, Compare> && std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>;

// Runtime functions called through `invoke(...)`. They are named functions
// rather than lambdas so the inlining plugin can pick them up by annotation.

template <typename T, typename Compare>
NAUTILUS_STD_INLINE void sort_impl(T* first, T* last) noexcept(nothrow_ordering_v<T, Compare>) {
	std::sort(first, last, Compare {});
}

template <typename T, typename Compare>
NAUTILUS_STD_INLINE void stable_sort_impl(T* first, T* last) noexcept(nothrow_ordering_v<T, Compare>) {
	std::stable_sort(first, last, Compare {});
}

template <typename T, typename Compare>
NAUTILUS_STD_INLINE void partial_sort_impl(T* first, T* middle, T* last) noexcept(nothrow_ordering_v<T, Compare>) {
	std::partial_sort(first, middle, last, Compare {});
}

template <typename T, typename Compare>
NAUTILUS_STD_INLINE T* partial_sort_copy_impl(T* first, T* last, T* d_first,
                                              T* d_last) noexcept(nothrow_ordering_v<T, Compare> &&
                                                                  std::is_nothrow_copy_assignable_v<T>) {
	return std::partial_sort_copy(first, last, d_first, d_last, Compare {});
}

template <typename T, typename Compare>
NAUTILUS_STD_INLINE void nth_element_impl(T* first, T* nth, T* last) noexcept(nothrow_ordering_v<T, Compare>) {
	std::nth_element(first, nth, last, Compare {});
}

template <typename T, typename Compare>
NAUTILUS_STD_INLINE bool is_sorted_impl(T* first, T* last) noexcept(nothrow_compare_v<T, Compare>) {
	return std::is_sorted(first, last, Compare {});
}

template <typename T, typename Compare>
NAUTILUS_STD_INLINE T* is_sorted_until_impl(T* first, T* last) noexcept(nothrow_compare_v<T, Compare>) {
	return std::is_sorted_until(first, last, Compare {});
}

template <typename T, typename Compare>
NAUTILUS_STD_INLINE T* min_element_impl(T* first, T* last) noexcept(nothrow_compare_v<T, Compare>) {
	return std::min_element(first, last, Compare {});
}

template <typename T, typename Compare>
NAUTILUS_STD_INLINE T* max_element_impl(T* first, T* last) noexcept(nothrow_compare_v<T, Compare>) {
	return std::max_element(first, last, Compare {});
}

} // namespace detail

/**
 * Sorts the elements in the range `[first, last)` in non-descending order.
 * The order of equal elements is not guaranteed to be preserved.
 */
template <typename Compare = std::less<>, typename T>
    requires detail::stateless_comparator<Compare>
void sort(val<T*> first, val<T*> last) {
	invoke(detail::sort_impl<T, Compare>, first, last);
}

/**
 * Sorts the elements in the range `[first, last)` in non-descending order.
 * The order of equal elements is guaranteed to be preserved.
 */
template <typename Compare = std::less<>, typename T>
    requires detail::stateless_comparator<Compare>
void stable_sort(val<T*> first, val<T*> last) {
	invoke(detail::stable_sort_impl<T, Compare>, first, last);
}

/**
 * Rearranges the elements such that `[first, middle)` contains the sorted
 * `middle - first` smallest elements of `[first, last)`. The order of the
 * remaining elements in `[middle, last)` is unspecified.
 */
template <typename Compare = std::less<>, typename T>
    requires detail::stateless_comparator<Compare>
void partial_sort(val<T*> first, val<T*> middle, val<T*> last) {
	invoke(detail::partial_sort_impl<T, Compare>, first, middle, last);
}

/**
 * Sorts some of the elements of `[first, last)` and copies them to
 * `[d_first, d_last)`. At most `d_last - d_first` elements are copied.
 *
 * @return A pointer to the element past the last element written to the
 * destination range.
 */
template <typename Compare = std::less<>, typename T>
    requires detail::stateless_comparator<Compare>
val<T*> partial_sort_copy(val<T*> first, val<T*> last, val<T*> d_first, val<T*> d_last) {
	return invoke(detail::partial_sort_copy_impl<T, Compare>, first, last, d_first, d_last);
}

/**
 * Partially sorts the range `[first, last)` such that the element pointed to
 * by `nth` is the element that would occur at that position if the range was
 * fully sorted. All elements before `nth` are less than or equal to the
 * elements after `nth`.
 */
template <typename Compare = std::less<>, typename T>
    requires detail::stateless_comparator<Compare>
void nth_element(val<T*> first, val<T*> nth, val<T*> last) {
	invoke(detail::nth_element_impl<T, Compare>, first, nth, last);
}

/**
 * Checks whether the elements in the range `[first, last)` are sorted in
 * non-descending order.
 */
template <typename Compare = std::less<>, typename T>
    requires detail::stateless_comparator<Compare>
val<bool> is_sorted(val<T*> first, val<T*> last) {
	return invoke(detail::is_sorted_impl<T, Compare>, first, last);
}

/**
 * Finds the largest range beginning at `first` in which the elements are
 * sorted in non-descending order.
 *
 * @return A pointer to the upper bound of that range.
 */
template <typename Compare = std::less<>, typename T>
    requires detail::stateless_comparator<Compare>
val<T*> is_sorted_until(val<T*> first, val<T*> last) {
	return invoke(detail::is_sorted_until_impl<T, Compare>, first, last);
}

/**
 * Finds the smallest element in the range `[first, last)`.
 *
 * @return A pointer to the (first) smallest element, or `last` if the range is
 * empty.
 */
template <typename Compare = std::less<>, typename T>
    requires detail::stateless_comparator<Compare>
val<T*> min_element(val<T*> first, val<T*> last) {
	return invoke(detail::min_element_impl<T, Compare>, first, last);
}

/**
 * Finds the largest element in the range `[first, last)`.
 *
 * @return A pointer to the (first) largest element, or `last` if the range is
 * empty.
 */
template <typename Compare = std::less<>, typename T>
    requires detail::stateless_comparator<Compare>
val<T*> max_element(val<T*> first, val<T*> last) {
	return invoke(detail::max_element_impl<T, Compare>, first, last);
}

} // namespace nautilus
