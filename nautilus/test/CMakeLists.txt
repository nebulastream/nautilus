
include(CTest)
include(Catch)

set(TEST_DATA_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/test/data/")

# Object library for sanitizer suppressions, linked into all test executables.
# Uses an OBJECT library to ensure the weak symbol is not discarded by the linker.
add_library(nautilus-sanitizer-suppressions OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/common/SanitizerSuppressions.cpp
)

add_subdirectory(benchmark)
add_subdirectory(execution-tests)
if (ENABLE_TRACING)
    add_subdirectory(ir-pass-tests)
endif ()
add_subdirectory(val-tests)

if(ENABLE_LLVM_IR_TEST)
    add_subdirectory(llvm-ir-test)
endif()

