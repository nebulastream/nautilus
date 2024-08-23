#include "nautilus/compiler/DumpHandler.hpp"
#include "iostream"

namespace nautilus::compiler {

DumpHandler::DumpHandler(const engine::Options& options, const nautilus::compiler::CompilationUnitID& id) : options(options), id(id) {
}

void DumpHandler::dump(std::string_view dumpName, const std::function<std::string()>& dumpFunction) const {
	if (shouldDump(dumpName)) {
		auto content = dumpFunction();
		std::cout << dumpName << " -- " << id << std::endl;
		std::cout << content << std::endl;
	}
}

bool DumpHandler::shouldDump(std::string_view dumpName) const {
	return options.getOptionOrDefault("dump.all", true) || options.getOptionOrDefault("dump." + std::string(dumpName), false);
}

} // namespace nautilus::compiler
