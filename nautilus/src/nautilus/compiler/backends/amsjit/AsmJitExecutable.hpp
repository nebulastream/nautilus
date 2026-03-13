
#pragma once

#include "nautilus/Executable.hpp"
#include <asmjit/core.h>
#include <memory>

namespace nautilus::compiler::asmjit {

/**
 * @brief Wraps an AsmJit JIT-compiled function as a Nautilus Executable.
 *
 * Keeps the JitRuntime alive (it owns the JIT memory) until this object
 * is destroyed, at which point the function memory is released.
 */
class AsmJitExecutable : public Executable {
public:
	AsmJitExecutable(std::unique_ptr<::asmjit::JitRuntime> runtime, void* fnPtr);
	~AsmJitExecutable() override;

	void* getInvocableFunctionPtr(const std::string& member) override;
	bool hasInvocableFunctionPtr() override;

private:
	std::unique_ptr<::asmjit::JitRuntime> runtime_;
	void* fnPtr_;
};

} // namespace nautilus::compiler::asmjit
