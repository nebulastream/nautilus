
add_subdirectory(backends)

if (ENABLE_TRACING)
    add_subdirectory(ir)
endif ()
add_source_files(nautilus JITCompiler.cpp Engine.cpp DumpHandler.cpp InlineFunctionRegistry.cpp MultiTierJitCompiler.cpp MultiTierExecutable.cpp)
