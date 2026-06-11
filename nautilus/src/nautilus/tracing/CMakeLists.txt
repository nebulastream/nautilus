
if (ENABLE_TRACING)
    add_subdirectory(tag)
    add_subdirectory(symbolic_execution)
    add_subdirectory(phases)
    add_subdirectory(exceptions)
    add_subdirectory(fiber)
    add_source_files(nautilus
            Block.cpp
            ExecutionTrace.cpp
            TracingUtil.cpp
            ExceptionBasedTraceContext.cpp
            LazyTraceContext.cpp
            StackCopyTraceContext.cpp
            ForkTraceContext.cpp
            TraceSerialization.cpp
            TraceOperation.cpp
            Snapshot.cpp)
endif ()
