#pragma once

#include <cstddef>
#include <nautilus/val.hpp>
#include <type_traits>

namespace nautilus {

/// Returns the SIMD register width in bytes for the current CPU, detected at runtime.
/// Uses cpuid on x86, feature registers on ARM. Result is cached after first call.
/// Possible return values: 16 (SSE2/NEON), 32 (AVX/AVX2), 64 (AVX-512).
size_t RuntimeSimdWidth();

/// Fixed-size aligned SIMD storage (internal).
template <typename T, size_t N>
struct alignas(sizeof(T) * N) vector_data {
	static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");
	static_assert((N & (N - 1)) == 0, "N must be a power of 2");
	static_assert(N >= 2, "N must be at least 2");
	T data[N];
};

/// Type tag representing a SIMD vector of type T.
/// The lane count is determined at runtime from CPU capabilities.
/// Use as val<vec<float>>, val<vec<int32_t>>, etc.
template <typename T>
struct vec {
	static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");
	using element_type = T;

	/// Maximum possible lane count (AVX-512). Useful for stack array sizing.
	static constexpr size_t max_lanes = 64 / sizeof(T);

	/// Returns the optimal lane count for type T on the current CPU (detected at runtime).
	static size_t Lanes() {
		return RuntimeSimdWidth() / sizeof(T);
	}
};

// ============================================================================
// detail — N-parameterized free functions specialized in vector.cpp.
// These are the traced entry points that backends can intercept.
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

// ============================================================================
// dispatch_by_lanes — runtime dispatch to the correct N-parameterized function.
//
// Calls fn(std::integral_constant<size_t, N>{}) with the N matching the
// current CPU's SIMD width. The dispatch runs once during tracing; the
// JIT-compiled code calls the correct width directly.
// ============================================================================

template <typename T, typename Fn>
auto dispatch_by_lanes(Fn&& fn) {
	constexpr size_t N16 = 16 / sizeof(T);
	constexpr size_t N32 = 32 / sizeof(T);
	constexpr size_t N64 = 64 / sizeof(T);
	size_t n = vec<T>::Lanes();
	if (n == N64) {
		return fn(std::integral_constant<size_t, N64> {});
	}
	if (n == N32) {
		return fn(std::integral_constant<size_t, N32> {});
	}
	return fn(std::integral_constant<size_t, N16> {});
}

// void-returning variant for store/scatter operations.
template <typename T, typename Fn>
void dispatch_by_lanes_void(Fn&& fn) {
	constexpr size_t N16 = 16 / sizeof(T);
	constexpr size_t N32 = 32 / sizeof(T);
	constexpr size_t N64 = 64 / sizeof(T);
	size_t n = vec<T>::Lanes();
	if (n == N64) {
		fn(std::integral_constant<size_t, N64> {});
		return;
	}
	if (n == N32) {
		fn(std::integral_constant<size_t, N32> {});
		return;
	}
	fn(std::integral_constant<size_t, N16> {});
}

// Cast helpers: convert between N-specific val pointers and the max-width
// pointer type used as uniform internal storage in val<vec<T>>.
// Safe because: during tracing, IR only sees opaque value references;
// during scalar fallback, the callee only accesses N elements.

template <typename T, size_t N>
val<vector_data<T, vec<T>::max_lanes>*> vec_to_max(val<vector_data<T, N>*> v) {
	return reinterpret_cast<const val<vector_data<T, vec<T>::max_lanes>*>&>(v);
}

template <typename T, size_t N>
val<vector_data<T, N>*> vec_from_max(val<vector_data<T, vec<T>::max_lanes>*> v) {
	return reinterpret_cast<const val<vector_data<T, N>*>&>(v);
}

} // namespace detail

// ============================================================================
// val<vec<T>> — SIMD vector specialization of val<T>.
//
// The lane count is detected at runtime from CPU capabilities.
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
	// Use max-width pointer type as uniform internal storage.
	// At dispatch boundaries, we cast to the actual N-specific type.
	// This is safe because: during tracing, the IR only sees opaque value
	// references; during scalar fallback, the callee only accesses N elements.
	static constexpr size_t MAX_N = vec<T>::max_lanes;
	using max_data_type = vector_data<T, MAX_N>;
	using ptr_type = val<max_data_type*>;

public:
	using raw_type = vec<T>;
	using element_type = T;

	/// Returns the lane count for the current CPU (runtime value).
	static size_t Lanes() {
		return vec<T>::Lanes();
	}

	explicit val(ptr_type ptr) : ptr_(ptr) {
	}

	/// Access internal data pointer (for advanced use).
	ptr_type Data() const {
		return ptr_;
	}

	// -- Load / Store --------------------------------------------------------

	/// Load a vector from contiguous memory.
	static val Load(val<const T*> ptr) {
		return val(detail::dispatch_by_lanes<T>(
		    [&](auto N_tag) { return detail::vec_to_max<T, N_tag.value>(detail::vec_load<T, N_tag.value>(ptr)); }));
	}

	/// Store this vector to contiguous memory.
	void Store(val<T*> ptr) const {
		detail::dispatch_by_lanes_void<T>(
		    [&](auto N_tag) { detail::vec_store<T, N_tag.value>(ptr, detail::vec_from_max<T, N_tag.value>(ptr_)); });
	}

	/// Broadcast a scalar value to all lanes.
	static val Broadcast(val<T> scalar) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_broadcast<T, N_tag.value>(scalar));
		}));
	}

	/// Gather: load elements from non-contiguous memory locations.
	/// Loads base[indices[0]], base[indices[1]], ..., base[indices[N-1]].
	template <typename IdxT = int32_t>
	static val Gather(val<const T*> base, val<const IdxT*> indices) {
		static_assert(std::is_same_v<IdxT, int32_t>, "Index type must be int32_t");
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_gather<T, N_tag.value>(base, indices));
		}));
	}

	/// Scatter: store elements to non-contiguous memory locations.
	/// Stores lane i to base[indices[i]].
	template <typename IdxT = int32_t>
	void Scatter(val<T*> base, val<const IdxT*> indices) const {
		static_assert(std::is_same_v<IdxT, int32_t>, "Index type must be int32_t");
		detail::dispatch_by_lanes_void<T>([&](auto N_tag) {
			detail::vec_scatter<T, N_tag.value>(base, indices, detail::vec_from_max<T, N_tag.value>(ptr_));
		});
	}

	// -- Lane access ---------------------------------------------------------

	/// Extract a single lane by index.
	val<T> Get(val<int32_t> idx) const {
		return detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_extract<T, N_tag.value>(detail::vec_from_max<T, N_tag.value>(ptr_), idx);
		});
	}

	/// Return a new vector with one lane replaced.
	val Set(val<int32_t> idx, val<T> value) const {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(
			    detail::vec_insert<T, N_tag.value>(detail::vec_from_max<T, N_tag.value>(ptr_), value, idx));
		}));
	}

	// -- Unary operations ----------------------------------------------------

	/// Element-wise absolute value.
	val Abs() const {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(
			    detail::vec_abs<T, N_tag.value>(detail::vec_from_max<T, N_tag.value>(ptr_)));
		}));
	}

	// -- Reductions (return scalar val<T>) -----------------------------------

	/// Sum all lanes.
	val<T> ReduceAdd() const {
		return detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_reduce_add<T, N_tag.value>(detail::vec_from_max<T, N_tag.value>(ptr_));
		});
	}

	/// Minimum across all lanes.
	val<T> ReduceMin() const {
		return detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_reduce_min<T, N_tag.value>(detail::vec_from_max<T, N_tag.value>(ptr_));
		});
	}

	/// Maximum across all lanes.
	val<T> ReduceMax() const {
		return detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_reduce_max<T, N_tag.value>(detail::vec_from_max<T, N_tag.value>(ptr_));
		});
	}

	// -- Arithmetic operators ------------------------------------------------

	friend val operator+(val a, val b) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_add<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	friend val operator-(val a, val b) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_sub<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	friend val operator*(val a, val b) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_mul<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	friend val operator/(val a, val b) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_div<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	friend val operator-(val a) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(
			    detail::vec_neg<T, N_tag.value>(detail::vec_from_max<T, N_tag.value>(a.ptr_)));
		}));
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
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_eq<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	friend val operator!=(val a, val b) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_ne<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	friend val operator<(val a, val b) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_lt<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	friend val operator<=(val a, val b) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_le<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	friend val operator>(val a, val b) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_gt<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	friend val operator>=(val a, val b) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_ge<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	// -- Bitwise operators ---------------------------------------------------

	friend val operator&(val a, val b) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_and<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	friend val operator|(val a, val b) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_or<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	friend val operator^(val a, val b) {
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_xor<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	// -- Shift operators (integer types only) --------------------------------

	friend val operator<<(val a, val b)
	    requires std::is_integral_v<T>
	{
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_shl<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
	}

	friend val operator>>(val a, val b)
	    requires std::is_integral_v<T>
	{
		return val(detail::dispatch_by_lanes<T>([&](auto N_tag) {
			return detail::vec_to_max<T, N_tag.value>(detail::vec_shr<T, N_tag.value>(
			    detail::vec_from_max<T, N_tag.value>(a.ptr_), detail::vec_from_max<T, N_tag.value>(b.ptr_)));
		}));
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
	return val<vec<T>>(detail::dispatch_by_lanes<T>([&](auto N_tag) {
		return detail::vec_to_max<T, N_tag.value>(detail::vec_min<T, N_tag.value>(
		    detail::vec_from_max<T, N_tag.value>(a.Data()), detail::vec_from_max<T, N_tag.value>(b.Data())));
	}));
}

/// Element-wise maximum.
template <typename T>
val<vec<T>> Max(val<vec<T>> a, val<vec<T>> b) {
	return val<vec<T>>(detail::dispatch_by_lanes<T>([&](auto N_tag) {
		return detail::vec_to_max<T, N_tag.value>(detail::vec_max<T, N_tag.value>(
		    detail::vec_from_max<T, N_tag.value>(a.Data()), detail::vec_from_max<T, N_tag.value>(b.Data())));
	}));
}

/// Fused multiply-add: a * b + c.
template <typename T>
val<vec<T>> Fma(val<vec<T>> a, val<vec<T>> b, val<vec<T>> c) {
	return val<vec<T>>(detail::dispatch_by_lanes<T>([&](auto N_tag) {
		return detail::vec_to_max<T, N_tag.value>(detail::vec_fma<T, N_tag.value>(
		    detail::vec_from_max<T, N_tag.value>(a.Data()), detail::vec_from_max<T, N_tag.value>(b.Data()),
		    detail::vec_from_max<T, N_tag.value>(c.Data())));
	}));
}

/// Select elements: where mask bits are set take a, otherwise take b.
template <typename T>
val<vec<T>> Blend(val<vec<T>> mask, val<vec<T>> a, val<vec<T>> b) {
	return val<vec<T>>(detail::dispatch_by_lanes<T>([&](auto N_tag) {
		return detail::vec_to_max<T, N_tag.value>(detail::vec_blend<T, N_tag.value>(
		    detail::vec_from_max<T, N_tag.value>(mask.Data()), detail::vec_from_max<T, N_tag.value>(a.Data()),
		    detail::vec_from_max<T, N_tag.value>(b.Data())));
	}));
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
