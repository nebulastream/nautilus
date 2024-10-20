git

if (ENABLE_TESTS)
    add_subdirectory(Catch2)
    include(Catch)
endif ()

if (ENABLE_LOGGING)
    set(SPDLOG_COMPILED_LIB ON)
    set(SPDLOG_BUILD_SHARED OFF)
    set(SPDLOG_FMT_EXTERNAL ON)
    add_subdirectory(spdlog)
endif ()

add_subdirectory(fmt)
disable_target_warnings(fmt)

if (ENABLE_STACKTRACE)
    # Also requires one of: libbfd (gnu binutils), libdwarf, libdw (elfutils)
    add_subdirectory(backward-cpp)
endif ()


if (ENABLE_BC_BACKEND)
    add_subdirectory(dyncall)
endif ()

if (ENABLE_ASMJIT_BACKEND)
    set(ASMJIT_STATIC ON)
    set(ASMJIT_BUILD_RELEASE ON)
    set(ASMJIT_NO_FOREIGN ON)
    set(ASMJIT_NO_DEPRECATED ON)
    add_subdirectory(asmjit)
    target_compile_options(asmjit PUBLIC -Wno-deprecated-anon-enum-enum-conversion -Wno-nested-anon-types -Wno-gnu-anonymous-struct)
endif ()
