

if (ENABLE_BC_BACKEND)
    # dyncall forward-call VM (Dyncall.cpp / DYNCALL_* opcodes) is used in both
    # modes for external proxy/runtime calls. Forward calls do not generate code
    # at runtime, so they are iOS-safe and always linked.
    target_link_libraries(nautilus PRIVATE dyncall_s)

    add_source_files(nautilus
            BCInterpreterBackend.cpp
            BCInterpreter.cpp
            BCLoweringProvider.cpp
            Dyncall.cpp
    )
endif ()
