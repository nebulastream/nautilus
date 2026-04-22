include(CTest)
include(Catch)

add_executable(nautilus-profile-tests
        BasicRegionTest.cpp
)

nautilus_inline(nautilus-profile-tests)

target_link_libraries(nautilus-profile-tests PUBLIC nautilus-profile Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-profile-tests PRIVATE spdlog::spdlog)
endif ()

target_include_directories(nautilus-profile-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/test/common>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/plugins/profile/src>)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-profile-tests EXTRA_ARGS --allow-running-no-tests)
