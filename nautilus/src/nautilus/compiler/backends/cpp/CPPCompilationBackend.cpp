
#include "nautilus/compiler/backends/cpp/CPPCompilationBackend.hpp"
#include "nautilus/compiler/backends/cpp/CPPCompiler.hpp"
#include "nautilus/compiler/backends/cpp/CPPExecutable.hpp"
#include "nautilus/compiler/backends/cpp/CPPLoweringProvider.hpp"
#include <iostream>
namespace nautilus::compiler::cpp {

std::unique_ptr<Executable> CPPCompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                           const DumpHandler& dumpHandler, const engine::Options&) {
	auto code = CPPLoweringProvider::lower(ir);
	dumpHandler.dump("after_c_generation", ".c", [&]() { return code; });

	auto compiler = CPPCompiler::create();
	auto res = compiler->compile("nautilus_" + ir->getId(), code);
	return std::make_unique<CPPExecutable>(res);
}

} // namespace nautilus::compiler::cpp
