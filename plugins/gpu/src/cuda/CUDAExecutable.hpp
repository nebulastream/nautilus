#pragma once

#include <nautilus/Executable.hpp>
#include <nautilus/compiler/backends/cpp/SharedLibrary.hpp>
#include <string>

namespace nautilus::compiler::cuda {

/// Executable wrapping a CUDA host+device shared library compiled by nvcc.
class CUDAExecutable : public Executable {
public:
	explicit CUDAExecutable(std::shared_ptr<cpp::SharedLibrary> lib);
	~CUDAExecutable() override = default;

	void* getInvocableFunctionPtr(const std::string& member) override;
	bool hasInvocableFunctionPtr() override;

private:
	std::shared_ptr<cpp::SharedLibrary> lib_;
};

} // namespace nautilus::compiler::cuda
