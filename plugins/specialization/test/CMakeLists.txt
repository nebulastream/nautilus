include(CTest)
include(Catch)

add_executable(nautilus-specialization-tests
        AssumeExecutionTest.cpp
)

nautilus_inline(nautilus-specialization-tests)

target_link_libraries(nautilus-specialization-tests PUBLIC nautilus-specialization Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-specialization-tests PRIVATE spdlog::spdlog)
endif ()

target_include_directories(nautilus-specialization-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/test/common>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/plugins/specialization/src>)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-specialization-tests EXTRA_ARGS --allow-running-no-tests)

# LLVM IR comparison tests for the specialization plugin's assume intrinsics
if (ENABLE_MLIR_BACKEND AND ENABLE_LLVM_IR_TEST)
    add_executable(nautilus-specialization-llvm-ir-test
            LLVMIRAssumeTest.cpp
    )

    nautilus_inline(nautilus-specialization-llvm-ir-test)

    target_link_libraries(nautilus-specialization-llvm-ir-test
            PRIVATE
            nautilus-specialization
            Catch2::Catch2WithMain
            nautilus-sanitizer-suppressions
    )
    if (ENABLE_LOGGING)
        target_link_libraries(nautilus-specialization-llvm-ir-test PRIVATE spdlog::spdlog)
    endif ()

    target_include_directories(nautilus-specialization-llvm-ir-test PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
            $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/test/llvm-ir-test>
            $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/plugins/specialization/src>
    )

    catch_discover_tests(nautilus-specialization-llvm-ir-test
            TEST_PREFIX "specialization-llvm-ir:"
            EXTRA_ARGS --allow-running-no-tests
            PROPERTIES LABELS "llvm;ir;specialization"
    )
endif ()
