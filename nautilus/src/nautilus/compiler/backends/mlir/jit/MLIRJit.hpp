#pragma once

#include <llvm/ADT/STLFunctionalExtras.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/Mangling.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/Error.h>
#include <memory>
#include <mlir/IR/BuiltinOps.h>

// Forward declaration so callers don't transitively pull in MLIR's upstream
// ExecutionEngine header. Phase 1 delegates to it internally; later phases
// drop the dependency entirely.
namespace mlir {
class ExecutionEngine;
}

namespace nautilus::compiler::mlir {

/**
 * Nautilus-owned JIT engine for a single compiled MLIR module.
 *
 * Phase 1: thin wrapper around mlir::ExecutionEngine, preserving existing
 * behavior (one engine per compiled module). Phase 2 replaces the internals
 * with a direct llvm::orc::LLJIT build so we can expose JITEventListener
 * registration, a caller-supplied ObjectCache, and the underlying LLJIT&
 * for advanced use — none of which upstream mlir::ExecutionEngine exposes.
 */
class MLIRJit {
public:
	struct Options {
		llvm::CodeGenOptLevel codeGenOptLevel = llvm::CodeGenOptLevel::Aggressive;
		llvm::function_ref<llvm::Error(llvm::Module*)> transformer = nullptr;
		// Phase 2 additions will live here: eventListeners, objectCache,
		// targetMachine, enableGDBListener.
	};

	~MLIRJit();
	MLIRJit(const MLIRJit&) = delete;
	MLIRJit& operator=(const MLIRJit&) = delete;
	MLIRJit(MLIRJit&&) noexcept;
	MLIRJit& operator=(MLIRJit&&) noexcept;

	static llvm::Expected<std::unique_ptr<MLIRJit>> create(::mlir::ModuleOp module, const Options& options);

	void registerSymbols(llvm::function_ref<llvm::orc::SymbolMap(llvm::orc::MangleAndInterner)> symbolMapFn);

	llvm::Expected<void*> lookup(llvm::StringRef name);
	llvm::Expected<void (*)(void**)> lookupPacked(llvm::StringRef name);

private:
	explicit MLIRJit(std::unique_ptr<::mlir::ExecutionEngine> engine);

	std::unique_ptr<::mlir::ExecutionEngine> engine_;
};

} // namespace nautilus::compiler::mlir
