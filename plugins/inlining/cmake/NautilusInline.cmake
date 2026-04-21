# NautilusInline.cmake
#
# Provides the globally-available `is_inlining_supported()` detector and the
# `nautilus_inline(target)` helper. Both are defined unconditionally when this
# file is included, so downstream CMake code can call `nautilus_inline()`
# regardless of whether the inlining plugin is actually being built.
#
# When the plugin is enabled (see plugins/inlining/CMakeLists.txt) and the
# current host/toolchain supports it, `InliningPass` is created as a SHARED
# library in plugins/inlining/pass/ and `nautilus_inline()` attaches it via
# `-fpass-plugin`. Otherwise `nautilus_inline()` degrades to a warning-only
# no-op so callers do not need to special-case plugin enablement.

include_guard(GLOBAL)

option(ENABLE_INLINING_PASS "Enable building inlining llvm pass" ON)

# Runs the host/toolchain detection and writes the result to ${result_var}
# in the parent scope. Prefer the cached value in NAUTILUS_INLINE_SUPPORTED
# once it has been populated by the top-level include below.
function(_detect_inlining_support result_var)
    if(NOT ENABLE_INLINING_PASS)
        set(${result_var} FALSE PARENT_SCOPE)
        return()
    endif ()

    # Check clang version
    # Inlining is supported for clang 19 through 21.
    # The LLVM pass API can change between major versions, so new versions should be validated before adding them here.
    if (NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang"
            AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.0.0"
            AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "22"))
        message(WARNING "Nautilus function inlining is not supported for the active compiler version and will not be applied. You can safely ignore this warning.")
        set(${result_var} FALSE PARENT_SCOPE)
        return()
    endif ()

    # Disable inlining for ARM
    # There are unfixed issues where either LLVM or the JIT compiler introduces additional, unhandled components to the IR on ARM
    string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SYSTEM_PROCESSOR)
    if (SYSTEM_PROCESSOR MATCHES "arm" OR SYSTEM_PROCESSOR MATCHES "aarch64")
        message(WARNING "Nautilus function inlining is not supported on ARM and will not be applied. You can safely ignore this warning.")
        set(${result_var} FALSE PARENT_SCOPE)
        return()
    endif ()

    set(${result_var} TRUE PARENT_SCOPE)
endfunction()

# Cached detector: run detection exactly once per configure and stash the
# result in an INTERNAL cache variable so every subsequent
# `nautilus_inline(...)` invocation is a cheap read (and so the toolchain
# warnings above are emitted only once).
function(is_inlining_supported result_var)
    if(NOT DEFINED NAUTILUS_INLINE_SUPPORTED_CACHED)
        _detect_inlining_support(_nautilus_inline_detected)
        set(NAUTILUS_INLINE_SUPPORTED_CACHED "${_nautilus_inline_detected}" CACHE INTERNAL
            "Whether the Nautilus inlining plugin is supported on this host/toolchain")
    endif ()
    set(${result_var} "${NAUTILUS_INLINE_SUPPORTED_CACHED}" PARENT_SCOPE)
endfunction()


is_inlining_supported(NAUTILUS_INLINE_SUPPORTED)

function(nautilus_inline target)
    is_inlining_supported(NAUTILUS_INLINE_SUPPORTED)
    if (NAUTILUS_INLINE_SUPPORTED)
        if (TARGET InliningPassDependency)
            add_dependencies(${target} InliningPassDependency)
            target_compile_options(${target} PRIVATE
                    "-fpass-plugin=$<TARGET_FILE:InliningPass>"
            )
        else ()
            # The toolchain supports inlining but the pass library isn't a
            # target in this configure. The most common cause is that the
            # top-level ENABLE_INLINING_PLUGIN gate is OFF (the gate that
            # actually add_subdirectory()'s plugins/inlining), not the
            # local ENABLE_INLINING_PASS in this file. Both gates must be
            # ON for the pass to be built. Warn rather than FATAL so that
            # callers like the demo can degrade gracefully.
            message(WARNING "nautilus_inline(${target}) called but the InliningPass target is not available. Enable both ENABLE_INLINING_PLUGIN (top-level) and ENABLE_INLINING_PASS (plugin-local) to build it. Skipping.")
        endif ()
    else ()
        message(WARNING "Function inlining requires clang 19-21 during compilation.")
    endif ()
endfunction()
