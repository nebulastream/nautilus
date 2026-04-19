
#include "CUDACompiler.hpp"
#include "nautilus/common/File.hpp"
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <stdexcept>

namespace nautilus::compiler::cuda {

std::string CUDACompiler::findNvcc() {
	// Try the PATH first
	auto tryWhich = [](const std::string& name) -> std::string {
		std::string cmd = "which " + name + " 2>/dev/null";
		FILE* fp = popen(cmd.c_str(), "r");
		if (!fp) {
			return {};
		}
		char buf[512];
		std::string result;
		while (fgets(buf, sizeof(buf), fp) != nullptr) {
			result += buf;
		}
		if (pclose(fp) == 0 && !result.empty()) {
			while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
				result.pop_back();
			}
			return result;
		}
		return {};
	};

	auto path = tryWhich("nvcc");
	if (!path.empty()) {
		return path;
	}

	// Common CUDA installation paths
	for (auto& candidate : {"/usr/local/cuda/bin/nvcc", "/opt/cuda/bin/nvcc"}) {
		if (std::filesystem::exists(candidate)) {
			return candidate;
		}
	}

	// Check CUDA_HOME environment variable
	if (auto* cudaHome = std::getenv("CUDA_HOME")) {
		auto candidate = std::string(cudaHome) + "/bin/nvcc";
		if (std::filesystem::exists(candidate)) {
			return candidate;
		}
	}

	throw std::runtime_error("CUDACompiler: could not find 'nvcc'. Ensure CUDA toolkit is installed.");
}

cpp::SharedLibraryPtr CUDACompiler::compile(const std::string& identifier, const std::string& code) {
	auto tmpDir = std::filesystem::temp_directory_path();
	auto sourceFileName = (tmpDir / (identifier + ".cu")).string();
	auto libraryFileName = (tmpDir / (identifier +
#ifdef __linux__
	                                  ".so"
#elif defined(__APPLE__)
	                                  ".dylib"
#else
#error "Unknown platform"
#endif
	                                  ))
	                           .string();

	// Write CUDA source
	common::File::createFile(sourceFileName, code);

	auto nvcc = findNvcc();

	// Build nvcc command: compile to shared library
	std::ostringstream cmd;
	cmd << nvcc;
	cmd << " -shared";
	cmd << " -Xcompiler -fPIC";
	cmd << " -std=c++20";
	cmd << " -o " << libraryFileName;
	cmd << " " << sourceFileName;

	runCommand(cmd.str());

	// Load the shared library
	auto sharedLibrary = cpp::SharedLibrary::load(libraryFileName);

	// Clean up
	std::filesystem::remove(sourceFileName);
	std::filesystem::remove(libraryFileName);

	return sharedLibrary;
}

void CUDACompiler::runCommand(const std::string& cmd) {
	std::string fullCmd = cmd + " 2>&1";
	FILE* fp = popen(fullCmd.c_str(), "r");
	if (fp == nullptr) {
		throw std::runtime_error("CUDACompiler: failed to run command: " + cmd);
	}

	std::ostringstream output;
	char buffer[8192];
	while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
		output << buffer;
	}

	auto ret = pclose(fp);
	if (ret != 0) {
		throw std::runtime_error("CUDACompiler: compilation failed:\n" + output.str());
	}
}

} // namespace nautilus::compiler::cuda
