include(CTest)
include(Catch)

add_executable(nautilus-inlining-tests
        InliningExecutionTest.cpp
        FunctionPtrInliningTest.cpp
        RegistryThreadSafetyTest.cpp
)

find_program(LLVM_AS_EXECUTABLE NAMES llvm-as-21 llvm-as HINTS ${LLVM_TOOLS_BINARY_DIR} REQUIRED)
set(PYTHON_UDF_BITCODE ${CMAKE_CURRENT_BINARY_DIR}/fixtures/python-add-export.bc)
set(ISOLATED_ADD_UDF_BITCODE ${CMAKE_CURRENT_BINARY_DIR}/fixtures/isolated-add.bc)
set(ISOLATED_SUB_UDF_BITCODE ${CMAKE_CURRENT_BINARY_DIR}/fixtures/isolated-sub.bc)
add_custom_command(
		OUTPUT ${PYTHON_UDF_BITCODE} ${ISOLATED_ADD_UDF_BITCODE} ${ISOLATED_SUB_UDF_BITCODE}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fixtures
		COMMAND ${LLVM_AS_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/python-add-export.ll -o ${PYTHON_UDF_BITCODE}
		COMMAND ${LLVM_AS_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/isolated-add.ll -o ${ISOLATED_ADD_UDF_BITCODE}
		COMMAND ${LLVM_AS_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/isolated-sub.ll -o ${ISOLATED_SUB_UDF_BITCODE}
		DEPENDS
			${CMAKE_CURRENT_SOURCE_DIR}/fixtures/python-add-export.ll
			${CMAKE_CURRENT_SOURCE_DIR}/fixtures/isolated-add.ll
			${CMAKE_CURRENT_SOURCE_DIR}/fixtures/isolated-sub.ll
		VERBATIM)
add_custom_target(nautilus-python-udf-bitcode DEPENDS
		${PYTHON_UDF_BITCODE} ${ISOLATED_ADD_UDF_BITCODE} ${ISOLATED_SUB_UDF_BITCODE})
add_dependencies(nautilus-inlining-tests nautilus-python-udf-bitcode)
target_compile_definitions(nautilus-inlining-tests PRIVATE
		NAUTILUS_PYTHON_UDF_BITCODE_PATH="${PYTHON_UDF_BITCODE}"
		NAUTILUS_ISOLATED_ADD_UDF_BITCODE_PATH="${ISOLATED_ADD_UDF_BITCODE}"
		NAUTILUS_ISOLATED_SUB_UDF_BITCODE_PATH="${ISOLATED_SUB_UDF_BITCODE}")

nautilus_inline(nautilus-inlining-tests)

target_link_libraries(nautilus-inlining-tests PUBLIC nautilus-inlining Catch2::Catch2WithMain nautilus-sanitizer-suppressions)
if (ENABLE_LOGGING)
    target_link_libraries(nautilus-inlining-tests PRIVATE spdlog::spdlog)
endif ()

# The plugin tests drive the same API as the core execution tests, but they
# deliberately include their OWN annotated copies of the common helpers
# (under plugins/inlining/test/common) so NAUTILUS_INLINE stays confined to
# the plugin. The core nautilus/test/common dir provides ExecutionTest.hpp
# (backend iteration helpers) which both trees share. The inlining-plugin
# smoke test additionally needs to see LLVMBackendHooks.hpp from the core
# nautilus src tree to verify the hook registration.
target_include_directories(nautilus-inlining-tests PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/test/common>
        $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/src>)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-inlining-tests EXTRA_ARGS --allow-running-no-tests)

# LLVM IR comparison tests for the inlining plugin. Gated on the same switch
# as the core nautilus-llvm-ir-test and the other plugin LLVM IR tests.
# The reference IR under `inlining-ir/` must diverge from the non-inlined
# reference IR under `nautilus/test/llvm-ir-test/reference-ir/`: if any of
# these tests starts matching the non-inlined core reference, the JIT-time
# inliner has silently regressed.
if (ENABLE_MLIR_BACKEND AND ENABLE_LLVM_IR_TEST)
    add_executable(nautilus-inlining-llvm-ir-test
            LLVMIRInliningTest.cpp
    )

    nautilus_inline(nautilus-inlining-llvm-ir-test)

    target_link_libraries(nautilus-inlining-llvm-ir-test
            PRIVATE
            nautilus-inlining
            Catch2::Catch2WithMain
            nautilus-sanitizer-suppressions
    )
    if (ENABLE_LOGGING)
        target_link_libraries(nautilus-inlining-llvm-ir-test PRIVATE spdlog::spdlog)
    endif ()

    target_include_directories(nautilus-inlining-llvm-ir-test PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
            $<BUILD_INTERFACE:${nautilus_SOURCE_DIR}/nautilus/test/llvm-ir-test>
    )

    catch_discover_tests(nautilus-inlining-llvm-ir-test
            TEST_PREFIX "inlining-llvm-ir:"
            EXTRA_ARGS --allow-running-no-tests
            PROPERTIES LABELS "llvm;ir;inlining"
    )
endif ()
