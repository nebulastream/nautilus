cmake_minimum_required(VERSION 3.22.1)
include(CMakeDependentOption)

option(ENABLE_LOGGING "Enable logging support" ON)
option(ENABLE_STACKTRACE "Enable stacktrace support" ON)
option(ENABLE_TESTS "Enable tests" ON)
option(ENABLE_COMPILER "Enables tracing and backend compilers" ON)
cmake_dependent_option(ENABLE_TRACING "Enable the tracing" ON "ENABLE_COMPILER" OFF)
cmake_dependent_option(ENABLE_MLIR_BACKEND "Enable the MLIR compiler backend" ON "ENABLE_COMPILER" OFF)
cmake_dependent_option(NAUTILUS_DOWNLOAD_MLIR "USE_PRE_BUILD_MLIR" ON "ENABLE_MLIR_BACKEND" OFF)
cmake_dependent_option(ENABLE_C_BACKEND "Enable the C compiler backend" ON "ENABLE_COMPILER" OFF)
cmake_dependent_option(ENABLE_BC_BACKEND "Enable the bytecode interpreter backend" ON "ENABLE_COMPILER" OFF)
cmake_dependent_option(ENABLE_ASMJIT_BACKEND "Enable the asmjit interpreter backend" OFF "ENABLE_COMPILER" OFF)

project(nautilus VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Werror=extra -Werror=all -fpermissive -fPIC -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(cmake/macros.cmake)

set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

if (ENABLE_MLIR_BACKEND)
    include(cmake/ImportMLIR.cmake)
endif ()

if (ENABLE_TESTS)
    set(CATCH_ENABLE_REPRODUCIBLE_BUILD OFF CACHE INTERNAL "Turn off tests")
    add_subdirectory(${THIRD_PARTY_DIR}/Catch2)
endif ()

if (ENABLE_LOGGING)
    set(SPDLOG_COMPILED_LIB ON)
    set(SPDLOG_BUILD_SHARED OFF)
    add_subdirectory(${THIRD_PARTY_DIR}/spdlog)
endif ()

if (ENABLE_STACKTRACE)
    # Also requires one of: libbfd (gnu binutils), libdwarf, libdw (elfutils)
    add_subdirectory(${THIRD_PARTY_DIR}/backward-cpp)
endif ()


if (ENABLE_BC_BACKEND)
    add_subdirectory(${THIRD_PARTY_DIR}/dyncall)
endif ()

if (ENABLE_ASMJIT_BACKEND)
    set(ASMJIT_STATIC ON)
    set(ASMJIT_BUILD_RELEASE ON)
    set(ASMJIT_NO_FOREIGN ON)
    set(ASMJIT_NO_DEPRECATED ON)
    add_subdirectory(${THIRD_PARTY_DIR}/asmjit)
    target_compile_options(asmjit PUBLIC -Wno-deprecated-anon-enum-enum-conversion -Wno-nested-anon-types -Wno-gnu-anonymous-struct)
endif ()

add_subdirectory(nautilus)
add_make_format()




