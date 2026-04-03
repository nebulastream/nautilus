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

# Public API headers (always available for CPU fallback)
target_include_directories(nautilus PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# GPU pass sources and include path (so MLIRCompilationBackend.cpp can find MLIRGPUPass.hpp)
# We use target_sources directly because plugins/gpu is added after nautilus/CMakeLists.txt
# processes get_source(), so add_source_files() would not be picked up.
target_include_directories(nautilus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_sources(nautilus PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/MLIRGPUPass.cpp
)

# CUDA backend (Linux/Windows with NVIDIA GPU)
if (ENABLE_GPU_CUDA)
	find_package(CUDAToolkit QUIET)
	if (CUDAToolkit_FOUND)
		add_compile_definitions(ENABLE_GPU_CUDA)
		target_link_libraries(nautilus PRIVATE CUDA::cuda_driver)
		target_sources(nautilus PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/src/cuda/CUDAKernelCompiler.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/src/cuda/CUDARuntimeLib.cpp
		)

		# Find libNVVM for compiling NVVM IR to PTX.
		# libNVVM is part of the CUDA toolkit, located in the nvvm/ subdirectory.
		find_library(NVVM_LIBRARY nvvm
			HINTS
			${CUDAToolkit_LIBRARY_DIR}/../nvvm/lib64
			${CUDAToolkit_LIBRARY_DIR}/../nvvm/lib
			${CUDAToolkit_LIBRARY_ROOT}/../nvvm/lib64
		)
		find_path(NVVM_INCLUDE_DIR nvvm.h
			HINTS
			${CUDAToolkit_LIBRARY_DIR}/../nvvm/include
			${CUDAToolkit_INCLUDE_DIRS}/../nvvm/include
		)
		if (NVVM_LIBRARY AND NVVM_INCLUDE_DIR)
			add_compile_definitions(HAVE_NVVM)
			target_link_libraries(nautilus PRIVATE ${NVVM_LIBRARY})
			target_include_directories(nautilus PRIVATE ${NVVM_INCLUDE_DIR})
			message(STATUS "GPU Backend: CUDA enabled with libNVVM (${NVVM_LIBRARY})")
		else ()
			message(STATUS "GPU Backend: CUDA enabled without libNVVM (PTX compilation will not work)")
			message(STATUS "GPU Backend: libNVVM searched in ${CUDAToolkit_LIBRARY_DIR}/../nvvm/")
		endif ()
	else ()
		message(WARNING "GPU Backend: CUDA requested but CUDAToolkit not found. CUDA backend disabled.")
		set(ENABLE_GPU_CUDA OFF)
	endif ()
endif ()

# Metal backend (macOS/iOS with Apple GPU)
if (ENABLE_GPU_METAL)
	find_library(METAL_FRAMEWORK Metal QUIET)
	find_library(FOUNDATION_FRAMEWORK Foundation QUIET)
	if (METAL_FRAMEWORK AND FOUNDATION_FRAMEWORK)
		add_compile_definitions(ENABLE_GPU_METAL)
		target_link_libraries(nautilus PRIVATE ${METAL_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
		target_sources(nautilus PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/src/metal/MetalKernelCompiler.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/src/metal/MetalRuntimeLib.mm
		)
		message(STATUS "GPU Backend: Metal enabled")
	else ()
		message(WARNING "GPU Backend: Metal requested but Metal framework not found. Metal backend disabled.")
		set(ENABLE_GPU_METAL OFF)
	endif ()
endif ()

if (NOT ENABLE_GPU_CUDA AND NOT ENABLE_GPU_METAL)
	message(STATUS "GPU Backend: No GPU target available. CPU fallback only.")
endif ()

# GPU tests
if (ENABLE_TESTS)
	add_subdirectory(test)
endif ()
