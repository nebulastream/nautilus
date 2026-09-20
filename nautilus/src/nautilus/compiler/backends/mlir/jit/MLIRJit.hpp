#pragma once

#include "nautilus/compiler/JitSymbolRegistry.hpp"
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

		// Register linked objects with the debugger through the GDB JIT
		// interface so GDB/LLDB (and the IDEs driving them) can resolve the
		// emitted DWARF and step into JIT-compiled frames.  Off by default:
		// the registration plugin keeps a copy of every debug object alive
		// for the lifetime of the JIT.
		bool enableDebuggerSupport = false;

		// Emit perf jitdump records (see llvm::orc::PerfSupportPlugin) for
		// every JIT-linked object, so `perf record` can symbolize JIT frames
		// and -- when `perfEmitDebugInfo` is set -- attribute samples down to
		// source lines. Linux/ELF only; a no-op with a warning elsewhere.
		bool enablePerfSupport = false;
		bool perfEmitDebugInfo = true;
		bool perfEmitUnwindInfo = true;

		// Publish every linked code range into the process-wide
		// JitSymbolRegistry, so an in-process sampling profiler can name JIT
		// frames without perf's external record/inject round trip. Shares the
		// region-qualified naming with the jitdump path, and is independent of
		// it -- either, both or neither may be enabled.
		bool enableJitSymbolRegistration = false;

		// The compile this module belongs to (IRGraph::getId()). Published with
		// every code range so a profile can tell two modules' identically named
		// `execute` symbols apart, and so the ranges can be dropped again when
		// this JIT -- and the code it owns -- is destroyed.
		std::string compilationUnitId;

		// Name each region() scope as its own jitdump symbol
		// (`execute::outer::hot`), recovered from the DWARF inline stack.
		// Requires `perfEmitDebugInfo`; without it there is no scope
		// information to recover.
		bool perfRegionSymbols = true;
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
	MLIRJit(std::unique_ptr<llvm::orc::LLJIT> jit, ModuleIndex moduleIndex);

	std::unique_ptr<llvm::orc::LLJIT> jit_;
	/// Zeroed when moved from, so only the owner that still holds the code
	/// withdraws its ranges.
	ModuleIndex moduleIndex_ = NO_MODULE;
};

} // namespace nautilus::compiler::mlir
