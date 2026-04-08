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

function(is_inlining_supported result_var)
    if(NOT ENABLE_INLINING_PASS)
        set(${result_var} FALSE PARENT_SCOPE)
        return()
    endif ()

    # Check clang version
    # right now, inlining is restricted to clang-19, but it can probably also support other clang versions without changes to the source code
    # however, this should be validated first, as the llvm pass API changes sometimes
    if (NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang"
            AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.0.0"
            AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "20"))
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
            message(WARNING "nautilus_inline(${target}) called but the InliningPass target is not available. Enable ENABLE_INLINING_PLUGIN to build it. Skipping.")
        endif ()
    else ()
        message(WARNING "Function inlining requires clang 19 during compilation. Probably also works with other clang versions. Adjust the version-check in CMake and find out")
    endif ()
endfunction()
