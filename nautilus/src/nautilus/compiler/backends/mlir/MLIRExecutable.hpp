#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/compiler/backends/mlir/jit/MLIRJit.hpp"

namespace nautilus::compiler::mlir {

/**
 * @brief Executable that calls into a code fragment that is compiled with MLIR.
 */
class MLIRExecutable : public Executable {
public:
	MLIRExecutable(std::unique_ptr<MLIRJit> jit);
	~MLIRExecutable() override;
	MLIRExecutable(const MLIRExecutable& other) = delete;
	MLIRExecutable(MLIRExecutable&& other) noexcept;
	MLIRExecutable& operator=(const MLIRExecutable& other) = delete;
	MLIRExecutable& operator=(MLIRExecutable&& other) noexcept;

protected:
	void* getInvocableFunctionPtr(const std::string& member) override;

public:
	bool hasInvocableFunctionPtr() override;

private:
	std::unique_ptr<MLIRJit> jit;
};
} // namespace nautilus::compiler::mlir
