#pragma once

#include <memory>
#include <string>
#include <vector>

namespace mlir {
class ModuleOp;
template <typename OpTy>
class OwningOpRef;
} // namespace mlir

namespace nautilus::compiler {
class DumpHandler;
} // namespace nautilus::compiler

namespace nautilus::engine {
class Options;
} // namespace nautilus::engine

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
///    - CUDA: replaces intrinsics with NVVM ops, lowers to PTX via libNVVM
///    - Metal: lowers to LLVM IR, post-processes for AIR, compiles via xcrun metal/metallib
///
/// 4. **Host Rewrite:** Replaces the sentinel sequence + kernel call with
///    GPU runtime API calls (gpuLoadModule, gpuGetKernel, gpuLaunchKernel, gpuSynchronize).
///    Kernel binary data is embedded as LLVM global constants in the host module.
///
/// 5. **Cleanup:** Removes kernel functions and unused GPU stub declarations from the module.
///
/// After the pass, getProxySymbols()/getProxyAddresses() return the GPU runtime
/// function symbols that must be registered with the JIT linker.
class MLIRGPUPass {
public:
	MLIRGPUPass();
	~MLIRGPUPass();

	/// Run the GPU pass on the given MLIR module.
	/// Modifies the module in-place: kernel functions are extracted and compiled,
	/// launch patterns are replaced with runtime API calls.
	void run(mlir::OwningOpRef<mlir::ModuleOp>& module, const compiler::DumpHandler& dumpHandler,
	         const engine::Options& options);

	/// Returns the GPU runtime function symbols that need JIT linker registration.
	/// Must be called after run().
	const std::vector<std::string>& getProxySymbols() const {
		return proxySymbols_;
	}

	/// Returns the GPU runtime function addresses corresponding to getProxySymbols().
	/// Must be called after run().
	const std::vector<void*>& getProxyAddresses() const {
		return proxyAddresses_;
	}

private:
	std::unique_ptr<GPUKernelCompiler> kernelCompiler_;
	std::vector<std::string> proxySymbols_;
	std::vector<void*> proxyAddresses_;
};

} // namespace nautilus::gpu
