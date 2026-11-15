#include "nautilus/compiler/backends/DebugSource.hpp"
#include <atomic>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

namespace nautilus::compiler {

namespace {

// $TMPDIR, $TEMP, or "/tmp".
std::string tempDir() {
	if (const char* t = std::getenv("TMPDIR")) {
		return t;
	}
	if (const char* t = std::getenv("TEMP")) {
		return t;
	}
	return "/tmp";
}

// DWARF that names a relative file leaves the debugger resolving it against DW_AT_comp_dir, which for a JIT module is
// not a directory the user controls.
std::string makeAbsolute(const std::string& path) {
	std::error_code ec;
	auto absolute = std::filesystem::absolute(path, ec);
	if (ec) {
		return path;
	}
	return absolute.lexically_normal().string();
}

std::string pathIn(const std::filesystem::path& dir, const std::string& extension) {
	static std::atomic<uint64_t> counter {0};
	auto name =
	    "nautilus_debug_" + std::to_string(::getpid()) + "_" + std::to_string(counter.fetch_add(1)) + "." + extension;
	return makeAbsolute((dir / name).string());
}

} // namespace

std::string newDebugSourcePath(bool inWorkingDirectory, const std::string& extension) {
	return pathIn(inWorkingDirectory ? std::filesystem::current_path() : std::filesystem::path(tempDir()), extension);
}

std::string debugSourceFallbackPath(const std::string& extension) {
	return pathIn(tempDir(), extension);
}

std::string writeDebugSource(const std::string& path, const std::string& text) {
	{
		std::ofstream out(path);
		out << text;
		if (out) {
			return path;
		}
	}
	const auto fallback = debugSourceFallbackPath("ir");
	std::cerr << "nautilus: could not write debug source file '" << path << "'; falling back to '" << fallback << "'\n";
	std::ofstream fallbackOut(fallback);
	fallbackOut << text;
	return fallback;
}

} // namespace nautilus::compiler
