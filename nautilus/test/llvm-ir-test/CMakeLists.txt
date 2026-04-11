# LLVM IR Comparison Tests

if(NOT ENABLE_MLIR_BACKEND)
    message(STATUS "Skipping LLVM IR tests (MLIR backend not enabled)")
    return()
endif()

# Create test executable for LLVM IR tests
add_executable(nautilus-llvm-ir-test
    LLVMIRTest.cpp
    LLVMIRIntrinsicTest.cpp
)

target_link_libraries(nautilus-llvm-ir-test
    PRIVATE
    nautilus
    Catch2::Catch2WithMain
    nautilus-sanitizer-suppressions
)

target_include_directories(nautilus-llvm-ir-test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/nautilus/include
)

target_compile_definitions(nautilus-llvm-ir-test
    PRIVATE
    TEST_LLVMIR_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}"
)

# Set C++ standard
set_target_properties(nautilus-llvm-ir-test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Register with CTest
catch_discover_tests(nautilus-llvm-ir-test
    TEST_PREFIX "llvm-ir:"
    PROPERTIES
        LABELS "llvm;ir;regression"
)

# Custom target to generate reference IR
add_custom_target(generate-reference-ir
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/nautilus-llvm-ir-test --generate-reference
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating reference LLVM IR files"
    DEPENDS nautilus-llvm-ir-test
)
