if (ENABLE_TRACING AND ENABLE_BENCHMARKS)
    add_executable(nautilus-benchmarks
            TracingBenchmark.cpp
            ExecutionBenchmark.cpp
            IRPassBenchmark.cpp
            TieredCompilationBenchmark.cpp
    )
    target_include_directories(nautilus-benchmarks PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
            $<INSTALL_INTERFACE:src/nautilus/>)
    target_include_directories(nautilus-benchmarks PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
            $<INSTALL_INTERFACE:src/nautilus/>)

    target_link_libraries(nautilus-benchmarks PUBLIC nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    catch_discover_tests(nautilus-benchmarks EXTRA_ARGS --allow-running-no-tests)

    # the tracer alone, for a profiler: nautilus-tracing-corpus [function [reps [nir]]]
    add_executable(nautilus-tracing-corpus TracingCorpus.cpp)
    target_include_directories(nautilus-tracing-corpus PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>)
    target_link_libraries(nautilus-tracing-corpus PRIVATE nautilus)

    # Per-stage compile cost per operation of small and large kernels, 0.3 pipeline against the NIR tracer on one
    # binary: nautilus-compile-bench [repetitions [kernel [backend [mode]]]]
    add_executable(nautilus-compile-bench CompileTimeBench.cpp)
    target_link_libraries(nautilus-compile-bench PRIVATE nautilus)

    # The fixed cost of a compile: registerFunction of tiny kernels on one engine, wall time against the stages
    # nautilus-fixed-cost-bench [repetitions [kernel [backend]]]
    add_executable(nautilus-fixed-cost-bench FixedCostBench.cpp)
    target_link_libraries(nautilus-fixed-cost-bench PRIVATE nautilus)

    # The latency corpus (docs/plans/nautilus-0.4/10-observability.md). A plain executable that replaces the global
    # operator new to count allocations; it uses only the public engine API.
    add_executable(nautilus-latency-corpus latency-corpus/LatencyCorpus.cpp)
    target_link_libraries(nautilus-latency-corpus PRIVATE nautilus)

    # kernels whose loops run long enough for the quality of the generated code to show (tools/asm-diff)
    add_executable(nautilus-asm-kernels ${PROJECT_SOURCE_DIR}/tools/asm-diff/kernels.cpp)
    target_link_libraries(nautilus-asm-kernels PRIVATE nautilus)
endif ()
