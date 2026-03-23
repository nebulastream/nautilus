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
	DECLARE_VECTOR_BINARY(T, N, SUFFIX, xor)

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

// int64_t
DECLARE_VECTOR_ALL(int64_t, 2, i64x2)
DECLARE_VECTOR_ALL(int64_t, 4, i64x4)
DECLARE_VECTOR_ALL(int64_t, 8, i64x8)

// clang-format on

} // namespace nautilus
