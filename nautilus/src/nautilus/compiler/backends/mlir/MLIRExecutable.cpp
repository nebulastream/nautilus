

#include "nautilus/compiler/backends/mlir/MLIRExecutable.hpp"
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include "nautilus/compiler/backends/mlir/MLIRPassManager.hpp"
#include <mlir/IR/MLIRContext.h>

namespace nautilus::compiler::mlir {
static std::mutex llvm_jit_mutex {};

MLIRExecutable::MLIRExecutable(std::unique_ptr<::mlir::ExecutionEngine> engine) : engine(std::move(engine)) {
}
MLIRExecutable::~MLIRExecutable() {
	if (engine) {
		std::scoped_lock lock(llvm_jit_mutex);
		engine.reset();
	}
}
MLIRExecutable::MLIRExecutable(MLIRExecutable&& other) noexcept : engine(std::move(other.engine)) {
}
MLIRExecutable& MLIRExecutable::operator=(MLIRExecutable&& other) noexcept {
	if (this == &other)
		return *this;
	engine = std::move(other.engine);
	return *this;
}
void* MLIRExecutable::getInvocableFunctionPtr(const std::string& member) {
	std::scoped_lock lock(llvm_jit_mutex);
	return engine->lookup(member).get();
}
bool MLIRExecutable::hasInvocableFunctionPtr() {
	return true;
}
} // namespace nautilus::compiler::mlir
