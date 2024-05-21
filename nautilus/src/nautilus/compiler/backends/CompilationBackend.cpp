#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/backends/amsjit/AsmJitCompilationBackend.hpp"
#include "nautilus/compiler/backends/bc/BCInterpreterBackend.hpp"
#include "nautilus/compiler/backends/cpp/CPPCompilationBackend.hpp"
#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus::compiler {
CompilationBackend::~CompilationBackend() = default;
#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)
CompilationBackendRegistry::CompilationBackendRegistry() {
#ifdef ENABLE_MLIR_BACKEND
	items.emplace("mlir", std::make_unique<mlir::MLIRCompilationBackend>());
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	items["asmjit"] = std::make_unique<asmjit::AsmJitCompilationBackend>();
#endif
#ifdef ENABLE_BC_BACKEND
	items["bc"] = std::make_unique<bc::BCInterpreterBackend>();
#endif
#ifdef ENABLE_C_BACKEND
	items["cpp"] = std::make_unique<cpp::CPPCompilationBackend>();
#endif
}
#else
CompilationBackendRegistry::CompilationBackendRegistry() {
}
#endif

CompilationBackend* CompilationBackendRegistry::getBackend(const std::string& name) {
	if (!items.contains(name)) {
		throw RuntimeException("Backend not available");
	}
	return items[name].get();
}

} // namespace nautilus::compiler
