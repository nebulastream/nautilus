#include "nautilus/compiler/backends/cpp/CPPNirBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/cpp/CPPCompiler.hpp"
#include "nautilus/compiler/backends/cpp/CPPExecutable.hpp"
#include "nautilus/compiler/backends/cpp/CPPNirLowering.hpp"
#include "nautilus/compiler/backends/nir/NirRuntime.hpp"
#include <atomic>
#include <chrono>
#include <unistd.h>

namespace nautilus::compiler::cpp {

std::unique_ptr<Executable> CPPNirBackend::compile(std::shared_ptr<const nir::Module> module,
                                                   const nirbackend::CompileContext& context) const {
	static std::atomic<uint64_t> counter {0};
	return compileNamed(std::move(module), context,
	                    "nir_" + std::to_string(::getpid()) + "_" + std::to_string(counter.fetch_add(1)));
}

std::unique_ptr<Executable> CPPNirBackend::compileNamed(std::shared_ptr<const nir::Module> module,
                                                        const nirbackend::CompileContext& context,
                                                        const std::string& unit) const {
	if (context.cancellation != nullptr && context.cancellation->cancelled()) {
		return nullptr;
	}
	const auto backendStart = std::chrono::steady_clock::now();

	const auto loweringStart = std::chrono::steady_clock::now();
	auto code = lowerNirToCpp(*module, context.host);
	context.dumpHandler.dump("after_c_generation", ".c", [&]() { return code; });
	if (context.statistics != nullptr) {
		context.statistics->recordTimingMs("cpp.loweringFromNir.ms", loweringStart);
		context.statistics->set("cpp.sourceSize.bytes", static_cast<int64_t>(code.size()));
	}

	const auto compileStart = std::chrono::steady_clock::now();
	auto compiler = CPPCompiler::create();
	auto library = compiler->compile("nautilus_" + unit, code);
	if (context.statistics != nullptr) {
		context.statistics->recordTimingMs("cpp.compile.ms", compileStart);
		context.statistics->recordTimingMs("backend.totalMs", backendStart);
	}
	return std::make_unique<CPPExecutable>(library, nirbackend::functionsNeedingCapture(*module), module);
}

} // namespace nautilus::compiler::cpp
