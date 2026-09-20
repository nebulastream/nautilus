include(CTest)
include(Catch)

add_executable(nautilus-builtin-tests
        prefetch/PrefetchExecutionTest.cpp
)

nautilus_inline(nautilus-builtin-tests)

target_link_libraries(nautilus-builtin-tests PUBLIC nautilus-builtin Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-builtin-tests PRIVATE spdlog::spdlog)
endif ()

target_include_directories(nautilus-builtin-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/test/common>)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-builtin-tests EXTRA_ARGS --allow-running-no-tests)

# LLVM IR comparison tests: verify prefetch() lowers to llvm.prefetch on the MLIR backend
if (ENABLE_MLIR_BACKEND AND ENABLE_LLVM_IR_TEST)
    add_executable(nautilus-builtin-llvm-ir-test
            prefetch/PrefetchLLVMIRTest.cpp
    )

    nautilus_inline(nautilus-builtin-llvm-ir-test)

    target_link_libraries(nautilus-builtin-llvm-ir-test
            PRIVATE
            nautilus-builtin
            Catch2::Catch2WithMain
            nautilus-sanitizer-suppressions
    )
    if (ENABLE_LOGGING)
        target_link_libraries(nautilus-builtin-llvm-ir-test PRIVATE spdlog::spdlog)
    endif ()

    # Access to LLVMIRTestUtil.hpp from the main test suite
    target_include_directories(nautilus-builtin-llvm-ir-test PRIVATE
            $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/test/llvm-ir-test>
    )

    catch_discover_tests(nautilus-builtin-llvm-ir-test
            TEST_PREFIX "builtin-llvm-ir:"
            EXTRA_ARGS --allow-running-no-tests
            PROPERTIES LABELS "llvm;ir;builtin"
    )
endif ()
