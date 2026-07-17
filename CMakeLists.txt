cmake_minimum_required(VERSION 3.22.1)
include(CMakeDependentOption)

project(nautilus VERSION 0.3)

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
cmake_dependent_option(ENABLE_MLIR_BACKEND "Enable the MLIR compiler backend" ON "ENABLE_TRACING" OFF)
cmake_dependent_option(NAUTILUS_DOWNLOAD_MLIR "Download pre-built MLIR binaries" ON "ENABLE_MLIR_BACKEND;NOT USE_EXTERNAL_MLIR" OFF)
cmake_dependent_option(ENABLE_C_BACKEND "Enable the C compiler backend" ON "ENABLE_TRACING" OFF)
cmake_dependent_option(ENABLE_BC_BACKEND "Enable the bytecode interpreter backend" ON "ENABLE_TRACING" OFF)
cmake_dependent_option(ENABLE_TBC_BACKEND "Enable the threaded bytecode interpreter backend" ON "ENABLE_TRACING" OFF)
cmake_dependent_option(ENABLE_BC_LIBFFI "Use libffi static-trampoline closures for the bytecode backend instead of dyncall callbacks (iOS-safe: no runtime-generated executable memory)" OFF "ENABLE_BC_BACKEND" OFF)
cmake_dependent_option(ENABLE_TBC_JIT "Enable the copy-and-patch JIT execution mode of the tbc backend (tbc.mode=jit)" ON "ENABLE_TBC_BACKEND" OFF)
cmake_dependent_option(ENABLE_ASMJIT_BACKEND "Enable the asmjit interpreter backend" ON "ENABLE_TRACING" OFF)
cmake_dependent_option(ENABLE_SHORT_CIRCUIT_BOOL "Trace val<bool> && / || with C++ short-circuit semantics (drops the operator overloads so the built-in operators route through val<bool>::operator bool())" OFF "ENABLE_TRACING" OFF)


option(ENABLE_TEST_COVERAGE "Enable the collection of test coverage." OFF)
option(ENABLE_ADDRESS_SANITIZER "Enable address sanitizer." OFF)
option(ENABLE_THREAD_SANITIZER "Enable thread sanitizer." OFF)
option(ENABLE_UB_SANITIZER "Enable undefined behavior sanitizer." OFF)
option(ENABLE_LEAK_SANITIZER "Enable leak sanitizer." OFF)
option(ENABLE_LLVM_IR_TEST "Enable LLVM IR comparison tests (requires llvm-diff-19 or later)" OFF)
cmake_dependent_option(ENABLE_FUZZING "Build the libFuzzer differential fuzz harness (Clang only)" OFF "ENABLE_TRACING" OFF)
cmake_dependent_option(ENABLE_FUZZER_REPLAY "Build the libFuzzer-free differential replay/smoke driver (any compiler)" OFF "ENABLE_TRACING" OFF)
option(ENABLE_SIMD_PLUGIN "Enable the SIMD vector plugin" ON)
option(ENABLE_STD_PLUGIN "Enable the standard library wrapper plugin" ON)
option(ENABLE_SPECIALIZATION_PLUGIN "Enable the code specialization hint plugin (profile + assume)" ON)
option(ENABLE_INLINING_PLUGIN "Enable the LLVM function inlining plugin" ON)
option(ENABLE_GPU_PLUGIN "Enable the GPU plugin (CUDA + Metal backends)" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_WARNINGS "-Wall -Wextra -pedantic-errors -Werror=extra-semi -Werror=extra -Werror=all -Werror=delete-non-virtual-dtor -Werror=deprecated -Werror=array-bounds -Werror=ignored-qualifiers -Werror=sign-compare")


# Check if compiler can determine illegal nautilus types at compile time.
# (c.f. TypeResolver::to_type)
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
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 22)
        set(CMAKE_CXX_WARNINGS "${CMAKE_CXX_WARNINGS} -Wno-c2y-extensions")
    endif ()
else ()
    set(CMAKE_CXX_WARNINGS "${CMAKE_CXX_WARNINGS} -Woverflow -Werror=return-type -Werror=return-local-addr  -Werror=write-strings -Wno-error=stringop-overflow")
endif ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_WARNINGS} -fpermissive -fno-omit-frame-pointer")

if (ENABLE_TEST_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoverage-mapping -fprofile-instr-generate")
endif ()

if (ENABLE_ADDRESS_SANITIZER)
    add_link_options(-fsanitize=address)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
elseif (ENABLE_THREAD_SANITIZER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
elseif (ENABLE_UB_SANITIZER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-sanitize-recover=all")
elseif (ENABLE_LEAK_SANITIZER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
endif ()

if (ENABLE_FUZZING)
    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "ENABLE_FUZZING requires Clang (libFuzzer is not available on this compiler).")
    endif ()
    # Instrument the whole build with SanitizerCoverage so libFuzzer gets
    # coverage feedback through the Nautilus library, not just the harness TU.
    # The fuzz executable itself links the libFuzzer runtime via -fsanitize=fuzzer
    # (see nautilus/test/fuzz/CMakeLists.txt).
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=fuzzer-no-link")
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

find_program(CCACHE_EXECUTABLE ccache)
if (CCACHE_EXECUTABLE AND ${USE_CCACHE_IF_AVAILABLE})
    message(STATUS "Using ccache: ${CCACHE_EXECUTABLE}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_EXECUTABLE}")
else()
    message(STATUS "Not using ccache")
endif ()

include(cmake/macros.cmake)

set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

if (ENABLE_MLIR_BACKEND AND NOT USE_EXTERNAL_MLIR)
    include(cmake/ImportMLIR.cmake)
endif ()

set(EXPORT_TARGETS)
if (ENABLE_TESTS OR ENABLE_BENCHMARKS)
    set(CATCH_ENABLE_REPRODUCIBLE_BUILD OFF CACHE INTERNAL "Disable reproducible build for Catch2")
    add_subdirectory(${THIRD_PARTY_DIR}/Catch2)
    disable_target_warnings(Catch2)
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


if (ENABLE_BC_BACKEND OR ENABLE_TBC_BACKEND)
    add_subdirectory(${THIRD_PARTY_DIR}/dyncall)
    list(APPEND EXPORT_TARGETS dyncall_s)

    if (ENABLE_BC_LIBFFI)
        # The bytecode backend exposes each compiled function as a real C
        # function pointer. By default this uses a dyncall callback, which
        # writes a trampoline into RWX memory at runtime — forbidden on iOS.
        # libffi (>= 3.4) builds closures via static trampolines (a read-only
        # code page + a separate writable data page, no runtime PROT_EXEC), so
        # the closure pointer stays usable where runtime codegen is banned.
        # We locate libffi rather than vendor it: desktop/CI provide it via the
        # system package; an iOS toolchain points at Apple's static-trampoline
        # libffi the same way.
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindNautilusFFI.cmake)
    endif ()
endif ()

# asmjit is needed by the asmjit backend and by the tbc copy-and-patch JIT
# (executable-memory management: JitAllocator/VirtMem).
if (ENABLE_ASMJIT_BACKEND OR ENABLE_TBC_JIT)
    set(ASMJIT_STATIC ON)
    set(ASMJIT_BUILD_RELEASE ON)
    set(ASMJIT_NO_FOREIGN ON)
    set(ASMJIT_NO_DEPRECATED ON)
    add_subdirectory(${THIRD_PARTY_DIR}/asmjit)
    disable_target_warnings(asmjit)
    get_target_property(_asmjit_includes asmjit INTERFACE_INCLUDE_DIRECTORIES)
    set_target_properties(asmjit PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_asmjit_includes}")
endif ()

# Define nautilus_inline() globally so callers can use it regardless of
# whether the inlining plugin is actually being built. When the plugin is
# disabled, the function degrades to a warning-only no-op.
include(${CMAKE_CURRENT_SOURCE_DIR}/plugins/inlining/cmake/NautilusInline.cmake)

add_subdirectory(nautilus)
if (ENABLE_SIMD_PLUGIN)
    add_subdirectory(plugins/simd)
endif ()
if (ENABLE_STD_PLUGIN)
    add_subdirectory(plugins/std)
endif ()
if (ENABLE_SPECIALIZATION_PLUGIN)
    add_subdirectory(plugins/specialization)
endif ()
if (ENABLE_INLINING_PLUGIN)
    add_subdirectory(plugins/inlining)
endif ()
if (ENABLE_GPU_PLUGIN)
    add_subdirectory(plugins/gpu)
endif ()
add_make_format()




