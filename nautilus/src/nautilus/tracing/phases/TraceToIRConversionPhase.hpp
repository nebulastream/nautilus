
#pragma once

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
	std::shared_ptr<compiler::ir::IRGraph> apply(std::shared_ptr<ExecutionTrace> trace);

private:
	using ValueFrame = compiler::Frame<compiler::ir::OperationIdentifier, compiler::ir::Operation*>;

	/**
	 * @brief Internal context object, which maintains statue during IR creation.
	 */
	class IRConversionContext {
	public:
		IRConversionContext(std::shared_ptr<ExecutionTrace> trace)
		    : trace(trace), ir(std::make_shared<compiler::ir::IRGraph>()) {};

		std::shared_ptr<compiler::ir::IRGraph> process();

	private:
		compiler::ir::BasicBlock* processBlock(int32_t scope, Block& block);

		void processOperation(int32_t scope, ValueFrame& frame, Block& currentBlock,
		                      compiler::ir::BasicBlock*& currentIRBlock, TraceOperation& operation);

		void processJMP(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* block, TraceOperation& operation);

		void processCMP(int32_t scope, ValueFrame& frame, Block& currentBlock, compiler::ir::BasicBlock* currentIRBlock,
		                TraceOperation& operation);

		void processAdd(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                TraceOperation& operation);

		void processSub(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                TraceOperation& operation);

		void processMul(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                TraceOperation& operation);

		void processDiv(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                TraceOperation& operation);

		void processMod(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                TraceOperation& operation);

		void processEquals(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                   TraceOperation& operation, compiler::ir::CompareOperation::Comparator comp);

		void processLessThan(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                     TraceOperation& operation);

		void processGreaterThan(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                        TraceOperation& operation);

		void processNegate(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                   TraceOperation& operation);
		void processNot(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                TraceOperation& operation);

		void processAnd(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                TraceOperation& operation);

		void processShift(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                  TraceOperation& operation, compiler::ir::ShiftOperation::ShiftType type);

		void processBinaryComp(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                       TraceOperation& operation, compiler::ir::BinaryCompOperation::Type type);

		void processOr(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		               TraceOperation& operation);

		void processLoad(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                 TraceOperation& operation);

		void processStore(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                  TraceOperation& operation);

		void processCall(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                 TraceOperation& operation);

		void processConst(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                  TraceOperation& operation);

		void processCast(int32_t scope, ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
		                 TraceOperation& operation);

		bool isBlockInLoop(uint32_t parentBlock, uint32_t currentBlock);

		std::vector<compiler::ir::OperationIdentifier> createBlockArguments(BlockRef val);

		void createBlockArguments(ValueFrame& frame, compiler::ir::BasicBlockInvocation& blockInvocation, BlockRef val);

		compiler::ir::OperationIdentifier createValueIdentifier(InputVariant val);

	private:
		std::shared_ptr<ExecutionTrace> trace;
		Type returnType;
		std::shared_ptr<compiler::ir::IRGraph> ir;
		std::unordered_map<uint32_t, compiler::ir::BasicBlock*> blockMap;
		std::vector<std::unique_ptr<compiler::ir::BasicBlock>> currentBasicBlocks;
	};
};

} // namespace nautilus::tracing
