#pragma once

#include <nautilus/vector.hpp>

// ============================================================================
// Forward declarations for all extern "C" vector implementation functions.
// These are defined in vector.cpp and referenced by the MLIR intrinsic plugin.
// ============================================================================

namespace nautilus {

// clang-format off

#define DECLARE_VECTOR_LOAD_STORE(T, N, SUFFIX)                            \
	extern "C" vector_data<T, N>* vector_load_##SUFFIX##_impl(const T*);   \
	extern "C" void vector_store_##SUFFIX##_impl(T*, vector_data<T, N>*);

#define DECLARE_VECTOR_BINARY(T, N, SUFFIX, NAME)                          \
	extern "C" vector_data<T, N>* vector_##NAME##_##SUFFIX##_impl(vector_data<T, N>*, vector_data<T, N>*);

#define DECLARE_VECTOR_UNARY(T, N, SUFFIX, NAME)                           \
	extern "C" vector_data<T, N>* vector_##NAME##_##SUFFIX##_impl(vector_data<T, N>*);

#define DECLARE_VECTOR_TERNARY(T, N, SUFFIX, NAME)                         \
	extern "C" vector_data<T, N>* vector_##NAME##_##SUFFIX##_impl(vector_data<T, N>*, vector_data<T, N>*, vector_data<T, N>*);

#define DECLARE_VECTOR_REDUCE(T, N, SUFFIX, NAME)                          \
	extern "C" T vector_reduce_##NAME##_##SUFFIX##_impl(vector_data<T, N>*);

#define DECLARE_VECTOR_BROADCAST(T, N, SUFFIX)                             \
	extern "C" vector_data<T, N>* vector_broadcast_##SUFFIX##_impl(T);

#define DECLARE_VECTOR_GATHER(T, N, SUFFIX)                                \
	extern "C" vector_data<T, N>* vector_gather_##SUFFIX##_impl(const T*, const int32_t*);

#define DECLARE_VECTOR_SCATTER(T, N, SUFFIX)                               \
	extern "C" void vector_scatter_##SUFFIX##_impl(T*, const int32_t*, vector_data<T, N>*);

#define DECLARE_VECTOR_EXTRACT(T, N, SUFFIX)                               \
	extern "C" T vector_extract_##SUFFIX##_impl(vector_data<T, N>*, int32_t);

#define DECLARE_VECTOR_INSERT(T, N, SUFFIX)                                \
	extern "C" vector_data<T, N>* vector_insert_##SUFFIX##_impl(vector_data<T, N>*, T, int32_t);

#define DECLARE_VECTOR_ALL(T, N, SUFFIX)                                   \
	DECLARE_VECTOR_LOAD_STORE(T, N, SUFFIX)                                \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, add)                               \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, sub)                               \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, mul)                               \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, div)                               \
	DECLARE_VECTOR_UNARY(T, N, SUFFIX, abs)                                \
	DECLARE_VECTOR_UNARY(T, N, SUFFIX, neg)                                \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, min)                               \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, max)                               \
	DECLARE_VECTOR_REDUCE(T, N, SUFFIX, add)                               \
	DECLARE_VECTOR_REDUCE(T, N, SUFFIX, min)                               \
	DECLARE_VECTOR_REDUCE(T, N, SUFFIX, max)                               \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, eq)                                \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, ne)                                \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, lt)                                \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, le)                                \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, gt)                                \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, ge)                                \
	DECLARE_VECTOR_TERNARY(T, N, SUFFIX, blend)                            \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, and)                               \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, or)                                \
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, xor)                               \
	DECLARE_VECTOR_BROADCAST(T, N, SUFFIX)                                 \
	DECLARE_VECTOR_GATHER(T, N, SUFFIX)                                    \
	DECLARE_VECTOR_SCATTER(T, N, SUFFIX)                                   \
	DECLARE_VECTOR_EXTRACT(T, N, SUFFIX)                                   \
	DECLARE_VECTOR_INSERT(T, N, SUFFIX)

#define DECLARE_VECTOR_FLOAT_ALL(T, N, SUFFIX)                             \
	DECLARE_VECTOR_ALL(T, N, SUFFIX)                                       \
	DECLARE_VECTOR_TERNARY(T, N, SUFFIX, fma)

// float
DECLARE_VECTOR_FLOAT_ALL(float, 4, f32x4)
DECLARE_VECTOR_FLOAT_ALL(float, 8, f32x8)
DECLARE_VECTOR_FLOAT_ALL(float, 16, f32x16)

// double
DECLARE_VECTOR_FLOAT_ALL(double, 2, f64x2)
DECLARE_VECTOR_FLOAT_ALL(double, 4, f64x4)
DECLARE_VECTOR_FLOAT_ALL(double, 8, f64x8)

// int32_t
DECLARE_VECTOR_ALL(int32_t, 4, i32x4)
DECLARE_VECTOR_ALL(int32_t, 8, i32x8)
DECLARE_VECTOR_ALL(int32_t, 16, i32x16)
DECLARE_VECTOR_BINARY(int32_t, 4, i32x4, shl)
DECLARE_VECTOR_BINARY(int32_t, 8, i32x8, shl)
DECLARE_VECTOR_BINARY(int32_t, 16, i32x16, shl)
DECLARE_VECTOR_BINARY(int32_t, 4, i32x4, shr)
DECLARE_VECTOR_BINARY(int32_t, 8, i32x8, shr)
DECLARE_VECTOR_BINARY(int32_t, 16, i32x16, shr)

// int64_t
DECLARE_VECTOR_ALL(int64_t, 2, i64x2)
DECLARE_VECTOR_ALL(int64_t, 4, i64x4)
DECLARE_VECTOR_ALL(int64_t, 8, i64x8)
DECLARE_VECTOR_BINARY(int64_t, 2, i64x2, shl)
DECLARE_VECTOR_BINARY(int64_t, 4, i64x4, shl)
DECLARE_VECTOR_BINARY(int64_t, 8, i64x8, shl)
DECLARE_VECTOR_BINARY(int64_t, 2, i64x2, shr)
DECLARE_VECTOR_BINARY(int64_t, 4, i64x4, shr)
DECLARE_VECTOR_BINARY(int64_t, 8, i64x8, shr)

// clang-format on

} // namespace nautilus
