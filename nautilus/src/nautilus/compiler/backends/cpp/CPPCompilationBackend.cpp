
#include "nautilus/compiler/backends/cpp/CPPCompilationBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/CapturedExceptionTransport.hpp"
#include "nautilus/compiler/backends/cpp/CPPCompiler.hpp"
#include "nautilus/compiler/backends/cpp/CPPCompilerFlags.hpp"
#include "nautilus/compiler/backends/cpp/CPPExecutable.hpp"
#include "nautilus/compiler/backends/cpp/CPPLoweringProvider.hpp"
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
namespace nautilus::compiler::cpp {

std::unique_ptr<Executable> CPPCompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                           const DumpHandler& dumpHandler,
                                                           const engine::Options& options,
                                                           CompilationStatistics* statistics) const {
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
	// `cpp.optimizationLevel` passes -O<n> to the C compiler (none by
	// default, i.e. -O0); `cpp.nativeArch` targets the host CPU.
	std::vector<std::string> extraFlags;
	if (options.hasOption("cpp.optimizationLevel")) {
		extraFlags.push_back("-O" + std::to_string(options.getOptionOrDefault("cpp.optimizationLevel", 0)));
	}
	if (options.getOptionOrDefault("cpp.nativeArch", false)) {
#if defined(__aarch64__)
		extraFlags.push_back(CPPCompilerFlags::CPU);
#else
		extraFlags.push_back(CPPCompilerFlags::ARCH);
#endif
	}
	auto res = compiler->compile("nautilus_" + ir->getId(), code, extraFlags);
	if (statistics != nullptr) {
		statistics->recordTimingMs("cpp.compile.ms", compileStart);
		statistics->recordTimingMs("backend.totalMs", backendStart);
	}
	return std::make_unique<CPPExecutable>(res, CapturedExceptionTransport::functionsNeedingCapture(*ir));
}

} // namespace nautilus::compiler::cpp
