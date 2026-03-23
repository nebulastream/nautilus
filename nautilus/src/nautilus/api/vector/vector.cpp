#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <nautilus/function.hpp>
#include <nautilus/vector.hpp>

namespace nautilus {

// ============================================================================
// Scalar default implementations (extern "C" for stable function pointers)
//
// These are the fallback implementations used by all non-MLIR backends.
// The MLIR backend intercepts these function pointers via the intrinsic
// plugin and replaces them with native vector dialect operations.
// ============================================================================

// Helper: comparison mask value (all-ones for true, zero for false)
template <typename T>
static T mask_true() {
	T result;
	std::memset(&result, 0xFF, sizeof(T));
	return result;
}

template <typename T>
static T mask_false() {
	return T(0);
}

// ============================================================================
// Ring buffer for thread-safe result storage.
// Prevents multiple concurrent calls from overwriting each other's results.
// ============================================================================

template <typename T, size_t N>
static vector_data<T, N>* nextSlot() {
	static constexpr size_t RING_SIZE = 32;
	static thread_local vector_data<T, N> ring[RING_SIZE];
	static thread_local size_t idx = 0;
	return &ring[idx++ & (RING_SIZE - 1)];
}

// ============================================================================
// Macros to reduce boilerplate for type/width instantiations
// ============================================================================

// clang-format off

#define VECTOR_IMPL_LOAD(T, N, SUFFIX)                                    \
	extern "C" vector_data<T, N>* vector_load_##SUFFIX##_impl(const T* ptr) { \
		auto* result = nextSlot<T, N>();                                  \
		for (size_t i = 0; i < N; i++) result->data[i] = ptr[i];         \
		return result;                                                    \
	}

#define VECTOR_IMPL_STORE(T, N, SUFFIX)                                          \
	extern "C" void vector_store_##SUFFIX##_impl(T* ptr, vector_data<T, N>* vec) { \
		for (size_t i = 0; i < N; i++) ptr[i] = vec->data[i];                   \
	}

#define VECTOR_IMPL_BINARY(T, N, SUFFIX, NAME, OP)                                                         \
	extern "C" vector_data<T, N>* vector_##NAME##_##SUFFIX##_impl(vector_data<T, N>* a, vector_data<T, N>* b) { \
		auto* result = nextSlot<T, N>();                                                                    \
		for (size_t i = 0; i < N; i++) result->data[i] = a->data[i] OP b->data[i];                         \
		return result;                                                                                      \
	}

#define VECTOR_IMPL_BINARY_FUNC(T, N, SUFFIX, NAME, FUNC)                                                  \
	extern "C" vector_data<T, N>* vector_##NAME##_##SUFFIX##_impl(vector_data<T, N>* a, vector_data<T, N>* b) { \
		auto* result = nextSlot<T, N>();                                                                    \
		for (size_t i = 0; i < N; i++) result->data[i] = FUNC(a->data[i], b->data[i]);                     \
		return result;                                                                                      \
	}

#define VECTOR_IMPL_UNARY_FUNC(T, N, SUFFIX, NAME, FUNC)                               \
	extern "C" vector_data<T, N>* vector_##NAME##_##SUFFIX##_impl(vector_data<T, N>* a) { \
		auto* result = nextSlot<T, N>();                                                \
		for (size_t i = 0; i < N; i++) result->data[i] = FUNC(a->data[i]);             \
		return result;                                                                  \
	}

#define VECTOR_IMPL_FMA(T, N, SUFFIX)                                                                                       \
	extern "C" vector_data<T, N>* vector_fma_##SUFFIX##_impl(vector_data<T, N>* a, vector_data<T, N>* b, vector_data<T, N>* c) { \
		auto* result = nextSlot<T, N>();                                                                                    \
		for (size_t i = 0; i < N; i++) result->data[i] = std::fma(a->data[i], b->data[i], c->data[i]);                     \
		return result;                                                                                                      \
	}

#define VECTOR_IMPL_REDUCE(T, N, SUFFIX, NAME, INIT, OP)                          \
	extern "C" T vector_reduce_##NAME##_##SUFFIX##_impl(vector_data<T, N>* a) { \
		T result = INIT;                                                       \
		for (size_t i = 0; i < N; i++) result = result OP a->data[i];          \
		return result;                                                         \
	}

#define VECTOR_IMPL_REDUCE_FUNC(T, N, SUFFIX, NAME, INIT, FUNC)                   \
	extern "C" T vector_reduce_##NAME##_##SUFFIX##_impl(vector_data<T, N>* a) { \
		T result = INIT;                                                       \
		for (size_t i = 0; i < N; i++) result = FUNC(result, a->data[i]);      \
		return result;                                                         \
	}

#define VECTOR_IMPL_CMP(T, N, SUFFIX, NAME, OP)                                                            \
	extern "C" vector_data<T, N>* vector_##NAME##_##SUFFIX##_impl(vector_data<T, N>* a, vector_data<T, N>* b) { \
		auto* result = nextSlot<T, N>();                                                                    \
		for (size_t i = 0; i < N; i++)                                                                      \
			result->data[i] = (a->data[i] OP b->data[i]) ? mask_true<T>() : mask_false<T>();               \
		return result;                                                                                      \
	}

#define VECTOR_IMPL_BLEND(T, N, SUFFIX)                                                                                     \
	extern "C" vector_data<T, N>* vector_blend_##SUFFIX##_impl(vector_data<T, N>* mask, vector_data<T, N>* a, vector_data<T, N>* b) { \
		auto* result = nextSlot<T, N>();                                                                                    \
		for (size_t i = 0; i < N; i++)                                                                                      \
			result->data[i] = (mask->data[i] != T(0)) ? a->data[i] : b->data[i];                                           \
		return result;                                                                                                      \
	}

#define VECTOR_IMPL_BITWISE(T, N, SUFFIX, NAME, OP)                                                        \
	extern "C" vector_data<T, N>* vector_##NAME##_##SUFFIX##_impl(vector_data<T, N>* a, vector_data<T, N>* b) { \
		auto* result = nextSlot<T, N>();                                                                    \
		using U = std::conditional_t<sizeof(T) == 4, uint32_t, uint64_t>;                                   \
		for (size_t i = 0; i < N; i++) {                                                                    \
			U ai, bi;                                                                                       \
			std::memcpy(&ai, &a->data[i], sizeof(T));                                                       \
			std::memcpy(&bi, &b->data[i], sizeof(T));                                                       \
			U ri = ai OP bi;                                                                                \
			std::memcpy(&result->data[i], &ri, sizeof(T));                                                  \
		}                                                                                                   \
		return result;                                                                                      \
	}

// ============================================================================
// Generate all extern "C" implementations for a given type/width
// ============================================================================

#define VECTOR_IMPL_ALL_OPS(T, N, SUFFIX)                         \
	VECTOR_IMPL_LOAD(T, N, SUFFIX)                                \
	VECTOR_IMPL_STORE(T, N, SUFFIX)                               \
	VECTOR_IMPL_BINARY(T, N, SUFFIX, add, +)                      \
	VECTOR_IMPL_BINARY(T, N, SUFFIX, sub, -)                      \
	VECTOR_IMPL_BINARY(T, N, SUFFIX, mul, *)                      \
	VECTOR_IMPL_BINARY(T, N, SUFFIX, div, /)                      \
	VECTOR_IMPL_UNARY_FUNC(T, N, SUFFIX, abs, std::abs)           \
	VECTOR_IMPL_BINARY_FUNC(T, N, SUFFIX, min, std::min)          \
	VECTOR_IMPL_BINARY_FUNC(T, N, SUFFIX, max, std::max)          \
	VECTOR_IMPL_CMP(T, N, SUFFIX, eq, ==)                         \
	VECTOR_IMPL_CMP(T, N, SUFFIX, ne, !=)                         \
	VECTOR_IMPL_CMP(T, N, SUFFIX, lt, <)                          \
	VECTOR_IMPL_CMP(T, N, SUFFIX, le, <=)                         \
	VECTOR_IMPL_CMP(T, N, SUFFIX, gt, >)                          \
	VECTOR_IMPL_CMP(T, N, SUFFIX, ge, >=)                         \
	VECTOR_IMPL_BLEND(T, N, SUFFIX)                               \
	VECTOR_IMPL_BITWISE(T, N, SUFFIX, and, &)                     \
	VECTOR_IMPL_BITWISE(T, N, SUFFIX, or, |)                      \
	VECTOR_IMPL_BITWISE(T, N, SUFFIX, xor, ^)

#define VECTOR_IMPL_FLOAT_OPS(T, N, SUFFIX)                       \
	VECTOR_IMPL_ALL_OPS(T, N, SUFFIX)                             \
	VECTOR_IMPL_FMA(T, N, SUFFIX)                                 \
	VECTOR_IMPL_REDUCE(T, N, SUFFIX, add, T(0), +)               \
	VECTOR_IMPL_REDUCE_FUNC(T, N, SUFFIX, min, a->data[0], std::min) \
	VECTOR_IMPL_REDUCE_FUNC(T, N, SUFFIX, max, a->data[0], std::max)

// Neg for float types
#define VECTOR_IMPL_NEG_FLOAT(T, N, SUFFIX)                                     \
	extern "C" vector_data<T, N>* vector_neg_##SUFFIX##_impl(vector_data<T, N>* a) { \
		auto* result = nextSlot<T, N>();                                        \
		for (size_t i = 0; i < N; i++) result->data[i] = -a->data[i];          \
		return result;                                                          \
	}

// Neg for integer types
#define VECTOR_IMPL_NEG_INT(T, N, SUFFIX)                                       \
	extern "C" vector_data<T, N>* vector_neg_##SUFFIX##_impl(vector_data<T, N>* a) { \
		auto* result = nextSlot<T, N>();                                        \
		for (size_t i = 0; i < N; i++) result->data[i] = -a->data[i];          \
		return result;                                                          \
	}

#define VECTOR_IMPL_INT_OPS(T, N, SUFFIX)                         \
	VECTOR_IMPL_ALL_OPS(T, N, SUFFIX)                             \
	VECTOR_IMPL_REDUCE(T, N, SUFFIX, add, T(0), +)               \
	VECTOR_IMPL_REDUCE_FUNC(T, N, SUFFIX, min, a->data[0], std::min) \
	VECTOR_IMPL_REDUCE_FUNC(T, N, SUFFIX, max, a->data[0], std::max)

// clang-format on

// ============================================================================
// Instantiate extern "C" implementations for supported types and widths
// ============================================================================

// float: 128-bit (4 lanes), 256-bit (8 lanes), 512-bit (16 lanes)
VECTOR_IMPL_FLOAT_OPS(float, 4, f32x4)
VECTOR_IMPL_NEG_FLOAT(float, 4, f32x4)
VECTOR_IMPL_FLOAT_OPS(float, 8, f32x8)
VECTOR_IMPL_NEG_FLOAT(float, 8, f32x8)
VECTOR_IMPL_FLOAT_OPS(float, 16, f32x16)
VECTOR_IMPL_NEG_FLOAT(float, 16, f32x16)

// double: 128-bit (2 lanes), 256-bit (4 lanes), 512-bit (8 lanes)
VECTOR_IMPL_FLOAT_OPS(double, 2, f64x2)
VECTOR_IMPL_NEG_FLOAT(double, 2, f64x2)
VECTOR_IMPL_FLOAT_OPS(double, 4, f64x4)
VECTOR_IMPL_NEG_FLOAT(double, 4, f64x4)
VECTOR_IMPL_FLOAT_OPS(double, 8, f64x8)
VECTOR_IMPL_NEG_FLOAT(double, 8, f64x8)

// int32_t: 128-bit (4 lanes), 256-bit (8 lanes), 512-bit (16 lanes)
VECTOR_IMPL_INT_OPS(int32_t, 4, i32x4)
VECTOR_IMPL_NEG_INT(int32_t, 4, i32x4)
VECTOR_IMPL_INT_OPS(int32_t, 8, i32x8)
VECTOR_IMPL_NEG_INT(int32_t, 8, i32x8)
VECTOR_IMPL_INT_OPS(int32_t, 16, i32x16)
VECTOR_IMPL_NEG_INT(int32_t, 16, i32x16)

// int64_t: 128-bit (2 lanes), 256-bit (4 lanes), 512-bit (8 lanes)
VECTOR_IMPL_INT_OPS(int64_t, 2, i64x2)
VECTOR_IMPL_NEG_INT(int64_t, 2, i64x2)
VECTOR_IMPL_INT_OPS(int64_t, 4, i64x4)
VECTOR_IMPL_NEG_INT(int64_t, 4, i64x4)
VECTOR_IMPL_INT_OPS(int64_t, 8, i64x8)
VECTOR_IMPL_NEG_INT(int64_t, 8, i64x8)

// ============================================================================
// Nautilus invoke() wrappers — template explicit specializations
//
// Vector<T> has a fixed lane count determined by preferred_vector_width<T>().
// We only instantiate wrappers for the hardware-detected width.
// N is derived from Vector<T>::lanes.
// ============================================================================

// clang-format off

#define VECTOR_INVOKE_LOAD(T, SUFFIX)                                                                                      \
	template <>                                                                                                            \
	Vector<T> vector_load<T>(val<const T*> ptr) {                                                                          \
		constexpr size_t N = Vector<T>::lanes;                                                                             \
		auto result = invoke<vector_data<T, N>*, const T*>(vector_load_##SUFFIX##_impl, ptr);                              \
		return Vector<T>(result);                                                                                          \
	}

#define VECTOR_INVOKE_STORE(T, SUFFIX)                                                                                     \
	template <>                                                                                                            \
	void vector_store<T>(val<T*> ptr, Vector<T> vec) {                                                                     \
		constexpr size_t N = Vector<T>::lanes;                                                                             \
		invoke<void, T*, vector_data<T, N>*>(vector_store_##SUFFIX##_impl, ptr, vec.data());                               \
	}

#define VECTOR_INVOKE_BINARY(T, SUFFIX, NAME)                                                                              \
	template <>                                                                                                            \
	Vector<T> vector_##NAME<T>(Vector<T> a, Vector<T> b) {                                                                 \
		constexpr size_t N = Vector<T>::lanes;                                                                             \
		auto result = invoke<vector_data<T, N>*, vector_data<T, N>*, vector_data<T, N>*>(                                  \
		    vector_##NAME##_##SUFFIX##_impl, a.data(), b.data());                                                          \
		return Vector<T>(result);                                                                                          \
	}

#define VECTOR_INVOKE_UNARY(T, SUFFIX, NAME)                                                                               \
	template <>                                                                                                            \
	Vector<T> vector_##NAME<T>(Vector<T> a) {                                                                              \
		constexpr size_t N = Vector<T>::lanes;                                                                             \
		auto result = invoke<vector_data<T, N>*, vector_data<T, N>*>(vector_##NAME##_##SUFFIX##_impl, a.data());           \
		return Vector<T>(result);                                                                                          \
	}

#define VECTOR_INVOKE_TERNARY(T, SUFFIX, NAME)                                                                             \
	template <>                                                                                                            \
	Vector<T> vector_##NAME<T>(Vector<T> a, Vector<T> b, Vector<T> c) {                                                    \
		constexpr size_t N = Vector<T>::lanes;                                                                             \
		auto result = invoke<vector_data<T, N>*, vector_data<T, N>*, vector_data<T, N>*, vector_data<T, N>*>(              \
		    vector_##NAME##_##SUFFIX##_impl, a.data(), b.data(), c.data());                                                \
		return Vector<T>(result);                                                                                          \
	}

#define VECTOR_INVOKE_REDUCE(T, SUFFIX, NAME)                                                                              \
	template <>                                                                                                            \
	val<T> vector_reduce_##NAME<T>(Vector<T> a) {                                                                          \
		constexpr size_t N = Vector<T>::lanes;                                                                             \
		return invoke<T, vector_data<T, N>*>(vector_reduce_##NAME##_##SUFFIX##_impl, a.data());                            \
	}

#define VECTOR_INVOKE_ALL(T, SUFFIX)                                                                                       \
	VECTOR_INVOKE_LOAD(T, SUFFIX)                                                                                          \
	VECTOR_INVOKE_STORE(T, SUFFIX)                                                                                         \
	VECTOR_INVOKE_BINARY(T, SUFFIX, add)                                                                                   \
	VECTOR_INVOKE_BINARY(T, SUFFIX, sub)                                                                                   \
	VECTOR_INVOKE_BINARY(T, SUFFIX, mul)                                                                                   \
	VECTOR_INVOKE_BINARY(T, SUFFIX, div)                                                                                   \
	VECTOR_INVOKE_UNARY(T, SUFFIX, abs)                                                                                    \
	VECTOR_INVOKE_UNARY(T, SUFFIX, neg)                                                                                    \
	VECTOR_INVOKE_BINARY(T, SUFFIX, min)                                                                                   \
	VECTOR_INVOKE_BINARY(T, SUFFIX, max)                                                                                   \
	VECTOR_INVOKE_REDUCE(T, SUFFIX, add)                                                                                   \
	VECTOR_INVOKE_REDUCE(T, SUFFIX, min)                                                                                   \
	VECTOR_INVOKE_REDUCE(T, SUFFIX, max)                                                                                   \
	VECTOR_INVOKE_BINARY(T, SUFFIX, eq)                                                                                    \
	VECTOR_INVOKE_BINARY(T, SUFFIX, ne)                                                                                    \
	VECTOR_INVOKE_BINARY(T, SUFFIX, lt)                                                                                    \
	VECTOR_INVOKE_BINARY(T, SUFFIX, le)                                                                                    \
	VECTOR_INVOKE_BINARY(T, SUFFIX, gt)                                                                                    \
	VECTOR_INVOKE_BINARY(T, SUFFIX, ge)                                                                                    \
	VECTOR_INVOKE_TERNARY(T, SUFFIX, blend)                                                                                \
	VECTOR_INVOKE_BINARY(T, SUFFIX, and)                                                                                   \
	VECTOR_INVOKE_BINARY(T, SUFFIX, or)                                                                                    \
	VECTOR_INVOKE_BINARY(T, SUFFIX, xor)

#define VECTOR_INVOKE_FLOAT_ALL(T, SUFFIX)                                                                                 \
	VECTOR_INVOKE_ALL(T, SUFFIX)                                                                                           \
	VECTOR_INVOKE_TERNARY(T, SUFFIX, fma)

// clang-format on

// ============================================================================
// Instantiate invoke() wrappers for the hardware-detected vector width.
// preferred_vector_width() selects the suffix at compile time.
// ============================================================================

// On SSE2/NEON: float=4, double=2, int32=4, int64=2
// On AVX/AVX2: float=8, double=4, int32=8, int64=4
// On AVX-512:  float=16, double=8, int32=16, int64=8

#if defined(__AVX512F__)
VECTOR_INVOKE_FLOAT_ALL(float, f32x16)
VECTOR_INVOKE_FLOAT_ALL(double, f64x8)
VECTOR_INVOKE_ALL(int32_t, i32x16)
VECTOR_INVOKE_ALL(int64_t, i64x8)
#elif defined(__AVX2__) || defined(__AVX__)
VECTOR_INVOKE_FLOAT_ALL(float, f32x8)
VECTOR_INVOKE_FLOAT_ALL(double, f64x4)
VECTOR_INVOKE_ALL(int32_t, i32x8)
VECTOR_INVOKE_ALL(int64_t, i64x4)
#else
// SSE2 / NEON / fallback (128-bit)
VECTOR_INVOKE_FLOAT_ALL(float, f32x4)
VECTOR_INVOKE_FLOAT_ALL(double, f64x2)
VECTOR_INVOKE_ALL(int32_t, i32x4)
VECTOR_INVOKE_ALL(int64_t, i64x2)
#endif

} // namespace nautilus
