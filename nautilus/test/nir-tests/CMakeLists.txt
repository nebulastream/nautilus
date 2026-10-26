add_executable(nautilus-nir-tests
        RecordTest.cpp
        ConstPoolTest.cpp
        BuilderTest.cpp
        RewriterTest.cpp
        VerifierModuleTest.cpp
)

target_link_libraries(nautilus-nir-tests PRIVATE nautilus-nir Catch2::Catch2WithMain)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-nir-tests EXTRA_ARGS --allow-running-no-tests)
