

#include "nautilus/compiler/backends/mlir/MLIRExecutable.hpp"
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include "nautilus/compiler/backends/mlir/MLIRPassManager.hpp"
#include <mlir/IR/MLIRContext.h>

namespace nautilus::compiler::mlir {
MLIRExecutable::MLIRExecutable(std::unique_ptr<::mlir::ExecutionEngine> engine) : engine(std::move(engine)) {
}

void* MLIRExecutable::getInvocableFunctionPtr(const std::string& member) {
	return engine->lookup(member).get();
}
bool MLIRExecutable::hasInvocableFunctionPtr() {
	return true;
}
} // namespace nautilus::compiler::mlir
