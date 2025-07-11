#include "nautilus/compiler/DumpHandler.hpp"
#include "fmt/core.h"
#include "nautilus/common/File.hpp"
#include <filesystem>

namespace nautilus::compiler {

std::filesystem::path getRootPath(const engine::Options&, const nautilus::compiler::CompilationUnitID& id,
                                  const bool shallCreateFolder) {
	auto path = std::filesystem::temp_directory_path() / "dump" / id;
	if (shallCreateFolder and not std::filesystem::exists(path)) {
		std::filesystem::create_directories(path);
	}
	return path;
}

DumpHandler::DumpHandler(const engine::Options& options, const nautilus::compiler::CompilationUnitID& id)
    : options(options), id(id), rootPath(getRootPath(options, id, shallCreateFolder())) {
}

bool DumpHandler::shallCreateFolder() const {
	// We should only create a folder, if we dump to a file later on.
	// This means that we must check if we generally dump something and dump something to a file
	bool generallyDump = shouldDump("after_tracing");
	generallyDump = generallyDump or shouldDump("after_ssa");
	generallyDump = generallyDump or shouldDump("after_ir_creation");
	generallyDump = generallyDump or shouldDump("after_mlir_generation");
	generallyDump = generallyDump or shouldDump("after_llvm_generation");
	generallyDump = generallyDump or shouldDump("after_cpp_generation");
	generallyDump = generallyDump or shouldDump("after_bc_generation");
	return generallyDump and dumpToFile();
}

void DumpHandler::dump(std::string_view dumpName, std::string_view extension,
                       const std::function<std::string()>& dumpFunction) const {
	if (shouldDump(dumpName)) {
		const auto content = dumpFunction();
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
