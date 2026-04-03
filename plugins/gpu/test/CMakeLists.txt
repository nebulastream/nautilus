# GPU Backend Tests
include(CTest)
include(Catch)

# CPU fallback test (no GPU hardware required)
add_executable(nautilus-gpu-fallback-test GPUFallbackTest.cpp)
target_link_libraries(nautilus-gpu-fallback-test PRIVATE nautilus Catch2::Catch2WithMain)
target_include_directories(nautilus-gpu-fallback-test PRIVATE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
)
catch_discover_tests(nautilus-gpu-fallback-test EXTRA_ARGS --allow-running-no-tests)
