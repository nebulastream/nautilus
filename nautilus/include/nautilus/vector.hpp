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

/// Fixed-size SIMD vector storage (internal implementation detail).
template <typename T, size_t N>
struct alignas(sizeof(T) * N) vector_data {
	static_assert(std::is_arithmetic_v<T>, "T must be an arithmetic type");
	static_assert((N & (N - 1)) == 0, "N must be a power of 2");
	static_assert(N >= 2, "N must be at least 2");
	T data[N];
};

/// Traced SIMD vector. The lane count is automatically determined from hardware.
/// Users interact only with Vector<T> — the lane count is a const member, not a template parameter.
template <typename T>
class Vector {
public:
	using element_type = T;
	static constexpr size_t lanes = preferred_vector_width<T>();
	using data_type = vector_data<T, lanes>;

	explicit Vector(val<data_type*> ptr) : ptr_(ptr) {
	}

	val<data_type*> data() const {
		return ptr_;
	}

	// ============================================================================
	// Operator overloads
	// ============================================================================

	friend Vector<T> operator+(Vector<T> a, Vector<T> b) {
		return vector_add(a, b);
	}

	friend Vector<T> operator-(Vector<T> a, Vector<T> b) {
		return vector_sub(a, b);
	}

	friend Vector<T> operator*(Vector<T> a, Vector<T> b) {
		return vector_mul(a, b);
	}

	friend Vector<T> operator/(Vector<T> a, Vector<T> b) {
		return vector_div(a, b);
	}

	Vector<T> operator-() {
		return vector_neg(*this);
	}

private:
	val<data_type*> ptr_;
};

// ============================================================================
// Load / Store
// ============================================================================

template <typename T>
Vector<T> vector_load(val<const T*> ptr);

template <typename T>
void vector_store(val<T*> ptr, Vector<T> vec);

// ============================================================================
// Arithmetic
// ============================================================================

template <typename T>
Vector<T> vector_add(Vector<T> a, Vector<T> b);

template <typename T>
Vector<T> vector_sub(Vector<T> a, Vector<T> b);

template <typename T>
Vector<T> vector_mul(Vector<T> a, Vector<T> b);

template <typename T>
Vector<T> vector_div(Vector<T> a, Vector<T> b);

// ============================================================================
// Math
// ============================================================================

template <typename T>
Vector<T> vector_abs(Vector<T> a);

template <typename T>
Vector<T> vector_neg(Vector<T> a);

template <typename T>
Vector<T> vector_min(Vector<T> a, Vector<T> b);

template <typename T>
Vector<T> vector_max(Vector<T> a, Vector<T> b);

template <typename T>
Vector<T> vector_fma(Vector<T> a, Vector<T> b, Vector<T> c);

// ============================================================================
// Reductions
// ============================================================================

template <typename T>
val<T> vector_reduce_add(Vector<T> a);

template <typename T>
val<T> vector_reduce_min(Vector<T> a);

template <typename T>
val<T> vector_reduce_max(Vector<T> a);

// ============================================================================
// Comparisons (return Vector with 0 / all-ones mask pattern)
// ============================================================================

template <typename T>
Vector<T> vector_eq(Vector<T> a, Vector<T> b);

template <typename T>
Vector<T> vector_ne(Vector<T> a, Vector<T> b);

template <typename T>
Vector<T> vector_lt(Vector<T> a, Vector<T> b);

template <typename T>
Vector<T> vector_le(Vector<T> a, Vector<T> b);

template <typename T>
Vector<T> vector_gt(Vector<T> a, Vector<T> b);

template <typename T>
Vector<T> vector_ge(Vector<T> a, Vector<T> b);

// ============================================================================
// Masking / Blend / Bitwise
// ============================================================================

/// Selects elements: where mask bits are set, takes from a; otherwise from b.
template <typename T>
Vector<T> vector_blend(Vector<T> mask, Vector<T> a, Vector<T> b);

template <typename T>
Vector<T> vector_and(Vector<T> a, Vector<T> b);

template <typename T>
Vector<T> vector_or(Vector<T> a, Vector<T> b);

template <typename T>
Vector<T> vector_xor(Vector<T> a, Vector<T> b);

} // namespace nautilus
