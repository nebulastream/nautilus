add_executable(nautilus-common-tests
        ExceptionTransportTest.cpp
)

target_link_libraries(nautilus-common-tests PRIVATE nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-common-tests EXTRA_ARGS --allow-running-no-tests)
