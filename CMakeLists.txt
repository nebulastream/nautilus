cmake_minimum_required(VERSION 3.22.1)
include(CMakeDependentOption)

option(ENABLE_LOGGING "Enable logging support" ON)
option(ENABLE_STACKTRACE "Enable stacktrace support" ON)
option(ENABLE_TESTS "Enable tests" ON)
option(ENABLE_BENCHMARKS "Enables tracing and backend benchmarks" OFF)
option(ENABLE_COMPILER "Enables tracing and backend compilers" ON)
option(USE_EXTERNAL_FMT "Use a externally provided version of fmt" OFF)
option(USE_EXTERNAL_SPDLOG "Use a externally provided version of spdlog" OFF)
option(USE_EXTERNAL_MLIR "Use a externally provided version of MLIR" OFF)
option(USE_CCACHE_IF_AVAILABLE "Use ccache to speed up rebuilds" ON)
cmake_dependent_option(ENABLE_TRACING "Enable the tracing" ON "ENABLE_COMPILER" OFF)
cmake_dependent_option(ENABLE_MLIR_BACKEND "Enable the MLIR compiler backend" ON "ENABLE_COMPILER" OFF)
cmake_dependent_option(NAUTILUS_DOWNLOAD_MLIR "USE_PRE_BUILD_MLIR" ON "ENABLE_MLIR_BACKEND;NOT USE_EXTERNAL_MLIR" OFF)
cmake_dependent_option(ENABLE_C_BACKEND "Enable the C compiler backend" ON "ENABLE_COMPILER" OFF)
cmake_dependent_option(ENABLE_BC_BACKEND "Enable the bytecode interpreter backend" ON "ENABLE_COMPILER" OFF)
cmake_dependent_option(ENABLE_ASMJIT_BACKEND "Enable the asmjit interpreter backend" OFF "ENABLE_COMPILER" OFF)

option(ENABLE_TEST_COVERAGE "Enable the collection of test coverage." OFF)
option(ENABLE_ADDRESS_SANITIZER "Enable address sanitizer." OFF)
option(ENABLE_THREAD_SANITIZER "Enable thread sanitizer." OFF)
option(ENABLE_UB_SANITIZER "Enable undefined behavior sanitizer." OFF)
project(nautilus VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_WARNINGS "-Wall -Wextra -pedantic-errors -Werror=extra-semi -Werror=extra -Werror=all -Werror=delete-non-virtual-dtor -Werror=deprecated -Werror=array-bounds -Werror=ignored-qualifiers -Werror=sign-compare")

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#include <typeinfo>
#include <type_traits>

template <typename T>
concept is_compatible = std::is_same_v<T, int> || std::is_same_v<T, bool>;

template <typename T>
struct TypeResolver {};

template <is_compatible T>
struct TypeResolver<T> {
	static constexpr int to_type() {
		using type = std::remove_cvref_t<T>;
		if constexpr (std::is_same_v<type, bool>) {
			return 0;
		} else {
			static_assert(false);
		}
	}
};


int main() {
	TypeResolver<bool>::to_type();
}
  " COMPILER_IS_SMART_WITH_CONSTEXPR)
if (COMPILER_IS_SMART_WITH_CONSTEXPR)
  add_compile_definitions(COMPILER_IS_SMART_WITH_CONSTEXPR)
else()
  message(WARNING "Compiler does not support compile-time checking of nautilus supported types. Checking at runtime.")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_WARNINGS "${CMAKE_CXX_WARNINGS} -Wpedantic-macros -Werror=integer-overflow -Werror=return-type -Werror=return-stack-address  -Werror=writable-strings")
else ()
    set(CMAKE_CXX_WARNINGS "${CMAKE_CXX_WARNINGS} -Woverflow -Werror=return-type -Werror=return-local-addr  -Werror=write-strings -Wno-error=stringop-overflow")
endif ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_WARNINGS} -fpermissive -fno-omit-frame-pointer")

if (ENABLE_TEST_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoverage-mapping -fprofile-instr-generate")
endif ()

if (ENABLE_SANITIZER)
    add_link_options(-fsanitize=address)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
elseif (ENABLE_THREAD_SANITIZER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
elseif (ENABLE_UB_SANITIZER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-sanitize-recover=all")
elseif (ENABLE_LEAK_SANITIZER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -O3")

find_program(CCACHE_EXECUTABLE ccache)
if (CCACHE_EXECUTABLE AND ${USE_CCACHE_IF_AVAILABLE})
    message(STATUS "Using ccache: ${CCACHE_EXECUTABLE}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_EXECUTABLE}")
endif ()

include(cmake/macros.cmake)

set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

if (ENABLE_MLIR_BACKEND AND NOT USE_EXTERNAL_MLIR)
    include(cmake/ImportMLIR.cmake)
endif ()

set(EXPORT_TARGETS)
if (ENABLE_TESTS OR ENABLE_BENCHMARKS)
    set(CATCH_ENABLE_REPRODUCIBLE_BUILD OFF CACHE INTERNAL "Turn off tests")
    add_subdirectory(${THIRD_PARTY_DIR}/Catch2)
    list(APPEND EXPORT_TARGETS Catch2)
endif ()

if (USE_EXTERNAL_FMT)
    find_package(fmt CONFIG REQUIRED)
else ()
    add_subdirectory(${THIRD_PARTY_DIR}/fmt EXCLUDE_FROM_ALL)
    disable_target_warnings(fmt)
    list(APPEND EXPORT_TARGETS fmt)
endif ()

if (ENABLE_LOGGING)
    if (USE_EXTERNAL_SPDLOG)
        find_package(spdlog CONFIG REQUIRED)
    else ()
        set(SPDLOG_COMPILED_LIB ON)
        set(SPDLOG_BUILD_SHARED OFF)
        set(SPDLOG_FMT_EXTERNAL ON)
        add_subdirectory(${THIRD_PARTY_DIR}/spdlog)
        list(APPEND EXPORT_TARGETS spdlog)
    endif ()
endif ()


if (ENABLE_STACKTRACE)
    # Also requires one of: libbfd (gnu binutils), libdwarf, libdw (elfutils)
    add_subdirectory(${THIRD_PARTY_DIR}/backward-cpp)
    list(APPEND EXPORT_TARGETS backward_interface)
endif ()


if (ENABLE_BC_BACKEND)
    add_subdirectory(${THIRD_PARTY_DIR}/dyncall)
    list(APPEND EXPORT_TARGETS dyncall_s)
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




