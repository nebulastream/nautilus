#pragma once

#include <memory>

namespace mlir {
class ModuleOp;
template <typename OpTy>
class OwningOpRef;
} // namespace mlir

namespace nautilus {
class DumpHandler;
namespace engine {
class Options;
} // namespace engine
} // namespace nautilus

namespace nautilus::gpu {

class GPUKernelCompiler;

/// @brief Post-lowering MLIR pass for GPU kernel compilation.
///
/// This pass runs after MLIR generation but before the standard lowering passes.
/// It performs the following steps:
///
/// 1. **Pattern Detection:** Scans the host function for the sentinel sequence:
///    - call @gpu_set_grid(gx, gy, gz)
///    - call @gpu_set_block(bx, by, bz)
///    - call @kernel_name(args...)
///
/// 2. **Kernel Extraction:** Clones the kernel function into a separate MLIR module.
///
/// 3. **Kernel Compilation:** Delegates to the platform-specific GPUKernelCompiler:
///    - CUDA: replaces intrinsics with NVVM ops, lowers to PTX
///    - Metal: walks MLIR operations, emits MSL source code
///
/// 4. **Host Rewrite:** Replaces the sentinel sequence + kernel call with
///    GPU runtime API calls (gpuLoadModule, gpuGetKernel, gpuLaunchKernel, etc.)
///
/// 5. **Cleanup:** Removes the kernel function from the host module.
class MLIRGPUPass {
public:
	MLIRGPUPass();
	~MLIRGPUPass();

	/// Run the GPU pass on the given MLIR module.
	/// Modifies the module in-place: kernel functions are extracted and compiled,
	/// launch patterns are replaced with runtime API calls.
	void run(mlir::OwningOpRef<mlir::ModuleOp>& module, const DumpHandler& dumpHandler,
	         const engine::Options& options);

private:
	std::unique_ptr<GPUKernelCompiler> kernelCompiler_;
};

} // namespace nautilus::gpu
