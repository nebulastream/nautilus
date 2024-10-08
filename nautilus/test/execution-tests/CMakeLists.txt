
add_executable(nautilus-execution-tests
        ExecutionTest.cpp
        BoolExecutionTest.cpp
        CastExecutionTest.cpp
)

target_link_libraries(nautilus-execution-tests PUBLIC nautilus Catch2::Catch2WithMain)
target_include_directories(nautilus-execution-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
        $<INSTALL_INTERFACE:src/nautilus/>)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

catch_discover_tests(nautilus-execution-tests EXTRA_ARGS --allow-running-no-tests)

if (ENABLE_TRACING AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    # using Clang
    add_executable(nautilus-tracing-tests
            TracingTest.cpp
    )

    target_include_directories(nautilus-tracing-tests PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
            $<INSTALL_INTERFACE:src/nautilus/>)

    target_include_directories(nautilus-tracing-tests PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
            $<INSTALL_INTERFACE:src/nautilus/>)

    target_link_libraries(nautilus-tracing-tests PUBLIC nautilus Catch2::Catch2WithMain)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    catch_discover_tests(nautilus-tracing-tests EXTRA_ARGS --allow-running-no-tests)
endif ()

add_subdirectory(std)