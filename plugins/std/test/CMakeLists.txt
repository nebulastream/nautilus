include(CTest)
include(Catch)

set(STD_PLUGIN_TEST_DATA_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/")

# Main std execution test executable
add_executable(nautilus-std-tests
	STDProxyTest.cpp
	OstreamTest.cpp
	StringTest.cpp
	BitIntrinsicExecutionTest.cpp
	MemoryIntrinsicExecutionTest.cpp
	ValueTypeExecutionTest.cpp
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

# Value type tracing tests (requires Clang for tracing support)
if (ENABLE_TRACING)
	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		add_executable(nautilus-std-tracing-tests
			ValueTypeTracingTest.cpp
		)

		target_link_libraries(nautilus-std-tracing-tests PUBLIC
			nautilus-std
			Catch2::Catch2WithMain
			nautilus-sanitizer-suppressions
		)

		target_include_directories(nautilus-std-tracing-tests PRIVATE
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
			$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/nautilus/src>)

		target_compile_definitions(nautilus-std-tracing-tests PRIVATE
			STD_PLUGIN_TEST_DATA_FOLDER="${STD_PLUGIN_TEST_DATA_FOLDER}")

		catch_discover_tests(nautilus-std-tracing-tests EXTRA_ARGS --allow-running-no-tests)
	else ()
		message(WARNING "Value Type Tracing Tests are only available on clang-based compilers!")
	endif ()
endif ()

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
