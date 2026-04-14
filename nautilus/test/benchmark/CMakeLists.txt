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

    # Plugin benchmarks: separate executable linking against nautilus-std and nautilus-simd.
    set(PLUGIN_BENCH_SOURCES PluginBenchmark.cpp)
    set(PLUGIN_BENCH_LIBS nautilus-std)

    if (ENABLE_SIMD_PLUGIN)
        list(APPEND PLUGIN_BENCH_SOURCES SimdBenchmark.cpp)
        list(APPEND PLUGIN_BENCH_LIBS nautilus-simd)
    endif ()

    add_executable(nautilus-plugin-benchmarks ${PLUGIN_BENCH_SOURCES})
    target_include_directories(nautilus-plugin-benchmarks PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
    target_link_libraries(nautilus-plugin-benchmarks PUBLIC
            ${PLUGIN_BENCH_LIBS} Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
    catch_discover_tests(nautilus-plugin-benchmarks EXTRA_ARGS --allow-running-no-tests)
endif ()
