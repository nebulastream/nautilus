
#include "nautilus/compiler/backends/cpp/CPPCompilationBackend.hpp"
#include "nautilus/compiler/backends/cpp/CPPCompiler.hpp"
#include "nautilus/compiler/backends/cpp/CPPExecutable.hpp"
#include "nautilus/compiler/backends/cpp/CPPLoweringProvider.hpp"
#include <iostream>
namespace nautilus::compiler::cpp {

std::unique_ptr<Executable> CPPCompilationBackend::compile(std::shared_ptr<ir::IRGraph> ir) {
	// auto timer = Timer<>("CPP");
	// timer.start();

	auto code = CPPLoweringProvider::lower(ir);
	// dumpHelper.dump("code.cpp", code);
	std::cout << code << std::endl;
	// timer.snapshot("CPPGen");

	auto compiler = CPPCompiler::create();
	auto res = compiler->compile("test", code);

	return std::make_unique<CPPExecutable>(res);
}

} // namespace nautilus::compiler::cpp
