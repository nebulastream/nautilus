
#include <nautilus/compiler/backends/cpp/CPPExecutable.hpp>
#include <nautilus/compiler/backends/cpp/SharedLibrary.hpp>
#include <utility>
namespace nautilus::compiler::cpp {

CPPExecutable::CPPExecutable(std::shared_ptr<SharedLibrary> obj,
                             std::unordered_set<std::string> functionsNeedingCapture, std::shared_ptr<const void> owner)
    : obj(std::move(obj)), functionsNeedingCapture_(std::move(functionsNeedingCapture)), owner_(std::move(owner)) {
}

void* CPPExecutable::getInvocableFunctionPtr(const std::string& member) {
	return obj->getInvocableFunctionPtr(member);
}

bool CPPExecutable::hasInvocableFunctionPtr() {
	return true;
}

} // namespace nautilus::compiler::cpp
