
#include "nautilus/compiler/backends/amsjit/AsmJitCompilationBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)
#include "nautilus/compiler/backends/amsjit/X64LoweringProvider.hpp"
#else
#include "nautilus/compiler/backends/amsjit/A64LoweringProvider.hpp"
#endif
#include "nautilus/compiler/backends/amsjit/AsmJitExecutable.hpp"
#include <chrono>
#include <memory>

namespace nautilus::compiler::asmjit {

std::unique_ptr<Executable> AsmJitCompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                              const DumpHandler& /*dumpHandler*/,
                                                              const engine::Options& options,
                                                              CompilationStatistics* statistics) const {
	const auto backendStart = std::chrono::steady_clock::now();

	auto runtime = std::make_unique<::asmjit::JitRuntime>();
	AsmJitLoweringProvider provider;

	const auto compileStart = std::chrono::steady_clock::now();
	// Thread the statistics sink through lower() so the optional post-RA
	// peephole pass can record its counters under backend-scoped keys
	// (asmjit.peephole.*).
	auto result = provider.lower(ir, *runtime, options, statistics);
	if (!result.basePtr) {
		return nullptr;
	}
	if (statistics != nullptr) {
		statistics->recordTimingMs("asmjit.compile.ms", compileStart);
		statistics->set("asmjit.codeSize.bytes", static_cast<int64_t>(result.codeSize));
		statistics->recordTimingMs("backend.totalMs", backendStart);
	}

	return std::make_unique<AsmJitExecutable>(std::move(runtime), result.basePtr, std::move(result.jitPtrs));
}

} // namespace nautilus::compiler::asmjit
