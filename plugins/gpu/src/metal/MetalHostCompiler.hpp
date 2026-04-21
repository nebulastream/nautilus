#pragma once

#include "nautilus/compiler/backends/cpp/SharedLibrary.hpp"
#include <string>

namespace nautilus::compiler::metal {

/// Compiles Objective-C++ host code (.mm) with Metal framework linking.
class MetalHostCompiler {
public:
	/// Compiles ObjC++ host code to a shared library. Returns a SharedLibrary.
	static cpp::SharedLibraryPtr compile(const std::string& identifier, const std::string& code);

private:
	static void runCompiler(const std::string& cmd);
};

} // namespace nautilus::compiler::metal
