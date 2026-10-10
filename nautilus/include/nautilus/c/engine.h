/*
 * Nautilus C API: the engine.
 *
 * An engine holds the engine-wide configuration (the options documented in
 * docs/options.md, e.g. "engine.backend") and compiles IR graphs built with
 * nautilus/c/ir.h through the same compiler the C++ NautilusEngine uses: same
 * backend selection, same option layering, same compilation statistics.
 *
 * Unlike nautilus_ir_graph_compile(), which names a backend per call, an
 * engine is configured once and shared. It may be used from several threads
 * at once, each compiling its own graph.
 *
 *     NautilusOptionsRef options = nautilus_options_create();
 *     nautilus_options_set_string(options, nautilus_string_ref("engine.backend"), nautilus_string_ref("mlir"));
 *     NautilusEngineRef engine = nautilus_engine_create(options);
 *     nautilus_options_dispose(options);
 *
 *     NautilusExecutableRef exe = nautilus_engine_compile(engine, graph, NULL);
 *     ...
 *     nautilus_executable_dispose(exe);
 *     nautilus_engine_dispose(engine);
 */
#ifndef NAUTILUS_C_ENGINE_H
#define NAUTILUS_C_ENGINE_H

#include "nautilus/c/common.h"
#include "nautilus/c/ir.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Owned by the caller. */
typedef struct NautilusOpaqueEngine* NautilusEngineRef;

/* Creates an engine. @p options (may be NULL) is copied. Fails with
 * NAUTILUS_ERROR_UNAVAILABLE if "engine.backend" names a backend this build
 * does not have. */
NAUTILUS_C_API NautilusEngineRef nautilus_engine_create(NautilusOptionsRef options);

/* Releases the engine. Executables it compiled stay valid. */
NAUTILUS_C_API void nautilus_engine_dispose(NautilusEngineRef engine);

/* The engine's compilation backend ("mlir", "bc", ..., or
 * "tiered(<tier0>,<tier1>)" for a tiered engine). Borrowed from the engine. */
NAUTILUS_C_API NautilusStringRef nautilus_engine_get_backend_name(NautilusEngineRef engine);

/* Compiles every function in @p graph synchronously with the engine's
 * primary backend: the one "engine.backend" pins, or the tier-1 backend of a
 * tiered engine (a prebuilt graph has nothing to promote between tiers).
 *
 * @p overrides (may be NULL) are layered on the engine's options for this
 * compile only, like per-module options in C++. The IR passes run first
 * unless nautilus_ir_graph_optimize() already ran them; the graph is
 * rewritten in place and can no longer be extended. Fails with
 * NAUTILUS_ERROR_INVALID_STATE if the engine has "engine.Compilation" set to
 * false: a prebuilt graph cannot run uncompiled. */
NAUTILUS_C_API NautilusExecutableRef nautilus_engine_compile(NautilusEngineRef engine, NautilusIRGraphRef graph,
                                                             NautilusOptionsRef overrides);

#ifdef __cplusplus
}
#endif

#endif /* NAUTILUS_C_ENGINE_H */
