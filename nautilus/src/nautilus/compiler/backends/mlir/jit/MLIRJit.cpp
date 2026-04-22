
#include "nautilus/compiler/backends/mlir/jit/MLIRJit.hpp"
#include <mlir/ExecutionEngine/ExecutionEngine.h>

namespace nautilus::compiler::mlir {

MLIRJit::MLIRJit(std::unique_ptr<::mlir::ExecutionEngine> engine) : engine_(std::move(engine)) {
}

MLIRJit::~MLIRJit() = default;
MLIRJit::MLIRJit(MLIRJit&&) noexcept = default;
MLIRJit& MLIRJit::operator=(MLIRJit&&) noexcept = default;

llvm::Expected<std::unique_ptr<MLIRJit>> MLIRJit::create(::mlir::ModuleOp module, const Options& options) {
	::mlir::ExecutionEngineOptions engineOptions;
	engineOptions.jitCodeGenOptLevel = options.codeGenOptLevel;
	engineOptions.transformer = options.transformer;

	auto maybeEngine = ::mlir::ExecutionEngine::create(module, engineOptions);
	if (!maybeEngine) {
		return maybeEngine.takeError();
	}
	return std::unique_ptr<MLIRJit>(new MLIRJit(std::move(*maybeEngine)));
}

void MLIRJit::registerSymbols(llvm::function_ref<llvm::orc::SymbolMap(llvm::orc::MangleAndInterner)> symbolMapFn) {
	engine_->registerSymbols(symbolMapFn);
}

llvm::Expected<void*> MLIRJit::lookup(llvm::StringRef name) {
	return engine_->lookup(name);
}

llvm::Expected<void (*)(void**)> MLIRJit::lookupPacked(llvm::StringRef name) {
	return engine_->lookupPacked(name);
}

} // namespace nautilus::compiler::mlir
