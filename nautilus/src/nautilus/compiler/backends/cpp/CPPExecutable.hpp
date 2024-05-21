#pragma once

#include <memory>
#include <nautilus/Executable.hpp>
#include <nautilus/compiler/backends/cpp/SharedLibrary.hpp>

namespace nautilus::compiler::cpp {

/**
 * @brief Implements the executable for the cpp backend
 */
class CPPExecutable : public Executable {
public:
	/**
	 * Constructor to create a cpp executable.
	 * @param obj the shared object, which we invoke at runtime.
	 */
	explicit CPPExecutable(std::shared_ptr<SharedLibrary> obj);

	~CPPExecutable() override = default;

public:
	void* getInvocableFunctionPtr(const std::string& member) override;

	bool hasInvocableFunctionPtr() override;

private:
	std::shared_ptr<SharedLibrary> obj;
};
} // namespace nautilus::compiler::cpp
