#pragma once

#include "nautilus/c/engine.h"
#include "nautilus/c/ir.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Graph builders written in plain C, so the tests also prove the headers
 * compile as C. Each returns 0 on success. */

/* int64_t add(int64_t a, int64_t b) */
int build_add(NautilusIRGraphRef graph);

/* int64_t sum_to(int64_t n): 0 + 1 + ... + (n - 1), as a loop over block arguments. */
int build_sum_loop(NautilusIRGraphRef graph);

/* int32_t max(int32_t a, int32_t b), once with if/else ("max_if") and once with select ("max_select"). */
int build_max(NautilusIRGraphRef graph);

/* int64_t factorial(int64_t n), recursive, through the builder's own callee id. */
int build_factorial(NautilusIRGraphRef graph);

/* int64_t call_external(int64_t x): returns @p fn(x, 2). */
int build_call_external(NautilusIRGraphRef graph, int64_t (*fn)(int64_t, int64_t));

/* void store_through(int64_t* out, int64_t value): goes through a stack slot, then stores *out = value + 1. */
int build_memory(NautilusIRGraphRef graph);

/* double scale(int32_t x): (double) x * 0.5 */
int build_float(NautilusIRGraphRef graph);

/* Compiles @p graph with a throwaway engine pinned to @p backend, which is
 * disposed before returning: the executable must outlive it. */
NautilusExecutableRef compile_with_engine(NautilusIRGraphRef graph, const char* backend);

#ifdef __cplusplus
}
#endif
