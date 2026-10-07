include(CTest)
include(Catch)
set(NAUTILUS_EXECUTION_TEST_SOURCES
        ExecutionTest.cpp
        backends/asmjit/AsmJitLoweringModeTest.cpp
        backends/asmjit/PostRAPeepholeTest.cpp
        backends/bc/BCDispatchModeTest.cpp
        backends/bc/BCRegisterCoalescingTest.cpp
        backends/bc/BCExceptionDispatchTest.cpp
        backends/tbc/TBCDispatchModeTest.cpp
        backends/tbc/TBCJitModeTest.cpp
        backends/tbc/TBCJitWhiteBoxTest.cpp
        backends/tbc/TBCMemoryOffsetTest.cpp
        BoolExecutionTest.cpp
        CalleeConformanceTest.cpp
        CastExecutionTest.cpp
        NarrowCallArgumentTest.cpp
        CompilationStatisticsTest.cpp
        ConcurrentCompilationTest.cpp
        ConcurrentExecutionTest.cpp
        DumpHelperTest.cpp
        FunctionPtrExecutionTest.cpp
		InvokeExceptionExecutionTest.cpp
        NoThrowInferenceIntegrationTest.cpp
        MemoryIntrinsicExecutionTest.cpp
        ModuleLifecycleTest.cpp
        ModuleTest.cpp
        TieredCompilationTest.cpp
        PointerExecutionTest.cpp
        RegionTest.cpp
        SelectExecutionTest.cpp
)

if (ENABLE_TRACING)
    list(APPEND NAUTILUS_EXECUTION_TEST_SOURCES ArtifactPreflightTest.cpp AllocationOriginTest.cpp AllocationOriginOther.cpp)
endif ()

if (ENABLE_BC_BACKEND)
    list(APPEND NAUTILUS_EXECUTION_TEST_SOURCES
            backends/bc/CompilationStatisticsTest.cpp)
endif ()

if (ENABLE_TBC_BACKEND)
    list(APPEND NAUTILUS_EXECUTION_TEST_SOURCES
            backends/tbc/CompilationStatisticsTest.cpp)
endif ()

if (ENABLE_ASMJIT_BACKEND)
    list(APPEND NAUTILUS_EXECUTION_TEST_SOURCES
            backends/asmjit/ConcurrentExecutionTest.cpp)
endif ()

if (ENABLE_MLIR_BACKEND)
    list(APPEND NAUTILUS_EXECUTION_TEST_SOURCES
            ABIExtensionAttributeTest.cpp
            backends/mlir/ModuleArtifactTest.cpp
            backends/mlir/ArtifactReviewRegressionTest.cpp
            backends/mlir/ArtifactTransportValidationTest.cpp
            DebugInfoExecutionTest.cpp
            PerfJitDumpExecutionTest.cpp)
endif ()

# NOT listed: BitIntrinsicExecutionTest.cpp. It is written against a bit
# intrinsic API (popcount / countl_zero / byteswap / rotl / rotr on val<T>) and
# a BitIntrinsicFunctions.hpp that Nautilus does not have -- unlike
# val_memcpy.hpp, no val_bit.hpp was ever added. Building it needs that feature
# first, not a source-list entry.

add_executable(nautilus-execution-tests ${NAUTILUS_EXECUTION_TEST_SOURCES})

target_link_libraries(nautilus-execution-tests PUBLIC nautilus Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(nautilus-execution-tests PRIVATE -fPIE)
    target_link_options(nautilus-execution-tests PRIVATE -pie "LINKER:--build-id=sha1")
endif ()
if (ENABLE_MLIR_BACKEND AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_library(nautilus-artifact-unidentified-proxy SHARED backends/mlir/ArtifactUnidentifiedProxy.cpp)
    target_link_options(nautilus-artifact-unidentified-proxy PRIVATE "LINKER:--build-id=none")
    target_link_libraries(nautilus-execution-tests PRIVATE nautilus-artifact-unidentified-proxy)
    add_library(nautilus-artifact-identified-proxy SHARED backends/mlir/ArtifactIdentifiedProxy.cpp)
    target_link_options(nautilus-artifact-identified-proxy PRIVATE "LINKER:--build-id=sha1")
    target_link_libraries(nautilus-execution-tests PRIVATE nautilus-artifact-identified-proxy)
    set_source_files_properties(backends/mlir/ModuleArtifactTest.cpp PROPERTIES
            COMPILE_DEFINITIONS ARTIFACT_IDENTIFIED_PROXY_PATH="$<TARGET_FILE:nautilus-artifact-identified-proxy>")
endif ()
if (ENABLE_MLIR_BACKEND)
    find_package(MLIR REQUIRED CONFIG)
    target_include_directories(nautilus-execution-tests SYSTEM PRIVATE ${MLIR_INCLUDE_DIRS})
    target_link_libraries(nautilus-execution-tests PRIVATE MLIRParser MLIRBytecodeWriter MLIRLLVMDialect)
endif ()
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
