
if (ENABLE_C_BACKEND)
    add_source_files(nautilus
            CPPCompiler.cpp
            CPPCompilationBackend.cpp
            CPPExecutable.cpp
            CPPLoweringProvider.cpp
            CPPCompilerFlags.cpp
            SharedLibrary.cpp
            ExecutablePath.cpp
    )
endif ()