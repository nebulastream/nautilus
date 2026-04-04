#pragma once

#include <cstddef>
#include <nautilus/val.hpp>
#include <type_traits>

namespace nautilus {

/// Determines the preferred vector width (number of lanes) for type T based on hardware.
template <typename T>
constexpr size_t preferred_vector_width() {
	static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");
#if defined(__AVX512F__)
	return 64 / sizeof(T); // 512-bit
#elif defined(__AVX2__) || defined(__AVX__)
	return 32 / sizeof(T); // 256-bit
#elif defined(__SSE2__) || defined(__ARM_NEON)
	return 16 / sizeof(T); // 128-bit
#else
	return 16 / sizeof(T); // fallback to 128-bit
#endif
}

/// Fixed-size aligned SIMD storage (internal).
template <typename T, size_t N>
struct alignas(sizeof(T) * N) vector_data {
	static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");
	static_assert((N & (N - 1)) == 0, "N must be a power of 2");
	static_assert(N >= 2, "N must be at least 2");
	T data[N];
};

/// Type tag representing a SIMD vector of type T.
/// The lane count is determined automatically from hardware capabilities.
/// Use as val<vec<float>>, val<vec<int32_t>>, etc.
template <typename T>
struct vec {
	static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");
	using element_type = T;
	static constexpr size_t lanes = preferred_vector_width<T>();
};

// ============================================================================
// detail — N-parameterized free functions specialized in vector.cpp.
// These are the traced entry points that backends can intercept.
// The val<vec<T>> class calls these with N = vec<T>::lanes.
// ============================================================================

namespace detail {

template <typename T, size_t N>
val<vector_data<T, N>*> vec_load(val<const T*> ptr);
template <typename T, size_t N>
void vec_store(val<T*> ptr, val<vector_data<T, N>*> v);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_add(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_sub(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_mul(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_div(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_abs(val<vector_data<T, N>*> a);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_neg(val<vector_data<T, N>*> a);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_min(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_max(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_fma(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b, val<vector_data<T, N>*> c);
template <typename T, size_t N>
val<T> vec_reduce_add(val<vector_data<T, N>*> a);
template <typename T, size_t N>
val<T> vec_reduce_min(val<vector_data<T, N>*> a);
template <typename T, size_t N>
val<T> vec_reduce_max(val<vector_data<T, N>*> a);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_eq(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_ne(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_lt(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_le(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_gt(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_ge(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_blend(val<vector_data<T, N>*> mask, val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_and(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_or(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_xor(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_broadcast(val<T> scalar);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_gather(val<const T*> base, val<const int32_t*> indices);
template <typename T, size_t N>
void vec_scatter(val<T*> base, val<const int32_t*> indices, val<vector_data<T, N>*> data);
template <typename T, size_t N>
val<T> vec_extract(val<vector_data<T, N>*> v, val<int32_t> idx);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_insert(val<vector_data<T, N>*> v, val<T> value, val<int32_t> idx);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_shl(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);
template <typename T, size_t N>
val<vector_data<T, N>*> vec_shr(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b);

} // namespace detail

// ============================================================================
// val<vec<T>> — SIMD vector specialization of val<T>.
//
// The lane count is fixed per element type based on hardware capabilities.
// Integrates with the Nautilus tracing and compilation pipeline.
//
// Usage:
//   auto v = val<vec<float>>::Load(ptr);
//   auto w = val<vec<float>>::Load(ptr2);
//   auto r = v + w;
//   r.Store(out);
//   val<float> sum = r.ReduceAdd();
// ============================================================================

template <typename T>
class val<vec<T>> {
	static constexpr size_t N = vec<T>::lanes;
	using data_type = vector_data<T, N>;
	using ptr_type = val<data_type*>;

public:
	using raw_type = vec<T>;
	using element_type = T;
	static constexpr size_t lanes = N;

	explicit val(ptr_type ptr) : ptr_(ptr) {
	}

	/// Access internal data pointer (for advanced use).
	ptr_type Data() const {
		return ptr_;
	}

	// -- Load / Store --------------------------------------------------------

	/// Load a vector from contiguous memory.
	static val Load(val<const T*> ptr) {
		return val(detail::vec_load<T, N>(ptr));
	}

	/// Store this vector to contiguous memory.
	void Store(val<T*> ptr) const {
		detail::vec_store<T, N>(ptr, ptr_);
	}

	/// Broadcast a scalar value to all lanes.
	static val Broadcast(val<T> scalar) {
		return val(detail::vec_broadcast<T, N>(scalar));
	}

	/// Gather: load elements from non-contiguous memory locations.
	/// Loads base[indices[0]], base[indices[1]], ..., base[indices[N-1]].
	template <typename IdxT = int32_t>
	static val Gather(val<const T*> base, val<const IdxT*> indices) {
		static_assert(std::is_same_v<IdxT, int32_t>, "Index type must be int32_t");
		return val(detail::vec_gather<T, N>(base, indices));
	}

	/// Scatter: store elements to non-contiguous memory locations.
	/// Stores lane i to base[indices[i]].
	template <typename IdxT = int32_t>
	void Scatter(val<T*> base, val<const IdxT*> indices) const {
		static_assert(std::is_same_v<IdxT, int32_t>, "Index type must be int32_t");
		detail::vec_scatter<T, N>(base, indices, ptr_);
	}

	// -- Lane access ---------------------------------------------------------

	/// Extract a single lane by index.
	val<T> Get(val<int32_t> idx) const {
		return detail::vec_extract<T, N>(ptr_, idx);
	}

	/// Return a new vector with one lane replaced.
	val Set(val<int32_t> idx, val<T> value) const {
		return val(detail::vec_insert<T, N>(ptr_, value, idx));
	}

	// -- Unary operations ----------------------------------------------------

	/// Element-wise absolute value.
	val Abs() const {
		return val(detail::vec_abs<T, N>(ptr_));
	}

	// -- Reductions (return scalar val<T>) -----------------------------------

	/// Sum all lanes.
	val<T> ReduceAdd() const {
		return detail::vec_reduce_add<T, N>(ptr_);
	}

	/// Minimum across all lanes.
	val<T> ReduceMin() const {
		return detail::vec_reduce_min<T, N>(ptr_);
	}

	/// Maximum across all lanes.
	val<T> ReduceMax() const {
		return detail::vec_reduce_max<T, N>(ptr_);
	}

	// -- Arithmetic operators ------------------------------------------------

	friend val operator+(val a, val b) {
		return val(detail::vec_add<T, N>(a.ptr_, b.ptr_));
	}

	friend val operator-(val a, val b) {
		return val(detail::vec_sub<T, N>(a.ptr_, b.ptr_));
	}

	friend val operator*(val a, val b) {
		return val(detail::vec_mul<T, N>(a.ptr_, b.ptr_));
	}

	friend val operator/(val a, val b) {
		return val(detail::vec_div<T, N>(a.ptr_, b.ptr_));
	}

	friend val operator-(val a) {
		return val(detail::vec_neg<T, N>(a.ptr_));
	}

	// -- Compound assignment -------------------------------------------------

	val& operator+=(val other) {
		*this = *this + other;
		return *this;
	}

	val& operator-=(val other) {
		*this = *this - other;
		return *this;
	}

	val& operator*=(val other) {
		*this = *this * other;
		return *this;
	}

	val& operator/=(val other) {
		*this = *this / other;
		return *this;
	}

	// -- Comparison operators (return element-wise mask vectors) --------------

	friend val operator==(val a, val b) {
		return val(detail::vec_eq<T, N>(a.ptr_, b.ptr_));
	}

	friend val operator!=(val a, val b) {
		return val(detail::vec_ne<T, N>(a.ptr_, b.ptr_));
	}

	friend val operator<(val a, val b) {
		return val(detail::vec_lt<T, N>(a.ptr_, b.ptr_));
	}

	friend val operator<=(val a, val b) {
		return val(detail::vec_le<T, N>(a.ptr_, b.ptr_));
	}

	friend val operator>(val a, val b) {
		return val(detail::vec_gt<T, N>(a.ptr_, b.ptr_));
	}

	friend val operator>=(val a, val b) {
		return val(detail::vec_ge<T, N>(a.ptr_, b.ptr_));
	}

	// -- Bitwise operators ---------------------------------------------------

	friend val operator&(val a, val b) {
		return val(detail::vec_and<T, N>(a.ptr_, b.ptr_));
	}

	friend val operator|(val a, val b) {
		return val(detail::vec_or<T, N>(a.ptr_, b.ptr_));
	}

	friend val operator^(val a, val b) {
		return val(detail::vec_xor<T, N>(a.ptr_, b.ptr_));
	}

	// -- Shift operators (integer types only) --------------------------------

	friend val operator<<(val a, val b)
	    requires std::is_integral_v<T>
	{
		return val(detail::vec_shl<T, N>(a.ptr_, b.ptr_));
	}

	friend val operator>>(val a, val b)
	    requires std::is_integral_v<T>
	{
		return val(detail::vec_shr<T, N>(a.ptr_, b.ptr_));
	}

	val& operator<<=(val other)
	    requires std::is_integral_v<T>
	{
		*this = *this << other;
		return *this;
	}

	val& operator>>=(val other)
	    requires std::is_integral_v<T>
	{
		*this = *this >> other;
		return *this;
	}

private:
	ptr_type ptr_;
};

// ============================================================================
// Top-level convenience functions
// ============================================================================

/// Element-wise minimum.
template <typename T>
val<vec<T>> Min(val<vec<T>> a, val<vec<T>> b) {
	return val<vec<T>>(detail::vec_min<T, vec<T>::lanes>(a.Data(), b.Data()));
}

/// Element-wise maximum.
template <typename T>
val<vec<T>> Max(val<vec<T>> a, val<vec<T>> b) {
	return val<vec<T>>(detail::vec_max<T, vec<T>::lanes>(a.Data(), b.Data()));
}

/// Fused multiply-add: a * b + c.
template <typename T>
val<vec<T>> Fma(val<vec<T>> a, val<vec<T>> b, val<vec<T>> c) {
	return val<vec<T>>(detail::vec_fma<T, vec<T>::lanes>(a.Data(), b.Data(), c.Data()));
}

/// Select elements: where mask bits are set take a, otherwise take b.
template <typename T>
val<vec<T>> Blend(val<vec<T>> mask, val<vec<T>> a, val<vec<T>> b) {
	return val<vec<T>>(detail::vec_blend<T, vec<T>::lanes>(mask.Data(), a.Data(), b.Data()));
}

// ============================================================================
// Convenience alias
// ============================================================================

/// Vector<T> is val<vec<T>> with hardware-auto-detected lane width.
template <typename T>
using Vector = val<vec<T>>;

// ============================================================================
// Convenience free function
// ============================================================================

/// Load a vector from contiguous memory (shorthand for Vector<T>::Load).
template <typename T>
Vector<T> vector_load(val<const T*> ptr) {
	return Vector<T>::Load(ptr);
}

} // namespace nautilus
