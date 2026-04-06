include(CTest)
include(Catch)

# Main std execution test executable
add_executable(nautilus-std-tests
	STDProxyTest.cpp
	OstreamTest.cpp
	StringTest.cpp
	BitIntrinsicExecutionTest.cpp
	MemoryIntrinsicExecutionTest.cpp
)

target_link_libraries(nautilus-std-tests PUBLIC
	nautilus-std
	Catch2::Catch2WithMain
	nautilus-sanitizer-suppressions
)

target_include_directories(nautilus-std-tests PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
catch_discover_tests(nautilus-std-tests EXTRA_ARGS --allow-running-no-tests)

# LLVM IR intrinsic tests (requires MLIR backend)
if (ENABLE_MLIR_BACKEND)
	add_executable(nautilus-std-llvm-ir-tests
		LLVMIRIntrinsicTest.cpp
		LLVMIRBitIntrinsicTest.cpp
	)

	target_link_libraries(nautilus-std-llvm-ir-tests PRIVATE
		nautilus-std
		Catch2::Catch2WithMain
		nautilus-sanitizer-suppressions
	)

	target_include_directories(nautilus-std-llvm-ir-tests PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/nautilus/test/llvm-ir-test>)

	catch_discover_tests(nautilus-std-llvm-ir-tests
		TEST_PREFIX "std-llvm-ir:"
		EXTRA_ARGS --allow-running-no-tests
		PROPERTIES
			LABELS "std;llvm;ir"
	)
endif ()
