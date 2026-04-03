
#include "MLIRGPUPass.hpp"
#include "GPUKernelCompiler.hpp"

#ifdef ENABLE_GPU_CUDA
#include "cuda/CUDAKernelCompiler.hpp"
#endif
#ifdef ENABLE_GPU_METAL
#include "metal/MetalKernelCompiler.hpp"
#endif

namespace nautilus::gpu {

MLIRGPUPass::MLIRGPUPass() {
	// Select platform-specific kernel compiler.
	// Priority: CUDA on Linux, Metal on macOS.
#ifdef ENABLE_GPU_CUDA
	kernelCompiler_ = std::make_unique<CUDAKernelCompiler>();
#elif defined(ENABLE_GPU_METAL)
	kernelCompiler_ = std::make_unique<MetalKernelCompiler>();
#endif
}

MLIRGPUPass::~MLIRGPUPass() = default;

void MLIRGPUPass::run(mlir::OwningOpRef<mlir::ModuleOp>& /*module*/, const compiler::DumpHandler& /*dumpHandler*/,
                      const engine::Options& /*options*/) {
	if (!kernelCompiler_) {
		return; // No GPU backend available -- CPU fallback mode
	}

	// TODO: Implementation steps (requires CUDA or Metal backend):
	//
	// 1. Walk the module to find the sentinel pattern:
	//    - call @gpu_set_grid(gx, gy, gz)
	//    - call @gpu_set_block(bx, by, bz)
	//    - call @kernel_function(args...)
	//
	// 2. For each detected kernel launch:
	//    a. Extract the kernel function (clone into separate module)
	//    b. Compile via kernelCompiler_->compile(kernelFunc, context)
	//    c. Embed compiled kernel data as a global constant
	//    d. Replace the sentinel + call sequence with:
	//       - call @_nrt_gpu_load_module(data, size)
	//       - call @_nrt_gpu_get_kernel(module, name)
	//       - alloca + store to pack kernel arguments into void**
	//       - call @_nrt_gpu_launch_kernel(kernel, gx,gy,gz, bx,by,bz, args, shared_mem)
	//       - call @_nrt_gpu_synchronize()
	//    e. Remove the original kernel function from the host module
	//
	// 3. Register GPU runtime functions as external proxy functions
	//    so the JIT linker can resolve them.
}

} // namespace nautilus::gpu
