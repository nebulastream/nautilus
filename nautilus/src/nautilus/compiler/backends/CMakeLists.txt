
if (ENABLE_TRACING)
    add_subdirectory(bc)
    add_subdirectory(cpp)
    add_subdirectory(mlir)
    add_subdirectory(tbc)
endif ()

if (ENABLE_ASMJIT_BACKEND)
    add_subdirectory(amsjit)
endif ()


add_source_files(nautilus
        CapturedExceptionTransport.cpp
        CompilationBackend.cpp
        Executable.cpp
)

# The interpreting backends (bc, tbc) hand out real C function pointers for their
# compiled functions through the shared NativeClosure component. Its mechanism is
# selected once, here, for both of them.
if (ENABLE_BC_BACKEND OR ENABLE_TBC_BACKEND)
    add_source_files(nautilus NativeClosure.cpp)

    if (ENABLE_FFI_CLOSURES)
        # iOS-safe closures via libffi static trampolines. dyncallback is
        # intentionally not linked in this configuration so no RWX-allocating
        # callback code is reachable at all.
        target_link_libraries(nautilus PRIVATE nautilus::ffi)
    else ()
        # Default: dyncall callbacks (dcbNewCallback) expose each function as a
        # C function pointer. Writes a trampoline into RWX memory at runtime.
        target_link_libraries(nautilus PRIVATE dyncallback_s)
        target_include_directories(nautilus PRIVATE ${PROJECT_SOURCE_DIR}/third_party/dyncall/dyncallback)
    endif ()
endif ()
