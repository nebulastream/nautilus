
#include "nautilus/compiler/backends/cpp/ExecutablePath.hpp"
#include <filesystem>

namespace nautilus::compiler::cpp::ExecutablePath {

std::ostream& operator<<(std::ostream& os, const RuntimePathConfig& config) {
	os << "\nclangBinaryPath: " << config.clangBinaryPath << "\n";
	os << "includePaths: \n";
	for (auto includeDir : config.includePaths) {
		os << "\t" << includeDir << "\n";
	}
	os << "libPaths:";
	for (auto libDirs : config.libPaths) {
		os << "\n\t" << libDirs;
	}
	return os;
}

RuntimePathConfig loadRuntimePathConfig() {
	auto runtimePathConfig = RuntimePathConfig();
#if defined(__APPLE__)
	runtimePathConfig.libs.push_back("-lc++");
#endif

	// verify is runtime path config is valid
	if (!std::filesystem::exists(runtimePathConfig.clangBinaryPath)) {
		// throw CompilerException("Selected clang binary path dose not exists. Path: " +
		// runtimePathConfig.clangBinaryPath);
	}

	for (auto includeDir : runtimePathConfig.includePaths) {
		if (!std::filesystem::exists(includeDir)) {
			//  throw CompilerException("Selected include path dose not exists. Path: " + includeDir);
		}
	}

	for (auto libDir : runtimePathConfig.libPaths) {
		if (!std::filesystem::exists(libDir)) {
			// throw CompilerException("Selected lib path dose not exists. Path: " + libDir);
		}
	}

	// NES_INFO("RuntimePathConfig: " << runtimePathConfig);

	return runtimePathConfig;
}

namespace detail {

std::filesystem::path recursiveFindFileReverse(std::filesystem::path currentPath, const std::string targetFileName) {
	while (!std::filesystem::is_directory(currentPath)) {
		currentPath = currentPath.parent_path();
	}
	while (currentPath != currentPath.root_directory()) {
		for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
			if (entry.is_directory()) {

				auto fname = entry.path().string();
				if (fname.ends_with(targetFileName)) {
					return entry.path();
				}
			}

			auto path = entry.path();
			auto fname = path.filename();
			if (fname.string().compare(targetFileName) == 0) {
				return path;
			}
		}
		currentPath = currentPath.parent_path();
	}
	return currentPath;
}
} // namespace detail

} // namespace nautilus::compiler::cpp::ExecutablePath
