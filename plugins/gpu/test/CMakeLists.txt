enable_testing()
include(CTest)
include(Catch)

set(GPU_TEST_DATA_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/data/")

# GPU execution tests (interpreter + compiler backends)
add_executable(nautilus-gpu-execution-tests
        GPUExecutionTest.cpp
)
nautilus_inline(nautilus-gpu-execution-tests)
target_link_libraries(nautilus-gpu-execution-tests PUBLIC nautilus-gpu Catch2::Catch2WithMain)
target_include_directories(nautilus-gpu-execution-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>)
# Only enable Metal GPU execution tests on Apple (requires Metal device + toolchain at runtime)
if (ENABLE_METAL_BACKEND AND APPLE)
    target_compile_definitions(nautilus-gpu-execution-tests PRIVATE ENABLE_METAL_EXECUTION)
endif ()
catch_discover_tests(nautilus-gpu-execution-tests EXTRA_ARGS --allow-running-no-tests)

if (ENABLE_TRACING)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # GPU tracing tests (trace / SSA / IR reference comparison)
        add_executable(nautilus-gpu-tracing-tests
                GPUTracingTest.cpp
        )
        nautilus_inline(nautilus-gpu-tracing-tests)
        target_link_libraries(nautilus-gpu-tracing-tests PUBLIC nautilus-gpu Catch2::Catch2WithMain)
        target_include_directories(nautilus-gpu-tracing-tests PRIVATE
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
                $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/nautilus/src>)
        target_compile_definitions(nautilus-gpu-tracing-tests PRIVATE GPU_TEST_DATA_FOLDER="${GPU_TEST_DATA_FOLDER}")
        catch_discover_tests(nautilus-gpu-tracing-tests EXTRA_ARGS --allow-running-no-tests)

        # GPU codegen tests (CUDA/Metal source generation reference comparison)
        if (ENABLE_CUDA_BACKEND OR ENABLE_METAL_BACKEND)
            add_executable(nautilus-gpu-codegen-tests
                    GPUCodegenTest.cpp
            )
            nautilus_inline(nautilus-gpu-codegen-tests)
            target_link_libraries(nautilus-gpu-codegen-tests PUBLIC nautilus-gpu Catch2::Catch2WithMain)
            target_include_directories(nautilus-gpu-codegen-tests PRIVATE
                    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
                    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/nautilus/src>)
            target_compile_definitions(nautilus-gpu-codegen-tests PRIVATE GPU_TEST_DATA_FOLDER="${GPU_TEST_DATA_FOLDER}")
            catch_discover_tests(nautilus-gpu-codegen-tests EXTRA_ARGS --allow-running-no-tests)
        endif ()
    endif ()
endif ()
