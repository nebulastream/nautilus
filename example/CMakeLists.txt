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
# GPU demo: pick a backend at configure time and only build the target if a
# usable runtime is present.  On macOS we need both the Metal backend enabled
# and the metallib toolchain installed.  On Linux we need the CUDA backend
# enabled and nvcc + CUDAToolkit (cudart) available so the demo can allocate
# unified-managed memory.
set(GPU_DEMO_BACKEND "")
if (TARGET nautilus-gpu)
    if (APPLE AND ENABLE_METAL_BACKEND)
        # metallib lives in the MetalToolchain asset, not the PATH. Match the
        # runtime lookup in MetalCompiler.cpp: try `xcrun --find metallib`,
        # falling back to DEVELOPER_DIR=/Applications/Xcode.app/... if Xcode
        # is installed but xcode-select still points at CommandLineTools.
        execute_process(
                COMMAND xcrun --sdk macosx --find metallib
                OUTPUT_VARIABLE METALLIB_EXECUTABLE
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET
                RESULT_VARIABLE _xcrun_metallib_rc)
        if (NOT _xcrun_metallib_rc EQUAL 0 OR NOT METALLIB_EXECUTABLE)
            set(METALLIB_EXECUTABLE "")
            if (EXISTS "/Applications/Xcode.app/Contents/Developer")
                execute_process(
                        COMMAND env DEVELOPER_DIR=/Applications/Xcode.app/Contents/Developer
                                xcrun --sdk macosx --find metallib
                        OUTPUT_VARIABLE METALLIB_EXECUTABLE
                        OUTPUT_STRIP_TRAILING_WHITESPACE
                        ERROR_QUIET
                        RESULT_VARIABLE _xcrun_metallib_rc2)
                if (NOT _xcrun_metallib_rc2 EQUAL 0)
                    set(METALLIB_EXECUTABLE "")
                endif ()
            endif ()
        endif ()
        if (METALLIB_EXECUTABLE)
            set(GPU_DEMO_BACKEND "metal")
        endif ()
    elseif (ENABLE_CUDA_BACKEND)
        find_program(NVCC_EXECUTABLE nvcc
                HINTS ENV CUDA_HOME PATHS /usr/local/cuda/bin /opt/cuda/bin)
        if (NVCC_EXECUTABLE)
            find_package(CUDAToolkit QUIET)
            if (CUDAToolkit_FOUND)
                set(GPU_DEMO_BACKEND "cuda")
            endif ()
        endif ()
    endif ()
endif ()

if (GPU_DEMO_BACKEND)
    add_executable(demo-gpu src/DemoGPU.cpp)
    target_link_libraries(demo-gpu PRIVATE nautilus nautilus-gpu)
    target_compile_definitions(demo-gpu PRIVATE GPU_DEMO_BACKEND="${GPU_DEMO_BACKEND}")
    if (GPU_DEMO_BACKEND STREQUAL "cuda")
        target_compile_definitions(demo-gpu PRIVATE GPU_DEMO_USE_CUDA)
        target_link_libraries(demo-gpu PRIVATE CUDA::cudart)
    endif ()
    message(STATUS "Building demo-gpu with ${GPU_DEMO_BACKEND} backend")
else ()
    message(STATUS "Skipping demo-gpu: no usable GPU backend (need Metal toolchain on macOS or nvcc + CUDAToolkit on Linux, with -DENABLE_GPU_PLUGIN=ON)")
endif ()
