
#include "nautilus/compiler/backends/amsjit/AsmJitExecutable.hpp"

namespace nautilus::compiler::asmjit {

AsmJitExecutable::AsmJitExecutable(std::unique_ptr<::asmjit::JitRuntime> runtime, void* fnPtr)
    : runtime_(std::move(runtime)), fnPtr_(fnPtr) {
}

AsmJitExecutable::~AsmJitExecutable() {
	if (fnPtr_ && runtime_) {
		runtime_->release(fnPtr_);
	}
}

void* AsmJitExecutable::getInvocableFunctionPtr(const std::string& /*member*/) {
	return fnPtr_;
}

bool AsmJitExecutable::hasInvocableFunctionPtr() {
	return true;
}

} // namespace nautilus::compiler::asmjit
