
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceOperation.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include <cassert>
#include <vector>

namespace nautilus::tracing {
using namespace compiler::ir;

OperationIdentifier createValueIdentifier(const TypedValueRef& val) {
	return {val.ref};
}

OperationIdentifier createValueIdentifier(const InputVariant& val) {
	if (const auto* valRef = std::get_if<TypedValueRef>(&val)) {
		return {valRef->ref};
	}
	throw NotImplementedException("wrong input variant");
}

std::shared_ptr<IRGraph> TraceToIRConversionPhase::apply(std::shared_ptr<TraceModule> traceModule,
                                                         const compiler::CompilationUnitID& id) {
	auto ir = std::make_shared<compiler::ir::IRGraph>(id);

	// Process all functions in sorted order for deterministic IR output.
	for (const auto& functionName : traceModule->getFunctionNames()) {
		auto* trace = traceModule->getFunction(functionName);
		auto phaseContext = IRConversionContext(trace, ir, id);
		ir->addFunctionOperation(phaseContext.processFunction(functionName));
	}

	return ir;
}

std::shared_ptr<IRGraph> TraceToIRConversionPhase::apply(std::shared_ptr<TraceModule> traceModule,
                                                         common::ArenaPool& pool,
                                                         const compiler::CompilationUnitID& id) {
	auto ir = std::make_shared<compiler::ir::IRGraph>(pool.acquire(), id);

	for (const auto& functionName : traceModule->getFunctionNames()) {
		auto* trace = traceModule->getFunction(functionName);
		auto phaseContext = IRConversionContext(trace, ir, id);
		ir->addFunctionOperation(phaseContext.processFunction(functionName));
	}

	return ir;
}

std::shared_ptr<IRGraph> TraceToIRConversionPhase::apply(std::shared_ptr<ExecutionTrace> trace,
                                                         const compiler::CompilationUnitID& id) {
	auto ir = std::make_shared<compiler::ir::IRGraph>(id);
	auto phaseContext = IRConversionContext(trace.get(), ir, id);
	return phaseContext.process();
}

std::shared_ptr<IRGraph> TraceToIRConversionPhase::apply(std::shared_ptr<ExecutionTrace> trace, common::ArenaPool& pool,
                                                         const compiler::CompilationUnitID& id) {
	auto ir = std::make_shared<compiler::ir::IRGraph>(pool.acquire(), id);
	auto phaseContext = IRConversionContext(trace.get(), ir, id);
	return phaseContext.process();
}

TraceToIRConversionPhase::IRConversionContext::IRConversionContext(ExecutionTrace* trace,
                                                                   std::shared_ptr<compiler::ir::IRGraph> ir,
                                                                   const compiler::CompilationUnitID&)
    : trace(trace), ir(std::move(ir)) {
}

std::shared_ptr<IRGraph> TraceToIRConversionPhase::IRConversionContext::process() {
	processBlock(*trace->getBlocks().front());
	auto* functionOperation = ir->getArena().create<FunctionOperation>(
	    "execute", std::move(currentBasicBlocks), std::vector<Type> {}, std::vector<std::string> {}, returnType);
	ir->addFunctionOperation(functionOperation);
	return ir;
}

FunctionOperation* TraceToIRConversionPhase::IRConversionContext::processFunction(const std::string& functionName) {
	// Clear state for this function
	currentBasicBlocks.clear();
	blockMap.clear();
	returnType = Type::v;

	// Process all blocks starting from the first block
	processBlock(*trace->getBlocks().front());

	// Create and return the function operation
	return ir->getArena().create<FunctionOperation>(functionName, std::move(currentBasicBlocks), std::vector<Type> {},
	                                                std::vector<std::string> {}, returnType);
}

BasicBlock* TraceToIRConversionPhase::IRConversionContext::processBlock(Block& block) {
	// create new frame and block
	auto& arena = ir->getArena();
	ValueFrame blockFrame;
	std::vector<BasicBlockArgument*> blockArguments;
	blockArguments.reserve(block.arguments.size());
	for (auto& arg : block.arguments) {
		auto argumentIdentifier = createValueIdentifier(arg);
		auto* blockArgument = arena.create<BasicBlockArgument>(argumentIdentifier, arg.type);
		blockFrame.setValue(argumentIdentifier, blockArgument);
		blockArguments.emplace_back(blockArgument);
	}
	auto* irBasicBlockPtr = arena.create<BasicBlock>(arena, block.blockId, std::move(blockArguments));
	currentBasicBlocks.emplace_back(irBasicBlockPtr);

	blockMap[block.blockId] = irBasicBlockPtr;
	for (auto* operation : block.operations) {
		processOperation(blockFrame, block, irBasicBlockPtr, *operation);
	}
	return irBasicBlockPtr;
}

void TraceToIRConversionPhase::IRConversionContext::processOperation(ValueFrame& frame, Block& currentBlock,
                                                                     BasicBlock*& currentIrBlock,
                                                                     TraceOperation& operation) {

	switch (operation.op) {
	case Op::ADD: {
		processBinaryOperator<AddOperation>(frame, currentIrBlock, operation);
		return;
	}
	case Op::SUB: {
		processBinaryOperator<SubOperation>(frame, currentIrBlock, operation);
		return;
	}
	case Op::DIV: {
		processBinaryOperator<DivOperation>(frame, currentIrBlock, operation);
		return;
	}
	case Op::MUL: {
		processBinaryOperator<MulOperation>(frame, currentIrBlock, operation);
		return;
	}
	case Op::MOD: {
		processBinaryOperator<ModOperation>(frame, currentIrBlock, operation);
		return;
	}
	case Op::EQ: {
		processLogicalComperator(frame, currentIrBlock, operation, CompareOperation::Comparator::EQ);
		return;
	}
	case Op::LT: {
		processLogicalComperator(frame, currentIrBlock, operation, CompareOperation::Comparator::LT);
		return;
	}
	case Op::GT: {
		processLogicalComperator(frame, currentIrBlock, operation, CompareOperation::Comparator::GT);
		return;
	}
	case Op::NEQ: {
		processLogicalComperator(frame, currentIrBlock, operation, CompareOperation::Comparator::NE);
		return;
	}
	case Op::LTE: {
		processLogicalComperator(frame, currentIrBlock, operation, CompareOperation::Comparator::LE);
		return;
	}
	case Op::GTE: {
		processLogicalComperator(frame, currentIrBlock, operation, CompareOperation::Comparator::GE);
		return;
	}
	case Op::NEGATE: {
		processUnaryOperator<NegateOperation>(frame, currentIrBlock, operation);
		return;
	}
	case Op::NOT: {
		processUnaryOperator<NotOperation>(frame, currentIrBlock, operation);
		return;
	}
	case Op::AND: {
		processBinaryOperator<AndOperation>(frame, currentIrBlock, operation);
		return;
	}
	case Op::OR: {
		processBinaryOperator<OrOperation>(frame, currentIrBlock, operation);
		return;
	}
	case Op::CMP: {
		processCMP(frame, currentBlock, currentIrBlock, operation);
		return;
	}
	case Op::JMP: {
		processJMP(frame, currentIrBlock, operation);
		return;
	}
	case Op::CONST: {
		processConst(frame, currentIrBlock, operation);
		return;
	}
	case Op::RETURN: {
		if (operation.input.empty()) {
			currentIrBlock->addOperation<ReturnOperation>();
			returnType = Type::v;
		} else {
			auto returnValue = frame.getValue(createValueIdentifier(operation.input[0]));
			currentIrBlock->addOperation<ReturnOperation>(returnValue);
			returnType = returnValue->getStamp();
		}
		return;
	}
	case Op::LOAD: {
		processLoad(frame, currentIrBlock, operation);
		return;
	};
	case Op::STORE: {
		processStore(frame, currentIrBlock, operation);
		return;
	};
	case Op::CAST: {
		processCast(frame, currentIrBlock, operation);
		return;
	};
	case Op::CALL:
		processCall(frame, currentIrBlock, operation);
		return;
	case Op::INDIRECT_CALL:
		processIndirectCall(frame, currentIrBlock, operation);
		return;
	case LSH:
		processShift(frame, currentIrBlock, operation, compiler::ir::ShiftOperation::LS);
		return;
	case RSH:
		processShift(frame, currentIrBlock, operation, compiler::ir::ShiftOperation::RS);
		return;
	case BOR:
		processBinaryComp(frame, currentIrBlock, operation, compiler::ir::BinaryCompOperation::Type::BOR);
		return;
	case BAND:
		processBinaryComp(frame, currentIrBlock, operation, compiler::ir::BinaryCompOperation::BAND);
		return;
	case BXOR:
		processBinaryComp(frame, currentIrBlock, operation, compiler::ir::BinaryCompOperation::XOR);
		return;
	case SELECT:
		processTernaryOperator<SelectOperation>(frame, currentIrBlock, operation);
		return;
	case ALLOCA:
		processAlloca(frame, currentIrBlock, operation);
		return;
	case FUNC_ADDR:
		processFuncAddr(frame, currentIrBlock, operation);
		return;
	default: {
		throw NotImplementedException("Operation type is not implemented.");
	}
	}
}

template <typename OpType>
void TraceToIRConversionPhase::IRConversionContext::processBinaryOperator(ValueFrame& frame,
                                                                          compiler::ir::BasicBlock* currentBlock,
                                                                          TraceOperation& op) {
	auto leftInput = frame.getValue(createValueIdentifier(op.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(op.input[1]));
	auto resultIdentifier = createValueIdentifier(op.resultRef);
	auto operation = currentBlock->addOperation<OpType>(resultIdentifier, leftInput, rightInput);
	frame.setValue(resultIdentifier, operation);
}

template <typename OpType>
void TraceToIRConversionPhase::IRConversionContext::processUnaryOperator(ValueFrame& frame,
                                                                         compiler::ir::BasicBlock* currentBlock,
                                                                         TraceOperation& op) {
	auto input = frame.getValue(createValueIdentifier(op.input[0]));
	auto resultIdentifier = createValueIdentifier(op.resultRef);
	auto operation = currentBlock->addOperation<OpType>(resultIdentifier, input);
	frame.setValue(resultIdentifier, operation);
}

template <typename OpType>
void TraceToIRConversionPhase::IRConversionContext::processTernaryOperator(ValueFrame& frame,
                                                                           compiler::ir::BasicBlock* currentBlock,
                                                                           TraceOperation& op) {
	auto firstInput = frame.getValue(createValueIdentifier(op.input[0]));
	auto secondInput = frame.getValue(createValueIdentifier(op.input[1]));
	auto thirdInput = frame.getValue(createValueIdentifier(op.input[2]));
	auto resultIdentifier = createValueIdentifier(op.resultRef);
	auto operation =
	    currentBlock->addOperation<OpType>(resultIdentifier, firstInput, secondInput, thirdInput, op.resultRef.type);
	frame.setValue(resultIdentifier, operation);
}

void TraceToIRConversionPhase::IRConversionContext::processJMP(ValueFrame& frame, BasicBlock* block,
                                                               TraceOperation& operation) {
	const BlockRef& blockRef = *get<BlockRef*>(operation.input[0]);
	BasicBlockInvocation blockInvocation;
	createBlockArguments(frame, blockInvocation, blockRef);

	if (blockMap.contains(blockRef.block)) {
		auto targetBlock = blockMap[blockRef.block];
		block->addNextBlock(targetBlock, blockInvocation.getArguments());
		return;
	}
	auto targetBlock = trace->getBlock(blockRef.block);
	auto resultTargetBlock = processBlock(trace->getBlock(blockRef.block));
	blockMap[blockRef.block] = resultTargetBlock;
	block->addNextBlock(resultTargetBlock, blockInvocation.getArguments());
}

void TraceToIRConversionPhase::IRConversionContext::processCMP(ValueFrame& frame, Block&, BasicBlock* currentIrBlock,
                                                               TraceOperation& operation) {
	assert(operation.input.size() == 4);
	auto valueRef = get<TypedValueRef>(operation.input[0]);
	const BlockRef& trueCaseBlockRef = *get<BlockRef*>(operation.input[1]);
	const BlockRef& falseCaseBlockRef = *get<BlockRef*>(operation.input[2]);
	auto probability = get<BranchProbability>(operation.input[3]);

	auto booleanValue = frame.getValue(createValueIdentifier(valueRef));
	auto& arena = ir->getArena();
	auto* ifOperation = arena.create<IfOperation>(arena, booleanValue, probability);
	auto trueCaseBlock = processBlock(trace->getBlock(trueCaseBlockRef.block));

	ifOperation->getTrueBlockInvocation().setBlock(trueCaseBlock);
	createBlockArguments(frame, ifOperation->getTrueBlockInvocation(), trueCaseBlockRef);

	auto falseCaseBlock = processBlock(trace->getBlock(falseCaseBlockRef.block));
	ifOperation->getFalseBlockInvocation().setBlock(falseCaseBlock);
	createBlockArguments(frame, ifOperation->getFalseBlockInvocation(), falseCaseBlockRef);
	currentIrBlock->addOperation(ifOperation);
}

void TraceToIRConversionPhase::IRConversionContext::createBlockArguments(ValueFrame& frame,
                                                                         BasicBlockInvocation& blockInvocation,
                                                                         const BlockRef& val) {
	auto& arena = ir->getArena();
	for (const auto& arg : val.arguments) {
		auto valueIdentifier = createValueIdentifier(arg);
		blockInvocation.addArgument(arena, frame.getValue(valueIdentifier));
	}
}

void TraceToIRConversionPhase::IRConversionContext::processBinaryComp(ValueFrame& frame,
                                                                      compiler::ir::BasicBlock* currentBlock,
                                                                      nautilus::tracing::TraceOperation& operation,
                                                                      compiler::ir::BinaryCompOperation::Type type) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto divOperation = currentBlock->addOperation<BinaryCompOperation>(resultIdentifier, leftInput, rightInput, type);
	frame.setValue(resultIdentifier, divOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processShift(ValueFrame& frame,
                                                                 compiler::ir::BasicBlock* currentBlock,
                                                                 nautilus::tracing::TraceOperation& operation,
                                                                 compiler::ir::ShiftOperation::ShiftType type) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto divOperation = currentBlock->addOperation<ShiftOperation>(resultIdentifier, leftInput, rightInput, type);
	frame.setValue(resultIdentifier, divOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processLogicalComperator(ValueFrame& frame,
                                                                             BasicBlock* currentBlock,
                                                                             TraceOperation& operation,
                                                                             CompareOperation::Comparator comp) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto compareOperation = currentBlock->addOperation<CompareOperation>(resultIdentifier, leftInput, rightInput, comp);
	frame.setValue(resultIdentifier, compareOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processLoad(ValueFrame& frame, BasicBlock* currentBlock,
                                                                TraceOperation& operation) {
	auto address = frame.getValue(createValueIdentifier(operation.input[0]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto resultType = operation.resultType;
	auto& arena = ir->getArena();
	auto* loadOperation = arena.create<LoadOperation>(arena, resultIdentifier, address, resultType);
	frame.setValue(resultIdentifier, loadOperation);
	currentBlock->addOperation(loadOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processStore(ValueFrame& frame, BasicBlock* currentBlock,
                                                                 TraceOperation& operation) {
	auto address = frame.getValue(createValueIdentifier(operation.input[0]));
	auto value = frame.getValue(createValueIdentifier(operation.input[1]));
	currentBlock->addOperation<StoreOperation>(value, address);
}

void TraceToIRConversionPhase::IRConversionContext::processCall(ValueFrame& frame, BasicBlock* currentBlock,
                                                                TraceOperation& operation) {
	const FunctionCall& functionCallTarget = *std::get<FunctionCall*>(operation.input[0]);
	auto inputArguments = std::vector<Operation*> {};
	for (const auto& argument : functionCallTarget.arguments) {
		auto input = frame.getValue(createValueIdentifier(argument));
		inputArguments.emplace_back(input);
	}

	auto resultType = operation.resultType;
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto proxyCallOperation = currentBlock->addOperation<ProxyCallOperation>(
	    functionCallTarget.mangledName, functionCallTarget.functionName, functionCallTarget.ptr, resultIdentifier,
	    inputArguments, resultType, functionCallTarget.fnAttrs);
	if (resultType != Type::v) {
		frame.setValue(resultIdentifier, proxyCallOperation);
	}
}

void TraceToIRConversionPhase::IRConversionContext::processIndirectCall(ValueFrame& frame, BasicBlock* currentBlock,
                                                                        TraceOperation& operation) {
	const IndirectFunctionCall& indirectCall = *std::get<IndirectFunctionCall*>(operation.input[0]);
	auto fnPtrOperand = frame.getValue(createValueIdentifier(indirectCall.fnPtr));
	auto inputArguments = std::vector<Operation*> {};
	for (const auto& argument : indirectCall.arguments) {
		inputArguments.emplace_back(frame.getValue(createValueIdentifier(argument)));
	}
	auto resultType = operation.resultType;
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto indirectCallOp = currentBlock->addOperation<IndirectCallOperation>(
	    resultIdentifier, fnPtrOperand, inputArguments, resultType, indirectCall.fnAttrs);
	if (resultType != Type::v) {
		frame.setValue(resultIdentifier, indirectCallOp);
	}
}

void TraceToIRConversionPhase::IRConversionContext::processFuncAddr(ValueFrame& frame, BasicBlock* currentBlock,
                                                                    TraceOperation& operation) {
	const FunctionCall& functionCallTarget = *std::get<FunctionCall*>(operation.input[0]);
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto funcAddrOp = currentBlock->addOperation<FunctionAddressOfOperation>(
	    functionCallTarget.mangledName, functionCallTarget.functionName, functionCallTarget.ptr, resultIdentifier);
	frame.setValue(resultIdentifier, funcAddrOp);
}

void TraceToIRConversionPhase::IRConversionContext::processConst(ValueFrame& frame, BasicBlock* currentBlock,
                                                                 TraceOperation& operation) {
	auto constant = std::get<ConstantLiteral>(operation.input[0]);
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto resultType = operation.resultType;
	Operation* constOperation;
	std::visit(
	    [&](auto&& value) {
		    using T = std::decay_t<decltype(value)>;
		    if constexpr (std::is_same_v<T, bool>) {
			    constOperation = currentBlock->addOperation<ConstBooleanOperation>(resultIdentifier, value);
		    } else if constexpr (std::is_integral_v<T>) {
			    constOperation = currentBlock->addOperation<ConstIntOperation>(resultIdentifier, value, resultType);
		    } else if constexpr (std::is_floating_point_v<T>) {
			    constOperation = currentBlock->addOperation<ConstFloatOperation>(resultIdentifier, value, resultType);
		    } else if constexpr (std::is_pointer_v<T>) {
			    constOperation = currentBlock->addOperation<ConstPtrOperation>(resultIdentifier, value);
		    } else {
			    // static_assert(false, "non-exhaustive visitor!");
		    }
	    },
	    constant);

	frame.setValue(resultIdentifier, constOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processCast(ValueFrame& frame, BasicBlock* currentBlock,
                                                                TraceOperation& operation) {
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto input = frame.getValue(createValueIdentifier(operation.input[0]));
	auto castOperation = currentBlock->addOperation<CastOperation>(resultIdentifier, input, operation.resultType);
	frame.setValue(resultIdentifier, castOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processAlloca(ValueFrame& frame, BasicBlock* currentBlock,
                                                                  TraceOperation& operation) {
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	AllocSize allocationSize = std::get<AllocSize>(operation.input[0]);
	auto allocaOperation = currentBlock->addOperation<AllocaOperation>(resultIdentifier, allocationSize);
	frame.setValue(resultIdentifier, allocaOperation);
}

} // namespace nautilus::tracing
