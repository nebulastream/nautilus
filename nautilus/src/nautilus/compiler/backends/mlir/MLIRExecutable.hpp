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
	/// `owner` keeps alive what the generated code points into (the NIR module: data objects, bindings).
	MLIRExecutable(std::unique_ptr<MLIRJit> jit, std::shared_ptr<const void> owner);
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
	// declared before the JIT: the code goes away before what it points into
	std::shared_ptr<const void> owner;
	std::unique_ptr<MLIRJit> jit;
};
} // namespace nautilus::compiler::mlir
