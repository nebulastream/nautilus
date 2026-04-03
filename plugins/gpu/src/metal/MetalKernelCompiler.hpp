#pragma once

#include "../GPUKernelCompiler.hpp"

namespace nautilus::gpu {

/// @brief Metal kernel compiler: MLIR -> LLVM IR -> AIR -> metallib.
///
/// Uses the same MLIR -> LLVM IR lowering pipeline as the CPU/CUDA paths,
/// then invokes Apple's Metal compiler toolchain (`xcrun metal` + `xcrun metallib`)
/// to produce a binary metallib that can be loaded at runtime.
///
/// This avoids writing a custom MSL code generator. Instead, we:
/// 1. Clone the kernel function into a fresh MLIR module
/// 2. Transform GPU intrinsic calls into extra kernel function parameters
///    (thread_position_in_threadgroup, threadgroup_position_in_grid, etc.)
/// 3. Run standard MLIR lowering passes (Arith->LLVM, Func->LLVM, CF->LLVM)
/// 4. Translate to LLVM IR with target triple `air64-apple-macosx15.0.0`
/// 5. Set device address space (1) on pointer arguments
/// 6. Add `air.kernel` metadata to mark the function as a compute kernel
/// 7. Write LLVM IR bitcode to a temp file
/// 8. Compile: `xcrun metal -c -x ir <input.bc> -o <output.air>`
/// 9. Package: `xcrun metallib <output.air> -o <output.metallib>`
/// 10. Return the metallib bytes
///
/// Requirements:
/// - macOS with Xcode command-line tools installed
/// - `xcrun metal` and `xcrun metallib` must be in PATH
class MetalKernelCompiler : public GPUKernelCompiler {
public:
	std::vector<uint8_t> compile(mlir::func::FuncOp kernelFunc, mlir::MLIRContext& context) override;
	std::string name() const override {
		return "Metal";
	}

private:
	/// Run an external command. Returns true on success (exit code 0).
	static bool runCommand(const std::string& command);
};

} // namespace nautilus::gpu
