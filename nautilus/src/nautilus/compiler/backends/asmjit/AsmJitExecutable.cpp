
#include "nautilus/compiler/backends/asmjit/AsmJitExecutable.hpp"

namespace nautilus::compiler::asmjit {

AsmJitExecutable::AsmJitExecutable(std::shared_ptr<::asmjit::JitRuntime> runtime, void* basePtr,
                                   std::unordered_map<std::string, void*> jitPtrs,
                                   std::unordered_set<std::string> functionsNeedingCapture,
                                   std::shared_ptr<const void> owner)
    : owner_(std::move(owner)), runtime_(std::move(runtime)), basePtr_(basePtr), jitPtrs_(std::move(jitPtrs)),
      functionsNeedingCapture_(std::move(functionsNeedingCapture)) {
}

std::shared_ptr<::asmjit::JitRuntime> AsmJitExecutable::sharedRuntime() {
	static const auto* const runtime =
	    new std::shared_ptr<::asmjit::JitRuntime>(std::make_shared<::asmjit::JitRuntime>());
	return *runtime;
}

AsmJitExecutable::~AsmJitExecutable() {
	// the code is about to go: a later compile reusing its addresses must not resolve to its names
	JitSymbolRegistry::instance().remove(symbols_);
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
