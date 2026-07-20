include(CTest)
include(Catch)

add_executable(nautilus-tracing-unit-tests
        AliveVariableHashTest.cpp
        ArenaTest.cpp
        SSACreationPhaseTest.cpp
        SSAVerifierTest.cpp
)

target_link_libraries(nautilus-tracing-unit-tests PRIVATE nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
target_include_directories(nautilus-tracing-unit-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-tracing-unit-tests EXTRA_ARGS --allow-running-no-tests)
