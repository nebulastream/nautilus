#pragma once
#include <nautilus/val_ptr.hpp>

namespace nautilus {

/**
 * Copies `count` bytes from memory area `src` to memory area `dest`.
 *
 * @param dest Destination memory area.
 * @param src Source memory area.
 * @param count Number of bytes to copy.
 * @return Pointer to the destination memory area `dest`.
 */
val<void*> memcpy(val<void*> dest, val<const void*> src, val<size_t> count);

/**
 * Moves `n` bytes from memory area `s2` to memory area `s1`.
 * Memory areas may overlap.
 *
 * @param s1 Destination memory area.
 * @param s2 Source memory area.
 * @param n Number of bytes to move.
 * @return Pointer to the destination memory area `s1`.
 */
val<void*> memmove(val<void*> s1, val<const void*> s2, val<size_t> n);

/**
 * Copies the C-string `s2` to `s1` (including the null terminator).
 *
 * @param s1 Destination C-string.
 * @param s2 Source C-string.
 * @return Pointer to the destination C-string `s1`.
 */
val<char*> strcpy(val<char*> s1, val<const char*> s2);

/**
 * Copies up to `n` characters from the C-string `s2` to `s1`.
 * If `s2` is less than `n` characters long, the remainder of `s1` is filled
 * with null characters.
 *
 * @param s1 Destination C-string.
 * @param s2 Source C-string.
 * @param n Maximum number of characters to copy.
 * @return Pointer to the destination C-string `s1`.
 */
val<char*> strncpy(val<char*> s1, val<const char*> s2, val<size_t> n);

/**
 * Appends the C-string `s2` to `s1` (including the null terminator).
 *
 * @param s1 Destination C-string.
 * @param s2 Source C-string.
 * @return Pointer to the destination C-string `s1`.
 */
val<char*> strcat(val<char*> s1, val<const char*> s2);

/**
 * Appends up to `n` characters from the C-string `s2` to `s1`.
 * `s1` will be null-terminated.
 *
 * @param s1 Destination C-string.
 * @param s2 Source C-string.
 * @param n Maximum number of characters to append.
 * @return Pointer to the destination C-string `s1`.
 */
val<char*> strncat(val<char*> s1, val<const char*> s2, val<size_t> n);

/**
 * Compares the first `n` bytes of memory areas `s1` and `s2`.
 *
 * @param s1 First memory area.
 * @param s2 Second memory area.
 * @param n Number of bytes to compare.
 * @return An integer less than, equal to, or greater than zero if `s1` is
 * found, respectively, to be less than, to match, or be greater than `s2`.
 */
val<int> memcmp(val<void*> s1, val<void*> s2, val<size_t> n);

/**
 * Compares the C-strings `s1` and `s2`.
 *
 * @param s1 First C-string.
 * @param s2 Second C-string.
 * @return An integer less than, equal to, or greater than zero if `s1` is
 * found, respectively, to be less than, to match, or be greater than `s2`.
 */
val<int> strcmp(val<const char*> s1, val<const char*> s2);

/**
 * Compares the C-strings `s1` and `s2` according to the current locale.
 *
 * @param s1 First C-string.
 * @param s2 Second C-string.
 * @return An integer less than, equal to, or greater than zero if `s1` is
 * found, respectively, to be less than, to match, or be greater than `s2`.
 */
val<int> strcoll(val<const char*> s1, val<const char*> s2);

/**
 * Compares up to `n` characters of the C-strings `s1` and `s2`.
 *
 * @param s1 First C-string.
 * @param s2 Second C-string.
 * @param n Maximum number of characters to compare.
 * @return An integer less than, equal to, or greater than zero if `s1` is
 * found, respectively, to be less than, to match, or be greater than `s2`.
 */
val<int> strncmp(val<const char*> s1, val<const char*> s2, val<size_t> n);

/**
 * Transforms the C-string `s2` into a form that can be compared using `strcoll`
 * and places the resulting string into `s1`.
 *
 * @param s1 Destination C-string.
 * @param s2 Source C-string.
 * @param n Maximum number of characters to write (including null character).
 * @return The length of the transformed string (not including the null
 * character).
 */
val<size_t> strxfrm(val<char*> s1, val<const char*> s2, val<size_t> n);

/**
 * Searches for the first occurrence of the character `c` (interpreted as an
 * unsigned char) in the first `n` bytes of the memory area `s`.
 *
 * @param s Memory area.
 * @param c Character to search for.
 * @param n Number of bytes to search.
 * @return A pointer to the matching byte, or nullptr if the character does not
 * occur.
 */
val<const void*> memchr(val<const void*> s, val<int> c, val<size_t> n);
val<void*> memchr(val<void*> s, val<int> c, val<size_t> n);

/**
 * Searches for the first occurrence of the character `c` in the C-string `s`.
 *
 * @param s C-string.
 * @param c Character to search for.
 * @return A pointer to the matching character, or nullptr if the character does
 * not occur.
 */
val<const char*> strchr(val<const char*> s, val<int> c);
val<char*> strchr(val<char*> s, val<int> c);

/**
 * Computes the length of the initial segment of the C-string `s1`
 * which consists entirely of characters not in the C-string `s2`.
 *
 * @param s1 First C-string.
 * @param s2 Second C-string.
 * @return The length of the initial segment of `s1` containing no characters
 * from `s2`.
 */
val<size_t> strcspn(val<const char*> s1, val<const char*> s2);

/**
 * Searches the C-string `s1` for any of the characters in the C-string `s2`.
 *
 * @param s1 First C-string.
 * @param s2 Second C-string.
 * @return A pointer to the first occurrence in `s1` of any of the characters in
 * `s2`, or nullptr if no such character is found.
 */
val<const char*> strpbrk(val<const char*> s1, val<const char*> s2);
val<char*> strpbrk(val<char*> s1, val<const char*> s2);

/**
 * Searches for the last occurrence of the character `c` in the C-string `s`.
 *
 * @param s C-string.
 * @param c Character to search for.
 * @return A pointer to the matching character, or nullptr if the character does
 * not occur.
 */
val<const char*> strrchr(val<const char*> s, val<int> c);
val<char*> strrchr(val<char*> s, val<int> c);

/**
 * Computes the length of the initial segment of the C-string `s1`
 * which consists entirely of characters in the C-string `s2`.
 *
 * @param s1 First C-string.
 * @param s2 Second C-string.
 * @return The length of the initial segment of `s1` containing only characters
 * from `s2`.
 */
val<size_t> strspn(val<const char*> s1, val<const char*> s2);

/**
 * Searches for the first occurrence of the C-string `s2` in the C-string `s1`.
 *
 * @param s1 First C-string.
 * @param s2 Second C-string.
 * @return A pointer to the first occurrence of `s2` in `s1`, or nullptr if `s2`
 * is not found.
 */
val<const char*> strstr(val<const char*> s1, val<const char*> s2);
val<char*> strstr(val<char*> s1, val<const char*> s2);

/**
 * Breaks the C-string `s1` into tokens separated by characters in the C-string
 * `s2`.
 *
 * @param s1 C-string to tokenize (subsequent calls should be nullptr).
 * @param s2 C-string containing delimiter characters.
 * @return A pointer to the next token, or nullptr if there are no more tokens.
 */
val<char*> strtok(val<char*> s1, val<const char*> s2);

/**
 * Fills the first `n` bytes of the memory area pointed to by `s` with the
 * constant byte `c`.
 *
 * @param s Memory area.
 * @param c Value to be set.
 * @param n Number of bytes to be set.
 * @return A pointer to the memory area `s`.
 */
val<void*> memset(val<void*> s, val<int> c, val<size_t> n);

/**
 * Returns a pointer to the textual representation of the error number `errnum`.
 *
 * @param errnum Error number.
 * @return A pointer to the textual representation of `errnum`.
 */
val<char*> strerror(val<int> errnum);

/**
 * Computes the length of the C-string `s` (excluding the null terminator).
 *
 * @param s C-string.
 * @return The number of characters in the C-string `s`.
 */
val<size_t> strlen(val<const char*> s);

} // namespace nautilus
