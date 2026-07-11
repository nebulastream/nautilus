cmake_minimum_required(VERSION 3.22.1)

project(nautilus-playground-runner)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Everything must be PIC: libnautilus.a and the MLIR archives are linked into
# libplayground_core.so below. -fno-omit-frame-pointer is required by the
# tracing runtime (nautilus also sets it as an INTERFACE option).
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# All five backends selectable in the playground. AsmJit is OFF by default in
# the top-level nautilus build; MLIR/C/BC/TBC default ON.
set(ENABLE_ASMJIT_BACKEND ON CACHE BOOL "" FORCE)
set(ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(ENABLE_BENCHMARKS OFF CACHE BOOL "" FORCE)
# Shipped plugins, so user code can #include <nautilus/std/...>,
# <nautilus/vector.hpp> (SIMD), and <nautilus/specialization/...>. The
# inlining plugin is excluded: it needs a Clang pass plugin attached to the
# user TU at compile time, which does not fit the prebuilt-core model.
set(ENABLE_STD_PLUGIN ON CACHE BOOL "" FORCE)
set(ENABLE_SIMD_PLUGIN ON CACHE BOOL "" FORCE)
set(ENABLE_SPECIALIZATION_PLUGIN ON CACHE BOOL "" FORCE)
# GPU plugin: the CUDA/Metal lowering is pure codegen (no SDK needed to
# build). The driver sets gpu.codegenOnly so the toolchain step (nvcc /
# xcrun) is skipped and users inspect the generated device/host sources.
set(ENABLE_GPU_PLUGIN ON CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../.. ${CMAKE_CURRENT_BINARY_DIR}/nautilus)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(WHOLE_ARCHIVE_PREFIX -Wl,--whole-archive)
    set(WHOLE_ARCHIVE_SUFFIX -Wl,--no-whole-archive)
else ()
    message(FATAL_ERROR "The playground runner requires GCC or Clang (whole-archive linking).")
endif ()

# libplayground_core.so: the whole nautilus static library and the shipped
# plugins (plus their transitive MLIR/backend dependencies) pre-linked into
# one shared library. Whole-archive is required — without it the linker
# would only pull the archive members referenced by core.cpp, i.e. nothing,
# and the per-request link of user_module.o would miss every nautilus
# symbol. The plugins additionally rely on whole-archive to keep their
# static-initializer registration TUs (StdPluginInit.cpp etc.), exactly as
# example/CMakeLists.txt documents. Pre-linking once at image build time
# makes the per-request link sub-second.
add_library(playground_core SHARED core.cpp)
target_link_libraries(playground_core PUBLIC
        ${WHOLE_ARCHIVE_PREFIX}
        nautilus nautilus-std nautilus-simd nautilus-specialization nautilus-gpu
        ${WHOLE_ARCHIVE_SUFFIX})

# libplayground_driver.a: the prebuilt driver holding main(). The per-request
# link is `user_module.o + libplayground_driver.a + libplayground_core.so`;
# the archive member containing main() is pulled in because crt1.o references
# it, and the user TU's playground_register resolves the driver's extern.
add_library(playground_driver STATIC driver.cpp)
target_link_libraries(playground_driver PRIVATE nautilus)

# Stage everything the slim runtime image needs under <build>/playground-dist:
#   lib/      libplayground_core.so, libplayground_driver.a
#   include/  public nautilus headers + the build-generated config.hpp
#   playground_pch.hpp (the .pch itself is generated inside the image, with
#   the exact flags entrypoint.sh compiles user code with)
set(PLAYGROUND_DIST ${CMAKE_CURRENT_BINARY_DIR}/playground-dist)
add_custom_target(playground-dist ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PLAYGROUND_DIST}/lib ${PLAYGROUND_DIST}/include/nautilus
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:playground_core> ${PLAYGROUND_DIST}/lib/
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:playground_driver> ${PLAYGROUND_DIST}/lib/
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../nautilus/include/nautilus
                ${PLAYGROUND_DIST}/include/nautilus
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../plugins/std/include/nautilus
                ${PLAYGROUND_DIST}/include/nautilus
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../plugins/simd/include/nautilus
                ${PLAYGROUND_DIST}/include/nautilus
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../plugins/specialization/include/nautilus
                ${PLAYGROUND_DIST}/include/nautilus
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../plugins/gpu/include/nautilus
                ${PLAYGROUND_DIST}/include/nautilus
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/nautilus/plugins/gpu/include/nautilus/gpu/config.hpp
                ${PLAYGROUND_DIST}/include/nautilus/gpu/config.hpp
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/nautilus/nautilus/include/nautilus/config.hpp
                ${PLAYGROUND_DIST}/include/nautilus/config.hpp
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/playground_pch.hpp
                ${PLAYGROUND_DIST}/playground_pch.hpp
        DEPENDS playground_core playground_driver
        COMMENT "Staging playground runtime artifacts into ${PLAYGROUND_DIST}")
