
if (ENABLE_TRACING)
    add_subdirectory(bc)
    add_subdirectory(cpp)
    add_subdirectory(mlir)
endif ()

if (ENABLE_ASMJIT_BACKEND)
    add_subdirectory(amsjit)
endif ()


add_source_files(nautilus
        CompilationBackend.cpp
        Executable.cpp
)