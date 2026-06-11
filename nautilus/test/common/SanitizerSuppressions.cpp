// Suppressions for known LLVM/MLIR internal memory leaks.
// LLVM uses static/global allocations for target info and JIT infrastructure
// that are intentionally never freed (reclaimed at process exit).
// These are not bugs in Nautilus code.

#if defined(__has_feature)
#if __has_feature(address_sanitizer) || __has_feature(leak_sanitizer)
#define HAS_SANITIZER 1
#endif
#elif defined(__SANITIZE_ADDRESS__)
#define HAS_SANITIZER 1
#endif

#ifdef HAS_SANITIZER
// The stack-copy tracer (engine.traceMode=stackCopyTracing) restores stack bytes of
// frames that already returned, which is incompatible with ASAN's use-after-return
// detection: the restored frames reference fake-stack frames that were released when
// the frame first returned. UAR detection must therefore be disabled for the test
// binaries that exercise stackCopyTracing.
extern "C" const char* __asan_default_options() {
	return "detect_stack_use_after_return=0";
}

extern "C" const char* __lsan_default_suppressions() {
	return "leak:llvm::orc::LLJIT\n"
	       "leak:llvm::orc::SelfExecutorProcessControl\n"
	       "leak:llvm::X86TargetLowering\n"
	       "leak:llvm::X86LegalizerInfo\n"
	       "leak:llvm::LLVMContextImpl\n"
	       "leak:llvm::GIMatchTableExecutor\n"
	       "leak:mlir::ExecutionEngine\n"
	       "leak:llvm::Module::Module\n"
	       "leak:llvm::MCSubtargetInfo\n"
	       "leak:llvm::DataLayout\n"
	       "leak:llvm::detail::IEEEFloat\n"
	       "leak:createX86MCInstrInfo\n"
	       "leak:createX86MCRegisterInfo\n"
	       "leak:llvm::X86_MC::createX86MCSubtargetInfo\n"
	       "leak:LegacyLegalizerInfo\n"
	       "leak:GenericLLVMIRPlatformSupport\n"
	       "leak:llvm::TargetLoweringBase\n"
	       "leak:llvm::Function::Create\n"
	       "leak:llvm::Function::BuildLazyArguments\n"
	       "leak:llvm::MCCFIInstruction\n"
	       "leak:llvm::APFloat\n"
	       "leak:LegalityPredicates\n"
	       "leak:llvm::orc::EPCDynamicLibrarySearchGenerator\n"
	       "leak:llvm::orc::DynamicLibrarySearchGenerator\n"
	       "leak:llvm::orc::JITDylib\n"
	       "leak:llvm::orc::setUpGenericLLVMIRPlatform\n"
	       "leak:llvm::sys::getProcessTriple\n"
	       "leak:llvm::raw_ostream\n"
	       "leak:nautilus::compiler::mlir::MLIRCompilationBackend::compile\n"
	       "leak:llvm::allocate_buffer\n";
}
#endif
