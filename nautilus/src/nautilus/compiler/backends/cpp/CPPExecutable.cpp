
#include <nautilus/compiler/backends/cpp/CPPExecutable.hpp>
#include <nautilus/compiler/backends/cpp/SharedLibrary.hpp>
#include <utility>
namespace nautilus::compiler::cpp {

CPPExecutable::CPPExecutable(std::shared_ptr<SharedLibrary> obj) : obj(std::move(obj)) {
}

void* CPPExecutable::getInvocableFunctionPtr(const std::string&) {
	return obj->getInvocableFunctionPtr("execute");
}

bool CPPExecutable::hasInvocableFunctionPtr() {
	return true;
}

} // namespace nautilus::compiler::cpp
