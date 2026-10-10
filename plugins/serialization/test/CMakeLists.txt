include(CTest)
include(Catch)

add_executable(nautilus-serialization-tests
        IRSerializationTest.cpp
)

target_link_libraries(nautilus-serialization-tests PRIVATE nautilus-serialization Catch2::Catch2WithMain
        nautilus-sanitizer-suppressions)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-serialization-tests PRIVATE spdlog::spdlog)
endif ()

# The tests trace functions and drive backends directly, so they need
# nautilus's internal headers and the plugin's own (for the checksum).
target_include_directories(nautilus-serialization-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-serialization-tests EXTRA_ARGS --allow-running-no-tests)

# Golden round-trip: the core tracing golden tests, rebuilt so that every IR
# they check is also serialized and materialized in both pointer modes (see
# NAUTILUS_TEST_IR_SERIALIZATION_ROUNDTRIP in TracingTest.cpp). The core suite
# builds those tests with Clang only, and so does this.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(NAUTILUS_EXECUTION_TESTS_DIR ${nautilus_SOURCE_DIR}/nautilus/test/execution-tests)
    add_executable(nautilus-serialization-golden-tests
            ${NAUTILUS_EXECUTION_TESTS_DIR}/TracingTest.cpp
    )
    target_compile_definitions(nautilus-serialization-golden-tests PRIVATE NAUTILUS_TEST_IR_SERIALIZATION_ROUNDTRIP)
    target_link_libraries(nautilus-serialization-golden-tests PRIVATE nautilus-serialization
            Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
    target_include_directories(nautilus-serialization-golden-tests PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/test/common>
            $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>)
    catch_discover_tests(nautilus-serialization-golden-tests
            TEST_PREFIX "serialization-golden:"
            EXTRA_ARGS --allow-running-no-tests)
endif ()
