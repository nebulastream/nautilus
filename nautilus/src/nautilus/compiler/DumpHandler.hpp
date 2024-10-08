#pragma once
#include "nautilus/JITCompiler.hpp"
#include "nautilus/options.hpp"
#include <filesystem>
#include <functional>
namespace nautilus::compiler {

class DumpHandler {
public:
	DumpHandler(const engine::Options& options, const CompilationUnitID& id);
	void dump(std::string_view dumpName, std::string_view extension, const std::function<std::string()>& dumpFunction) const;

private:
	[[nodiscard]] bool shouldDump(std::string_view dumpName) const;
	[[nodiscard]] bool dumpToConsole() const;
	[[nodiscard]] bool dumpToFile() const;
	const engine::Options& options;
	[[maybe_unused]] const CompilationUnitID& id;
	const std::filesystem::path rootPath;
};

} // namespace nautilus::compiler
