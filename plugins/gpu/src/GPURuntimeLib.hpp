#pragma once

#include <cstddef>
#include <cstdint>

/// @file GPURuntimeLib.hpp
/// @brief Unified GPU runtime interface.
///
/// These functions have the SAME signature on all platforms (CUDA, Metal).
/// The implementation differs based on which backend is compiled (ENABLE_GPU_CUDA / ENABLE_GPU_METAL).
/// They are registered as external proxy functions in the JIT-compiled host code.

namespace nautilus::gpu::runtime {

/// Load a compiled GPU module (PTX for CUDA, pre-compiled metallib for Metal).
/// @return Opaque handle to the loaded module.
void* gpuLoadModule(const void* kernelData, size_t kernelSize);

/// Get a kernel function handle from a loaded module.
/// @return Opaque handle to the kernel (CUfunction for CUDA, MTLComputePipelineState* for Metal).
void* gpuGetKernel(void* module, const char* kernelName);

/// Launch a GPU kernel with the given grid and block dimensions.
/// @param args Array of pointers to kernel arguments.
void gpuLaunchKernel(void* kernel, uint32_t gridX, uint32_t gridY, uint32_t gridZ, uint32_t blockX, uint32_t blockY,
                     uint32_t blockZ, void** args, size_t sharedMem);

/// Synchronize the GPU device (wait for all pending operations to complete).
void gpuSynchronize();

/// Allocate device memory.
void* gpuMalloc(size_t size);

/// Free device memory.
void gpuFree(void* ptr);

/// Copy data from host to device.
void gpuMemcpyHtoD(void* dst, const void* src, size_t size);

/// Copy data from device to host.
void gpuMemcpyDtoH(void* dst, const void* src, size_t size);

} // namespace nautilus::gpu::runtime
