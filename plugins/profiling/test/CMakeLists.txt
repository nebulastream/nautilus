include(CTest)
include(Catch)

add_executable(nautilus-profiling-tests
        ProfilingTest.cpp
)

nautilus_inline(nautilus-profiling-tests)

target_link_libraries(nautilus-profiling-tests PUBLIC nautilus-profiling Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-profiling-tests PRIVATE spdlog::spdlog)
endif ()

target_include_directories(nautilus-profiling-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/test/common>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/plugins/profiling/src>)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-profiling-tests EXTRA_ARGS --allow-running-no-tests)
