#pragma once

#include <cstdint>
#include <nautilus/val.hpp>

namespace nautilus::builtin {

/// Returns the number of set bits in `x` (population count), mirroring
/// GCC/Clang's `__builtin_popcount` family. The bits are counted on the
/// two's-complement representation at the width of `T`, so for signed types
/// `popcount(val<int8_t>(-1)) == 8`. The result has the same type as the
/// input and always lies in `[0, bit width of T]`.
///
/// On the MLIR backend this lowers to LLVM's `llvm.ctpop` intrinsic, and on
/// the AsmJit backend to a native population-count sequence (x86-64 POPCNT
/// when the host supports it, AArch64 CNT + ADDV). Every other backend calls a
/// portable `std::popcount`-based fallback.
val<int8_t> popcount(val<int8_t> x);
val<int16_t> popcount(val<int16_t> x);
val<int32_t> popcount(val<int32_t> x);
val<int64_t> popcount(val<int64_t> x);
val<uint8_t> popcount(val<uint8_t> x);
val<uint16_t> popcount(val<uint16_t> x);
val<uint32_t> popcount(val<uint32_t> x);
val<uint64_t> popcount(val<uint64_t> x);

} // namespace nautilus::builtin
