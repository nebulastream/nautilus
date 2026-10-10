/*
 * C API for the Nautilus engine.
 *
 * An engine holds the engine-wide configuration (the options documented in
 * docs/options.md, e.g. "engine.backend") and compiles IR graphs built with
 * nautilus/c/ir.h through the same compiler the C++ NautilusEngine uses:
 * same backend selection, same option layering, same compilation statistics.
 *
 * Unlike nautilus_ir_graph_compile(), which names a backend per call, an
 * engine is configured once and can be shared: nautilus_engine_compile() may
 * be called from several threads at once, as long as each call compiles a
 * different graph.
 *
 *     NautilusIROptionsRef options = nautilus_ir_options_create();
 *     nautilus_ir_options_set_string(options, "engine.backend", "mlir");
 *     NautilusEngineRef engine = nautilus_engine_create(options);
 *     nautilus_ir_options_dispose(options);
 *
 *     NautilusIRExecutableRef exe = nautilus_engine_compile(engine, graph, NULL);
 *     ...
 *     nautilus_ir_executable_dispose(exe);
 *     nautilus_engine_dispose(engine);
 */
#ifndef NAUTILUS_C_ENGINE_H
#define NAUTILUS_C_ENGINE_H

#include "nautilus/c/ir.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NautilusOpaqueEngine* NautilusEngineRef;

/* Creates an engine. @p options (may be NULL) is copied, so it can be
 * disposed right after. Returns NULL on failure. */
NautilusEngineRef nautilus_engine_create(NautilusIROptionsRef options);

/* Releases the engine. Executables it compiled stay valid. */
void nautilus_engine_dispose(NautilusEngineRef engine);

/* Name of the engine's compilation backend ("mlir", "bc", ..., or
 * "tiered(<tier0>,<tier1>)" for a tiered engine). Borrowed from the engine. */
const char* nautilus_engine_get_backend_name(NautilusEngineRef engine);

/* Compiles every function in @p graph synchronously with the engine's
 * primary backend: the one "engine.backend" pins, or the tier-1 backend of a
 * tiered engine (a prebuilt graph cannot be promoted between tiers).
 *
 * @p overrides (may be NULL) are layered on the engine's options for this
 * compile only, like per-module options in C++. Runs the IR pass pipeline
 * first unless nautilus_ir_graph_optimize() already did; the graph is
 * rewritten in place and can no longer be extended afterwards. Fails if the
 * engine was created with "engine.Compilation" set to false. */
NautilusIRExecutableRef nautilus_engine_compile(NautilusEngineRef engine, NautilusIRGraphRef graph,
                                                NautilusIROptionsRef overrides);

#ifdef __cplusplus
}
#endif

#endif /* NAUTILUS_C_ENGINE_H */
