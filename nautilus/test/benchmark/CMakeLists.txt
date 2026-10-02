if (ENABLE_TRACING AND ENABLE_BENCHMARKS)
    add_executable(nautilus-benchmarks
            TracingBenchmark.cpp
            ExecutionBenchmark.cpp
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

    # Runner behind tools/backend-explorer: compiles and runs BenchmarkKernels.hpp under one backend configuration.
    add_executable(nautilus-backend-explorer BackendExplorerRunner.cpp)
    target_include_directories(nautilus-backend-explorer PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>)
    target_link_libraries(nautilus-backend-explorer PRIVATE nautilus nautilus-sanitizer-suppressions)
endif ()
