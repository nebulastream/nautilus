#pragma once

#include <filesystem>
#include <ostream>
#include <vector>

namespace nautilus::compiler::cpp::ExecutablePath {

/**
 * @brief This class holds information about the runtime path of different nes components.
 */
class RuntimePathConfig {
public:
	std::string clangBinaryPath;
	std::vector<std::string> includePaths;
	std::vector<std::string> libPaths;
	std::vector<std::string> libs;

	friend std::ostream& operator<<(std::ostream& os, const RuntimePathConfig& config);
};

bool isInInstallDir();

[[maybe_unused]] RuntimePathConfig loadRuntimePathConfig();

/**
 * @brief Gets the path of the current executable.
 * @return std::filesystem::path
 */
std::filesystem::path getExecutablePath();

/**
 * @brief Gets the path to the public includes.
 * @return std::filesystem::path
 */
std::filesystem::path getPublicIncludes();

/**
 * @brief Gets the path to the nes lib
 * @return std::filesystem::path
 */
std::filesystem::path getLibPath(std::string libName);

/**
 * @brief Gets the path to the nes clang executable
 * @return std::filesystem::path
 */
[[maybe_unused]] std::filesystem::path getClangPath();

} // namespace nautilus::compiler::cpp::ExecutablePath
