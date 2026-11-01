#include "nautilus/compiler/backends/mlir/MLIRThreadState.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/ControlFlow/IR/ControlFlow.h>
#include <mlir/Dialect/Func/Extensions/AllExtensions.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/Transforms/InlinerInterfaceImpl.h>
#include <mlir/Dialect/Math/IR/Math.h>
#include <mlir/IR/DialectRegistry.h>
#include <mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h>
#include <mutex>

namespace nautilus::compiler::mlir {

const llvm::orc::JITTargetMachineBuilder& MLIRThreadState::hostBuilder() {
	// detecting the host (CPU name and feature string) is the same for every thread, so it happens once per process
	static const llvm::orc::JITTargetMachineBuilder builder = [] {
		llvm::InitializeNativeTarget();
		llvm::InitializeNativeTargetAsmPrinter();
		auto detected = llvm::orc::JITTargetMachineBuilder::detectHost();
		if (!detected) {
			throw RuntimeException("mlir: the host cannot be detected: " + llvm::toString(detected.takeError()));
		}
		return std::move(*detected);
	}();
	return builder;
}

MLIRThreadState& MLIRThreadState::current() {
	thread_local MLIRThreadState state;
	return state;
}

::mlir::MLIRContext& MLIRThreadState::context(bool multithreading) {
	if (context_ == nullptr) {
		::mlir::DialectRegistry registry;
		registry.insert<::mlir::arith::ArithDialect, ::mlir::cf::ControlFlowDialect, ::mlir::math::MathDialect,
		                ::mlir::LLVM::LLVMDialect, ::mlir::func::FuncDialect>();
		::mlir::func::registerAllExtensions(registry);
		registerBuiltinDialectTranslation(registry);
		registerLLVMDialectTranslation(registry);
		::mlir::LLVM::registerInlinerInterface(registry);
		context_ = std::make_unique<::mlir::MLIRContext>(registry, ::mlir::MLIRContext::Threading::DISABLED);
		context_->loadAllAvailableDialects();
		multithreading_ = false;
		++contextsCreated_;
	}
	if (multithreading != multithreading_) {
		context_->enableMultithreading(multithreading);
		multithreading_ = multithreading;
	}
	return *context_;
}

llvm::TargetMachine& MLIRThreadState::targetMachine(const std::string& pinnedCpu, llvm::CodeGenOptLevel level) {
	auto& slot = machines_[{pinnedCpu, static_cast<int>(level)}];
	if (slot == nullptr) {
		auto builder = MLIRThreadState::hostBuilder();
		if (!pinnedCpu.empty()) {
			builder.setCPU(pinnedCpu);
			// an explicit baseline feature set (rather than the host's) keeps the decisions of the target-dependent
			// transforms deterministic
			builder.setFeatures("+64bit,+sse2");
		}
		builder.setCodeGenOptLevel(level);
		auto machine = builder.createTargetMachine();
		if (!machine) {
			throw RuntimeException("mlir: no target machine for the host: " + llvm::toString(machine.takeError()));
		}
		slot = std::move(*machine);
		++targetMachinesCreated_;
	}
	return *slot;
}

} // namespace nautilus::compiler::mlir
