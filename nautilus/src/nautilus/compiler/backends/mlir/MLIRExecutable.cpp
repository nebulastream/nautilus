

#include "nautilus/compiler/backends/mlir/MLIRExecutable.hpp"
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include "nautilus/compiler/backends/mlir/MLIRPassManager.hpp"
#include <mlir/IR/MLIRContext.h>

namespace nautilus::compiler::mlir {
static std::mutex llvm_jit_mutex {};

MLIRExecutable::MLIRExecutable(std::unique_ptr<MLIRJit> jit) : jit(std::move(jit)) {
}
MLIRExecutable::MLIRExecutable(std::unique_ptr<MLIRJit> jit, std::shared_ptr<const void> owner)
    : owner(std::move(owner)), jit(std::move(jit)) {
}
MLIRExecutable::~MLIRExecutable() {
	if (jit) {
		std::scoped_lock lock(llvm_jit_mutex);
		jit.reset();
	}
}
MLIRExecutable::MLIRExecutable(MLIRExecutable&& other) noexcept
    : owner(std::move(other.owner)), jit(std::move(other.jit)) {
}
MLIRExecutable& MLIRExecutable::operator=(MLIRExecutable&& other) noexcept {
	if (this == &other)
		return *this;
	owner = std::move(other.owner);
	jit = std::move(other.jit);
	return *this;
}
void* MLIRExecutable::getInvocableFunctionPtr(const std::string& member) {
	std::scoped_lock lock(llvm_jit_mutex);
	return jit->lookup(member).get();
}
bool MLIRExecutable::hasInvocableFunctionPtr() {
	return true;
}
} // namespace nautilus::compiler::mlir
