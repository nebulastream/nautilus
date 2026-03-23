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
// Operations use detail::VectorBase<T, N> with both T and N as template params.
// We instantiate for ALL supported widths (128, 256, 512) so both the default
// Vector<T> and VectorFactory<Bits> work on any platform.
// ============================================================================

// clang-format off

#define VECTOR_INVOKE_LOAD(T, N, SUFFIX)                                                                                   \
	template <>                                                                                                            \
	detail::VectorBase<T, N> vector_load_n<T, N>(val<const T*> ptr) {                                                      \
		auto result = invoke<vector_data<T, N>*, const T*>(vector_load_##SUFFIX##_impl, ptr);                              \
		return detail::VectorBase<T, N>(result);                                                                           \
	}

#define VECTOR_INVOKE_STORE(T, N, SUFFIX)                                                                                  \
	template <>                                                                                                            \
	void vector_store<T, N>(val<T*> ptr, detail::VectorBase<T, N> vec) {                                                   \
		invoke<void, T*, vector_data<T, N>*>(vector_store_##SUFFIX##_impl, ptr, vec.data());                               \
	}

#define VECTOR_INVOKE_BINARY(T, N, SUFFIX, NAME)                                                                           \
	template <>                                                                                                            \
	detail::VectorBase<T, N> vector_##NAME<T, N>(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b) {                 \
		auto result = invoke<vector_data<T, N>*, vector_data<T, N>*, vector_data<T, N>*>(                                  \
		    vector_##NAME##_##SUFFIX##_impl, a.data(), b.data());                                                          \
		return detail::VectorBase<T, N>(result);                                                                           \
	}

#define VECTOR_INVOKE_UNARY(T, N, SUFFIX, NAME)                                                                            \
	template <>                                                                                                            \
	detail::VectorBase<T, N> vector_##NAME<T, N>(detail::VectorBase<T, N> a) {                                             \
		auto result = invoke<vector_data<T, N>*, vector_data<T, N>*>(vector_##NAME##_##SUFFIX##_impl, a.data());           \
		return detail::VectorBase<T, N>(result);                                                                           \
	}

#define VECTOR_INVOKE_TERNARY(T, N, SUFFIX, NAME)                                                                          \
	template <>                                                                                                            \
	detail::VectorBase<T, N> vector_##NAME<T, N>(detail::VectorBase<T, N> a, detail::VectorBase<T, N> b,                   \
	                                             detail::VectorBase<T, N> c) {                                             \
		auto result = invoke<vector_data<T, N>*, vector_data<T, N>*, vector_data<T, N>*, vector_data<T, N>*>(              \
		    vector_##NAME##_##SUFFIX##_impl, a.data(), b.data(), c.data());                                                \
		return detail::VectorBase<T, N>(result);                                                                           \
	}

#define VECTOR_INVOKE_REDUCE(T, N, SUFFIX, NAME)                                                                           \
	template <>                                                                                                            \
	val<T> vector_reduce_##NAME<T, N>(detail::VectorBase<T, N> a) {                                                        \
		return invoke<T, vector_data<T, N>*>(vector_reduce_##NAME##_##SUFFIX##_impl, a.data());                            \
	}

#define VECTOR_INVOKE_ALL(T, N, SUFFIX)                                                                                    \
	VECTOR_INVOKE_LOAD(T, N, SUFFIX)                                                                                       \
	VECTOR_INVOKE_STORE(T, N, SUFFIX)                                                                                      \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, add)                                                                                \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, sub)                                                                                \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, mul)                                                                                \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, div)                                                                                \
	VECTOR_INVOKE_UNARY(T, N, SUFFIX, abs)                                                                                 \
	VECTOR_INVOKE_UNARY(T, N, SUFFIX, neg)                                                                                 \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, min)                                                                                \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, max)                                                                                \
	VECTOR_INVOKE_REDUCE(T, N, SUFFIX, add)                                                                                \
	VECTOR_INVOKE_REDUCE(T, N, SUFFIX, min)                                                                                \
	VECTOR_INVOKE_REDUCE(T, N, SUFFIX, max)                                                                                \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, eq)                                                                                 \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, ne)                                                                                 \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, lt)                                                                                 \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, le)                                                                                 \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, gt)                                                                                 \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, ge)                                                                                 \
	VECTOR_INVOKE_TERNARY(T, N, SUFFIX, blend)                                                                             \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, and)                                                                                \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, or)                                                                                 \
	VECTOR_INVOKE_BINARY(T, N, SUFFIX, xor)

#define VECTOR_INVOKE_FLOAT_ALL(T, N, SUFFIX)                                                                              \
	VECTOR_INVOKE_ALL(T, N, SUFFIX)                                                                                        \
	VECTOR_INVOKE_TERNARY(T, N, SUFFIX, fma)

// clang-format on

// ============================================================================
// Instantiate invoke() wrappers for ALL widths (128, 256, 512).
// This enables both Vector<T> (default width) and VectorFactory<Bits>.
// ============================================================================

// 128-bit (SSE / NEON)
VECTOR_INVOKE_FLOAT_ALL(float, 4, f32x4)
VECTOR_INVOKE_FLOAT_ALL(double, 2, f64x2)
VECTOR_INVOKE_ALL(int32_t, 4, i32x4)
VECTOR_INVOKE_ALL(int64_t, 2, i64x2)

// 256-bit (AVX / AVX2)
VECTOR_INVOKE_FLOAT_ALL(float, 8, f32x8)
VECTOR_INVOKE_FLOAT_ALL(double, 4, f64x4)
VECTOR_INVOKE_ALL(int32_t, 8, i32x8)
VECTOR_INVOKE_ALL(int64_t, 4, i64x4)

// 512-bit (AVX-512)
VECTOR_INVOKE_FLOAT_ALL(float, 16, f32x16)
VECTOR_INVOKE_FLOAT_ALL(double, 8, f64x8)
VECTOR_INVOKE_ALL(int32_t, 16, i32x16)
VECTOR_INVOKE_ALL(int64_t, 8, i64x8)

} // namespace nautilus
