#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace mlir {
class MLIRContext;
namespace func {
class FuncOp;
} // namespace func
} // namespace mlir

namespace nautilus::gpu {

/// Abstract interface for platform-specific GPU kernel compilers.
///
/// Implementations:
/// - CUDAKernelCompiler: MLIR -> LLVM IR -> NVVM intrinsics -> PTX via libNVVM
/// - MetalKernelCompiler: MLIR -> LLVM IR -> AIR -> metallib via xcrun metal/metallib
class GPUKernelCompiler {
public:
	virtual ~GPUKernelCompiler() = default;

	/// Compile a kernel function (as MLIR func.func) to a GPU binary or source blob.
	///
	/// @param kernelFunc The kernel function to compile (will be cloned, not modified).
	/// @param context    The MLIR context for creating new operations.
	/// @return The compiled kernel data (PTX bytes, MSL source, etc.)
	virtual std::vector<uint8_t> compile(mlir::func::FuncOp kernelFunc, mlir::MLIRContext& context) = 0;

	/// Returns a human-readable name for this compiler (e.g. "CUDA", "Metal").
	virtual std::string name() const = 0;
};

} // namespace nautilus::gpu
