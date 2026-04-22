
#include "nautilus/compiler/backends/mlir/jit/MLIRJit.hpp"
#include "nautilus/compiler/backends/mlir/jit/PackFunctionArguments.hpp"
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/Error.h>
#include <llvm/TargetParser/Triple.h>
#include <mlir/Target/LLVMIR/Export.h>

namespace nautilus::compiler::mlir {

namespace {

// Non-templated error factory. We deliberately avoid llvm::make_error<StringError>
// because it instantiates llvm::ErrorInfo<...> in our TU and requires typeinfo
// for llvm::ErrorInfoBase, which LLVM (built with -fno-rtti) does not export.
llvm::Error makeStringError(const llvm::Twine& message) {
	return llvm::createStringError(llvm::inconvertibleErrorCode(), message);
}

} // namespace

MLIRJit::MLIRJit(std::unique_ptr<llvm::orc::LLJIT> jit, llvm::orc::RTDyldObjectLinkingLayer* objectLayer)
    : jit_(std::move(jit)), objectLayer_(objectLayer) {
}

MLIRJit::~MLIRJit() = default;
MLIRJit::MLIRJit(MLIRJit&&) noexcept = default;
MLIRJit& MLIRJit::operator=(MLIRJit&&) noexcept = default;

llvm::Expected<std::unique_ptr<MLIRJit>> MLIRJit::create(::mlir::ModuleOp module, const Options& options) {
	auto ctx = std::make_unique<llvm::LLVMContext>();
	auto llvmModule = ::mlir::translateModuleToLLVMIR(module, *ctx);
	if (!llvmModule) {
		return makeStringError("could not convert to LLVM IR");
	}

	auto tmBuilderOrError = llvm::orc::JITTargetMachineBuilder::detectHost();
	if (!tmBuilderOrError) {
		return tmBuilderOrError.takeError();
	}
	tmBuilderOrError->setCodeGenOptLevel(options.codeGenOptLevel);

	// Build a one-shot TargetMachine only to seed the module's data layout and
	// triple; LLJIT constructs its own TargetMachines via the builder below.
	auto tmOrError = tmBuilderOrError->createTargetMachine();
	if (!tmOrError) {
		return tmOrError.takeError();
	}
	llvmModule->setDataLayout((*tmOrError)->createDataLayout());
	llvmModule->setTargetTriple((*tmOrError)->getTargetTriple());

	detail::packFunctionArguments(llvmModule.get());

	const auto dataLayout = llvmModule->getDataLayout();
	const auto targetTriple = llvmModule->getTargetTriple();

	// Stash the layer pointer from inside the object-layer creator so we can
	// reach it after LLJIT construction for addEventListener().
	llvm::orc::RTDyldObjectLinkingLayer* rawLayer = nullptr;

	auto objectLinkingLayerCreator =
	    [&rawLayer, &options,
	     &targetTriple](llvm::orc::ExecutionSession& session) -> std::unique_ptr<llvm::orc::ObjectLayer> {
		auto layer = std::make_unique<llvm::orc::RTDyldObjectLinkingLayer>(
		    session, [](const llvm::MemoryBuffer&) { return std::make_unique<llvm::SectionMemoryManager>(); });

		for (auto* listener : options.eventListeners) {
			if (listener != nullptr) {
				layer->registerJITEventListener(*listener);
			}
		}

		// COFF binaries (Windows) need special handling for exported symbol
		// visibility. Mirrors upstream mlir::ExecutionEngine.
		if (targetTriple.isOSBinFormatCOFF()) {
			layer->setOverrideObjectFlagsWithResponsibilityFlags(true);
			layer->setAutoClaimResponsibilityForObjectSymbols(true);
		}

		rawLayer = layer.get();
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

	llvm::orc::ThreadSafeModule tsm(std::move(llvmModule), std::move(ctx));
	if (options.transformer) {
		auto transformErr =
		    tsm.withModuleDo([&options](llvm::Module& m) -> llvm::Error { return options.transformer(&m); });
		if (transformErr) {
			return std::move(transformErr);
		}
	}
	if (auto err = jit->addIRModule(std::move(tsm))) {
		return std::move(err);
	}

	// Resolve symbols that are statically linked in the current process.
	auto& mainJD = jit->getMainJITDylib();
	auto generatorOrErr = llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(dataLayout.getGlobalPrefix());
	if (!generatorOrErr) {
		return generatorOrErr.takeError();
	}
	mainJD.addGenerator(std::move(*generatorOrErr));

	// Execute the module's global constructors. Upstream skips this on AArch64
	// due to a known LLVM bug (llvm/llvm-project#71963); mirror that.
	if (!jit->getTargetTriple().isAArch64()) {
		if (auto err = jit->initialize(mainJD)) {
			return std::move(err);
		}
	}

	return std::unique_ptr<MLIRJit>(new MLIRJit(std::move(jit), rawLayer));
}

void MLIRJit::registerSymbols(llvm::function_ref<llvm::orc::SymbolMap(llvm::orc::MangleAndInterner)> symbolMapFn) {
	auto& mainJD = jit_->getMainJITDylib();
	llvm::cantFail(mainJD.define(llvm::orc::absoluteSymbols(
	    symbolMapFn(llvm::orc::MangleAndInterner(mainJD.getExecutionSession(), jit_->getDataLayout())))));
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

void MLIRJit::addEventListener(llvm::JITEventListener* listener) {
	if (listener != nullptr && objectLayer_ != nullptr) {
		objectLayer_->registerJITEventListener(*listener);
	}
}

} // namespace nautilus::compiler::mlir
