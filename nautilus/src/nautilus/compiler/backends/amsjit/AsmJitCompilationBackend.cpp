
#include "nautilus/compiler/backends/amsjit/AsmJitCompilationBackend.hpp"
#include "nautilus/compiler/backends/amsjit/A64LoweringProvider.hpp"
#include "nautilus/compiler/backends/amsjit/AsmJitExecutable.hpp"
#include <memory>

namespace nautilus::compiler::asmjit {

std::unique_ptr<Executable> AsmJitCompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                              const DumpHandler& /*dumpHandler*/,
                                                              const engine::Options& /*options*/) const {
	auto runtime = std::make_unique<::asmjit::JitRuntime>();
	AsmJitLoweringProvider provider;
	void* fn = provider.lower(ir, *runtime);
	if (!fn) {
		return nullptr;
	}
	return std::make_unique<AsmJitExecutable>(std::move(runtime), fn);
}

} // namespace nautilus::compiler::asmjit
