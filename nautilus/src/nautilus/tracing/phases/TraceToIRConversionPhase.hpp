
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

namespace nautilus::tracing {

/**
 * @brief Translates a trace into a corresponding IR fragment.
 */
class TraceToIRConversionPhase {
public:
	/**
	 * @brief Performs the conversion and returns a IR fragment for the given trace
	 * @param trace
	 * @return IR
	 */
	std::shared_ptr<compiler::ir::IRGraph> apply(std::shared_ptr<ExecutionTrace> trace, const compiler::CompilationUnitID& id = "");

private:
	using ValueFrame = compiler::Frame<compiler::ir::OperationIdentifier, compiler::ir::Operation*>;

	/**
	 * @brief Internal context object, which maintains statue during IR creation.
	 */
	class IRConversionContext {
	public:
		IRConversionContext(std::shared_ptr<ExecutionTrace> trace, const compiler::CompilationUnitID& id) : trace(trace), ir(std::make_shared<compiler::ir::IRGraph>(id)) {
		}

		std::shared_ptr<compiler::ir::IRGraph> process();

	private:
		compiler::ir::BasicBlock* processBlock(Block& block);

		void processOperation(ValueFrame& frame, Block& currentBlock, compiler::ir::BasicBlock*& currentIRBlock, TraceOperation& operation);

		void processJMP(ValueFrame& frame, compiler::ir::BasicBlock* block, TraceOperation& operation);

		void processCMP(ValueFrame& frame, Block& currentBlock, compiler::ir::BasicBlock* currentIRBlock, TraceOperation& operation);

		void processLogicalComperator(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation, compiler::ir::CompareOperation::Comparator comp);

		void processNegate(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);
		void processNot(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void processShift(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation, compiler::ir::ShiftOperation::ShiftType type);

		void processBinaryComp(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation, compiler::ir::BinaryCompOperation::Type type);

		void processLoad(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void processStore(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void processCall(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void processConst(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void processCast(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

		void createBlockArguments(ValueFrame& frame, compiler::ir::BasicBlockInvocation& blockInvocation, BlockRef val);

		template <typename OpType>
		void processBinaryOperator(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);
		template <typename OpType>
		void processUnaryOperator(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& operation);

	private:
		std::shared_ptr<ExecutionTrace> trace;
		Type returnType;
		std::shared_ptr<compiler::ir::IRGraph> ir;
		std::unordered_map<uint32_t, compiler::ir::BasicBlock*> blockMap;
		std::vector<std::unique_ptr<compiler::ir::BasicBlock>> currentBasicBlocks;
	};
};

} // namespace nautilus::tracing
