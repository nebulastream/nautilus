
add_subdirectory(backends)

if (ENABLE_TRACING)
    add_subdirectory(ir)
    # the temporary NIR -> 0.3 IR bridge of the cut-over, for the backends without a NIR port
    add_source_files(nautilus NirToIR.cpp)
endif ()
# JitSymbolRegistry is unconditional: it is a plain address->name table with no
# compiler dependencies, and a build without a JIT backend simply leaves it empty
# rather than failing to link for a consumer that queries it.
add_source_files(nautilus CompilationPipeline.cpp TieredCompiler.cpp Engine.cpp DumpHandler.cpp CompilationStatistics.cpp JitSymbolRegistry.cpp)
