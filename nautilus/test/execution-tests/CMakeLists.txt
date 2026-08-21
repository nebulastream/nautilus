include(CTest)
include(Catch)
set(NAUTILUS_EXECUTION_TEST_SOURCES
        ExecutionTest.cpp
        AsmJitLoweringModeTest.cpp
        BCDispatchModeTest.cpp
        BCRegisterCoalescingTest.cpp
        TBCDispatchModeTest.cpp
        TBCJitModeTest.cpp
        TBCJitWhiteBoxTest.cpp
        TBCMemoryOffsetTest.cpp
        BoolExecutionTest.cpp
        CastExecutionTest.cpp
        CompilationStatisticsTest.cpp
        ConcurrentCompilationTest.cpp
        ConcurrentExecutionTest.cpp
        DumpHelperTest.cpp
        FunctionPtrExecutionTest.cpp
        ModuleLifecycleTest.cpp
        ModuleTest.cpp
        PostRAPeepholeTest.cpp
        TieredCompilationTest.cpp
        PointerExecutionTest.cpp
        RegionTest.cpp
        SelectExecutionTest.cpp
)

if (ENABLE_MLIR_BACKEND)
    list(APPEND NAUTILUS_EXECUTION_TEST_SOURCES
            DebugInfoExecutionTest.cpp)
endif ()

add_executable(nautilus-execution-tests ${NAUTILUS_EXECUTION_TEST_SOURCES})

target_link_libraries(nautilus-execution-tests PUBLIC nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-execution-tests PRIVATE spdlog::spdlog)
endif ()
target_include_directories(nautilus-execution-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>
        $<INSTALL_INTERFACE:src/nautilus/>)
target_include_directories(nautilus-execution-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/>
)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

catch_discover_tests(nautilus-execution-tests EXTRA_ARGS --allow-running-no-tests)

if (ENABLE_TRACING)
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

        target_link_libraries(nautilus-tracing-tests PUBLIC nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
        list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
        catch_discover_tests(nautilus-tracing-tests EXTRA_ARGS --allow-running-no-tests)
    else ()
        message(WARNING "Tracing Tests are only available on clang-based compilers!")
    endif ()
endif ()

