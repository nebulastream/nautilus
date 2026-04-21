
#include "MetalExecutable.hpp"
#include <filesystem>
#include <utility>

namespace nautilus::compiler::metal {

MetalExecutable::MetalExecutable(std::shared_ptr<cpp::SharedLibrary> hostLib, std::string metallibPath)
    : hostLib_(std::move(hostLib)), metallibPath_(std::move(metallibPath)) {
}

MetalExecutable::~MetalExecutable() {
	if (!metallibPath_.empty()) {
		std::filesystem::remove(metallibPath_);
	}
}

void* MetalExecutable::getInvocableFunctionPtr(const std::string& member) {
	return hostLib_->getInvocableFunctionPtr(member);
}

bool MetalExecutable::hasInvocableFunctionPtr() {
	return true;
}

} // namespace nautilus::compiler::metal
