include(CTest)
include(Catch)
set(NAUTILUS_EXECUTION_TEST_SOURCES
        ExecutionTest.cpp
        AsmJitLoweringModeTest.cpp
        BCDispatchModeTest.cpp
        BCRegisterCoalescingTest.cpp
        TBCDispatchModeTest.cpp
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
        SelectExecutionTest.cpp
)

if (ENABLE_TRACING)
    # Exercises the stack-copy tracer's context-switch primitives, which are only
    # compiled when tracing is enabled.
    list(APPEND NAUTILUS_EXECUTION_TEST_SOURCES FiberContextTest.cpp)
endif ()

if (ENABLE_MLIR_BACKEND)
    list(APPEND NAUTILUS_EXECUTION_TEST_SOURCES
            MLIRJitEventListenerTest.cpp
            MLIRJitEventListenerTestFixture.cpp
            DebugInfoExecutionTest.cpp)
endif ()

add_executable(nautilus-execution-tests ${NAUTILUS_EXECUTION_TEST_SOURCES})

target_link_libraries(nautilus-execution-tests PUBLIC nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-execution-tests PRIVATE spdlog::spdlog)
endif ()
if (ENABLE_MLIR_BACKEND)
    # The fixture TU subclasses llvm::JITEventListener. Match how LLVM itself
    # is built (-fno-rtti) so the derived vtable does not reference typeinfo
    # symbols that the LLVM libs never export. The test TU itself still needs
    # RTTI (val<T> uses typeid).
    find_package(MLIR QUIET CONFIG)
    if (MLIR_INCLUDE_DIRS)
        target_include_directories(nautilus-execution-tests SYSTEM PRIVATE ${MLIR_INCLUDE_DIRS})
    endif ()
    set_source_files_properties(MLIRJitEventListenerTestFixture.cpp
            PROPERTIES COMPILE_OPTIONS -fno-rtti)
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

