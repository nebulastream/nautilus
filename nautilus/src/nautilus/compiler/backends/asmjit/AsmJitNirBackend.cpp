#include "nautilus/compiler/backends/asmjit/AsmJitNirBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/NirCodeProfile.hpp"
#include "nautilus/compiler/backends/asmjit/AsmJitExecutable.hpp"
#include "nautilus/compiler/backends/asmjit/AsmJitNirLowering.hpp"
#include "nautilus/compiler/backends/nir/NirRuntime.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <chrono>

namespace nautilus::compiler::asmjit {

std::unique_ptr<Executable> AsmJitNirBackend::compile(std::shared_ptr<const nir::Module> module,
                                                      const nirbackend::CompileContext& context) const {
#if defined(NAUTILUS_ASMJIT_NIR_LOWERING)
	if (context.cancellation != nullptr && context.cancellation->cancelled()) {
		return nullptr;
	}
	const auto backendStart = std::chrono::steady_clock::now();
	auto runtime = AsmJitExecutable::sharedRuntime();

	const auto compileStart = std::chrono::steady_clock::now();
	auto result =
	    lowerNirModule(*module, *runtime, context.options, context.dumpHandler, context.statistics, context.host);
	if (result.basePtr == nullptr) {
		throw RuntimeException("asmjit: the JIT runtime refused the generated code");
	}

	// D14: with `perf` or `perf.sample`, the symbols of the functions and regions, and the line table of the code
	ModuleIndex symbols = NO_MODULE;
	if (profilesCode(context.options)) {
		NirCodeMap map;
		map.base = reinterpret_cast<uintptr_t>(result.basePtr);
		map.size = static_cast<uint32_t>(result.codeSize);
		map.functionOffsets = std::move(result.functionOffsets);
		map.marks = std::move(result.marks);
		symbols = publishNirCode(*module, map, context.options, newCodeUnit("asmjit"));
	}

	// the captured textual representations; lowering only fills them in when the matching dump is enabled
	context.dumpHandler.dump("after_asmjit_generation", "asmjit", [&]() { return result.asmjitIR; });
	context.dumpHandler.dump("after_asmjit_assembly", "asm", [&]() { return result.assembly; });

	if (context.statistics != nullptr) {
		context.statistics->recordTimingMs("asmjit.compile.ms", compileStart);
		context.statistics->set("asmjit.codeSize.bytes", static_cast<int64_t>(result.codeSize));
		context.statistics->recordTimingMs("backend.totalMs", backendStart);
	}
	auto executable = std::make_unique<AsmJitExecutable>(std::move(runtime), result.basePtr, std::move(result.jitPtrs),
	                                                     nirbackend::functionsNeedingCapture(*module), module);
	executable->publishedSymbols(symbols);
	return executable;
#else
	(void) module;
	(void) context;
	throw NotImplementedException("asmjit on NIR is not implemented for this architecture yet");
#endif
}

} // namespace nautilus::compiler::asmjit
