include(CTest)
include(Catch)

if (ENABLE_TRACING)
    add_executable(nautilus-scoped-tracing-tests
            PathPredicateStoreTest.cpp
            ScopedTraceContextTest.cpp
    )

    target_include_directories(nautilus-scoped-tracing-tests PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
            $<INSTALL_INTERFACE:src/nautilus/>)

    target_include_directories(nautilus-scoped-tracing-tests PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src>
            $<INSTALL_INTERFACE:src/nautilus/>)

    target_link_libraries(nautilus-scoped-tracing-tests PUBLIC nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    catch_discover_tests(nautilus-scoped-tracing-tests EXTRA_ARGS --allow-running-no-tests)
endif ()
