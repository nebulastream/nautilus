include(CTest)
include(Catch)
add_executable(nautilus-execution-tests
        ExecutionTest.cpp
        BoolExecutionTest.cpp
        CastExecutionTest.cpp
        DumpHelperTest.cpp
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

# SharedTrie tests - separate binary with access to tracing internals
add_executable(nautilus-shared-trie-tests
        SharedTrieTest.cpp
)
target_include_directories(nautilus-shared-trie-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
)
target_link_libraries(nautilus-shared-trie-tests PUBLIC Catch2::Catch2WithMain)
catch_discover_tests(nautilus-shared-trie-tests EXTRA_ARGS --allow-running-no-tests)

# SharedHashMap tests - separate binary with access to tracing internals
add_executable(nautilus-shared-hashmap-tests
        SharedHashMapTest.cpp
)
target_include_directories(nautilus-shared-hashmap-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
)
target_link_libraries(nautilus-shared-hashmap-tests PUBLIC Catch2::Catch2WithMain fmt::fmt)
catch_discover_tests(nautilus-shared-hashmap-tests EXTRA_ARGS --allow-running-no-tests)

if (ENABLE_TRACING)
    # Minimal Trace tests - standalone, doesn't require full nautilus
    add_executable(nautilus-trace-tests
            TraceTest.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/nautilus/tracing/TracerTrace.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/nautilus/tracing/Snapshot.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/nautilus/common/TypedValueRefHolder.cpp
    )
    target_include_directories(nautilus-trace-tests PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/nautilus/include>
    )
    target_link_libraries(nautilus-trace-tests PUBLIC Catch2::Catch2WithMain fmt::fmt)
    catch_discover_tests(nautilus-trace-tests EXTRA_ARGS --allow-running-no-tests)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # using Clang
        add_executable(nautilus-tracing-tests
                TracingTest.cpp
        )
        if (ENABLE_LOGGING)
            target_link_libraries(nautilus-tracing-tests PRIVATE spdlog::spdlog)
        endif ()
        set_target_properties(nautilus-tracing-tests PROPERTIES
                POSITION_INDEPENDENT_CODE OFF
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
