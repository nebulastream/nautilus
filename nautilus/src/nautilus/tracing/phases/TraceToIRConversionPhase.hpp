
#pragma once

#include "nautilus/JITCompiler.hpp"
#include "nautilus/compiler/Frame.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/tracing/Block.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceOperation.hpp"
#include <memory>
#include <unordered_map>

namespace nautilus::debug {
class DwarfVariableResolver;
} // namespace nautilus::debug

namespace nautilus::tracing {

class TraceModule;
class SourceLocationResolver;

/// Optional resolvers used by @c TraceToIRConversionPhase to bake
/// source-location and variable-name metadata onto the @c IRGraph
/// while the trace's @c TagRecorder is still alive. Either field may
/// be null; passing a null resolver disables that level of resolution.
struct DebugInfoResolvers {
	SourceLocationResolver* sourceLocationResolver = nullptr;
	debug::DwarfVariableResolver* variableResolver = nullptr;

	[[nodiscard]] bool empty() const noexcept {
		return sourceLocationResolver == nullptr && variableResolver == nullptr;
	}
};

/**
 * @brief Translates a trace into a corresponding IR fragment.
 */
class TraceToIRConversionPhase {
public:
	/**
	 * @brief Performs the conversion and returns a IR fragment for the given trace module
	 * @param traceModule Module containing all function traces
	 * @param resolvers   Optional source-location and variable-name resolvers; when
	 *                    non-null, their output is baked into the IR graph's debug
	 *                    side-table so the resulting IR is self-contained.
	 * @return IR graph containing all functions
	 */
	std::shared_ptr<compiler::ir::IRGraph> apply(std::shared_ptr<TraceModule> traceModule,
	                                             const compiler::CompilationUnitID& id = "",
	                                             DebugInfoResolvers resolvers = {});

	/**
	 * @brief Pool-backed variant of @ref apply.  The IR graph's arena is
	 * acquired from @p pool and recycled into it when the IR graph dies,
	 * amortising the per-IRGraph heap allocation across many compiles.
	 */
	std::shared_ptr<compiler::ir::IRGraph> apply(std::shared_ptr<TraceModule> traceModule, common::ArenaPool& pool,
	                                             const compiler::CompilationUnitID& id = "",
	                                             DebugInfoResolvers resolvers = {});

	/**
	 * @brief Performs the conversion and returns a IR fragment for the given trace (legacy method)
	 * @param trace Single execution trace
	 * @return IR graph containing a single function
	 */
	std::shared_ptr<compiler::ir::IRGraph> apply(std::shared_ptr<ExecutionTrace> trace,
	                                             const compiler::CompilationUnitID& id = "",
	                                             DebugInfoResolvers resolvers = {});

	/**
	 * @brief Pool-backed variant of the single-trace @ref apply.
	 */
	std::shared_ptr<compiler::ir::IRGraph> apply(std::shared_ptr<ExecutionTrace> trace, common::ArenaPool& pool,
	                                             const compiler::CompilationUnitID& id = "",
	                                             DebugInfoResolvers resolvers = {});

private:
	using ValueFrame = compiler::Frame<compiler::ir::OperationIdentifier, compiler::ir::Operation*>;

	/**
	 * @brief Internal context object, which maintains statue during IR creation.
	 */
	class IRConversionContext {
	public:
		IRConversionContext(ExecutionTrace* trace, std::shared_ptr<compiler::ir::IRGraph> ir,
		                    const compiler::CompilationUnitID& id, DebugInfoResolvers resolvers = {});

		std::shared_ptr<compiler::ir::IRGraph> process();

		/**
		 * @brief Processes a single function trace and returns a FunctionOperation
		 * @param functionName The name of the function being processed
		 * @param attributes Generic key-value attributes to attach to the FunctionOperation
		 * @return Arena-allocated pointer to the generated FunctionOperation
		 */
		compiler::ir::FunctionOperation*
		processFunction(const std::string& functionName,
		                const std::unordered_map<std::string, std::string>& attributes = {});

	private:
		compiler::ir::BasicBlock* processBlock(Block& block);

		void processOperation(ValueFrame& frame, Block& currentBlock, compiler::ir::BasicBlock*& currentIRBlock,
		                      TraceOperation& operation);

		void processJMP(ValueFrame& frame, compiler::ir::BasicBlock* block, TraceOperation& operation);

		void processCMP(ValueFrame& frame, Block& currentBlock, compiler::ir::BasicBlock* currentIRBlock,
		                TraceOperation& operation);

		void processLogicalComperator(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                              TraceOperation& operation, compiler::ir::CompareOperation::Comparator comp);

		void processNegate(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);
		void processNot(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void processShift(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation,
		                  compiler::ir::ShiftOperation::ShiftType type);

		void processBinaryComp(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation,
		                       compiler::ir::BinaryCompOperation::Type type);

		void processLoad(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void processStore(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);
		void processAlloca(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void processCall(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void processIndirectCall(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void processFuncAddr(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void processConst(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void processCast(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void createBlockArguments(ValueFrame& frame, compiler::ir::BasicBlockInvocation& blockInvocation,
		                          const BlockRef& val);

		template <typename OpType>
		void processBinaryOperator(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                           TraceOperation& operation);
		template <typename OpType>
		void processUnaryOperator(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);
		template <typename OpType>
		void processTernaryOperator(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                            TraceOperation& operation);

		/// Resolves the trace operation's tag chain to a SourceFrame
		/// stack and (optionally) a DWARF-recovered variable name, then
		/// stashes the result on the IR graph's debug side-table keyed
		/// by @p irOp's identifier. No-op when no resolvers were
		/// supplied. Called immediately after each IR op is created so
		/// the live @c TagRecorder is still in scope.
		void bakeDebugInfo(const compiler::ir::Operation* irOp, const TraceOperation& traceOp);

	private:
		ExecutionTrace* trace;
		Type returnType;
		std::shared_ptr<compiler::ir::IRGraph> ir;
		std::unordered_map<uint32_t, compiler::ir::BasicBlock*> blockMap;
		std::vector<compiler::ir::BasicBlock*> currentBasicBlocks;
		DebugInfoResolvers resolvers_;
	};
};

} // namespace nautilus::tracing
