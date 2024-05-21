

#include <dlfcn.h>
#include <filesystem>
#include <nautilus/compiler/backends/cpp/SharedLibrary.hpp>
#include <nautilus/exceptions/RuntimeException.hpp>
#include <utility>

namespace nautilus::compiler::cpp {

SharedLibrary::SharedLibrary(void* shareLib, std::string soAbsolutePath)
    : shareLib(shareLib), soAbsolutePath(std::move(soAbsolutePath)) {
}

SharedLibrary::~SharedLibrary() {
	auto returnCode = dlclose(shareLib);
	if (returnCode != 0) {
		// throw RuntimeException("SharedLibrary: error during dlclose. error code: " + std::to_string(returnCode));
	}
	std::filesystem::remove(soAbsolutePath);
}

SharedLibraryPtr SharedLibrary::load(const std::string& absoluteFilePath) {
	auto* shareLib = dlopen(absoluteFilePath.c_str(), RTLD_NOW);
	auto* error = dlerror();
	if (error) {
		throw RuntimeException("Could not load shared library: " + absoluteFilePath + " Error:" + error);
	}
	if (!shareLib) {
		throw RuntimeException("Could not load shared library: " + absoluteFilePath);
	}

	return std::make_shared<SharedLibrary>(shareLib, absoluteFilePath);
}

void* SharedLibrary::getInvocableFunctionPtr(const std::string& mangeldSymbolName) {
	auto* symbol = dlsym(shareLib, mangeldSymbolName.c_str());
	auto* error = dlerror();

	if (error) {
		throw RuntimeException("Could not load symbol: " + mangeldSymbolName + " Error:" + error);
	}

	return symbol;
}

} // namespace nautilus::compiler::cpp
