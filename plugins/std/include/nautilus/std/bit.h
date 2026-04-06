#pragma once
#include <nautilus/val.hpp>

namespace nautilus {

// ============================================================================
// Bit manipulation functions (C++20 <bit> header)
// ============================================================================

/**
 * @brief Counts the number of consecutive 0 bits in the value of x, starting
 * from the most significant bit ("left").
 *
 * @param x Unsigned integer value.
 * @return Number of consecutive 0 bits from the left.
 */
val<uint8_t> countl_zero(val<uint8_t> x);
val<uint16_t> countl_zero(val<uint16_t> x);
val<uint32_t> countl_zero(val<uint32_t> x);
val<uint64_t> countl_zero(val<uint64_t> x);

/**
 * @brief Counts the number of consecutive 1 bits in the value of x, starting
 * from the most significant bit ("left").
 *
 * @param x Unsigned integer value.
 * @return Number of consecutive 1 bits from the left.
 */
val<uint8_t> countl_one(val<uint8_t> x);
val<uint16_t> countl_one(val<uint16_t> x);
val<uint32_t> countl_one(val<uint32_t> x);
val<uint64_t> countl_one(val<uint64_t> x);

/**
 * @brief Counts the number of consecutive 0 bits in the value of x, starting
 * from the least significant bit ("right").
 *
 * @param x Unsigned integer value.
 * @return Number of consecutive 0 bits from the right.
 */
val<uint8_t> countr_zero(val<uint8_t> x);
val<uint16_t> countr_zero(val<uint16_t> x);
val<uint32_t> countr_zero(val<uint32_t> x);
val<uint64_t> countr_zero(val<uint64_t> x);

/**
 * @brief Counts the number of consecutive 1 bits in the value of x, starting
 * from the least significant bit ("right").
 *
 * @param x Unsigned integer value.
 * @return Number of consecutive 1 bits from the right.
 */
val<uint8_t> countr_one(val<uint8_t> x);
val<uint16_t> countr_one(val<uint16_t> x);
val<uint32_t> countr_one(val<uint32_t> x);
val<uint64_t> countr_one(val<uint64_t> x);

/**
 * @brief Counts the number of 1 bits in the value of x (population count).
 *
 * @param x Unsigned integer value.
 * @return Number of 1 bits in x.
 */
val<uint8_t> popcount(val<uint8_t> x);
val<uint16_t> popcount(val<uint16_t> x);
val<uint32_t> popcount(val<uint32_t> x);
val<uint64_t> popcount(val<uint64_t> x);

/**
 * @brief Checks if x is a power of two (has only a single 1 bit).
 *
 * @param x Unsigned integer value.
 * @return true if x is a power of two, false otherwise.
 */
val<bool> has_single_bit(val<uint8_t> x);
val<bool> has_single_bit(val<uint16_t> x);
val<bool> has_single_bit(val<uint32_t> x);
val<bool> has_single_bit(val<uint64_t> x);

/**
 * @brief Calculates the number of bits needed to represent the value x.
 *
 * @param x Unsigned integer value.
 * @return Number of bits needed to represent x.
 */
val<uint8_t> bit_width(val<uint8_t> x);
val<uint16_t> bit_width(val<uint16_t> x);
val<uint32_t> bit_width(val<uint32_t> x);
val<uint64_t> bit_width(val<uint64_t> x);

/**
 * @brief Finds the smallest power of two not less than x.
 *
 * @param x Unsigned integer value.
 * @return Smallest power of two >= x.
 */
val<uint8_t> bit_ceil(val<uint8_t> x);
val<uint16_t> bit_ceil(val<uint16_t> x);
val<uint32_t> bit_ceil(val<uint32_t> x);
val<uint64_t> bit_ceil(val<uint64_t> x);

/**
 * @brief Finds the largest power of two not greater than x.
 *
 * @param x Unsigned integer value.
 * @return Largest power of two <= x.
 */
val<uint8_t> bit_floor(val<uint8_t> x);
val<uint16_t> bit_floor(val<uint16_t> x);
val<uint32_t> bit_floor(val<uint32_t> x);
val<uint64_t> bit_floor(val<uint64_t> x);

/**
 * @brief Rotates the bits in x left by s positions.
 *
 * @param x Unsigned integer value.
 * @param s Number of positions to rotate.
 * @return x rotated left by s positions.
 */
val<uint8_t> rotl(val<uint8_t> x, val<uint8_t> s);
val<uint16_t> rotl(val<uint16_t> x, val<uint16_t> s);
val<uint32_t> rotl(val<uint32_t> x, val<uint32_t> s);
val<uint64_t> rotl(val<uint64_t> x, val<uint64_t> s);

/**
 * @brief Rotates the bits in x right by s positions.
 *
 * @param x Unsigned integer value.
 * @param s Number of positions to rotate.
 * @return x rotated right by s positions.
 */
val<uint8_t> rotr(val<uint8_t> x, val<uint8_t> s);
val<uint16_t> rotr(val<uint16_t> x, val<uint16_t> s);
val<uint32_t> rotr(val<uint32_t> x, val<uint32_t> s);
val<uint64_t> rotr(val<uint64_t> x, val<uint64_t> s);

/**
 * @brief Reverses the byte order of x (byteswap).
 *
 * @param x Unsigned integer value.
 * @return x with bytes in reversed order.
 */
val<uint16_t> byteswap(val<uint16_t> x);
val<uint32_t> byteswap(val<uint32_t> x);
val<uint64_t> byteswap(val<uint64_t> x);

} // namespace nautilus
