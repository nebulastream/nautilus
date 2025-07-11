add_executable(nautilus-std-tests
        STDProxyTest.cpp
        OstreamTest.cpp
        StringTest.cpp
)

target_link_libraries(nautilus-std-tests PUBLIC nautilus Catch2::Catch2WithMain)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-std-tests EXTRA_ARGS --allow-running-no-tests)
