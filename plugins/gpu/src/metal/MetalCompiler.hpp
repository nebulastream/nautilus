#pragma once

#include <string>

namespace nautilus::compiler::metal {

/// Compiles Metal Shading Language source code to a .metallib file.
class MetalCompiler {
public:
	/// Compiles MSL source to a .metallib file. Returns the path to the .metallib.
	static std::string compile(const std::string& identifier, const std::string& metalSource);

private:
	static void runCommand(const std::string& cmd);
};

} // namespace nautilus::compiler::metal
