#include "nautilus/compiler/DirectExecutable.hpp"

namespace nautilus::compiler {

/**
 * @brief Generic invocable wrapper for direct execution without compilation.
 */
class DirectExecutable::DirectInvocable : public Executable::GenericInvocable {
public:
	explicit DirectInvocable(MultiTierJitCompiler::wrapper_function function) : function_(std::move(function)) {
	}

	std::any invokeGeneric(const std::vector<std::any>&) override {
		// Execute the wrapper function directly
		// The wrapper function handles argument setup and return value extraction
		function_();
		return nullptr; // Wrapper function manages return values internally
	}

private:
	MultiTierJitCompiler::wrapper_function function_;
};

DirectExecutable::DirectExecutable(MultiTierJitCompiler::wrapper_function function)
    : function_(std::move(function)) {
}

void* DirectExecutable::getInvocableFunctionPtr(const std::string&) {
	// Direct execution doesn't provide function pointers
	return nullptr;
}

bool DirectExecutable::hasInvocableFunctionPtr() {
	return false;
}

std::unique_ptr<Executable::GenericInvocable> DirectExecutable::getGenericInvocable(const std::string&) {
	return std::make_unique<DirectInvocable>(function_);
}

} // namespace nautilus::compiler
