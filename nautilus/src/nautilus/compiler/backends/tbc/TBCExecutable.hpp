#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include <memory>
#include <string>
#include <unordered_set>

namespace nautilus::compiler::tbc {

/**
 * @brief Executable wrapper around a TBCProgram.
 *
 * Deliberately exposes NO native function pointer: fabricating one for an
 * interpreted function requires runtime-generated trampolines (bc uses
 * dyncallback), which allocate executable memory and are therefore forbidden
 * on iOS. Callers go through the GenericInvocable path instead, which the
 * Executable/Module machinery supports transparently.
 */
class TBCExecutable : public Executable {
public:
	/// @param functionsNeedingCapture names of the module's functions whose
	///        compiled body has at least one captured-exception call site (see
	///        CapturedExceptionTransport::functionsNeedingCapture). A function
	///        not in this set is reported as NativeUnwind: it never touches
	///        the ExceptionFrame machinery, so the generic Invocable skips
	///        pushing one for it.
	TBCExecutable(std::shared_ptr<TBCProgram> program, std::unordered_set<std::string> functionsNeedingCapture);

	[[nodiscard]] void* getInvocableFunctionPtr(const std::string& member) override;
	bool hasInvocableFunctionPtr() override;
	std::unique_ptr<GenericInvocable> getGenericInvocable(const std::string& member) override;

	[[nodiscard]] ExceptionPropagationMode getExceptionPropagationMode() const override {
		return ExceptionPropagationMode::CapturedHostRethrow;
	}

	[[nodiscard]] ExceptionPropagationMode getExceptionPropagationMode(const std::string& member) const override {
		return functionsNeedingCapture_.contains(member) ? ExceptionPropagationMode::CapturedHostRethrow
		                                                 : ExceptionPropagationMode::NativeUnwind;
	}

private:
	std::shared_ptr<TBCProgram> program;
	std::unordered_set<std::string> functionsNeedingCapture_;
};

} // namespace nautilus::compiler::tbc
