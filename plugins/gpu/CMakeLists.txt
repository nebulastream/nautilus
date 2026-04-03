# GPU Backend Plugin for Nautilus
# This plugin adds GPU kernel compilation support via the MLIR backend.
# It supports CUDA (NVIDIA) and Metal (Apple) targets.

# Platform detection and sub-backend selection
if (APPLE)
	option(ENABLE_GPU_METAL "Enable Metal GPU backend" ON)
	option(ENABLE_GPU_CUDA "Enable CUDA GPU backend" OFF)
else ()
	option(ENABLE_GPU_METAL "Enable Metal GPU backend" OFF)
	option(ENABLE_GPU_CUDA "Enable CUDA GPU backend" ON)
endif ()

# Public API headers
target_include_directories(nautilus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Common GPU sources (platform-agnostic)
add_source_files(nautilus
	${CMAKE_CURRENT_SOURCE_DIR}/src/MLIRGPUPass.cpp
)

# CUDA backend (Linux/Windows with NVIDIA GPU)
if (ENABLE_GPU_CUDA)
	find_package(CUDAToolkit REQUIRED)
	add_compile_definitions(ENABLE_GPU_CUDA)
	target_link_libraries(nautilus PRIVATE CUDA::cuda_driver)
	# LLVM NVPTX target for PTX code generation
	target_link_libraries(nautilus PRIVATE LLVMNVPTXCodeGen LLVMNVPTXDesc LLVMNVPTXInfo)
	add_source_files(nautilus
		${CMAKE_CURRENT_SOURCE_DIR}/src/cuda/CUDAKernelCompiler.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/cuda/CUDARuntimeLib.cpp
	)
	message(STATUS "GPU Backend: CUDA enabled")
endif ()

# Metal backend (macOS/iOS with Apple GPU)
if (ENABLE_GPU_METAL)
	add_compile_definitions(ENABLE_GPU_METAL)
	find_library(METAL_FRAMEWORK Metal REQUIRED)
	find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
	target_link_libraries(nautilus PRIVATE ${METAL_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
	add_source_files(nautilus
		${CMAKE_CURRENT_SOURCE_DIR}/src/metal/MetalKernelCompiler.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/metal/MetalRuntimeLib.mm
	)
	message(STATUS "GPU Backend: Metal enabled")
endif ()

if (NOT ENABLE_GPU_CUDA AND NOT ENABLE_GPU_METAL)
	message(WARNING "GPU Backend enabled but no GPU target selected (ENABLE_GPU_CUDA or ENABLE_GPU_METAL)")
endif ()

# GPU tests
if (ENABLE_TESTS)
	add_subdirectory(test)
endif ()
