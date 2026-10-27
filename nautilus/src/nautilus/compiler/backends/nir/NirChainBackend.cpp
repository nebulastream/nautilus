#include "nautilus/compiler/backends/nir/NirChainBackend.hpp"
#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus::compiler::nirbackend {

std::unique_ptr<Executable> NirChainBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                     const DumpHandler& dumpHandler, const engine::Options& options,
                                                     CompilationStatistics* statistics) const {
	auto module = std::static_pointer_cast<const nir::Module>(ir->getNirModule());
	if (!module) {
		throw RuntimeException("The backend 'nirtbc' compiles the NIR of the trace: set tracing.nirChain=true");
	}
	const auto* backend = NirBackendRegistry::instance().find("tbc");
	if (backend == nullptr) {
		throw RuntimeException("The backend 'nirtbc' needs the tbc backend");
	}
	// the traced module is process-local: its pointers are addresses, so the host resolves nothing
	CompileContext context {dumpHandler, options, statistics, nullptr, {}};
	return backend->compile(std::move(module), context);
}

} // namespace nautilus::compiler::nirbackend
