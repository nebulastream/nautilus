#include "nautilus/compiler/backends/mlir/ThreadTargetMachineCompiler.hpp"
#include "nautilus/compiler/backends/mlir/MLIRThreadState.hpp"

namespace nautilus::compiler::mlir {

namespace {

// Compiled without RTTI, like LLVM: the base class has no typeinfo to refer to (see CMakeLists.txt).
class ThreadTargetMachineCompiler final : public llvm::orc::IRCompileLayer::IRCompiler {
public:
	ThreadTargetMachineCompiler(llvm::orc::IRSymbolMapper::ManglingOptions options, llvm::CodeGenOptLevel level)
	    : IRCompiler(std::move(options)), level_(level) {
	}

	llvm::Expected<std::unique_ptr<llvm::MemoryBuffer>> operator()(llvm::Module& module) override {
		auto& machine = MLIRThreadState::current().targetMachine("", level_);
		return llvm::orc::SimpleCompiler(machine)(module);
	}

private:
	llvm::CodeGenOptLevel level_;
};

} // namespace

std::unique_ptr<llvm::orc::IRCompileLayer::IRCompiler>
makeThreadTargetMachineCompiler(const llvm::orc::JITTargetMachineBuilder& builder, llvm::CodeGenOptLevel level) {
	return std::make_unique<ThreadTargetMachineCompiler>(
	    llvm::orc::irManglingOptionsFromTargetOptions(builder.getOptions()), level);
}

} // namespace nautilus::compiler::mlir
