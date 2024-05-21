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

protected:
	void* getInvocableFunctionPtr(const std::string& member) override;

public:
	bool hasInvocableFunctionPtr() override;

private:
	std::unique_ptr<::mlir::ExecutionEngine> engine;
};
} // namespace nautilus::compiler::mlir
