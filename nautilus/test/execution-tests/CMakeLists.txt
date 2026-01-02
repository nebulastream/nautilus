include(CTest)
include(Catch)
add_executable(nautilus-execution-tests
        ExecutionTest.cpp
        BoolExecutionTest.cpp
        CastExecutionTest.cpp
        DumpHelperTest.cpp
        SelectExecutionTest.cpp
)

nautilus_inline(nautilus-execution-tests)

target_link_libraries(nautilus-execution-tests PUBLIC nautilus Catch2::Catch2WithMain)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-execution-tests PRIVATE spdlog::spdlog)
endif ()
target_include_directories(nautilus-execution-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
        $<INSTALL_INTERFACE:src/nautilus/>)
target_include_directories(nautilus-execution-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/nautilus/compiler/>
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

catch_discover_tests(nautilus-execution-tests EXTRA_ARGS --allow-running-no-tests)

if (ENABLE_TRACING)
    # SharedTrie/Map tests - separate binary with access to tracing internals
    add_executable(nautilus-tracing-datastructure-tests
            SharedTrieTest.cpp
            SharedHashMapTest.cpp
            TraceTest.cpp
    )
    target_include_directories(nautilus-tracing-datastructure-tests PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
    )
    target_link_libraries(nautilus-tracing-datastructure-tests PUBLIC Catch2::Catch2WithMain nautilus)
    catch_discover_tests(nautilus-tracing-datastructure-tests EXTRA_ARGS --allow-running-no-tests)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
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
    else ()
        message(WARNING "Tracing Tests are only available on clang-based compilers!")
    endif ()
endif ()

add_subdirectory(std)
