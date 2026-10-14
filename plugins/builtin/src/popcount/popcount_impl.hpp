#pragma once

#include <cstdint>

// Forward declarations for the extern "C" popcount implementation functions.
// There is one function per integer type: the function pointer identity is
// what the MLIR and AsmJit intrinsic plugins key their lowering on, and the
// AsmJit handler needs to know the operand's width and signedness (see
// MLIRPopcountIntrinsics.cpp and AsmJitPopcountIntrinsics.cpp). Defined in
// popcount.cpp.

namespace nautilus::builtin::detail {

extern "C" int8_t nautilus_popcount_i8(int8_t x);
extern "C" int16_t nautilus_popcount_i16(int16_t x);
extern "C" int32_t nautilus_popcount_i32(int32_t x);
extern "C" int64_t nautilus_popcount_i64(int64_t x);
extern "C" uint8_t nautilus_popcount_u8(uint8_t x);
extern "C" uint16_t nautilus_popcount_u16(uint16_t x);
extern "C" uint32_t nautilus_popcount_u32(uint32_t x);
extern "C" uint64_t nautilus_popcount_u64(uint64_t x);

} // namespace nautilus::builtin::detail
