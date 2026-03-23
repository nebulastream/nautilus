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

namespace detail {

/// Internal base class for all vector types. Parameterized by element type and lane count.
/// Users do not interact with this class directly — they use Vector<T> or VectorFactory.
template <typename T, size_t N>
class VectorBase {
public:
	using element_type = T;
	using data_type = vector_data<T, N>;
	static constexpr size_t lanes = N;

	explicit VectorBase(val<data_type*> ptr) : ptr_(ptr) {
	}

	val<data_type*> data() const {
		return ptr_;
	}

private:
	val<data_type*> ptr_;
};

} // namespace detail

// ============================================================================
// Operations — declared on detail::VectorBase<T, N> so they work for any width.
// N is automatically deduced from the arguments.
// ============================================================================

// Load / Store
template <typename T, size_t N>
detail::VectorBase<T, N> vector_load_n(val<const T*> ptr);

template <typename T, size_t N>
void vector_store(val<T*> ptr, detail::VectorBase<T, N> vec);

// Arithmetic
template <typename T, size_t N>
detail::VectorBase<T, N> vector_add(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_sub(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_mul(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_div(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

// Math
template <typename T, size_t N>
detail::VectorBase<T, N> vector_abs(detail::VectorBase<T, N> a);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_neg(detail::VectorBase<T, N> a);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_min(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_max(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_fma(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b, detail::VectorBase<T, N> c);

// Reductions
template <typename T, size_t N>
val<T> vector_reduce_add(detail::VectorBase<T, N> a);

template <typename T, size_t N>
val<T> vector_reduce_min(detail::VectorBase<T, N> a);

template <typename T, size_t N>
val<T> vector_reduce_max(detail::VectorBase<T, N> a);

// Comparisons
template <typename T, size_t N>
detail::VectorBase<T, N> vector_eq(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_ne(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_lt(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_le(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_gt(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_ge(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

// Masking / Blend / Bitwise
template <typename T, size_t N>
detail::VectorBase<T, N> vector_blend(detail::VectorBase<T, N> mask, detail::VectorBase<T, N> a,
                                      detail::VectorBase<T, N> b);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_and(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_or(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

template <typename T, size_t N>
detail::VectorBase<T, N> vector_xor(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b);

// ============================================================================
// Operator overloads on VectorBase (inherited by all vector types)
// ============================================================================

template <typename T, size_t N>
detail::VectorBase<T, N> operator+(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b) {
	return vector_add(a, b);
}

template <typename T, size_t N>
detail::VectorBase<T, N> operator-(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b) {
	return vector_sub(a, b);
}

template <typename T, size_t N>
detail::VectorBase<T, N> operator*(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b) {
	return vector_mul(a, b);
}

template <typename T, size_t N>
detail::VectorBase<T, N> operator/(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b) {
	return vector_div(a, b);
}

template <typename T, size_t N>
detail::VectorBase<T, N> operator-(detail::VectorBase<T, N> a) {
	return vector_neg(a);
}

// ============================================================================
// Vector<T> — default vector with hardware-auto-detected lane width.
// The lane count is a const member, NOT a template parameter.
// ============================================================================

template <typename T>
class Vector : public detail::VectorBase<T, preferred_vector_width<T>()> {
	using Base = detail::VectorBase<T, preferred_vector_width<T>()>;

public:
	using Base::Base;

	/// Load a vector from contiguous memory.
	static Vector<T> Load(val<const T*> ptr) {
		return Vector<T>(vector_load_n<T, Base::lanes>(ptr).data());
	}

	/// Store this vector to contiguous memory.
	void Store(val<T*> ptr) const {
		vector_store(ptr, static_cast<const Base&>(*this));
	}
};

// ============================================================================
// VectorFactory<Bits> — factory for explicit width selection.
// Returns VectorBase<T, N> with N derived from the bit width.
// ============================================================================

template <size_t Bits>
struct VectorFactory {
	static_assert(Bits == 128 || Bits == 256 || Bits == 512, "Supported widths: 128, 256, 512");

	template <typename T>
	static constexpr size_t lanes = Bits / (sizeof(T) * 8);

	template <typename T>
	static detail::VectorBase<T, lanes<T>> Load(val<const T*> ptr) {
		return vector_load_n<T, lanes<T>>(ptr);
	}

	template <typename T>
	static void Store(val<T*> ptr, detail::VectorBase<T, lanes<T>> vec) {
		vector_store(ptr, vec);
	}
};

// ============================================================================
// Convenience: vector_load<T> returns the default-width Vector<T>.
// ============================================================================

template <typename T>
Vector<T> vector_load(val<const T*> ptr) {
	constexpr size_t N = preferred_vector_width<T>();
	return Vector<T>(vector_load_n<T, N>(ptr).data());
}

} // namespace nautilus
