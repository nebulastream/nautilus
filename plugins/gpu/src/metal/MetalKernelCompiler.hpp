#pragma once

#include "../GPUKernelCompiler.hpp"

namespace nautilus::gpu {

/// @brief Metal kernel compiler: MLIR -> MSL (Metal Shading Language) source code.
///
/// Since Apple's LLVM AIR backend is not open-source, this compiler generates
/// MSL source code that is compiled at runtime by the Metal framework via
/// MTLDevice::newLibraryWithSource:. This approach mirrors Nautilus's existing
/// C++ backend (nautilus/src/nautilus/compiler/backends/cpp/).
///
/// Pipeline:
/// 1. Clone kernel function into a fresh MLIR module
/// 2. Walk MLIR operations and emit corresponding MSL code:
///    - arith::AddIOp -> "+"
///    - arith::CmpIOp -> comparison operators
///    - LLVM::LoadOp / LLVM::StoreOp -> pointer dereference
///    - cf::BrOp / cf::CondBrOp -> goto / if-else
///    - Control flow maps directly to MSL control flow
/// 3. Replace GPU intrinsic calls with Metal kernel attributes:
///    - gpu_threadIdx_x -> thread_position_in_threadgroup.x
///    - gpu_blockIdx_x  -> threadgroup_position_in_grid.x
///    - gpu_blockDim_x  -> threads_per_threadgroup.x
///    - gpu_gridDim_x   -> threadgroups_per_grid.x
///    - gpu_syncThreads  -> threadgroup_barrier(mem_flags::mem_threadgroup)
/// 4. Generate kernel function signature with Metal attributes:
///    ```metal
///    kernel void myKernel(
///        device uint32_t* a [[buffer(0)]],
///        device uint32_t* b [[buffer(1)]],
///        uint3 threadIdx [[thread_position_in_threadgroup]],
///        uint3 blockIdx  [[threadgroup_position_in_grid]],
///        uint3 blockDim  [[threads_per_threadgroup]],
///        uint3 gridDim   [[threadgroups_per_grid]]
///    ) { ... }
///    ```
/// 5. Return MSL source as bytes (compiled at runtime by Metal framework)
class MetalKernelCompiler : public GPUKernelCompiler {
public:
	std::vector<uint8_t> compile(mlir::func::FuncOp kernelFunc, mlir::MLIRContext& context) override;
	std::string name() const override {
		return "Metal";
	}
};

} // namespace nautilus::gpu
