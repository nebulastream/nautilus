
#include "MetalHostCompiler.hpp"
#include "nautilus/common/File.hpp"
#include <cstdio>
#include <filesystem>
#include <sstream>
#include <stdexcept>

namespace nautilus::compiler::metal {

cpp::SharedLibraryPtr MetalHostCompiler::compile(const std::string& identifier, const std::string& code) {
	auto tmpDir = std::filesystem::temp_directory_path();
	auto sourceFileName = (tmpDir / (identifier + "_host.mm")).string();
	auto libraryFileName = (tmpDir / (identifier + "_host.dylib")).string();

	// Write ObjC++ source
	common::File::createFile(sourceFileName, code);

	// Build compiler command
	std::ostringstream cmd;
	cmd << "cc";
	cmd << " -std=c++20";
	cmd << " -fPIC";
	cmd << " -shared";
	cmd << " -g";
	cmd << " -fobjc-arc";
	cmd << " -framework Metal";
	cmd << " -framework Foundation";
	cmd << " -lc++";
#ifdef __APPLE__
	cmd << " -DTARGET_OS_IPHONE=0";
	cmd << " -DTARGET_OS_SIMULATOR=0";
#endif
	cmd << " -o " << libraryFileName;
	cmd << " " << sourceFileName;

	runCompiler(cmd.str());

	// Load the shared library
	auto sharedLibrary = cpp::SharedLibrary::load(libraryFileName);

	// Clean up
	std::filesystem::remove(sourceFileName);
	std::filesystem::remove(libraryFileName);

	return sharedLibrary;
}

void MetalHostCompiler::runCompiler(const std::string& cmd) {
	std::string fullCmd = cmd + " 2>&1";
	FILE* fp = popen(fullCmd.c_str(), "r");
	if (fp == nullptr) {
		throw std::runtime_error("MetalHostCompiler: failed to run compiler");
	}

	std::ostringstream output;
	char buffer[8192];
	while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
		output << buffer;
	}

	auto ret = pclose(fp);
	if (ret != 0) {
		throw std::runtime_error("MetalHostCompiler: compilation failed:\n" + output.str());
	}
}

} // namespace nautilus::compiler::metal
