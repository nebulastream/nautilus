cmake_minimum_required(VERSION 3.22.1)

project(nautilus-example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CATCH_ENABLE_REPRODUCIBLE_BUILD OFF CACHE INTERNAL "Turn off tests")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Werror=extra -Werror=all -fpermissive -fPIC -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Force the shipped plugins ON so the plugin demos below can link against them.
# These options are declared by the top-level nautilus CMakeLists.txt and,
# when ON, cause add_subdirectory(plugins/...) to register the plugin targets
# (nautilus-std, nautilus-simd, nautilus-specialization).
set(ENABLE_STD_PLUGIN ON CACHE BOOL "" FORCE)
set(ENABLE_SIMD_PLUGIN ON CACHE BOOL "" FORCE)
set(ENABLE_SPECIALIZATION_PLUGIN ON CACHE BOOL "" FORCE)
set(ENABLE_INLINING_PLUGIN ON CACHE BOOL "" FORCE)
# Enable the AsmJit backend so demo_backends can include it in its comparison.
# This is OFF by default in the top-level nautilus build but is safe on
# x86-64 Linux / macOS and ARM64 Linux.
set(ENABLE_ASMJIT_BACKEND ON CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/nautilus)

# Core demos — only need the nautilus library.
add_executable(demo src/DemoJit.cpp)
target_link_libraries(demo PRIVATE nautilus)

add_executable(demo_modules src/DemoModules.cpp)
target_link_libraries(demo_modules PRIVATE nautilus)

add_executable(demo_backends src/DemoBackends.cpp)
target_link_libraries(demo_backends PRIVATE nautilus)

add_executable(demo_runtime_calls src/DemoRuntimeCalls.cpp)
target_link_libraries(demo_runtime_calls PRIVATE nautilus)

# Constant-folding tracer demo — the sources stay in example/src/
# alongside the other examples, but the primitives it uses
# (`nautilus::assume_stable`, `nautilus::unroll_scope`) are declared
# and implemented in the partial-evaluation plugin at
# plugins/partial_evaluation/. When ENABLE_CONSTANT_TRACER is on that
# plugin's sources are compiled directly into the `nautilus` target
# (see plugins/partial_evaluation/CMakeLists.txt), so linking against
# `nautilus` is enough — no extra link dependency on the plugin.
# When ENABLE_CONSTANT_TRACER is off the demo still builds and runs
# with every fold counter reported as zero.
add_executable(demo_constant_folding src/DemoConstantFolding.cpp)
target_link_libraries(demo_constant_folding PRIVATE nautilus)

# Plugin demos — each links the plugin library it exercises.
# All plugin libraries use --whole-archive (on GNU/Clang) to force their
# static initializer translation units (e.g. StdPluginInit.cpp,
# SpecializationPluginInit.cpp) to be linked in. Without this, the linker
# drops those TUs because the demo code never references their symbols
# directly, and the MLIR intrinsic plugins are never registered — so
# operations like sqrt or nautilus_assume fall back to opaque invoke()
# calls instead of lowering to native LLVM intrinsics.
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(WHOLE_ARCHIVE_PREFIX -Wl,--whole-archive)
    set(WHOLE_ARCHIVE_SUFFIX -Wl,--no-whole-archive)
else ()
    set(WHOLE_ARCHIVE_PREFIX "")
    set(WHOLE_ARCHIVE_SUFFIX "")
endif ()

add_executable(demo_std_plugin src/DemoStdPlugin.cpp)
target_link_libraries(demo_std_plugin PRIVATE
        nautilus ${WHOLE_ARCHIVE_PREFIX} nautilus-std ${WHOLE_ARCHIVE_SUFFIX})

add_executable(demo_simd_plugin src/DemoSimdPlugin.cpp)
target_link_libraries(demo_simd_plugin PRIVATE
        nautilus ${WHOLE_ARCHIVE_PREFIX} nautilus-simd ${WHOLE_ARCHIVE_SUFFIX})

add_executable(demo_specialization_plugin src/DemoSpecializationPlugin.cpp)
target_link_libraries(demo_specialization_plugin PRIVATE
        nautilus ${WHOLE_ARCHIVE_PREFIX} nautilus-specialization ${WHOLE_ARCHIVE_SUFFIX})

# Inlining plugin demo — needs both the plugin library AND the Clang pass
# plugin attached to this TU so NAUTILUS_INLINE-annotated helpers get their
# bitcode extracted at compile time. `is_inlining_supported()` gates builds
# on compiler/host support (Clang 19-21 on non-ARM, non-AppleClang); on
# unsupported toolchains the demo is silently skipped. Unlike the other
# plugin demos, `nautilus-inlining` does not need `--whole-archive` because
# `<nautilus/inline.hpp>` uses an anchor trick to force its init TU to be
# pulled out of the static archive.
if (ENABLE_INLINING_PLUGIN)
    is_inlining_supported(NAUTILUS_INLINE_SUPPORTED)
    if (NAUTILUS_INLINE_SUPPORTED)
        add_executable(demo_inlining_plugin src/DemoInliningPlugin.cpp)
        nautilus_inline(demo_inlining_plugin)
        target_link_libraries(demo_inlining_plugin PRIVATE nautilus nautilus-inlining)
    else ()
        message(STATUS "Skipping demo_inlining_plugin: inlining plugin is not supported on this toolchain.")
    endif ()
endif ()
