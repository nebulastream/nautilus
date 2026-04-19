
#include "CUDAExecutable.hpp"
#include <utility>

namespace nautilus::compiler::cuda {

CUDAExecutable::CUDAExecutable(std::shared_ptr<cpp::SharedLibrary> lib) : lib_(std::move(lib)) {
}

void* CUDAExecutable::getInvocableFunctionPtr(const std::string& member) {
	return lib_->getInvocableFunctionPtr(member);
}

bool CUDAExecutable::hasInvocableFunctionPtr() {
	return true;
}

} // namespace nautilus::compiler::cuda
