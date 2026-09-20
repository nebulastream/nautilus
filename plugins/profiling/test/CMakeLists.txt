include(CTest)
include(Catch)

set(PROFILING_TEST_SOURCES
        CounterTest.cpp
        MeasurementTest.cpp
        SampleReportTest.cpp
)

# The sampler needs a JIT backend to have something to symbolize; without the
# MLIR backend nothing publishes JIT symbols and the test would have nothing to
# assert. Its pure-formatting half (SampleReportTest) builds either way.
if (NAUTILUS_PROFILING_SUPPORTED AND ENABLE_MLIR_BACKEND)
    list(APPEND PROFILING_TEST_SOURCES SamplerTest.cpp BackendCoverageTest.cpp)
endif ()

add_executable(nautilus-profiling-tests ${PROFILING_TEST_SOURCES})

target_link_libraries(nautilus-profiling-tests PUBLIC nautilus-profiling Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-profiling-tests PRIVATE spdlog::spdlog)
endif ()

target_include_directories(nautilus-profiling-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/test/common>)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-profiling-tests
        TEST_PREFIX "profiling:"
        EXTRA_ARGS --allow-running-no-tests
        PROPERTIES LABELS "profiling")
