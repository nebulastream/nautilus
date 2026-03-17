
#include "nautilus/compiler/backends/amsjit/AsmJitExecutable.hpp"

namespace nautilus::compiler::asmjit {

AsmJitExecutable::AsmJitExecutable(std::unique_ptr<::asmjit::JitRuntime> runtime, void* basePtr,
                                   std::unordered_map<std::string, void*> jitPtrs)
    : runtime_(std::move(runtime)), basePtr_(basePtr), jitPtrs_(std::move(jitPtrs)) {
}

AsmJitExecutable::~AsmJitExecutable() {
	// All functions share one JIT allocation — release exactly once.
	if (basePtr_ && runtime_) {
		runtime_->release(basePtr_);
	}
}

void* AsmJitExecutable::getInvocableFunctionPtr(const std::string& member) {
	auto it = jitPtrs_.find(member);
	return (it != jitPtrs_.end()) ? it->second : nullptr;
}

bool AsmJitExecutable::hasInvocableFunctionPtr() {
	return true;
}

} // namespace nautilus::compiler::asmjit
