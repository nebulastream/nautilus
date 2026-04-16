
add_subdirectory(backends)

if (ENABLE_TRACING)
    add_subdirectory(ir)
endif ()
add_source_files(nautilus LegacyCompiler.cpp TieredCompiler.cpp Engine.cpp DumpHandler.cpp CompilationStatistics.cpp)
