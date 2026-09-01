#pragma once

#include <memory>
#include <nautilus/Executable.hpp>
#include <nautilus/compiler/backends/cpp/SharedLibrary.hpp>
#include <string>
#include <unordered_set>

namespace nautilus::compiler::cpp {

/**
 * @brief Implements the executable for the cpp backend
 */
class CPPExecutable : public Executable {
public:
	/**
	 * Constructor to create a cpp executable.
	 * @param obj the shared object, which we invoke at runtime.
	 * @param functionsNeedingCapture names of the module's functions whose
	 *        compiled body has at least one captured-exception call site (see
	 *        CapturedExceptionTransport::functionsNeedingCapture). A function
	 *        not in this set is reported as NativeUnwind: it never touches
	 *        the ExceptionFrame machinery, so it is safe to call directly.
	 */
	CPPExecutable(std::shared_ptr<SharedLibrary> obj, std::unordered_set<std::string> functionsNeedingCapture);

	~CPPExecutable() override = default;

public:
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
	std::shared_ptr<SharedLibrary> obj;
	std::unordered_set<std::string> functionsNeedingCapture_;
};
} // namespace nautilus::compiler::cpp
