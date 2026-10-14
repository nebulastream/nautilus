#include "popcount_impl.hpp"
#include <bit>
#include <nautilus/builtin/popcount/popcount.hpp>
#include <nautilus/function.hpp>
#include <type_traits>

// ============================================================================
// Portable fallback implementations.
//
// These are the bodies that run whenever a backend does not intercept the
// call with a native instruction (MLIR/AsmJit with intrinsics registered do;
// the C++, bytecode and interpreter paths all fall through to these). Signed
// inputs are counted on their two's-complement bit pattern at their own
// width, which is what `llvm.ctpop` on the same-width integer computes.
// ============================================================================

namespace nautilus::builtin::detail {

namespace {
template <typename T>
T popcountImpl(T x) {
	return static_cast<T>(std::popcount(static_cast<std::make_unsigned_t<T>>(x)));
}
} // namespace

extern "C" int8_t nautilus_popcount_i8(int8_t x) {
	return popcountImpl(x);
}

extern "C" int16_t nautilus_popcount_i16(int16_t x) {
	return popcountImpl(x);
}

extern "C" int32_t nautilus_popcount_i32(int32_t x) {
	return popcountImpl(x);
}

extern "C" int64_t nautilus_popcount_i64(int64_t x) {
	return popcountImpl(x);
}

extern "C" uint8_t nautilus_popcount_u8(uint8_t x) {
	return popcountImpl(x);
}

extern "C" uint16_t nautilus_popcount_u16(uint16_t x) {
	return popcountImpl(x);
}

extern "C" uint32_t nautilus_popcount_u32(uint32_t x) {
	return popcountImpl(x);
}

extern "C" uint64_t nautilus_popcount_u64(uint64_t x) {
	return popcountImpl(x);
}

} // namespace nautilus::builtin::detail

namespace nautilus::builtin {

val<int8_t> popcount(val<int8_t> x) {
	return invoke<int8_t, int8_t>(detail::nautilus_popcount_i8, x);
}

val<int16_t> popcount(val<int16_t> x) {
	return invoke<int16_t, int16_t>(detail::nautilus_popcount_i16, x);
}

val<int32_t> popcount(val<int32_t> x) {
	return invoke<int32_t, int32_t>(detail::nautilus_popcount_i32, x);
}

val<int64_t> popcount(val<int64_t> x) {
	return invoke<int64_t, int64_t>(detail::nautilus_popcount_i64, x);
}

val<uint8_t> popcount(val<uint8_t> x) {
	return invoke<uint8_t, uint8_t>(detail::nautilus_popcount_u8, x);
}

val<uint16_t> popcount(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(detail::nautilus_popcount_u16, x);
}

val<uint32_t> popcount(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(detail::nautilus_popcount_u32, x);
}

val<uint64_t> popcount(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(detail::nautilus_popcount_u64, x);
}

} // namespace nautilus::builtin
