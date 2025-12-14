#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/std/bit.h>

namespace nautilus::engine {

// ============================================================================
// Bit Intrinsic Test Functions - Unary Operations
// ============================================================================

// Test function for countl_zero intrinsic
val<uint32_t> countlZeroFunction(val<uint32_t> x) {
	return countl_zero(x);
}

// Test function for countr_zero intrinsic
val<uint32_t> countrZeroFunction(val<uint32_t> x) {
	return countr_zero(x);
}

// Test function for popcount intrinsic
val<uint32_t> popcountFunction(val<uint32_t> x) {
	return popcount(x);
}

// Test function for countl_zero (64-bit) intrinsic
val<uint64_t> countlZero64Function(val<uint64_t> x) {
	return countl_zero(x);
}

// Test function for countr_zero (64-bit) intrinsic
val<uint64_t> countrZero64Function(val<uint64_t> x) {
	return countr_zero(x);
}

// Test function for popcount (64-bit) intrinsic
val<uint64_t> popcount64Function(val<uint64_t> x) {
	return popcount(x);
}

// Test function for byteswap (32-bit) intrinsic
val<uint32_t> byteswap32Function(val<uint32_t> x) {
	return byteswap(x);
}

// Test function for byteswap (64-bit) intrinsic
val<uint64_t> byteswap64Function(val<uint64_t> x) {
	return byteswap(x);
}

// ============================================================================
// Bit Intrinsic Test Functions - Binary Operations
// ============================================================================

// Test function for rotl (rotate left) intrinsic
val<uint32_t> rotlFunction(val<uint32_t> x, val<uint32_t> s) {
	return rotl(x, s);
}

// Test function for rotr (rotate right) intrinsic
val<uint32_t> rotrFunction(val<uint32_t> x, val<uint32_t> s) {
	return rotr(x, s);
}

// Test function for rotl (64-bit) intrinsic
val<uint64_t> rotl64Function(val<uint64_t> x, val<uint64_t> s) {
	return rotl(x, s);
}

// Test function for rotr (64-bit) intrinsic
val<uint64_t> rotr64Function(val<uint64_t> x, val<uint64_t> s) {
	return rotr(x, s);
}

} // namespace nautilus::engine
