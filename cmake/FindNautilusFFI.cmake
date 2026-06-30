# Locate libffi for the bytecode backend's iOS-safe closure path and expose it
# as the imported target `nautilus::ffi`.
#
# The bytecode backend uses libffi closures (built with static trampolines) to
# hand out a real C function pointer without allocating runtime-executable (RWX)
# memory, which is required on iOS. libffi >= 3.4 enables static trampolines by
# default on Apple and Linux targets.
#
# Resolution order:
#   1. pkg-config (libffi), which is how desktop/CI and cross toolchains ship it.
#   2. A manual find_path/find_library fallback.
#
# An iOS toolchain provides Apple's static-trampoline libffi through the same
# discovery (sysroot pkg-config / CMAKE_PREFIX_PATH), so no code change is needed
# to target the device.

if (TARGET nautilus::ffi)
    return()
endif ()

find_package(PkgConfig QUIET)
if (PkgConfig_FOUND)
    pkg_check_modules(NAUTILUS_FFI QUIET libffi)
endif ()

if (NAUTILUS_FFI_FOUND)
    set(_nautilus_ffi_include_dirs "${NAUTILUS_FFI_INCLUDE_DIRS}")
    # Prefer the static archive when pkg-config resolves a concrete path.
    if (NAUTILUS_FFI_LINK_LIBRARIES)
        set(_nautilus_ffi_link "${NAUTILUS_FFI_LINK_LIBRARIES}")
    else ()
        set(_nautilus_ffi_link "${NAUTILUS_FFI_LIBRARIES}")
    endif ()
    set(_nautilus_ffi_version "${NAUTILUS_FFI_VERSION}")
else ()
    find_path(NAUTILUS_FFI_INCLUDE_DIR ffi.h)
    find_library(NAUTILUS_FFI_LIBRARY NAMES ffi libffi)
    if (NOT NAUTILUS_FFI_INCLUDE_DIR OR NOT NAUTILUS_FFI_LIBRARY)
        message(FATAL_ERROR
                "ENABLE_BC_LIBFFI is ON but libffi was not found. Install libffi "
                ">= 3.4 (e.g. 'apt install libffi-dev' / 'brew install libffi') or "
                "point CMAKE_PREFIX_PATH / a toolchain file at a libffi build that "
                "uses static trampolines.")
    endif ()
    set(_nautilus_ffi_include_dirs "${NAUTILUS_FFI_INCLUDE_DIR}")
    set(_nautilus_ffi_link "${NAUTILUS_FFI_LIBRARY}")
    set(_nautilus_ffi_version "unknown")
endif ()

add_library(nautilus::ffi INTERFACE IMPORTED)
set_target_properties(nautilus::ffi PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_nautilus_ffi_include_dirs}"
        INTERFACE_LINK_LIBRARIES "${_nautilus_ffi_link}")

message(STATUS "Bytecode backend: using libffi closures (version ${_nautilus_ffi_version}) for iOS-safe function pointers")
