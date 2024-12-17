#pragma once

#include "nautilus/Executable.hpp"
#include <mlir/ExecutionEngine/ExecutionEngine.h>

namespace nautilus::compiler::mlir {

/**
 * @brief Executable that calls into a code fragment that is compiled with MLIR.
 */
class MLIRExecutable : public Executable {
public:
	MLIRExecutable(std::unique_ptr<::mlir::ExecutionEngine> engine);
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
	std::unique_ptr<::mlir::ExecutionEngine> engine;
};
} // namespace nautilus::compiler::mlir
