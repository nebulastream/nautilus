include(CTest)
include(Catch)

add_executable(nautilus-prefetch-tests
        PrefetchExecutionTest.cpp
)

nautilus_inline(nautilus-prefetch-tests)

target_link_libraries(nautilus-prefetch-tests PUBLIC nautilus-prefetch Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-prefetch-tests PRIVATE spdlog::spdlog)
endif ()

target_include_directories(nautilus-prefetch-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/test/common>)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-prefetch-tests EXTRA_ARGS --allow-running-no-tests)
