#include "nautilus/compiler/backends/mlir/jit/MLIRJit.hpp"
#include "nautilus/compiler/JitSymbolRegistry.hpp"
#include "nautilus/compiler/backends/mlir/jit/PackFunctionArguments.hpp"
#include <exception>
#include <llvm/ExecutionEngine/Orc/Debugging/DebuggerSupport.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/ObjectTransformLayer.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/TargetParser/Triple.h>
#include <mlir/Target/LLVMIR/Export.h>
#include <utility>

#if defined(__linux__)
#include "nautilus/compiler/backends/mlir/debug/JitSymbolRegistrationPlugin.hpp"
#include "nautilus/compiler/backends/mlir/debug/PerfJitDumpPlugin.hpp"
#include <llvm/ExecutionEngine/Orc/Debugging/DebugInfoSupport.h>
#include <llvm/ExecutionEngine/Orc/TargetProcess/JITLoaderPerf.h>
#endif

namespace nautilus::compiler::mlir {

namespace {

// Non-templated error factory. We deliberately avoid llvm::make_error<StringError>
// because it instantiates llvm::ErrorInfo<...> in our TU and requires typeinfo
// for llvm::ErrorInfoBase, which LLVM (built with -fno-rtti) does not export.
llvm::Error makeStringError(const llvm::Twine& message) {
	return llvm::createStringError(llvm::inconvertibleErrorCode(), message);
}

// Installs perf jitdump support (llvm::orc::PerfSupportPlugin) on `layer`, so
// `perf record` can read symbols (and, when `emitDebugInfo` is set, line
// tables) for every object it links. Linux/ELF only -- `PerfSupportPlugin`
// itself refuses non-ELF targets, and the jitdump writer entry points below
// are compiled only on Linux (LLVMOrcTargetProcess's JITLoaderPerf.cpp is
// `#ifdef __linux__`), so referencing them from a TU built for another
// platform would fail to link.
//
// The writer is Nautilus's own PerfJitDumpPlugin rather than LLVM's
// PerfSupportPlugin: the latter emits a whole object's debug records ahead of
// its code records, which `perf inject --jit` mis-pairs, and it has no way to
// surface region() scopes at all. See PerfJitDumpPlugin.hpp.
//
// Either way the three loader entry points are referenced by address rather
// than resolved through `PerfSupportPlugin::Create`, which looks them up via a
// JITDylib lookup falling through to `dlsym` on the current process. Nothing
// in a statically linked Nautilus binary references `JITLoaderPerf.o`, so the
// archive member implementing them is never pulled into the link at all, and
// `dlsym` would also need `-rdynamic` even if it were. Taking their addresses
// directly forces the archive member in and sidesteps the lookup entirely.
void installPerfSupport([[maybe_unused]] llvm::orc::ObjectLinkingLayer& layer,
                        [[maybe_unused]] llvm::orc::ExecutionSession& session,
                        [[maybe_unused]] const llvm::Triple& targetTriple, [[maybe_unused]] bool emitDebugInfo,
                        [[maybe_unused]] bool emitUnwindInfo, [[maybe_unused]] bool emitRegionSymbols) {
#if defined(__linux__)
	if (!targetTriple.isOSBinFormatELF()) {
		llvm::errs() << "nautilus: perf is set but the target is not ELF; perf jitdump support is "
		                "Linux/ELF-only and will be skipped.\n";
		return;
	}
	// JITLink prunes .debug_* sections before PostFixup; PerfSupportPlugin's
	// PostFixup pass builds a DWARFContext from them, so without this
	// preservation pass it silently emits JIT_CODE_LOAD records with no
	// JIT_CODE_DEBUG_INFO alongside -- symbols but no source attribution.
	// Installed unconditionally (not just when `emitDebugInfo` is set):
	// preservation is a cheap, idempotent PrePrune pass either way.
	layer.addPlugin(std::make_shared<llvm::orc::DebugInfoPreservationPlugin>());
	layer.addPlugin(std::make_shared<PerfJitDumpPlugin>(
	    session.getExecutorProcessControl(), llvm::orc::ExecutorAddr::fromPtr(&llvm_orc_registerJITLoaderPerfStart),
	    llvm::orc::ExecutorAddr::fromPtr(&llvm_orc_registerJITLoaderPerfEnd),
	    llvm::orc::ExecutorAddr::fromPtr(&llvm_orc_registerJITLoaderPerfImpl), emitDebugInfo, emitUnwindInfo,
	    emitRegionSymbols));
#else
	llvm::errs() << "nautilus: perf is set but perf jitdump support is Linux-only; skipping.\n";
#endif
}

// Installs the in-process JIT symbol registry publisher on `layer`, so a
// sampling profiler inside this process can resolve JIT addresses to
// region-qualified names. Unlike the jitdump path this needs no external
// tooling and no ELF-specific machinery -- it only reads DWARF that is already
// there and appends to a table -- but it is kept Linux-guarded alongside its
// sibling because DebugInfoPreservationPlugin and the DWARF context are what
// both depend on, and there is no in-process sampler to serve elsewhere.
void installJitSymbolRegistration([[maybe_unused]] llvm::orc::ObjectLinkingLayer& layer,
                                  [[maybe_unused]] bool emitRegionSymbols, [[maybe_unused]] ModuleIndex moduleIndex) {
#if defined(__linux__)
	// JITLink prunes .debug_* sections before PostFixup, and the region names
	// are recovered from exactly those sections. Installed unconditionally and
	// idempotently, the same way the jitdump path does it -- adding it twice
	// when both are enabled is harmless.
	layer.addPlugin(std::make_shared<llvm::orc::DebugInfoPreservationPlugin>());
	layer.addPlugin(std::make_shared<JitSymbolRegistrationPlugin>(emitRegionSymbols, moduleIndex));
#endif
}

} // namespace

MLIRJit::ObjectCapture::ObjectCapture(std::function<void(std::string_view)> objectPreflight)
    : objectPreflight_(std::move(objectPreflight)) {
}

llvm::Error MLIRJit::ObjectCapture::validate(llvm::StringRef object) const {
	try {
		if (objectPreflight_) {
			objectPreflight_(std::string_view(object.data(), object.size()));
		}
	} catch (const std::exception& error) {
		return makeStringError(llvm::Twine("MLIR object preflight failed: ") + error.what());
	}
	return llvm::Error::success();
}

void MLIRJit::ObjectCapture::record(llvm::StringRef object) {
	std::lock_guard guard(mutex_);
	objects_.emplace_back(object);
}

std::optional<std::string> MLIRJit::ObjectCapture::getObject() const {
	std::lock_guard guard(mutex_);
	if (objects_.size() != 1) {
		return std::nullopt;
	}
	return objects_.front();
}

MLIRJit::MLIRJit(std::unique_ptr<llvm::orc::LLJIT> jit, ModuleIndex moduleIndex)
    : jit_(std::move(jit)), moduleIndex_(moduleIndex) {
}

MLIRJit::~MLIRJit() {
	// Destroying the LLJIT frees the code its ranges describe, so the registry
	// has to forget them here -- otherwise a later compile reusing those
	// addresses resolves to this module's names, and a long-running engine
	// accumulates ranges for code that no longer exists.
	JitSymbolRegistry::instance().remove(moduleIndex_);
}

// Move has to clear the source's index by hand. The defaulted version copies
// it, and both objects then run a destructor that withdraws the same ranges --
// the second of which would be withdrawing live code's symbols.
MLIRJit::MLIRJit(MLIRJit&& other) noexcept
    : jit_(std::move(other.jit_)), moduleIndex_(std::exchange(other.moduleIndex_, NO_MODULE)) {
}

MLIRJit& MLIRJit::operator=(MLIRJit&& other) noexcept {
	if (this != &other) {
		JitSymbolRegistry::instance().remove(moduleIndex_);
		jit_ = std::move(other.jit_);
		moduleIndex_ = std::exchange(other.moduleIndex_, NO_MODULE);
	}
	return *this;
}

llvm::Expected<std::unique_ptr<MLIRJit>> MLIRJit::createJit(const Options& options) {
	// Interned once here rather than per linked object: the handle is what the
	// registration plugin stamps on every range, and what this JIT withdraws by
	// when it is destroyed. NO_MODULE when registration is off, which makes
	// both the stamping and the withdrawal no-ops.
	const ModuleIndex moduleIndex = options.enableJitSymbolRegistration
	                                    ? JitSymbolRegistry::instance().intern(options.compilationUnitId)
	                                    : NO_MODULE;

	auto tmBuilderOrError = llvm::orc::JITTargetMachineBuilder::detectHost();
	if (!tmBuilderOrError) {
		return tmBuilderOrError.takeError();
	}
	tmBuilderOrError->setCodeGenOptLevel(options.codeGenOptLevel);

	const auto targetTriple = tmBuilderOrError->getTargetTriple();

	// Instantiate a JITLink-based object linking layer. Only machine code with
	// reliable exception-handling unwind info (personality + LSDA) is linked;
	// the legacy RuntimeDyld layer cannot relocate these and causes crashes or
	// misordered cleanups when exceptions cross JIT frames.
	auto objectLinkingLayerCreator =
	    [targetTriple, enablePerfSupport = options.enablePerfSupport, perfEmitDebugInfo = options.perfEmitDebugInfo,
	     perfEmitUnwindInfo = options.perfEmitUnwindInfo, perfRegionSymbols = options.perfRegionSymbols,
	     enableJitSymbolRegistration = options.enableJitSymbolRegistration,
	     moduleIndex](llvm::orc::ExecutionSession& session) -> std::unique_ptr<llvm::orc::ObjectLayer> {
		auto layer = std::make_unique<llvm::orc::ObjectLinkingLayer>(session);

		// COFF binaries (Windows) need special handling for exported symbol
		// visibility. Mirrors upstream mlir::ExecutionEngine.
		if (targetTriple.isOSBinFormatCOFF()) {
			layer->setOverrideObjectFlagsWithResponsibilityFlags(true);
			layer->setAutoClaimResponsibilityForObjectSymbols(true);
		}

		if (enablePerfSupport) {
			installPerfSupport(*layer, session, targetTriple, perfEmitDebugInfo, perfEmitUnwindInfo, perfRegionSymbols);
		}

		if (enableJitSymbolRegistration) {
			installJitSymbolRegistration(*layer, perfRegionSymbols, moduleIndex);
		}

		return layer;
	};

	// Let LLJIT build its own ConcurrentIRCompiler from the JITTargetMachineBuilder.
	// We avoid constructing llvm::orc::TMOwningSimpleCompiler directly because
	// LLVM is compiled with -fno-rtti and exporting RTTI for its polymorphic
	// types across TU boundaries would require us to match.
	auto jitOrErr = llvm::orc::LLJITBuilder()
	                    .setJITTargetMachineBuilder(std::move(*tmBuilderOrError))
	                    .setObjectLinkingLayerCreator(objectLinkingLayerCreator)
	                    .create();
	if (!jitOrErr) {
		return jitOrErr.takeError();
	}
	auto jit = std::move(*jitOrErr);

	// Installs the JITLink debug-object registration plugin, which notifies
	// the debugger via __jit_debug_register_code once an object is linked.
	// Emitting DWARF alone is not enough: without this the debugger never
	// learns that the executable memory belongs to a module with line tables.
	if (options.enableDebuggerSupport) {
		if (auto err = llvm::orc::enableDebuggerSupport(*jit)) {
			return err;
		}
	}

	if (options.objectCapture) {
		jit->getObjTransformLayer().setTransform(
		    [capture = options.objectCapture](
		        std::unique_ptr<llvm::MemoryBuffer> object) -> llvm::Expected<std::unique_ptr<llvm::MemoryBuffer>> {
			    if (auto error = capture->validate(object->getBuffer())) {
				    return error;
			    }
			    capture->record(object->getBuffer());
			    return object;
		    });
	}

	// Resolve symbols that are statically linked in the current process.
	if (options.allowCurrentProcessSymbols) {
		auto& mainJD = jit->getMainJITDylib();
		// GCC 12+ at -O3 (and with asan) raises a spurious -Wmaybe-uninitialized
		// inside the inlined move constructor of llvm::unique_function for the
		// defaulted AddAbsoluteSymbolsFn parameter. The warning is a known
		// false positive in gcc's inliner; suppress locally.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
		auto generatorOrErr =
		    llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(jit->getDataLayout().getGlobalPrefix());
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
		if (!generatorOrErr) {
			return generatorOrErr.takeError();
		}
		mainJD.addGenerator(std::move(*generatorOrErr));
	}

	return std::unique_ptr<MLIRJit>(new MLIRJit(std::move(jit), moduleIndex));
}

llvm::Expected<std::unique_ptr<MLIRJit>> MLIRJit::create(::mlir::ModuleOp module, const Options& options) {
	auto ctx = std::make_unique<llvm::LLVMContext>();
	auto llvmModule = ::mlir::translateModuleToLLVMIR(module, *ctx);
	if (!llvmModule) {
		return makeStringError("could not convert to LLVM IR");
	}
	auto result = createJit(options);
	if (!result) {
		return result.takeError();
	}
	auto& jit = (*result)->getLLJIT();
	llvmModule->setDataLayout(jit.getDataLayout());
	llvmModule->setTargetTriple(jit.getTargetTriple());
	detail::packFunctionArguments(llvmModule.get());

	llvm::orc::ThreadSafeModule tsm(std::move(llvmModule), std::move(ctx));
	if (options.transformer) {
		if (auto error = tsm.withModuleDo([&options](llvm::Module& m) { return options.transformer(&m); })) {
			return error;
		}
	}
	if (auto error = jit.addIRModule(std::move(tsm))) {
		return error;
	}
	return result;
}

llvm::Expected<std::unique_ptr<MLIRJit>> MLIRJit::createFromObject(std::unique_ptr<llvm::MemoryBuffer> object,
                                                                   const Options& options) {
	if (!object || object->getBuffer().empty()) {
		return makeStringError("artifact object is empty");
	}
	auto result = createJit(options);
	if (!result) {
		return result.takeError();
	}
	if (auto error = (*result)->getLLJIT().addObjectFile(std::move(object))) {
		return error;
	}
	return result;
}

llvm::Error
MLIRJit::registerSymbols(llvm::function_ref<llvm::orc::SymbolMap(llvm::orc::MangleAndInterner)> symbolMapFn) {
	auto& mainJD = jit_->getMainJITDylib();
	return mainJD.define(llvm::orc::absoluteSymbols(
	    symbolMapFn(llvm::orc::MangleAndInterner(mainJD.getExecutionSession(), jit_->getDataLayout()))));
}

llvm::Error MLIRJit::initialize() {
	if (!jit_->getTargetTriple().isAArch64()) {
		return jit_->initialize(jit_->getMainJITDylib());
	}
	return llvm::Error::success();
}

llvm::Expected<void*> MLIRJit::lookup(llvm::StringRef name) {
	auto expectedSymbol = jit_->lookup(name);
	if (!expectedSymbol) {
		return expectedSymbol.takeError();
	}
	if (void* fptr = expectedSymbol->toPtr<void*>()) {
		return fptr;
	}
	return makeStringError("looked up function is null");
}

llvm::Expected<void (*)(void**)> MLIRJit::lookupPacked(llvm::StringRef name) {
	auto result = lookup(detail::makePackedFunctionName(name));
	if (!result) {
		return result.takeError();
	}
	return reinterpret_cast<void (*)(void**)>(*result);
}

} // namespace nautilus::compiler::mlir
