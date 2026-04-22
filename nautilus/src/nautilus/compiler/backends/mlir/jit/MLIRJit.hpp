#pragma once

#include <llvm/ADT/STLFunctionalExtras.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/Mangling.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/Error.h>
#include <memory>
#include <mlir/IR/BuiltinOps.h>
#include <vector>

namespace nautilus::compiler::mlir {

/**
 * Nautilus-owned JIT engine for a single compiled MLIR module.
 *
 * Replaces mlir::ExecutionEngine so we control the pieces that upstream
 * hides behind a private LLJIT member: in particular, arbitrary
 * JITEventListener attachment against the object-linking layer (upstream
 * exposes only a boolean for the built-in perf/gdb listeners) and direct
 * access to the underlying LLJIT for extension use.
 */
class MLIRJit {
public:
	struct Options {
		llvm::CodeGenOptLevel codeGenOptLevel = llvm::CodeGenOptLevel::Aggressive;
		llvm::function_ref<llvm::Error(llvm::Module*)> transformer = nullptr;
		// Listeners attached to the object-linking layer before the module is
		// materialized. Supports custom profiling (VTune, perf maps, in-process
		// callbacks) which upstream mlir::ExecutionEngine disallows.
		std::vector<llvm::JITEventListener*> eventListeners;
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

	// Escape hatches. Upstream mlir::ExecutionEngine refuses to expose these;
	// the reason for this class's existence is that they are public here.
	llvm::orc::LLJIT& getLLJIT() {
		return *jit_;
	}
	llvm::orc::ExecutionSession& getExecutionSession() {
		return jit_->getExecutionSession();
	}
	// Attach a listener after construction. The listener must outlive this JIT.
	void addEventListener(llvm::JITEventListener* listener);

private:
	MLIRJit(std::unique_ptr<llvm::orc::LLJIT> jit, llvm::orc::RTDyldObjectLinkingLayer* objectLayer);

	std::unique_ptr<llvm::orc::LLJIT> jit_;
	llvm::orc::RTDyldObjectLinkingLayer* objectLayer_ = nullptr;
};

} // namespace nautilus::compiler::mlir
