#pragma once

#include <nautilus/Executable.hpp>
#include <stdexcept>
#include <string>

namespace nautilus::gpu {

/**
 * Returned by the CUDA/Metal backends when `gpu.codegenOnly` is set: the
 * device (and host) sources were generated and dumped, but the GPU toolchain
 * step (nvcc / xcrun metal) was skipped, so there is nothing to invoke.
 * Useful on machines without the toolchain (CI, code-inspection tools).
 */
class CodegenOnlyExecutable final : public compiler::Executable {
public:
	void* getInvocableFunctionPtr(const std::string& member) override {
		throw std::runtime_error("gpu.codegenOnly is set: '" + member +
		                         "' was not compiled — only device/host code was generated.");
	}

	bool hasInvocableFunctionPtr() override {
		return false;
	}
};

} // namespace nautilus::gpu
