#pragma once

#include <llvm/ADT/STLFunctionalExtras.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/Mangling.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/Error.h>
#include <memory>
#include <mlir/IR/BuiltinOps.h>

namespace nautilus::compiler::mlir {

/**
 * Nautilus-owned JIT engine for a single compiled MLIR module.
 *
 * Replaces mlir::ExecutionEngine so we control the pieces that upstream
 * hides behind a private LLJIT member: in particular, direct access to the
 * underlying LLJIT for extension use.
 *
 * The object linking layer is JITLink-based (ObjectLinkingLayer) rather than
 * the legacy RuntimeDyld. JITLink is required for correct exception-handling
 * unwind info relocations (personality & LSDA / .eh_frame entries), without
 * which C++ exceptions that propagate through JIT-compiled frames crash or
 * run cleanups in the wrong order.
 */
class MLIRJit {
public:
	struct Options {
		llvm::CodeGenOptLevel codeGenOptLevel = llvm::CodeGenOptLevel::Aggressive;
		llvm::function_ref<llvm::Error(llvm::Module*)> transformer = nullptr;
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

	// Escape hatch. Upstream mlir::ExecutionEngine refuses to expose its
	// LLJIT; the reason for this class's existence is that it is public here.
	llvm::orc::LLJIT& getLLJIT() {
		return *jit_;
	}
	llvm::orc::ExecutionSession& getExecutionSession() {
		return jit_->getExecutionSession();
	}

private:
	MLIRJit(std::unique_ptr<llvm::orc::LLJIT> jit);

	std::unique_ptr<llvm::orc::LLJIT> jit_;
};

} // namespace nautilus::compiler::mlir
