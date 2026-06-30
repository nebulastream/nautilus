

if (ENABLE_BC_BACKEND)
    # dyncall forward-call VM (Dyncall.cpp / DYNCALL_* opcodes) is used in both
    # modes for external proxy/runtime calls. Forward calls do not generate code
    # at runtime, so they are iOS-safe and always linked.
    target_link_libraries(nautilus PRIVATE dyncall_s)

    if (ENABLE_BC_LIBFFI)
        # iOS-safe closures via libffi static trampolines instead of dyncall
        # callbacks. dyncallback is intentionally not linked here so no
        # RWX-allocating callback code is reachable.
        target_link_libraries(nautilus PRIVATE nautilus::ffi)
    else ()
        # Default: dyncall callbacks (dcbNewCallback) expose each function as a
        # C function pointer. Writes a trampoline into RWX memory at runtime.
        target_link_libraries(nautilus PRIVATE dyncallback_s)
        # Add dyncallback include path for dyncall_callback.h
        target_include_directories(nautilus PRIVATE ${PROJECT_SOURCE_DIR}/third_party/dyncall/dyncallback)
    endif ()

    add_source_files(nautilus
            BCInterpreterBackend.cpp
            BCInterpreter.cpp
            BCLoweringProvider.cpp
            Dyncall.cpp
    )
endif ()
