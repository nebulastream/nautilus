
# Constant-folding tracer — trace-time partial-evaluation machinery for
# nautilus. Ships LazyTracedRef<T>, the Constant registry, Stage 2
# stratified widening, observability counters, and the user-facing
# primitives `assume_stable` and `unroll_scope`.
#
# Unlike the other plugins (std, simd, specialization, inlining) this
# one is *not* a separate linkable library. The reason: val<T>'s state
# field type is a compile-time typedef that resolves to either
# TypedValueRefHolder (default) or LazyTracedRef<T> (when
# ENABLE_CONSTANT_TRACER is on), so every core translation unit that
# uses val<T> needs the plugin's headers at compile time. Decoupling
# that into a runtime-loaded library would require type-erased or
# std::variant state on every val<T>, paying the cost in every op.
#
# Instead, when the option is on:
#   - The plugin's sources are compiled directly into the `nautilus`
#     target via target_sources(nautilus ...).
#   - The plugin's include directory is added to nautilus's PUBLIC
#     include path so val_*.hpp and callers can reference
#     nautilus/partial_evaluation/* headers.
#
# The plugin directory therefore exists purely for code organization —
# it keeps the ~1500 LOC of trace-time PE machinery out of
# `nautilus/src/nautilus/tracing/` where it would dilute the core
# tracer's story.

if (NOT ENABLE_CONSTANT_TRACER)
    return()
endif ()

target_sources(nautilus PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ConstantTracerState.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/PELazyTraceContext.cpp
)

target_include_directories(nautilus PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/>)

# Tests live next to the plugin sources; pulled in under ENABLE_TESTS.
# Compiled only when ENABLE_CONSTANT_TRACER is on (this whole file
# early-returns otherwise).
if (ENABLE_TESTS)
    add_subdirectory(test)
endif ()

# The end-to-end demo lives in example/src/DemoConstantFolding.cpp.
# Because this plugin's sources are compiled *into* the `nautilus` target
# (not as a separate library), the demo links only against nautilus and
# picks up the plugin's primitives transitively via headers.
