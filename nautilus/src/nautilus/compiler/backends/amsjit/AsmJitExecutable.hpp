
#pragma once

#include "nautilus/Executable.hpp"
#include <asmjit/core.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace nautilus::compiler::asmjit {

/**
 * @brief Wraps AsmJit JIT-compiled functions as a Nautilus Executable.
 *
 * All functions are compiled into a single JIT allocation. The runtime owns
 * that allocation and must release it exactly once via the base pointer.
 */
class AsmJitExecutable : public Executable {
public:
	/// @param functionsNeedingCapture names of the module's functions whose
	///        compiled body has at least one captured-exception call site (see
	///        CapturedExceptionTransport::functionsNeedingCapture). A function
	///        not in this set is reported as NativeUnwind: it never touches
	///        the ExceptionFrame machinery, so it is safe to call directly.
	AsmJitExecutable(std::unique_ptr<::asmjit::JitRuntime> runtime, void* basePtr,
	                 std::unordered_map<std::string, void*> jitPtrs,
	                 std::unordered_set<std::string> functionsNeedingCapture);
	~AsmJitExecutable() override;

	void* getInvocableFunctionPtr(const std::string& member) override;
	bool hasInvocableFunctionPtr() override;

	[[nodiscard]] ExceptionPropagationMode getExceptionPropagationMode() const override {
		return ExceptionPropagationMode::CapturedHostRethrow;
	}

	[[nodiscard]] ExceptionPropagationMode getExceptionPropagationMode(const std::string& member) const override {
		return functionsNeedingCapture_.contains(member) ? ExceptionPropagationMode::CapturedHostRethrow
		                                                 : ExceptionPropagationMode::NativeUnwind;
	}

private:
	std::unique_ptr<::asmjit::JitRuntime> runtime_;
	/// Start of the single JIT memory block — released once in the destructor.
	void* basePtr_;
	/// Per-function pointers (offsets within the JIT block).
	std::unordered_map<std::string, void*> jitPtrs_;
	std::unordered_set<std::string> functionsNeedingCapture_;
};

} // namespace nautilus::compiler::asmjit
