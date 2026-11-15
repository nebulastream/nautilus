# Plugin pass hooks (plan M3.9). The extension registry freezes with the first engine of the process and the passes of
# the test plugin are registered for the whole process, so these tests are a binary of their own: no other test sees a
# plugin pass, and the statistics of a default compile (`irPasses.pipelineIterations` = 0) stay checkable elsewhere.
if (ENABLE_TRACING)
    add_executable(nautilus-pass-tests PluginPassTest.cpp)
    target_link_libraries(nautilus-pass-tests PRIVATE nautilus nautilus-nir Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
    target_include_directories(nautilus-pass-tests PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/>)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    catch_discover_tests(nautilus-pass-tests EXTRA_ARGS --allow-running-no-tests)
endif ()
