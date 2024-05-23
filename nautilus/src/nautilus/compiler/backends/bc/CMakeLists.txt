

if (ENABLE_BC_BACKEND)
    # Link the external library to nes-runtime
    target_link_libraries(nautilus PRIVATE dyncall_s)

    add_source_files(nautilus
            BCInterpreterBackend.cpp
            BCInterpreter.cpp
            BCLoweringProvider.cpp
            Dyncall.cpp
    )
endif ()