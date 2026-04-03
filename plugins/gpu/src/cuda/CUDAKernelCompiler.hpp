#pragma once

#include "../GPUKernelCompiler.hpp"

namespace nautilus::gpu {

/// @brief CUDA kernel compiler: MLIR -> PTX via LLVM NVPTX backend.
///
/// Pipeline:
/// 1. Clone kernel function into a fresh MLIR module
/// 2. Replace GPU intrinsic proxy calls with NVVM LLVM intrinsics:
///    - gpu_threadIdx_x -> llvm.nvvm.read.ptx.sreg.tid.x
///    - gpu_blockIdx_x  -> llvm.nvvm.read.ptx.sreg.ctaid.x
///    - gpu_blockDim_x  -> llvm.nvvm.read.ptx.sreg.ntid.x
///    - gpu_gridDim_x   -> llvm.nvvm.read.ptx.sreg.nctaid.x
///    - gpu_syncThreads  -> llvm.nvvm.barrier0
///    (and y/z variants)
/// 3. Add nvvm.kernel attribute to the function
/// 4. Run standard MLIR lowering passes (Math->LLVM, Func->LLVM, CF->LLVM, Arith->LLVM)
/// 5. Translate MLIR to LLVM IR with target triple nvptx64-nvidia-cuda
/// 6. Run LLVM optimization passes for NVPTX
/// 7. Emit PTX via LLVM NVPTX backend
///
/// All operations stay within the LLVM dialect -- NVVM intrinsics are expressed
/// as llvm.call ops to well-known LLVM intrinsic names.
class CUDAKernelCompiler : public GPUKernelCompiler {
public:
	std::vector<uint8_t> compile(mlir::func::FuncOp kernelFunc, mlir::MLIRContext& context) override;
	std::string name() const override {
		return "CUDA";
	}
};

} // namespace nautilus::gpu
