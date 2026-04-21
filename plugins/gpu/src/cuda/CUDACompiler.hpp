#pragma once

#include "nautilus/compiler/backends/cpp/SharedLibrary.hpp"
#include <string>

namespace nautilus::compiler::cuda {

/// Compiles CUDA source code (.cu) to a shared library using nvcc.
class CUDACompiler {
public:
	/// Compiles .cu source to a shared library. Returns a SharedLibrary.
	static cpp::SharedLibraryPtr compile(const std::string& identifier, const std::string& code);

private:
	static std::string findNvcc();
	static void runCommand(const std::string& cmd);
};

} // namespace nautilus::compiler::cuda
