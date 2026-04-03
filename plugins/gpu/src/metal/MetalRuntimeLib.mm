
#include "../GPURuntimeLib.hpp"

// TODO: #import <Metal/Metal.h>
// TODO: #import <Foundation/Foundation.h>

namespace nautilus::gpu::runtime {

// TODO: Implementation wrapping Metal framework.
// Each function maps to a Metal API call:
//
// gpuLoadModule -> MTLDevice::newLibraryWithSource:options:error:
//   - Takes MSL source string (kernelData), compiles at runtime
//   - Returns id<MTLLibrary> (cast to void*)
//   - Must manage default MTLDevice singleton
//   - Must create MTLCommandQueue (cached per device)
//
// gpuGetKernel -> MTLLibrary::newFunctionWithName: + MTLDevice::newComputePipelineStateWithFunction:
//   - Returns id<MTLComputePipelineState> (cast to void*)
//
// gpuLaunchKernel:
//   - Create MTLCommandBuffer from command queue
//   - Create MTLComputeCommandEncoder
//   - Set compute pipeline state
//   - Set buffer arguments: encoder.setBuffer(mtlBuffer, offset:0, index:N)
//     (args[i] must be MTLBuffer* -- created via gpuMalloc)
//   - Dispatch: encoder.dispatchThreadgroups(
//       MTLSize(gridX, gridY, gridZ),
//       threadsPerThreadgroup: MTLSize(blockX, blockY, blockZ))
//   - End encoding and commit command buffer
//
// gpuSynchronize -> MTLCommandBuffer::waitUntilCompleted
//   - Wait for the most recent command buffer to complete
//
// gpuMalloc -> MTLDevice::newBufferWithLength:options:
//   - Use MTLResourceStorageModeShared for unified memory (Apple Silicon)
//   - Returns id<MTLBuffer> (cast to void*)
//
// gpuFree -> Release MTLBuffer
//   - ARC handles deallocation when cast back to id<MTLBuffer>
//
// gpuMemcpyHtoD -> memcpy into MTLBuffer::contents()
//   - With StorageModeShared, CPU and GPU share the same memory
//   - Simple memcpy into buffer.contents is sufficient
//
// gpuMemcpyDtoH -> memcpy from MTLBuffer::contents()
//   - With StorageModeShared, just read from buffer.contents
//
// Note: Metal's unified memory model (MTLResourceStorageModeShared) on Apple
// Silicon means gpuMemcpyHtoD/DtoH are effectively just memcpy operations.
// No actual DMA transfer is needed.

} // namespace nautilus::gpu::runtime
