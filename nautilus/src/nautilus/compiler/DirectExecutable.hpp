#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/MultiTierJitCompiler.hpp"
#include <functional>

namespace nautilus::compiler {

/**
 * @brief Executable that directly invokes the original traced function without any compilation.
 *
 * This provides the absolute lowest latency tier 1 option - no compilation overhead at all.
 * The function executes directly with val<T> tracing wrappers.
 *
 * Use case: Extremely fast startup when you need immediate execution, then transition
 * to compiled code after the function becomes hot.
 */
class DirectExecutable : public Executable {
public:
	explicit DirectExecutable(MultiTierJitCompiler::wrapper_function function);

	~DirectExecutable() override = default;

	void* getInvocableFunctionPtr(const std::string& member) override;

	bool hasInvocableFunctionPtr() override;

	std::unique_ptr<GenericInvocable> getGenericInvocable(const std::string& member) override;

private:
	MultiTierJitCompiler::wrapper_function function_;

	class DirectInvocable;
};

} // namespace nautilus::compiler
