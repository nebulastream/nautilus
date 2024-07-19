
add_subdirectory(exceptions)
if (ENABLE_LOGGING)
    add_subdirectory(tracing)
endif ()
add_subdirectory(compiler)
add_subdirectory(api)

if (ENABLE_LOGGING)
    add_source_files(nautilus logging.cpp)
endif ()