#pragma once

#include <nautilus/val.hpp>
namespace nautilus {

/**
 * @brief Allocates size bytes of uninitialized storage.
 * If allocation succeeds, returns a pointer to the lowest (first) byte in the
 * allocated memory block that is suitably aligned for any scalar type (at least
 * as strictly as std::max_align_t) (implicitly creating objects in the
 * destination area). If size is zero, the behavior is implementation defined
 * (null pointer may be returned, or some non-null pointer may be returned that
 * may not be used to access storage, but has to be passed to std::free).
 *
 * @param size number of bytes to allocate
 * @return On success, returns the pointer to the beginning of newly allocated
 * memory. To avoid a memory leak, the returned pointer must be deallocated with
 * std::free() or std::realloc(). On failure, returns a null pointer.
 */
val<void*> malloc(val<size_t> size);

/**
 * @brief Allocate size bytes of uninitialized storage whose alignment is
 * specified by alignment (implicitly creating objects in the destination area).
 * The size parameter must be an integral multiple of alignment.
 *
 * @param alignment specifies the alignment. Must be a valid alignment supported
 * by the implementation.
 * @param size number of bytes to allocate. An integral multiple of alignment.
 * @return On success, returns the pointer to the beginning of newly allocated
 * memory. To avoid a memory leak, the returned pointer must be deallocated with
 * std::free() or std::realloc(). On failure, returns a null pointer.
 */
val<void*> aligned_alloc(val<size_t> alignment, val<size_t> size);

/**
 * @brief Allocates memory for an array of num objects of size size, initializes
 * it to all bits zero (implicitly creating objects in the destination area). If
 * allocation succeeds, returns a pointer to the lowest (first) byte in the
 * allocated memory block that is suitably aligned for any object type. If size
 * is zero, the behavior is implementation defined (null pointer may be
 * returned, or some non-null pointer may be returned that may not be used to
 * access storage).
 *
 * @param num number of objects
 * @param size size of each object
 * @return On success, returns the pointer to the beginning of newly allocated
 * memory. To avoid a memory leak, the returned pointer must be deallocated with
 * std::free() or std::realloc(). On failure, returns a null pointer.
 */
val<void*> calloc(val<size_t> num, val<size_t> size);

/**
 * @brief Reallocates the given area of memory (implicitly creating objects in
 * the destination area). It must be previously allocated by std::malloc,
 * std::calloc or std::realloc and not yet freed with std::free, otherwise, the
 * results are undefined.
 *
 * @param ptr pointer to the memory area to be reallocated
 * @param new_size 	new size of the array
 * @return On success, returns a pointer to the beginning of newly allocated
 * memory. To avoid a memory leak, the returned pointer must be deallocated with
 * std::free or std::realloc. The original pointer ptr is invalidated and any
 * access to it is undefined behavior (even if reallocation was in-place).
 */
val<void*> realloc(val<void*> ptr, val<size_t> new_size);

/**
 * @brief Deallocates the space previously allocated by std::malloc,
 * std::calloc, std::aligned_alloc(since C++17), or std::realloc. If ptr is a
 * null pointer, the function does nothing. *
 * @param ptr pointer to the memory to deallocate
 */
void free(val<void*> ptr);

/**
 * @brief Interprets a floating point value in a byte string pointed to by str.
 * @param str pointer to the null-terminated byte string to be interpreted
 */
val<double> atof(val<const char*> str);

/**
 * @brief Interprets an integer value in a byte string pointed to by str. The
 * implied radix is always 10.
 * @param str 	pointer to the null-terminated byte string to be interpreted
 */
val<int> atoi(val<const char*> str);
val<long> atol(val<const char*> str);
val<long long> atoll(val<const char*> str);

/**
 * @brief Interprets an integer value in a byte string pointed to by str.
 * @param str pointer to the null-terminated byte string to be interpreted
 * @param str_end pointer to a pointer to character
 * @param base	base of the interpreted integer value
 */
val<long> strtol(val<const char*> str, val<const char**> strstr, val<int> base);
val<long long> strtoll(val<const char*> str, val<const char**> strstr, val<int> base);

/**
 * @brief Interprets an integer value in a byte string pointed to by str.
 * @param str pointer to the null-terminated byte string to be interpreted
 * @param str_end pointer to a pointer to character
 * @param base	base of the interpreted integer value
 */
val<unsigned long> strtoul(val<const char*> str, val<const char**> strstr, val<int> base);
val<unsigned long long> strtoull(val<const char*> str, val<const char**> strstr, val<int> base);

/**
 * @brief Interprets a floating point value in a byte string pointed to by str.
 * @param str pointer to the null-terminated byte string to be interpreted
 * @param str_end pointer to a pointer to character
 */
val<float> strtof(val<const char*> str, val<const char**> strstr, val<int> base);
val<double> strtod(val<const char*> str, val<const char**> strstr, val<int> base);

} // namespace nautilus
