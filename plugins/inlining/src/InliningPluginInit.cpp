// This translation unit installs the inlining plugin's LLVM backend hooks at
// library load time. It is always compiled; the MLIR-dependent hook
// installation is gated on NAUTILUS_INLINING_MLIR_ENABLED so the TU remains
// link-safe when the MLIR backend is disabled.
//
// The unconditional `registerInliningHooksPleaseIgnoreThisThanks` function
// exists purely as an anchor: `inline.hpp` takes its address via
// `__attribute__((used)) static const auto`, which forces any consumer TU
// that transitively includes `<nautilus/inline.hpp>` to pull this TU out of
// the static archive. That in turn carries the file-scope static registrar
// below into the final link, so the hooks actually get installed.
#include "nautilus/inline.hpp"

#ifdef NAUTILUS_INLINING_MLIR_ENABLED
#include "LLVMInliningUtils.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include <llvm/IR/Module.h>
#include <sstream>
#endif

int registerInliningHooksPleaseIgnoreThisThanks() {
	return 0;
}

#ifdef NAUTILUS_INLINING_MLIR_ENABLED
namespace {

struct InliningPluginRegistrar {
	InliningPluginRegistrar() {
		auto& hooks = nautilus::compiler::mlir::getLLVMBackendHooks();

		// Hook 1: run the JIT-time inliner on the generated llvm::Module
		// immediately before the LLVM optimizer pipeline.
		hooks.preOptModuleTransform = [](llvm::Module& module) {
			nautilus::compiler::mlir::inlineFunctions(module);
		};

		// Hook 2: contribute the inline registry's (name -> address) pairs
		// to the JIT's ORC symbol map. The symbol names are hex-formatted
		// runtime addresses, matching the names used by the rewritten
		// inlinable functions.
		hooks.jitSymbolContributor = [](const nautilus::compiler::mlir::SymbolContributor& contribute) {
			for (const auto& [key, value] : InlineFunctionRegistry::instance().getSymbolTable()) {
				auto hexStr = nautilus::compiler::mlir::ptrToHex(value);
				contribute(hexStr, value);
			}
		};

		// Hook 3: when lowering an external proxy call whose target has
		// registered bitcode, emit the hex-formatted pointer address as the
		// MLIR function name so the JIT-time inliner can pick it up.
		hooks.proxyCallNameOverride = [](void* functionPtr) -> std::optional<std::string> {
			if (!InlineFunctionRegistry::instance().containsFunctionBitcode(functionPtr)) {
				return std::nullopt;
			}
			std::stringstream ss;
			ss << functionPtr;
			return ss.str();
		};
	}
};

static InliningPluginRegistrar registrar_;

} // namespace
#endif
