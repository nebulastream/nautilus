# GPU Backend Tests

# CPU fallback test (no GPU hardware required)
# TODO: Add test executable once implementation is complete
# add_executable(gpu-fallback-test GPUFallbackTest.cpp)
# target_link_libraries(gpu-fallback-test PRIVATE nautilus Catch2::Catch2WithMain)
# add_test(NAME gpu-fallback-test COMMAND gpu-fallback-test)

# GPU integration tests (require GPU hardware)
# if (ENABLE_GPU_CUDA)
#     add_executable(gpu-cuda-test GPULaunchTest.cpp)
#     target_link_libraries(gpu-cuda-test PRIVATE nautilus Catch2::Catch2WithMain)
#     add_test(NAME gpu-cuda-test COMMAND gpu-cuda-test)
# endif()
# if (ENABLE_GPU_METAL)
#     add_executable(gpu-metal-test GPULaunchTest.cpp)
#     target_link_libraries(gpu-metal-test PRIVATE nautilus Catch2::Catch2WithMain)
#     add_test(NAME gpu-metal-test COMMAND gpu-metal-test)
# endif()
