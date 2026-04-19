#pragma once

#include <nautilus/Executable.hpp>
#include <nautilus/compiler/backends/cpp/SharedLibrary.hpp>
#include <string>

namespace nautilus::compiler::metal {

/// Executable that owns both a host shared library and a compiled .metallib file.
class MetalExecutable : public Executable {
public:
	MetalExecutable(std::shared_ptr<cpp::SharedLibrary> hostLib, std::string metallibPath);
	~MetalExecutable() override;

	void* getInvocableFunctionPtr(const std::string& member) override;
	bool hasInvocableFunctionPtr() override;

private:
	std::shared_ptr<cpp::SharedLibrary> hostLib_;
	std::string metallibPath_;
};

} // namespace nautilus::compiler::metal
