

if (ENABLE_BC_BACKEND)
    # Link the external library to nes-runtime
    target_link_libraries(nautilus PRIVATE dyncall_s dyncallback_s)

    # Add dyncallback include path for dyncall_callback.h
    target_include_directories(nautilus PRIVATE ${PROJECT_SOURCE_DIR}/third_party/dyncall/dyncallback)

    add_source_files(nautilus
            BCInterpreterBackend.cpp
            BCInterpreter.cpp
            BCLoweringProvider.cpp
            Dyncall.cpp
    )
endif ()
