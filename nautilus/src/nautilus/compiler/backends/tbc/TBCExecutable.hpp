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
 * Every function has a real C function pointer available from the native
 * closure built for it at compile time (see TBCClosure.hpp), returned by
 * getInvocableFunctionPtr(). Under NAUTILUS_FFI_CLOSURES those closures use
 * libffi static trampolines, so the pointers exist without any
 * runtime-generated executable memory and the backend stays usable on iOS.
 *
 * hasInvocableFunctionPtr() is nonetheless false: the entry path stays on the
 * GenericInvocable / invokeRaw route, which is ~1.4x faster per call than
 * marshalling through the closure (see TBCExecutable.cpp).
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
