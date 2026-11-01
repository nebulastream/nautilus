#pragma once

#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Target/TargetMachine.h>
#include <map>
#include <memory>
#include <mlir/IR/MLIRContext.h>
#include <string>
#include <utility>

namespace nautilus::compiler::mlir {

/// What a thread that compiles with MLIR keeps from one compile to the next (M4.5, #561): the `MLIRContext` with its
/// dialect registry, and the `TargetMachine` of each (CPU, optimization level) it asked for. Building them is most of
/// the fixed cost of a small compile (10.2 ms against 4.9 ms per TPC-H kernel, 05-backends.md §2), so a worker that
/// promotes many functions pays it once. The state belongs to the thread and is never shared: neither the context nor a
/// target machine may be used by two threads at once.
class MLIRThreadState {
public:
	/// The state of the calling thread.
	static MLIRThreadState& current();

	/// The builder for the host (CPU name and features detected once per process).
	static const llvm::orc::JITTargetMachineBuilder& hostBuilder();

	/// The context, with every dialect of the backend loaded and the LLVM translations registered. Without
	/// `multithreading` it has no thread pool: a compile lowers one module, so a pool per worker would only cost
	/// threads. Unused ops, types and attributes of earlier compiles stay interned in it.
	::mlir::MLIRContext& context(bool multithreading);

	/// The target machine for the host, or for `pinnedCpu` when it is not empty (a fixed baseline feature set makes the
	/// optimizer's decisions independent of the machine).
	llvm::TargetMachine& targetMachine(const std::string& pinnedCpu, llvm::CodeGenOptLevel level);

	/// How often this thread built a context and a target machine (the rest were reuses).
	[[nodiscard]] uint32_t contextsCreated() const {
		return contextsCreated_;
	}
	[[nodiscard]] uint32_t targetMachinesCreated() const {
		return targetMachinesCreated_;
	}

private:
	MLIRThreadState() = default;

	std::unique_ptr<::mlir::MLIRContext> context_;
	bool multithreading_ = false;
	std::map<std::pair<std::string, int>, std::unique_ptr<llvm::TargetMachine>> machines_;
	uint32_t contextsCreated_ = 0;
	uint32_t targetMachinesCreated_ = 0;
};

} // namespace nautilus::compiler::mlir
