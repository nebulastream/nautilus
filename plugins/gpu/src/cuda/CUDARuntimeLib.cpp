
#include "../GPURuntimeLib.hpp"

// TODO: #include <cuda.h>

namespace nautilus::gpu::runtime {

// TODO: Implementation wrapping CUDA Driver API.
// Each function maps to a CUDA Driver API call:
//
// gpuLoadModule  -> cuModuleLoadData()
//   - Takes PTX bytes, returns CUmodule (cast to void*)
//   - Must ensure CUDA context is initialized (cuInit, cuCtxCreate)
//
// gpuGetKernel   -> cuModuleGetFunction()
//   - Takes CUmodule + kernel name, returns CUfunction (cast to void*)
//
// gpuLaunchKernel -> cuLaunchKernel()
//   - Maps grid/block dims to CUDA launch parameters
//   - Passes args array directly (void**)
//
// gpuSynchronize -> cuCtxSynchronize()
//
// gpuMalloc      -> cuMemAlloc()
//   - Returns CUdeviceptr (cast to void*)
//
// gpuFree        -> cuMemFree()
//
// gpuMemcpyHtoD  -> cuMemcpyHtoD()
//
// gpuMemcpyDtoH  -> cuMemcpyDtoH()

} // namespace nautilus::gpu::runtime
