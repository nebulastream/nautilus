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

/// Type tag representing a SIMD vector of N elements of type T.
/// Use as val<vec<float>> or val<vec<int32_t, 8>>.
template <typename T, size_t N = preferred_vector_width<T>()>
struct vec {
	static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");
	static_assert((N & (N - 1)) == 0, "N must be a power of 2");
	static_assert(N >= 2, "N must be at least 2");
	using element_type = T;
	static constexpr size_t lanes = N;
};

// ============================================================================
// Forward declarations for low-level free functions (specialized in vector.cpp).
// These are the traced entry points that backends can intercept.
// ============================================================================

template <typename T, size_t N>
val<vec<T, N>> vector_load_n(val<const T*> ptr);
template <typename T, size_t N>
void vector_store(val<T*> ptr, val<vec<T, N>> v);
template <typename T, size_t N>
val<vec<T, N>> vector_add(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_sub(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_mul(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_div(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_abs(val<vec<T, N>> a);
template <typename T, size_t N>
val<vec<T, N>> vector_neg(val<vec<T, N>> a);
template <typename T, size_t N>
val<vec<T, N>> vector_min(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_max(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_fma(val<vec<T, N>> a, val<vec<T, N>> b, val<vec<T, N>> c);
template <typename T, size_t N>
val<T> vector_reduce_add(val<vec<T, N>> a);
template <typename T, size_t N>
val<T> vector_reduce_min(val<vec<T, N>> a);
template <typename T, size_t N>
val<T> vector_reduce_max(val<vec<T, N>> a);
template <typename T, size_t N>
val<vec<T, N>> vector_eq(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_ne(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_lt(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_le(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_gt(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_ge(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_blend(val<vec<T, N>> mask, val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_and(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_or(val<vec<T, N>> a, val<vec<T, N>> b);
template <typename T, size_t N>
val<vec<T, N>> vector_xor(val<vec<T, N>> a, val<vec<T, N>> b);

// ============================================================================
// val<vec<T, N>> — SIMD vector specialization of val<T>.
//
// Integrates with the Nautilus tracing and compilation pipeline.
// T is the element type (float, double, int32_t, int64_t).
// N is the number of lanes (defaults to hardware-preferred width).
//
// Usage:
//   auto v = val<vec<float>>::Load(ptr);     // load from memory
//   auto w = val<vec<float>>::Load(ptr2);
//   auto r = v + w;                          // element-wise add
//   r.Store(out);                            // store to memory
//   val<float> sum = r.ReduceAdd();          // horizontal sum
// ============================================================================

template <typename T, size_t N>
class val<vec<T, N>> {
public:
	using raw_type = vec<T, N>;
	using element_type = T;
	using data_type = vector_data<T, N>;
	static constexpr size_t lanes = N;

	explicit val(val<data_type*> ptr) : ptr_(ptr) {
	}

	/// Access internal data pointer (for low-level use and invoke() wrappers).
	val<data_type*> Data() const {
		return ptr_;
	}

	// -- Load / Store --------------------------------------------------------

	/// Load N contiguous elements from memory.
	static val Load(val<const T*> ptr) {
		return vector_load_n<T, N>(ptr);
	}

	/// Store this vector to N contiguous elements in memory.
	void Store(val<T*> ptr) const {
		vector_store<T, N>(ptr, *this);
	}

	// -- Unary operations ----------------------------------------------------

	/// Element-wise absolute value.
	val Abs() const {
		return vector_abs<T, N>(*this);
	}

	// -- Reductions (return scalar val<T>) -----------------------------------

	/// Sum all lanes.
	val<T> ReduceAdd() const {
		return vector_reduce_add<T, N>(*this);
	}

	/// Minimum across all lanes.
	val<T> ReduceMin() const {
		return vector_reduce_min<T, N>(*this);
	}

	/// Maximum across all lanes.
	val<T> ReduceMax() const {
		return vector_reduce_max<T, N>(*this);
	}

	// -- Arithmetic operators ------------------------------------------------

	friend val operator+(val a, val b) {
		return vector_add<T, N>(a, b);
	}

	friend val operator-(val a, val b) {
		return vector_sub<T, N>(a, b);
	}

	friend val operator*(val a, val b) {
		return vector_mul<T, N>(a, b);
	}

	friend val operator/(val a, val b) {
		return vector_div<T, N>(a, b);
	}

	friend val operator-(val a) {
		return vector_neg<T, N>(a);
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
		return vector_eq<T, N>(a, b);
	}

	friend val operator!=(val a, val b) {
		return vector_ne<T, N>(a, b);
	}

	friend val operator<(val a, val b) {
		return vector_lt<T, N>(a, b);
	}

	friend val operator<=(val a, val b) {
		return vector_le<T, N>(a, b);
	}

	friend val operator>(val a, val b) {
		return vector_gt<T, N>(a, b);
	}

	friend val operator>=(val a, val b) {
		return vector_ge<T, N>(a, b);
	}

	// -- Bitwise operators ---------------------------------------------------

	friend val operator&(val a, val b) {
		return vector_and<T, N>(a, b);
	}

	friend val operator|(val a, val b) {
		return vector_or<T, N>(a, b);
	}

	friend val operator^(val a, val b) {
		return vector_xor<T, N>(a, b);
	}

private:
	val<data_type*> ptr_;
};

// ============================================================================
// Top-level convenience functions
// ============================================================================

/// Element-wise minimum.
template <typename T, size_t N>
val<vec<T, N>> Min(val<vec<T, N>> a, val<vec<T, N>> b) {
	return vector_min<T, N>(a, b);
}

/// Element-wise maximum.
template <typename T, size_t N>
val<vec<T, N>> Max(val<vec<T, N>> a, val<vec<T, N>> b) {
	return vector_max<T, N>(a, b);
}

/// Fused multiply-add: a * b + c.
template <typename T, size_t N>
val<vec<T, N>> Fma(val<vec<T, N>> a, val<vec<T, N>> b, val<vec<T, N>> c) {
	return vector_fma<T, N>(a, b, c);
}

/// Select elements: where mask bits are set take a, otherwise take b.
template <typename T, size_t N>
val<vec<T, N>> Blend(val<vec<T, N>> mask, val<vec<T, N>> a, val<vec<T, N>> b) {
	return vector_blend<T, N>(mask, a, b);
}

// ============================================================================
// Convenience aliases
// ============================================================================

/// Vector<T> is val<vec<T>> with hardware-auto-detected lane width.
/// Vector<T, N> specifies explicit lane count.
template <typename T, size_t N = preferred_vector_width<T>()>
using Vector = val<vec<T, N>>;

// ============================================================================
// VectorFactory<Bits> — factory for explicit bit-width selection (128, 256, 512).
// ============================================================================

template <size_t Bits>
struct VectorFactory {
	static_assert(Bits == 128 || Bits == 256 || Bits == 512, "Supported widths: 128, 256, 512");

	template <typename T>
	static constexpr size_t lanes = Bits / (sizeof(T) * 8);

	template <typename T>
	static val<vec<T, lanes<T>>> Load(val<const T*> ptr) {
		return val<vec<T, lanes<T>>>::Load(ptr);
	}

	template <typename T>
	static void Store(val<T*> ptr, val<vec<T, lanes<T>>> v) {
		v.Store(ptr);
	}
};

// ============================================================================
// Convenience: vector_load<T> loads a default-width Vector<T>.
// ============================================================================

template <typename T>
Vector<T> vector_load(val<const T*> ptr) {
	return Vector<T>::Load(ptr);
}

} // namespace nautilus
