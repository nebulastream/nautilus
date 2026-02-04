
if (ENABLE_TRACING)
    add_subdirectory(tag)
    add_subdirectory(phases)
    add_subdirectory(exceptions)
    add_subdirectory(symbolic_tracing)
    add_subdirectory(fork_tracing)
    add_source_files(nautilus
            Block.cpp
            ExecutionTrace.cpp
            TracerTrace.cpp
            TracerTraceConversion.cpp
            TracingUtil.cpp
            TraceContext.cpp
            TraceOperation.cpp
            Snapshot.cpp)
endif ()
