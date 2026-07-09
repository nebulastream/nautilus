#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include <memory>

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
	explicit TBCExecutable(std::shared_ptr<TBCProgram> program);

	[[nodiscard]] void* getInvocableFunctionPtr(const std::string& member) override;
	bool hasInvocableFunctionPtr() override;
	std::unique_ptr<GenericInvocable> getGenericInvocable(const std::string& member) override;

private:
	std::shared_ptr<TBCProgram> program;
};

} // namespace nautilus::compiler::tbc
