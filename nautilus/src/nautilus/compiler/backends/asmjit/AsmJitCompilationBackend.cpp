
#include "nautilus/compiler/backends/asmjit/AsmJitCompilationBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/CapturedExceptionTransport.hpp"
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)
#include "nautilus/compiler/backends/asmjit/X64LoweringProvider.hpp"
#else
#include "nautilus/compiler/backends/asmjit/A64LoweringProvider.hpp"
#endif
#include "nautilus/compiler/backends/asmjit/AsmJitExecutable.hpp"
#include "nautilus/compiler/backends/asmjit/AsmJitNirBackend.hpp"
#include <chrono>
#include <memory>

namespace nautilus::compiler::asmjit {

std::unique_ptr<Executable> AsmJitCompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                              const DumpHandler& dumpHandler,
                                                              const engine::Options& options,
                                                              CompilationStatistics* statistics) const {
	// The graph of the NIR front end carries the module and no IR: it compiles natively (M4.4)
	if (ir->getNirModule() != nullptr && ir->getFunctionOperations().empty()) {
		auto module = std::static_pointer_cast<const nir::Module>(ir->getNirModule());
		// the traced module is process-local: its pointers are addresses, so the host resolves nothing
		const nirbackend::CompileContext context {dumpHandler, options, statistics, nullptr,
		                                          nirbackend::hostEnvironmentFor(ir->getImportLink())};
		return AsmJitNirBackend().compile(std::move(module), context);
	}
	const auto backendStart = std::chrono::steady_clock::now();

	auto runtime = std::make_unique<::asmjit::JitRuntime>();
	AsmJitLoweringProvider provider;

	const auto compileStart = std::chrono::steady_clock::now();
	// Thread the statistics sink through lower() so the optional post-RA
	// peephole pass can record its counters under backend-scoped keys
	// (asmjit.peephole.*). The DumpHandler lets lower() capture the AsmJit
	// builder IR and the generated assembly when those dumps are requested.
	auto result = provider.lower(ir, *runtime, options, dumpHandler, statistics);
	if (!result.basePtr) {
		return nullptr;
	}

	// Emit the captured textual representations. lower() only fills these in when the
	// matching dump is enabled, so the dump callbacks are cheap no-ops otherwise.
	dumpHandler.dump("after_asmjit_generation", "asmjit", [&]() { return result.asmjitIR; });
	dumpHandler.dump("after_asmjit_assembly", "asm", [&]() { return result.assembly; });

	if (statistics != nullptr) {
		statistics->recordTimingMs("asmjit.compile.ms", compileStart);
		statistics->set("asmjit.codeSize.bytes", static_cast<int64_t>(result.codeSize));
		statistics->recordTimingMs("backend.totalMs", backendStart);
	}

	return std::make_unique<AsmJitExecutable>(std::move(runtime), result.basePtr, std::move(result.jitPtrs),
	                                          CapturedExceptionTransport::functionsNeedingCapture(*ir));
}

} // namespace nautilus::compiler::asmjit
