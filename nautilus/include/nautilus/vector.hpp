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

/// Fixed-size SIMD vector storage.
template <typename T, size_t N>
struct alignas(sizeof(T) * N) vector_data {
	static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");
	static_assert((N & (N - 1)) == 0, "N must be a power of 2");
	static_assert(N >= 2, "N must be at least 2");
	T data[N];
};

// ============================================================================
// Forward declarations
// ============================================================================

template <typename T, size_t N>
class SIMD;

// Low-level free functions — specialized in vector.cpp via invoke().
// These are the traced entry points that backends can intercept.
template <typename T, size_t N>
SIMD<T, N> vector_load_n(val<const T*> ptr);
template <typename T, size_t N>
void vector_store(val<T*> ptr, SIMD<T, N> vec);
template <typename T, size_t N>
SIMD<T, N> vector_add(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_sub(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_mul(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_div(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_abs(SIMD<T, N> a);
template <typename T, size_t N>
SIMD<T, N> vector_neg(SIMD<T, N> a);
template <typename T, size_t N>
SIMD<T, N> vector_min(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_max(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_fma(SIMD<T, N> a, SIMD<T, N> b, SIMD<T, N> c);
template <typename T, size_t N>
val<T> vector_reduce_add(SIMD<T, N> a);
template <typename T, size_t N>
val<T> vector_reduce_min(SIMD<T, N> a);
template <typename T, size_t N>
val<T> vector_reduce_max(SIMD<T, N> a);
template <typename T, size_t N>
SIMD<T, N> vector_eq(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_ne(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_lt(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_le(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_gt(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_ge(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_blend(SIMD<T, N> mask, SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_and(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_or(SIMD<T, N> a, SIMD<T, N> b);
template <typename T, size_t N>
SIMD<T, N> vector_xor(SIMD<T, N> a, SIMD<T, N> b);

// ============================================================================
// SIMD<T, N> — Fixed-width SIMD vector.
//
// T is the element type (float, double, int32_t, int64_t).
// N is the number of lanes (must be a power of 2, >= 2).
//
// Use SIMD<T, N>::Load(ptr) to load from memory and vec.Store(ptr) to write
// back. Arithmetic, comparison, and bitwise operators are all overloaded.
// ============================================================================

template <typename T, size_t N>
class SIMD {
public:
	using element_type = T;
	using data_type = vector_data<T, N>;
	static constexpr size_t lanes = N;

	explicit SIMD(val<data_type*> ptr) : ptr_(ptr) {
	}

	/// Access internal data pointer (for use with low-level free functions).
	val<data_type*> Data() const {
		return ptr_;
	}

	// -- Load / Store --------------------------------------------------------

	/// Load N contiguous elements from memory into a SIMD vector.
	static SIMD Load(val<const T*> ptr) {
		return vector_load_n<T, N>(ptr);
	}

	/// Store this vector to N contiguous elements in memory.
	void Store(val<T*> ptr) const {
		vector_store<T, N>(ptr, *this);
	}

	// -- Unary operations ----------------------------------------------------

	/// Element-wise absolute value.
	SIMD Abs() const {
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

	friend SIMD operator+(SIMD a, SIMD b) {
		return vector_add<T, N>(a, b);
	}

	friend SIMD operator-(SIMD a, SIMD b) {
		return vector_sub<T, N>(a, b);
	}

	friend SIMD operator*(SIMD a, SIMD b) {
		return vector_mul<T, N>(a, b);
	}

	friend SIMD operator/(SIMD a, SIMD b) {
		return vector_div<T, N>(a, b);
	}

	friend SIMD operator-(SIMD a) {
		return vector_neg<T, N>(a);
	}

	// -- Compound assignment -------------------------------------------------

	SIMD& operator+=(SIMD other) {
		*this = *this + other;
		return *this;
	}

	SIMD& operator-=(SIMD other) {
		*this = *this - other;
		return *this;
	}

	SIMD& operator*=(SIMD other) {
		*this = *this * other;
		return *this;
	}

	SIMD& operator/=(SIMD other) {
		*this = *this / other;
		return *this;
	}

	// -- Comparison operators (return element-wise mask vectors) --------------

	friend SIMD operator==(SIMD a, SIMD b) {
		return vector_eq<T, N>(a, b);
	}

	friend SIMD operator!=(SIMD a, SIMD b) {
		return vector_ne<T, N>(a, b);
	}

	friend SIMD operator<(SIMD a, SIMD b) {
		return vector_lt<T, N>(a, b);
	}

	friend SIMD operator<=(SIMD a, SIMD b) {
		return vector_le<T, N>(a, b);
	}

	friend SIMD operator>(SIMD a, SIMD b) {
		return vector_gt<T, N>(a, b);
	}

	friend SIMD operator>=(SIMD a, SIMD b) {
		return vector_ge<T, N>(a, b);
	}

	// -- Bitwise operators ---------------------------------------------------

	friend SIMD operator&(SIMD a, SIMD b) {
		return vector_and<T, N>(a, b);
	}

	friend SIMD operator|(SIMD a, SIMD b) {
		return vector_or<T, N>(a, b);
	}

	friend SIMD operator^(SIMD a, SIMD b) {
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
SIMD<T, N> Min(SIMD<T, N> a, SIMD<T, N> b) {
	return vector_min<T, N>(a, b);
}

/// Element-wise maximum.
template <typename T, size_t N>
SIMD<T, N> Max(SIMD<T, N> a, SIMD<T, N> b) {
	return vector_max<T, N>(a, b);
}

/// Fused multiply-add: a * b + c.
template <typename T, size_t N>
SIMD<T, N> Fma(SIMD<T, N> a, SIMD<T, N> b, SIMD<T, N> c) {
	return vector_fma<T, N>(a, b, c);
}

/// Select elements: where mask bits are set take a, otherwise take b.
template <typename T, size_t N>
SIMD<T, N> Blend(SIMD<T, N> mask, SIMD<T, N> a, SIMD<T, N> b) {
	return vector_blend<T, N>(mask, a, b);
}

// ============================================================================
// Vector<T> — SIMD vector with hardware-auto-detected lane width.
// ============================================================================

template <typename T>
using Vector = SIMD<T, preferred_vector_width<T>()>;

// ============================================================================
// VectorFactory<Bits> — factory for explicit width selection (128, 256, 512).
// ============================================================================

template <size_t Bits>
struct VectorFactory {
	static_assert(Bits == 128 || Bits == 256 || Bits == 512, "Supported widths: 128, 256, 512");

	template <typename T>
	static constexpr size_t lanes = Bits / (sizeof(T) * 8);

	template <typename T>
	static SIMD<T, lanes<T>> Load(val<const T*> ptr) {
		return SIMD<T, lanes<T>>::Load(ptr);
	}

	template <typename T>
	static void Store(val<T*> ptr, SIMD<T, lanes<T>> vec) {
		vec.Store(ptr);
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
