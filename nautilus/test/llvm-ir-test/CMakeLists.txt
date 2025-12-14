# LLVM IR Comparison Tests

if(NOT ENABLE_MLIR_BACKEND)
    message(STATUS "Skipping LLVM IR tests (MLIR backend not enabled)")
    return()
endif()

# Create test executable for general LLVM IR tests
add_executable(nautilus-llvm-ir-test
    LLVMIRTest.cpp
)

target_link_libraries(nautilus-llvm-ir-test
    PRIVATE
    nautilus
    Catch2::Catch2WithMain
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

# Create test executable for intrinsic LLVM IR tests
add_executable(nautilus-llvm-ir-intrinsic-test
    LLVMIRIntrinsicTest.cpp
)

target_link_libraries(nautilus-llvm-ir-intrinsic-test
    PRIVATE
    nautilus
    Catch2::Catch2WithMain
)

target_include_directories(nautilus-llvm-ir-intrinsic-test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/nautilus/include
)

target_compile_definitions(nautilus-llvm-ir-intrinsic-test
    PRIVATE
    TEST_LLVMIR_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}"
)

# Set C++ standard
set_target_properties(nautilus-llvm-ir-intrinsic-test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Register intrinsic tests with CTest
catch_discover_tests(nautilus-llvm-ir-intrinsic-test
    TEST_PREFIX "llvm-ir-intrinsic:"
    PROPERTIES
        LABELS "llvm;ir;intrinsic;regression"
)

# Add custom test for llvm-diff comparison
find_program(LLVM_DIFF llvm-diff)
if(LLVM_DIFF)
    message(STATUS "Found llvm-diff: ${LLVM_DIFF}")

    # Custom target to run IR comparison
    add_test(
        NAME llvm-ir:llvm-diff-comparison
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/compare-ir.sh
                ${CMAKE_BINARY_DIR}/nautilus-ir-test-output
    )

    set_tests_properties(llvm-ir:llvm-diff-comparison PROPERTIES
        LABELS "llvm;ir;regression"
        DEPENDS nautilus-llvm-ir-test
    )
else()
    message(WARNING "llvm-diff not found - IR comparison tests will be skipped")
endif()

# Custom target to generate reference IR
add_custom_target(generate-reference-ir
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/nautilus-llvm-ir-test --generate-reference
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating reference LLVM IR files"
    DEPENDS nautilus-llvm-ir-test
)

# Install test scripts
install(
    PROGRAMS compare-ir.sh
    DESTINATION test/llvm-ir-test
)
