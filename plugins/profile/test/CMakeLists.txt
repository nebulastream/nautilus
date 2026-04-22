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

# Standalone demo that exercises nested Nautilus functions + invoke() +
# sampler and writes /tmp/nautilus-nested-profile.json. Not a Catch2 test —
# run manually: ./plugins/profile/test/demo_nested_profile
add_executable(demo_nested_profile DemoNestedProfile.cpp)
# -rdynamic so the sampler's dladdr sees nativeMix and other local symbols
# in backtraces.
set_target_properties(demo_nested_profile PROPERTIES ENABLE_EXPORTS TRUE)
target_link_libraries(demo_nested_profile PRIVATE nautilus-profile)
if (ENABLE_LOGGING)
    target_link_libraries(demo_nested_profile PRIVATE spdlog::spdlog)
endif ()
nautilus_inline(demo_nested_profile)
