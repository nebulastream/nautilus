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
 */

namespace detail {

template <typename Compare>
concept stateless_comparator = std::is_empty_v<Compare> && std::is_default_constructible_v<Compare>;

/// The runtime call is marked `noexcept` (and thus gets the `nounwind`
/// attribute) only if neither the comparator nor moving elements can throw.
template <typename T, typename Compare>
inline constexpr bool nothrow_ordering_v =
    std::is_nothrow_invocable_r_v<bool, Compare&, T&, T&> && std::is_nothrow_move_constructible_v<T> &&
    std::is_nothrow_move_assignable_v<T>;

} // namespace detail

/**
 * Sorts the elements in the range `[first, last)` in non-descending order.
 * The order of equal elements is not guaranteed to be preserved.
 */
template <typename Compare = std::less<>, typename T>
    requires detail::stateless_comparator<Compare>
void sort(val<T*> first, val<T*> last) {
	constexpr bool is_noexcept = detail::nothrow_ordering_v<T, Compare>;
	invoke(+[](T* f, T* l) noexcept(is_noexcept) -> void { std::sort(f, l, Compare {}); }, first, last);
}

/**
 * Sorts the elements in the range `[first, last)` in non-descending order.
 * The order of equal elements is guaranteed to be preserved.
 */
template <typename Compare = std::less<>, typename T>
    requires detail::stateless_comparator<Compare>
void stable_sort(val<T*> first, val<T*> last) {
	// std::stable_sort may allocate a temporary buffer but falls back to an
	// in-place merge sort if the allocation fails, so it does not throw by itself.
	constexpr bool is_noexcept = detail::nothrow_ordering_v<T, Compare>;
	invoke(+[](T* f, T* l) noexcept(is_noexcept) -> void { std::stable_sort(f, l, Compare {}); }, first, last);
}

/**
 * Rearranges the elements such that `[first, middle)` contains the sorted
 * `middle - first` smallest elements of `[first, last)`. The order of the
 * remaining elements in `[middle, last)` is unspecified.
 */
template <typename Compare = std::less<>, typename T>
    requires detail::stateless_comparator<Compare>
void partial_sort(val<T*> first, val<T*> middle, val<T*> last) {
	constexpr bool is_noexcept = detail::nothrow_ordering_v<T, Compare>;
	invoke(
	    +[](T* f, T* m, T* l) noexcept(is_noexcept) -> void { std::partial_sort(f, m, l, Compare {}); }, first, middle,
	    last);
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
	constexpr bool is_noexcept = detail::nothrow_ordering_v<T, Compare> && std::is_nothrow_copy_assignable_v<T>;
	return invoke(
	    +[](T* f, T* l, T* df, T* dl) noexcept(is_noexcept) -> T* {
		    return std::partial_sort_copy(f, l, df, dl, Compare {});
	    },
	    first, last, d_first, d_last);
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
	constexpr bool is_noexcept = detail::nothrow_ordering_v<T, Compare>;
	invoke(
	    +[](T* f, T* n, T* l) noexcept(is_noexcept) -> void { std::nth_element(f, n, l, Compare {}); }, first, nth,
	    last);
}

/**
 * Checks whether the elements in the range `[first, last)` are sorted in
 * non-descending order.
 */
template <typename Compare = std::less<>, typename T>
    requires detail::stateless_comparator<Compare>
val<bool> is_sorted(val<T*> first, val<T*> last) {
	constexpr bool is_noexcept = detail::nothrow_ordering_v<T, Compare>;
	return invoke(
	    +[](T* f, T* l) noexcept(is_noexcept) -> bool { return std::is_sorted(f, l, Compare {}); }, first, last);
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
	constexpr bool is_noexcept = detail::nothrow_ordering_v<T, Compare>;
	return invoke(
	    +[](T* f, T* l) noexcept(is_noexcept) -> T* { return std::is_sorted_until(f, l, Compare {}); }, first, last);
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
	constexpr bool is_noexcept = detail::nothrow_ordering_v<T, Compare>;
	return invoke(
	    +[](T* f, T* l) noexcept(is_noexcept) -> T* { return std::min_element(f, l, Compare {}); }, first, last);
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
	constexpr bool is_noexcept = detail::nothrow_ordering_v<T, Compare>;
	return invoke(
	    +[](T* f, T* l) noexcept(is_noexcept) -> T* { return std::max_element(f, l, Compare {}); }, first, last);
}

} // namespace nautilus
