include(CTest)
include(Catch)

add_executable(nautilus-std-tests
        STDProxyTest.cpp
        OstreamTest.cpp
        StringTest.cpp
        VectorTest.cpp
        BitIntrinsicExecutionTest.cpp
        MemoryIntrinsicExecutionTest.cpp
        AtomicTest.cpp
)

nautilus_inline(nautilus-std-tests)

target_link_libraries(nautilus-std-tests PUBLIC nautilus-std Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-std-tests PRIVATE spdlog::spdlog)
endif ()

target_include_directories(nautilus-std-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/nautilus/test/common>)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-std-tests EXTRA_ARGS --allow-running-no-tests)

# LLVM IR comparison tests for std intrinsics
if (ENABLE_MLIR_BACKEND AND ENABLE_LLVM_IR_TEST)
    add_executable(nautilus-std-llvm-ir-test
            LLVMIRBitIntrinsicTest.cpp
            LLVMIRCMathIntrinsicTest.cpp
    )

    nautilus_inline(nautilus-std-llvm-ir-test)

    target_link_libraries(nautilus-std-llvm-ir-test
            PRIVATE
            nautilus-std
            Catch2::Catch2WithMain
            nautilus-sanitizer-suppressions
    )
    if (ENABLE_LOGGING)
        target_link_libraries(nautilus-std-llvm-ir-test PRIVATE spdlog::spdlog)
    endif ()

    target_include_directories(nautilus-std-llvm-ir-test PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/nautilus/test/llvm-ir-test>
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/plugins/std/src>
    )

    catch_discover_tests(nautilus-std-llvm-ir-test
            TEST_PREFIX "std-llvm-ir:"
            EXTRA_ARGS --allow-running-no-tests
            PROPERTIES LABELS "llvm;ir;std"
    )
endif ()
