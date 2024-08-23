#pragma once
#include "nautilus/JITCompiler.hpp"
#include "nautilus/options.hpp"
#include <functional>
namespace nautilus::compiler {

class DumpHandler {
public:
	DumpHandler(const engine::Options& options, const CompilationUnitID& id);
	void dump(std::string_view dumpName, const std::function<std::string()>& dumpFunction) const;

private:
	bool shouldDump(std::string_view dumpName) const;
	const engine::Options& options;
	const CompilationUnitID& id;
};

} // namespace nautilus::compiler
