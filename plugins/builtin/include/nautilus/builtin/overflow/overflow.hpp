#pragma once

#include <cstdint>
#include <nautilus/val.hpp>
#include <nautilus/val_ptr.hpp>

namespace nautilus::builtin {

namespace detail {
// Portable runtime helpers (AsmJit, C++, bytecode, interpreter); the MLIR backend replaces each
// call with the matching LLVM `*.with.overflow` intrinsic. They store the wrapped result through
// `out` and return whether the exact result did not fit.
bool add_overflow_i32_impl(int32_t a, int32_t b, int32_t* out) noexcept;
bool sub_overflow_i32_impl(int32_t a, int32_t b, int32_t* out) noexcept;
bool mul_overflow_i32_impl(int32_t a, int32_t b, int32_t* out) noexcept;
bool add_overflow_i64_impl(int64_t a, int64_t b, int64_t* out) noexcept;
bool sub_overflow_i64_impl(int64_t a, int64_t b, int64_t* out) noexcept;
bool mul_overflow_i64_impl(int64_t a, int64_t b, int64_t* out) noexcept;
// 128-bit operands in memory (16 bytes, any alignment): `out` may alias `a` or `b`.
bool add_overflow_i128_impl(const void* a, const void* b, void* out) noexcept;
bool sub_overflow_i128_impl(const void* a, const void* b, void* out) noexcept;
bool mul_overflow_i128_impl(const void* a, const void* b, void* out) noexcept;
} // namespace detail

/// Overflow-checked signed arithmetic (like `__builtin_{add,sub,mul}_overflow`): writes the
/// wrapped result to `*out` and returns true when the exact result does not fit the type.
/// MLIR lowers these to `llvm.s{add,sub,mul}.with.overflow`, so no division or widening
/// is needed to detect a multiply overflow.
val<bool> add_overflow(val<int32_t> a, val<int32_t> b, val<int32_t*> out);
val<bool> sub_overflow(val<int32_t> a, val<int32_t> b, val<int32_t*> out);
val<bool> mul_overflow(val<int32_t> a, val<int32_t> b, val<int32_t*> out);
val<bool> add_overflow(val<int64_t> a, val<int64_t> b, val<int64_t*> out);
val<bool> sub_overflow(val<int64_t> a, val<int64_t> b, val<int64_t*> out);
val<bool> mul_overflow(val<int64_t> a, val<int64_t> b, val<int64_t*> out);

/// The same for signed 128-bit integers held in memory (16 bytes each, any alignment; `out`
/// may alias an operand). MLIR lowers them to the `i128` intrinsics.
val<bool> add_overflow_i128(val<const void*> a, val<const void*> b, val<void*> out);
val<bool> sub_overflow_i128(val<const void*> a, val<const void*> b, val<void*> out);
val<bool> mul_overflow_i128(val<const void*> a, val<const void*> b, val<void*> out);

} // namespace nautilus::builtin
