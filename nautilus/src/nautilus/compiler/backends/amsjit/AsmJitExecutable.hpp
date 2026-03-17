
#pragma once

#include "nautilus/Executable.hpp"
#include <asmjit/core.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace nautilus::compiler::asmjit {

/**
 * @brief Wraps AsmJit JIT-compiled functions as a Nautilus Executable.
 *
 * All functions are compiled into a single JIT allocation. The runtime owns
 * that allocation and must release it exactly once via the base pointer.
 */
class AsmJitExecutable : public Executable {
public:
	AsmJitExecutable(std::unique_ptr<::asmjit::JitRuntime> runtime, void* basePtr,
	                 std::unordered_map<std::string, void*> jitPtrs);
	~AsmJitExecutable() override;

	void* getInvocableFunctionPtr(const std::string& member) override;
	bool hasInvocableFunctionPtr() override;

private:
	std::unique_ptr<::asmjit::JitRuntime> runtime_;
	/// Start of the single JIT memory block — released once in the destructor.
	void* basePtr_;
	/// Per-function pointers (offsets within the JIT block).
	std::unordered_map<std::string, void*> jitPtrs_;
};

} // namespace nautilus::compiler::asmjit
