/*
 * Nautilus C API: shared definitions.
 *
 * Everything the IR (nautilus/c/ir.h) and engine (nautilus/c/engine.h) APIs
 * have in common: versioning, error reporting, strings, options and compiled
 * executables.
 *
 * Conventions
 * -----------
 * The API is designed to be bound mechanically (e.g. with Rust bindgen) and
 * wrapped safely:
 *
 * - ABI types. Enumerations are fixed-width integer typedefs with explicit
 *   constant values that never change meaning; unknown values are rejected
 *   with NAUTILUS_ERROR_INVALID_ARGUMENT instead of being undefined behavior.
 *   Booleans are C99 `bool`.
 * - Handles. Every object is an opaque pointer typedef (`...Ref`). Handles
 *   that own resources have a matching `_dispose` function, which accepts
 *   NULL. All other handles are borrowed from an owner and documented so.
 * - Errors. A function that creates an object returns its handle, or NULL on
 *   failure. Any other function that can fail returns a NautilusStatus and
 *   writes its results through out-parameters, which are left untouched on
 *   failure. Plain accessors on a valid handle cannot fail. Every failure
 *   records a status code and message for the calling thread, readable with
 *   nautilus_last_error_code() / nautilus_last_error_message(). Successful
 *   calls do not clear it. No C++ exception ever crosses the API.
 * - Strings. Input strings are NautilusStringRef: a pointer and a length, not
 *   NUL-terminated (although they may be). Borrowed output strings are
 *   NautilusStringRef too, and are additionally NUL-terminated. Owned output
 *   strings are NautilusString and must be released with
 *   nautilus_string_dispose().
 * - Arrays. Lists are read with copy-out accessors:
 *       size_t f(handle, ElementType* out, size_t capacity)
 *   copies up to @p capacity elements to @p out and returns the total number
 *   available; call it with (NULL, 0) to size the buffer.
 *
 * Thread safety
 * -------------
 * - A graph, its function builders and the function, block and value refs
 *   borrowed from it may be moved to another thread, but must not be used
 *   from two threads at once.
 * - An engine may be used from several threads at once.
 * - An executable may be queried from several threads at once, and remains
 *   valid after the graph and engine that produced it are disposed.
 * - Options objects are not thread-safe; functions that take one copy it.
 * - The last error is per thread.
 */
#ifndef NAUTILUS_C_COMMON_H
#define NAUTILUS_C_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if defined(_WIN32)
#if defined(NAUTILUS_C_API_BUILD)
#define NAUTILUS_C_API __declspec(dllexport)
#else
#define NAUTILUS_C_API
#endif
#elif defined(__GNUC__)
#define NAUTILUS_C_API __attribute__((visibility("default")))
#else
#define NAUTILUS_C_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ── Versioning ─────────────────────────────────────────────────────────── */

/* The major version changes on any incompatible change to the API or ABI;
 * the minor version when functions or enumeration values are added. */
#define NAUTILUS_C_API_VERSION_MAJOR 0
#define NAUTILUS_C_API_VERSION_MINOR 1
#define NAUTILUS_C_API_VERSION ((NAUTILUS_C_API_VERSION_MAJOR << 16) | NAUTILUS_C_API_VERSION_MINOR)

/* Version of the library actually loaded, encoded as NAUTILUS_C_API_VERSION.
 * Bindings should check that its major version matches the headers they were
 * generated from, and its minor version is at least theirs. */
NAUTILUS_C_API uint32_t nautilus_c_api_version(void);

/* ── Errors ─────────────────────────────────────────────────────────────── */

typedef int32_t NautilusStatus;
enum {
	NAUTILUS_OK = 0,
	/* A NULL handle, an out-of-range index or enumeration value, or a value
	 * of the wrong kind. */
	NAUTILUS_ERROR_INVALID_ARGUMENT = 1,
	/* An operand or argument has the wrong IR type. */
	NAUTILUS_ERROR_TYPE_MISMATCH = 2,
	/* The object cannot do this now, e.g. appending after a terminator or
	 * extending an already compiled graph. */
	NAUTILUS_ERROR_INVALID_STATE = 3,
	/* No function, callee or attribute with that name or id. */
	NAUTILUS_ERROR_NOT_FOUND = 4,
	/* The requested backend is not compiled into this build. */
	NAUTILUS_ERROR_UNAVAILABLE = 5,
	/* The IR verifier rejected the graph. */
	NAUTILUS_ERROR_VERIFICATION_FAILED = 6,
	/* The IR passes or the backend failed. */
	NAUTILUS_ERROR_COMPILATION_FAILED = 7,
	NAUTILUS_ERROR_OUT_OF_MEMORY = 8,
	/* A bug in Nautilus; the message has the details. */
	NAUTILUS_ERROR_INTERNAL = 9,
};

/* ── Strings ────────────────────────────────────────────────────────────── */

/* A borrowed string. @p data may be NULL only when @p length is 0. */
typedef struct {
	const char* data;
	size_t length;
} NautilusStringRef;

/* A string owned by the caller; release it with nautilus_string_dispose(). Its
 * data is NUL-terminated. */
typedef struct {
	char* data;
	size_t length;
} NautilusString;

NAUTILUS_C_API void nautilus_string_dispose(NautilusString str);

/* Convenience for C callers: a NautilusStringRef over a NUL-terminated string. */
static inline NautilusStringRef nautilus_string_ref(const char* cstr) {
	NautilusStringRef ref;
	ref.data = cstr;
	ref.length = cstr != NULL ? strlen(cstr) : 0;
	return ref;
}

/* Status of the last failed call on this thread, or NAUTILUS_OK if none has
 * failed yet. */
NAUTILUS_C_API NautilusStatus nautilus_last_error_code(void);

/* Message of the last failed call on this thread; empty if none has failed.
 * Valid until the next failing call on this thread. */
NAUTILUS_C_API NautilusStringRef nautilus_last_error_message(void);

/* ── Options ────────────────────────────────────────────────────────────── */

/* Engine and compilation options, as documented in docs/options.md (for
 * example "engine.backend", "dump.all", "ir.enableLICM"). */
typedef struct NautilusOpaqueOptions* NautilusOptionsRef;

NAUTILUS_C_API NautilusOptionsRef nautilus_options_create(void);
NAUTILUS_C_API void nautilus_options_dispose(NautilusOptionsRef options);
NAUTILUS_C_API NautilusStatus nautilus_options_set_bool(NautilusOptionsRef options, NautilusStringRef name, bool value);
NAUTILUS_C_API NautilusStatus nautilus_options_set_int(NautilusOptionsRef options, NautilusStringRef name,
                                                       int32_t value);
NAUTILUS_C_API NautilusStatus nautilus_options_set_double(NautilusOptionsRef options, NautilusStringRef name,
                                                          double value);
NAUTILUS_C_API NautilusStatus nautilus_options_set_string(NautilusOptionsRef options, NautilusStringRef name,
                                                          NautilusStringRef value);

/* ── Executables ────────────────────────────────────────────────────────── */

/* Compiled code for every function of a graph. Owned by the caller. */
typedef struct NautilusOpaqueExecutable* NautilusExecutableRef;

NAUTILUS_C_API void nautilus_executable_dispose(NautilusExecutableRef executable);

/* Writes the native entry point of the compiled function @p name to @p out;
 * cast it to the function's C signature. The pointer is valid while the
 * executable is alive. NAUTILUS_ERROR_NOT_FOUND if there is no such
 * function. */
NAUTILUS_C_API NautilusStatus nautilus_executable_get_function(NautilusExecutableRef executable, NautilusStringRef name,
                                                               void** out);

#ifdef __cplusplus
}
#endif

#endif /* NAUTILUS_C_COMMON_H */
