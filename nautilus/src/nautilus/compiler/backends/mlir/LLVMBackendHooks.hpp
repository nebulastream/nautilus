#pragma once

#include <functional>
#include <optional>
#include <string>

namespace llvm {
class Module;
} // namespace llvm

namespace nautilus::compiler::mlir {

// Callback used by jitSymbolContributor to add (name -> address) entries
// into the ORC symbol map of the MLIR execution engine.
using SymbolContributor = std::function<void(const std::string& name, void* addr)>;

// Generic extension points that the MLIR backend consults during LLVM IR
// generation and JIT compilation. A plugin may install any subset of the
// hooks from a file-scope static registrar; hooks that are not set are simply
// skipped by the core call sites.
struct LLVMBackendHooks {
	// Runs on the generated llvm::Module once, immediately before the LLVM
	// optimizer pipeline is applied.
	std::function<void(llvm::Module&)> preOptModuleTransform;

	// Invoked inside the JIT's symbol-map closure. The contributor adds
	// (name, address) pairs into the ORC symbol map.
	std::function<void(const SymbolContributor&)> jitSymbolContributor;

	// Called during MLIR lowering for external proxy calls. Returns an
	// alternative function name to emit given the runtime address, or
	// std::nullopt to use the default name.
	std::function<std::optional<std::string>(void* functionPtr)> proxyCallNameOverride;
};

// Meyers singleton - avoids static initialization-order issues between the
// core library and any plugin that writes to the hooks from its own
// file-scope registrar.
LLVMBackendHooks& getLLVMBackendHooks();

} // namespace nautilus::compiler::mlir
