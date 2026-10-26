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

    # The latency corpus (docs/plans/nautilus-0.4/10-observability.md). A plain executable that replaces the global
    # operator new to count allocations; it uses only the public engine API.
    add_executable(nautilus-latency-corpus latency-corpus/LatencyCorpus.cpp)
    target_link_libraries(nautilus-latency-corpus PRIVATE nautilus)
endif ()
