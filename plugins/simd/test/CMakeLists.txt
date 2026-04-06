include(CTest)
include(Catch)

add_executable(nautilus-simd-tests
        VectorExecutionTest.cpp
)

nautilus_inline(nautilus-simd-tests)

target_link_libraries(nautilus-simd-tests PUBLIC nautilus-simd Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-simd-tests PRIVATE spdlog::spdlog)
endif ()

target_include_directories(nautilus-simd-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-simd-tests EXTRA_ARGS --allow-running-no-tests)

# LLVM IR comparison tests for SIMD vector operations
if (ENABLE_MLIR_BACKEND AND ENABLE_LLVM_IR_TEST)
    add_executable(nautilus-simd-llvm-ir-test
            VectorLLVMIRTest.cpp
    )

    nautilus_inline(nautilus-simd-llvm-ir-test)

    target_link_libraries(nautilus-simd-llvm-ir-test
            PRIVATE
            nautilus-simd
            Catch2::Catch2WithMain
            nautilus-sanitizer-suppressions
    )
    if (ENABLE_LOGGING)
        target_link_libraries(nautilus-simd-llvm-ir-test PRIVATE spdlog::spdlog)
    endif ()

    # Access to LLVMIRTestUtil.hpp from the main test suite
    target_include_directories(nautilus-simd-llvm-ir-test PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/nautilus/test/llvm-ir-test>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/plugins/simd/src>
    )

    catch_discover_tests(nautilus-simd-llvm-ir-test
            TEST_PREFIX "simd-llvm-ir:"
            EXTRA_ARGS --allow-running-no-tests
            PROPERTIES LABELS "llvm;ir;simd"
    )
endif ()
