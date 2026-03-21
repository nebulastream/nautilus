
#include "nautilus/compiler/backends/amsjit/AsmJitCompilationBackend.hpp"
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)
#include "nautilus/compiler/backends/amsjit/X64LoweringProvider.hpp"
#else
#include "nautilus/compiler/backends/amsjit/A64LoweringProvider.hpp"
#endif
#include "nautilus/compiler/backends/amsjit/AsmJitExecutable.hpp"
#include <memory>

namespace nautilus::compiler::asmjit {

std::unique_ptr<Executable> AsmJitCompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                              const DumpHandler& /*dumpHandler*/,
                                                              const engine::Options& /*options*/) const {
	auto runtime = std::make_unique<::asmjit::JitRuntime>();
	AsmJitLoweringProvider provider;

	auto result = provider.lower(ir, *runtime);
	if (!result.basePtr) {
		return nullptr;
	}

	return std::make_unique<AsmJitExecutable>(std::move(runtime), result.basePtr, std::move(result.jitPtrs));
}

} // namespace nautilus::compiler::asmjit
