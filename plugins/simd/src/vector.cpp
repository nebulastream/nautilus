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
// ============================================================================

template <typename T, size_t N>
static vector_data<T, N>* nextSlot() {
	static constexpr size_t RING_SIZE = 32;
	static thread_local vector_data<T, N> ring[RING_SIZE];
	static thread_local size_t idx = 0;
	return &ring[idx++ & (RING_SIZE - 1)];
}

// ============================================================================
// Macros for extern "C" scalar fallback implementations
// ============================================================================

// clang-format off

#define VECTOR_IMPL_LOAD(T, N, SUFFIX)                                    \
	extern "C" vector_data<T, N>* vector_load_##SUFFIX##_impl(const T* ptr) { \
		auto* result = nextSlot<T, N>();                                  \
		for (size_t i = 0; i < N; i++) result->data[i] = ptr[i];         \
		return result;                                                    \
	}

#define VECTOR_IMPL_STORE(T, N, SUFFIX)                                          \
	extern "C" void vector_store_##SUFFIX##_impl(T* ptr, vector_data<T, N>* v) { \
		for (size_t i = 0; i < N; i++) ptr[i] = v->data[i];                     \
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

#define VECTOR_IMPL_NEG(T, N, SUFFIX)                                            \
	extern "C" vector_data<T, N>* vector_neg_##SUFFIX##_impl(vector_data<T, N>* a) { \
		auto* result = nextSlot<T, N>();                                          \
		for (size_t i = 0; i < N; i++) result->data[i] = -a->data[i];            \
		return result;                                                            \
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

#define VECTOR_IMPL_BROADCAST(T, N, SUFFIX)                                       \
	extern "C" vector_data<T, N>* vector_broadcast_##SUFFIX##_impl(T scalar) {    \
		auto* result = nextSlot<T, N>();                                          \
		for (size_t i = 0; i < N; i++) result->data[i] = scalar;                 \
		return result;                                                            \
	}

#define VECTOR_IMPL_GATHER(T, N, SUFFIX)                                                               \
	extern "C" vector_data<T, N>* vector_gather_##SUFFIX##_impl(const T* base, const int32_t* indices) { \
		auto* result = nextSlot<T, N>();                                                              \
		for (size_t i = 0; i < N; i++) result->data[i] = base[indices[i]];                            \
		return result;                                                                                \
	}

#define VECTOR_IMPL_SCATTER(T, N, SUFFIX)                                                                        \
	extern "C" void vector_scatter_##SUFFIX##_impl(T* base, const int32_t* indices, vector_data<T, N>* data) { \
		for (size_t i = 0; i < N; i++) base[indices[i]] = data->data[i];                                      \
	}

#define VECTOR_IMPL_EXTRACT(T, N, SUFFIX)                                                     \
	extern "C" T vector_extract_##SUFFIX##_impl(vector_data<T, N>* v, int32_t idx) {          \
		return v->data[idx];                                                                  \
	}

#define VECTOR_IMPL_INSERT(T, N, SUFFIX)                                                                        \
	extern "C" vector_data<T, N>* vector_insert_##SUFFIX##_impl(vector_data<T, N>* v, T value, int32_t idx) { \
		auto* result = nextSlot<T, N>();                                                                    \
		for (size_t i = 0; i < N; i++) result->data[i] = v->data[i];                                       \
		result->data[idx] = value;                                                                          \
		return result;                                                                                      \
	}

#define VECTOR_IMPL_SHL(T, N, SUFFIX)                                                                           \
	extern "C" vector_data<T, N>* vector_shl_##SUFFIX##_impl(vector_data<T, N>* a, vector_data<T, N>* b) {     \
		auto* result = nextSlot<T, N>();                                                                    \
		for (size_t i = 0; i < N; i++) result->data[i] = a->data[i] << b->data[i];                         \
		return result;                                                                                      \
	}

#define VECTOR_IMPL_SHR(T, N, SUFFIX)                                                                           \
	extern "C" vector_data<T, N>* vector_shr_##SUFFIX##_impl(vector_data<T, N>* a, vector_data<T, N>* b) {     \
		auto* result = nextSlot<T, N>();                                                                    \
		for (size_t i = 0; i < N; i++) result->data[i] = a->data[i] >> b->data[i];                         \
		return result;                                                                                      \
	}

#define VECTOR_IMPL_ALL_OPS(T, N, SUFFIX)                         \
	VECTOR_IMPL_LOAD(T, N, SUFFIX)                                \
	VECTOR_IMPL_STORE(T, N, SUFFIX)                               \
	VECTOR_IMPL_BINARY(T, N, SUFFIX, add, +)                      \
	VECTOR_IMPL_BINARY(T, N, SUFFIX, sub, -)                      \
	VECTOR_IMPL_BINARY(T, N, SUFFIX, mul, *)                      \
	VECTOR_IMPL_BINARY(T, N, SUFFIX, div, /)                      \
	VECTOR_IMPL_UNARY_FUNC(T, N, SUFFIX, abs, std::abs)           \
	VECTOR_IMPL_NEG(T, N, SUFFIX)                                 \
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
	VECTOR_IMPL_BITWISE(T, N, SUFFIX, xor, ^)                     \
	VECTOR_IMPL_BROADCAST(T, N, SUFFIX)                           \
	VECTOR_IMPL_GATHER(T, N, SUFFIX)                              \
	VECTOR_IMPL_SCATTER(T, N, SUFFIX)                             \
	VECTOR_IMPL_EXTRACT(T, N, SUFFIX)                             \
	VECTOR_IMPL_INSERT(T, N, SUFFIX)

#define VECTOR_IMPL_FLOAT_OPS(T, N, SUFFIX)                       \
	VECTOR_IMPL_ALL_OPS(T, N, SUFFIX)                             \
	VECTOR_IMPL_FMA(T, N, SUFFIX)                                 \
	VECTOR_IMPL_REDUCE(T, N, SUFFIX, add, T(0), +)               \
	VECTOR_IMPL_REDUCE_FUNC(T, N, SUFFIX, min, a->data[0], std::min) \
	VECTOR_IMPL_REDUCE_FUNC(T, N, SUFFIX, max, a->data[0], std::max)

#define VECTOR_IMPL_INT_OPS(T, N, SUFFIX)                         \
	VECTOR_IMPL_ALL_OPS(T, N, SUFFIX)                             \
	VECTOR_IMPL_REDUCE(T, N, SUFFIX, add, T(0), +)               \
	VECTOR_IMPL_REDUCE_FUNC(T, N, SUFFIX, min, a->data[0], std::min) \
	VECTOR_IMPL_REDUCE_FUNC(T, N, SUFFIX, max, a->data[0], std::max) \
	VECTOR_IMPL_SHL(T, N, SUFFIX)                                \
	VECTOR_IMPL_SHR(T, N, SUFFIX)

// clang-format on

// ============================================================================
// Instantiate extern "C" implementations for all supported types and widths.
// All widths are compiled so the MLIR backend can intercept any of them.
// ============================================================================

VECTOR_IMPL_FLOAT_OPS(float, 4, f32x4)
VECTOR_IMPL_FLOAT_OPS(float, 8, f32x8)
VECTOR_IMPL_FLOAT_OPS(float, 16, f32x16)

VECTOR_IMPL_FLOAT_OPS(double, 2, f64x2)
VECTOR_IMPL_FLOAT_OPS(double, 4, f64x4)
VECTOR_IMPL_FLOAT_OPS(double, 8, f64x8)

VECTOR_IMPL_INT_OPS(int32_t, 4, i32x4)
VECTOR_IMPL_INT_OPS(int32_t, 8, i32x8)
VECTOR_IMPL_INT_OPS(int32_t, 16, i32x16)

VECTOR_IMPL_INT_OPS(int64_t, 2, i64x2)
VECTOR_IMPL_INT_OPS(int64_t, 4, i64x4)
VECTOR_IMPL_INT_OPS(int64_t, 8, i64x8)

// ============================================================================
// detail::invoke() wrappers — explicit specializations.
// Each calls invoke() with the corresponding extern "C" function pointer.
// All widths are instantiated so the MLIR intrinsic plugin can intercept them.
// ============================================================================

namespace detail {

// clang-format off

#define VEC_INVOKE_LOAD(T, N, SUFFIX)                                                                    \
	template <>                                                                                          \
	val<vector_data<T, N>*> vec_load<T, N>(val<const T*> ptr) {                                          \
		return invoke<vector_data<T, N>*, const T*>(vector_load_##SUFFIX##_impl, ptr);                   \
	}

#define VEC_INVOKE_STORE(T, N, SUFFIX)                                                                   \
	template <>                                                                                          \
	void vec_store<T, N>(val<T*> ptr, val<vector_data<T, N>*> v) {                                       \
		invoke<void, T*, vector_data<T, N>*>(vector_store_##SUFFIX##_impl, ptr, v);                      \
	}

#define VEC_INVOKE_BINARY(T, N, SUFFIX, NAME)                                                            \
	template <>                                                                                          \
	val<vector_data<T, N>*> vec_##NAME<T, N>(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b) {     \
		return invoke<vector_data<T, N>*, vector_data<T, N>*, vector_data<T, N>*>(                       \
		    vector_##NAME##_##SUFFIX##_impl, a, b);                                                      \
	}

#define VEC_INVOKE_UNARY(T, N, SUFFIX, NAME)                                                             \
	template <>                                                                                          \
	val<vector_data<T, N>*> vec_##NAME<T, N>(val<vector_data<T, N>*> a) {                                \
		return invoke<vector_data<T, N>*, vector_data<T, N>*>(vector_##NAME##_##SUFFIX##_impl, a);       \
	}

#define VEC_INVOKE_TERNARY(T, N, SUFFIX, NAME)                                                           \
	template <>                                                                                          \
	val<vector_data<T, N>*> vec_##NAME<T, N>(val<vector_data<T, N>*> a, val<vector_data<T, N>*> b,       \
	                                         val<vector_data<T, N>*> c) {                                \
		return invoke<vector_data<T, N>*, vector_data<T, N>*, vector_data<T, N>*, vector_data<T, N>*>(   \
		    vector_##NAME##_##SUFFIX##_impl, a, b, c);                                                   \
	}

#define VEC_INVOKE_REDUCE(T, N, SUFFIX, NAME)                                                            \
	template <>                                                                                          \
	val<T> vec_reduce_##NAME<T, N>(val<vector_data<T, N>*> a) {                                          \
		return invoke<T, vector_data<T, N>*>(vector_reduce_##NAME##_##SUFFIX##_impl, a);                 \
	}

#define VEC_INVOKE_BROADCAST(T, N, SUFFIX)                                                                \
	template <>                                                                                          \
	val<vector_data<T, N>*> vec_broadcast<T, N>(val<T> scalar) {                                         \
		return invoke<vector_data<T, N>*, T>(vector_broadcast_##SUFFIX##_impl, scalar);                  \
	}

#define VEC_INVOKE_GATHER(T, N, SUFFIX)                                                                   \
	template <>                                                                                          \
	val<vector_data<T, N>*> vec_gather<T, N>(val<const T*> base, val<const int32_t*> indices) {           \
		return invoke<vector_data<T, N>*, const T*, const int32_t*>(                                     \
		    vector_gather_##SUFFIX##_impl, base, indices);                                               \
	}

#define VEC_INVOKE_SCATTER(T, N, SUFFIX)                                                                  \
	template <>                                                                                          \
	void vec_scatter<T, N>(val<T*> base, val<const int32_t*> indices, val<vector_data<T, N>*> data) {    \
		invoke<void, T*, const int32_t*, vector_data<T, N>*>(                                            \
		    vector_scatter_##SUFFIX##_impl, base, indices, data);                                        \
	}

#define VEC_INVOKE_EXTRACT(T, N, SUFFIX)                                                                  \
	template <>                                                                                          \
	val<T> vec_extract<T, N>(val<vector_data<T, N>*> v, val<int32_t> idx) {                              \
		return invoke<T, vector_data<T, N>*, int32_t>(vector_extract_##SUFFIX##_impl, v, idx);           \
	}

#define VEC_INVOKE_INSERT(T, N, SUFFIX)                                                                   \
	template <>                                                                                          \
	val<vector_data<T, N>*> vec_insert<T, N>(val<vector_data<T, N>*> v, val<T> value, val<int32_t> idx) {\
		return invoke<vector_data<T, N>*, vector_data<T, N>*, T, int32_t>(                               \
		    vector_insert_##SUFFIX##_impl, v, value, idx);                                               \
	}

#define VEC_INVOKE_ALL(T, N, SUFFIX)                                                                     \
	VEC_INVOKE_LOAD(T, N, SUFFIX)                                                                        \
	VEC_INVOKE_STORE(T, N, SUFFIX)                                                                       \
	VEC_INVOKE_BINARY(T, N, SUFFIX, add)                                                                 \
	VEC_INVOKE_BINARY(T, N, SUFFIX, sub)                                                                 \
	VEC_INVOKE_BINARY(T, N, SUFFIX, mul)                                                                 \
	VEC_INVOKE_BINARY(T, N, SUFFIX, div)                                                                 \
	VEC_INVOKE_UNARY(T, N, SUFFIX, abs)                                                                  \
	VEC_INVOKE_UNARY(T, N, SUFFIX, neg)                                                                  \
	VEC_INVOKE_BINARY(T, N, SUFFIX, min)                                                                 \
	VEC_INVOKE_BINARY(T, N, SUFFIX, max)                                                                 \
	VEC_INVOKE_REDUCE(T, N, SUFFIX, add)                                                                 \
	VEC_INVOKE_REDUCE(T, N, SUFFIX, min)                                                                 \
	VEC_INVOKE_REDUCE(T, N, SUFFIX, max)                                                                 \
	VEC_INVOKE_BINARY(T, N, SUFFIX, eq)                                                                  \
	VEC_INVOKE_BINARY(T, N, SUFFIX, ne)                                                                  \
	VEC_INVOKE_BINARY(T, N, SUFFIX, lt)                                                                  \
	VEC_INVOKE_BINARY(T, N, SUFFIX, le)                                                                  \
	VEC_INVOKE_BINARY(T, N, SUFFIX, gt)                                                                  \
	VEC_INVOKE_BINARY(T, N, SUFFIX, ge)                                                                  \
	VEC_INVOKE_TERNARY(T, N, SUFFIX, blend)                                                              \
	VEC_INVOKE_BINARY(T, N, SUFFIX, and)                                                                 \
	VEC_INVOKE_BINARY(T, N, SUFFIX, or)                                                                  \
	VEC_INVOKE_BINARY(T, N, SUFFIX, xor)                                                                 \
	VEC_INVOKE_BROADCAST(T, N, SUFFIX)                                                                   \
	VEC_INVOKE_GATHER(T, N, SUFFIX)                                                                      \
	VEC_INVOKE_SCATTER(T, N, SUFFIX)                                                                     \
	VEC_INVOKE_EXTRACT(T, N, SUFFIX)                                                                     \
	VEC_INVOKE_INSERT(T, N, SUFFIX)

#define VEC_INVOKE_FLOAT_ALL(T, N, SUFFIX)                                                               \
	VEC_INVOKE_ALL(T, N, SUFFIX)                                                                         \
	VEC_INVOKE_TERNARY(T, N, SUFFIX, fma)

// clang-format on

// 128-bit (SSE / NEON)
VEC_INVOKE_FLOAT_ALL(float, 4, f32x4)
VEC_INVOKE_FLOAT_ALL(double, 2, f64x2)
VEC_INVOKE_ALL(int32_t, 4, i32x4)
VEC_INVOKE_ALL(int64_t, 2, i64x2)
VEC_INVOKE_BINARY(int32_t, 4, i32x4, shl)
VEC_INVOKE_BINARY(int32_t, 4, i32x4, shr)
VEC_INVOKE_BINARY(int64_t, 2, i64x2, shl)
VEC_INVOKE_BINARY(int64_t, 2, i64x2, shr)

// 256-bit (AVX / AVX2)
VEC_INVOKE_FLOAT_ALL(float, 8, f32x8)
VEC_INVOKE_FLOAT_ALL(double, 4, f64x4)
VEC_INVOKE_ALL(int32_t, 8, i32x8)
VEC_INVOKE_ALL(int64_t, 4, i64x4)
VEC_INVOKE_BINARY(int32_t, 8, i32x8, shl)
VEC_INVOKE_BINARY(int32_t, 8, i32x8, shr)
VEC_INVOKE_BINARY(int64_t, 4, i64x4, shl)
VEC_INVOKE_BINARY(int64_t, 4, i64x4, shr)

// 512-bit (AVX-512)
VEC_INVOKE_FLOAT_ALL(float, 16, f32x16)
VEC_INVOKE_FLOAT_ALL(double, 8, f64x8)
VEC_INVOKE_ALL(int32_t, 16, i32x16)
VEC_INVOKE_ALL(int64_t, 8, i64x8)
VEC_INVOKE_BINARY(int32_t, 16, i32x16, shl)
VEC_INVOKE_BINARY(int32_t, 16, i32x16, shr)
VEC_INVOKE_BINARY(int64_t, 8, i64x8, shl)
VEC_INVOKE_BINARY(int64_t, 8, i64x8, shr)

} // namespace detail

} // namespace nautilus
