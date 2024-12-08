#include "nautilus/compiler/DumpHandler.hpp"
#include "fmt/core.h"
#include "nautilus/common/File.hpp"
#include <filesystem>

namespace nautilus::compiler {

std::filesystem::path getRootPath(const engine::Options&, const nautilus::compiler::CompilationUnitID& id) {
	auto path = std::filesystem::temp_directory_path() / "dump" / id;
	if (!std::filesystem::exists(path)) {
		std::filesystem::create_directories(path);
	}
	return path;
}

DumpHandler::DumpHandler(const engine::Options& options, const nautilus::compiler::CompilationUnitID& id)
    : options(options), id(id), rootPath(getRootPath(options, id)) {
}

void DumpHandler::dump(std::string_view dumpName, std::string_view extension,
                       const std::function<std::string()>& dumpFunction) const {
	if (shouldDump(dumpName)) {
		auto content = dumpFunction();
		forceDump(dumpName, extension, content);
	}
}

void DumpHandler::forceDump(std::string_view dumpName, std::string_view extension, const std::string& content) const {
	if (dumpToConsole()) {
		fmt::println("{} -- {}", dumpName, id);
		fmt::println("{}", content);
	}
	if (dumpToFile()) {
		auto filePath = rootPath / fmt::format("{}.{}", dumpName, extension);
		common::File::createFile(filePath.string(), content);
		fmt::println("{} -- {} -- file://{}", dumpName, id, filePath.native());
	}
}

bool DumpHandler::shouldDump(std::string_view dumpName) const {
	return options.getOptionOrDefault("dump.all", false) ||
	       options.getOptionOrDefault("dump." + std::string(dumpName), false);
}

bool DumpHandler::dumpToConsole() const {
	return options.getOptionOrDefault("dump.console", false);
}

bool DumpHandler::dumpToFile() const {
	return options.getOptionOrDefault("dump.file", true);
}

} // namespace nautilus::compiler
