
#include "MetalCompiler.hpp"
#include "nautilus/common/File.hpp"
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <stdexcept>

namespace nautilus::compiler::metal {

/// Locate a Metal developer tool using xcrun.
/// Sets DEVELOPER_DIR to Xcode if needed to find the Metal toolchain.
static std::string findMetalTool(const std::string& tool) {
	// Try xcrun with the default developer directory first
	auto tryXcrun = [&](const std::string& devDir) -> std::string {
		std::string cmd;
		if (!devDir.empty()) {
			cmd = "DEVELOPER_DIR=" + devDir + " ";
		}
		cmd += "xcrun --sdk macosx --find " + tool + " 2>/dev/null";
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

	// Try default xcrun
	auto path = tryXcrun("");
	if (!path.empty()) {
		return path;
	}

	// Try with Xcode developer directory
	path = tryXcrun("/Applications/Xcode.app/Contents/Developer");
	if (!path.empty()) {
		return path;
	}

	throw std::runtime_error("MetalCompiler: could not find '" + tool +
	                         "'. Ensure Xcode is installed with Metal developer tools.\n"
	                         "Run: xcodebuild -downloadComponent MetalToolchain");
}

/// Build a command prefix with the correct DEVELOPER_DIR if Xcode is installed.
static std::string devDirPrefix() {
	if (std::filesystem::exists("/Applications/Xcode.app/Contents/Developer")) {
		return "DEVELOPER_DIR=/Applications/Xcode.app/Contents/Developer ";
	}
	return "";
}

std::string MetalCompiler::compile(const std::string& identifier, const std::string& metalSource) {
	auto tmpDir = std::filesystem::temp_directory_path();
	auto metalPath = (tmpDir / (identifier + ".metal")).string();
	auto airPath = (tmpDir / (identifier + ".air")).string();
	auto metallibPath = (tmpDir / (identifier + ".metallib")).string();

	// Write MSL source to temp file
	common::File::createFile(metalPath, metalSource);

	auto metalBin = findMetalTool("metal");
	auto metallibBin = findMetalTool("metallib");
	auto prefix = devDirPrefix();

	// Compile .metal -> .air
	runCommand(prefix + metalBin + " -c " + metalPath + " -o " + airPath);

	// Link .air -> .metallib
	runCommand(prefix + metallibBin + " " + airPath + " -o " + metallibPath);

	// Clean up intermediates
	std::filesystem::remove(metalPath);
	std::filesystem::remove(airPath);

	return metallibPath;
}

void MetalCompiler::runCommand(const std::string& cmd) {
	std::string fullCmd = cmd + " 2>&1";
	FILE* fp = popen(fullCmd.c_str(), "r");
	if (fp == nullptr) {
		throw std::runtime_error("MetalCompiler: failed to run command: " + cmd);
	}

	std::ostringstream output;
	char buffer[4096];
	while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
		output << buffer;
	}

	auto ret = pclose(fp);
	if (ret != 0) {
		throw std::runtime_error("MetalCompiler: command failed: " + cmd + "\n" + output.str());
	}
}

} // namespace nautilus::compiler::metal
