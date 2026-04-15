include(CTest)
include(Catch)

add_executable(nautilus-ir-pass-tests
        EmptyBlockEliminationTest.cpp
        IRVerifierTest.cpp
        IRPassManagerTest.cpp
)

target_link_libraries(nautilus-ir-pass-tests PRIVATE nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
target_include_directories(nautilus-ir-pass-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-ir-pass-tests EXTRA_ARGS --allow-running-no-tests)
