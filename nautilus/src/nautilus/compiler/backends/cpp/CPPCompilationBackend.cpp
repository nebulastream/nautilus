
#include "nautilus/compiler/backends/cpp/CPPCompilationBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/CapturedExceptionTransport.hpp"
#include "nautilus/compiler/backends/cpp/CPPCompiler.hpp"
#include "nautilus/compiler/backends/cpp/CPPExecutable.hpp"
#include "nautilus/compiler/backends/cpp/CPPLoweringProvider.hpp"
#include "nautilus/compiler/backends/cpp/CPPNirBackend.hpp"
#include <chrono>
#include <iostream>
namespace nautilus::compiler::cpp {

std::unique_ptr<Executable> CPPCompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                           const DumpHandler& dumpHandler,
                                                           const engine::Options& options,
                                                           CompilationStatistics* statistics) const {
	// The graph of the NIR front end carries the module and no IR: it compiles natively (M4.3)
	if (ir->getNirModule() != nullptr && ir->getFunctionOperations().empty()) {
		auto module = std::static_pointer_cast<const nir::Module>(ir->getNirModule());
		// the traced module is process-local: its pointers are addresses, so the host resolves nothing
		const nirbackend::CompileContext context {dumpHandler, options, statistics, nullptr, {}};
		return CPPNirBackend().compileNamed(std::move(module), context, ir->getId());
	}
	const auto backendStart = std::chrono::steady_clock::now();

	const auto loweringStart = std::chrono::steady_clock::now();
	auto code = CPPLoweringProvider::lower(ir);
	dumpHandler.dump("after_c_generation", ".c", [&]() { return code; });
	if (statistics != nullptr) {
		statistics->recordTimingMs("cpp.loweringFromIR.ms", loweringStart);
		statistics->set("cpp.sourceSize.bytes", static_cast<int64_t>(code.size()));
	}

	const auto compileStart = std::chrono::steady_clock::now();
	auto compiler = CPPCompiler::create();
	auto res = compiler->compile("nautilus_" + ir->getId(), code);
	if (statistics != nullptr) {
		statistics->recordTimingMs("cpp.compile.ms", compileStart);
		statistics->recordTimingMs("backend.totalMs", backendStart);
	}
	return std::make_unique<CPPExecutable>(res, CapturedExceptionTransport::functionsNeedingCapture(*ir));
}

} // namespace nautilus::compiler::cpp
