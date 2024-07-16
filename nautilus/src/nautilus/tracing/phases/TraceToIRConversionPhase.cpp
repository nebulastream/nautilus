
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/common/traceing.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <cstdint>
#include <vector>

namespace nautilus::tracing {
using namespace compiler::ir;

std::shared_ptr<IRGraph> TraceToIRConversionPhase::apply(std::shared_ptr<ExecutionTrace> trace) {
	auto phaseContext = IRConversionContext(std::move(trace));
	return phaseContext.process();
};

std::shared_ptr<IRGraph> TraceToIRConversionPhase::IRConversionContext::process() {
	auto& rootBlock = trace->getBlocks().front();
	processBlock(0, rootBlock);

	// trace->getBlock(trace->getReturn().front().blockIndex).operations.back();
	auto returnStamp = returnType;
	auto functionOperation =
	    std::make_unique<FunctionOperation>("execute", currentBasicBlocks,
	                                        /*argumentTypes*/ std::vector<Type> {},
	                                        /*arguments*/ std::vector<std::string> {}, returnStamp);

	ir->addRootOperation(std::move(functionOperation));
	return ir;
}

BasicBlock* TraceToIRConversionPhase::IRConversionContext::processBlock(int32_t scope, Block& block) {
	// create new frame and block
	ValueFrame blockFrame;
	std::vector<std::unique_ptr<BasicBlockArgument>> blockArguments;
	for (auto& arg : block.arguments) {
		auto argumentIdentifier = createValueIdentifier(arg);
		// TODO fix arg type
		auto blockArgument = std::make_unique<BasicBlockArgument>(argumentIdentifier, arg.type);
		blockFrame.setValue(argumentIdentifier, blockArgument.get());
		blockArguments.emplace_back(std::move(blockArgument));
	}

	auto operations = std::vector<std::unique_ptr<Operation>> {};
	auto id = std::to_string(block.blockId);
	auto& irBasicBlock = currentBasicBlocks.emplace_back(std::make_unique<BasicBlock>(id, scope,
	                                                                                  /*operations*/ operations,
	                                                                                  /*arguments*/ blockArguments));
	auto irBasicBlockPtr = irBasicBlock.get();

	blockMap[block.blockId] = irBasicBlockPtr;
	for (auto& operation : block.operations) {
		processOperation(scope, blockFrame, block, irBasicBlockPtr, operation);
	}
	return irBasicBlockPtr;
}

void TraceToIRConversionPhase::IRConversionContext::processOperation(int32_t scope, ValueFrame& frame,
                                                                     Block& currentBlock, BasicBlock*& currentIrBlock,
                                                                     TraceOperation& operation) {

	switch (operation.op) {
	case Op::ADD: {
		processAdd(scope, frame, currentIrBlock, operation);
		return;
	};
	case Op::SUB: {
		processSub(scope, frame, currentIrBlock, operation);
		return;
	};
	case Op::DIV: {
		processDiv(scope, frame, currentIrBlock, operation);
		return;
	};
	case Op::MUL: {
		processMul(scope, frame, currentIrBlock, operation);
		return;
	};
	case Op::EQ: {
		processEquals(scope, frame, currentIrBlock, operation, CompareOperation::Comparator::EQ);
		return;
	};
	case Op::LT: {
		processEquals(scope, frame, currentIrBlock, operation, CompareOperation::Comparator::LT);
		return;
	};
	case Op::GT: {
		processEquals(scope, frame, currentIrBlock, operation, CompareOperation::Comparator::GT);
		return;
	};
	case Op::NEQ: {
		processEquals(scope, frame, currentIrBlock, operation, CompareOperation::Comparator::NE);
		return;
	};
	case Op::LTE: {
		processEquals(scope, frame, currentIrBlock, operation, CompareOperation::Comparator::LE);
		return;
	};
	case Op::GTE: {
		processEquals(scope, frame, currentIrBlock, operation, CompareOperation::Comparator::GE);
		return;
	};
	case Op::NEGATE: {
		processNegate(scope, frame, currentIrBlock, operation);
		return;
	};
	case Op::AND: {
		processAnd(scope, frame, currentIrBlock, operation);
		return;
	};
	case Op::OR: {
		processOr(scope, frame, currentIrBlock, operation);
		return;
	};
	case Op::CMP: {
		processCMP(scope, frame, currentBlock, currentIrBlock, operation);
		return;
	};
	case Op::JMP: {
		processJMP(scope, frame, currentIrBlock, operation);
		return;
	};
	case Op::CONST: {
		processConst(scope, frame, currentIrBlock, operation);
		return;
	};
	case Op::ASSIGN:
		break;
	case Op::RETURN: {
		returnType = operation.resultType;
		if (returnType == Type::v) {
			currentIrBlock->addOperation<ReturnOperation>();
		} else {
			auto returnValue = frame.getValue(createValueIdentifier(operation.resultRef));
			currentIrBlock->addOperation<ReturnOperation>(returnValue);
		}

		return;
	};
	case Op::LOAD: {
		processLoad(scope, frame, currentIrBlock, operation);
		return;
	};
	case Op::STORE: {
		processStore(scope, frame, currentIrBlock, operation);
		return;
	};
	case Op::CAST: {
		processCast(scope, frame, currentIrBlock, operation);
		return;
	};
	case Op::CALL:
		processCall(scope, frame, currentIrBlock, operation);
		return;
	case FREE:
		break;
	case NOT:
		break;
	case MOD:
		processMod(scope, frame, currentIrBlock, operation);
		return;
	case LSH:
		processShift(scope, frame, currentIrBlock, operation, compiler::ir::ShiftOperation::LS);
		return;
	case RSH:
		processShift(scope, frame, currentIrBlock, operation, compiler::ir::ShiftOperation::RS);
		return;
	case BOR:
		processBinaryComp(scope, frame, currentIrBlock, operation, compiler::ir::BinaryCompOperation::Type::BOR);
		return;
	case BAND:
		processBinaryComp(scope, frame, currentIrBlock, operation, compiler::ir::BinaryCompOperation::BAND);
		return;
	}

	throw NotImplementedException("Type is not implemented.");
}

void TraceToIRConversionPhase::IRConversionContext::processJMP(int32_t scope, ValueFrame& frame, BasicBlock* block,
                                                               TraceOperation& operation) {
	// NES_DEBUG("current block " << block->getIdentifier() << " " << operation);
	auto blockRef = get<BlockRef>(operation.input[0]);
	BasicBlockInvocation blockInvocation;
	createBlockArguments(frame, blockInvocation, blockRef);

	if (blockMap.contains(blockRef.block)) {
		auto targetBlock = blockMap[blockRef.block];
		block->addNextBlock(targetBlock, blockInvocation.getArguments());
		return;
	}
	auto targetBlock = trace->getBlock(blockRef.block);
	auto resultTargetBlock = processBlock(scope - 1, trace->getBlock(blockRef.block));
	blockMap[blockRef.block] = resultTargetBlock;
	block->addNextBlock(resultTargetBlock, blockInvocation.getArguments());
}

void TraceToIRConversionPhase::IRConversionContext::processCMP(int32_t scope, ValueFrame& frame, Block&,
                                                               BasicBlock* currentIrBlock, TraceOperation& operation) {

	auto valueRef = get<value_ref>(operation.input[0]);
	auto trueCaseBlockRef = get<BlockRef>(operation.input[1]);
	auto falseCaseBlockRef = get<BlockRef>(operation.input[2]);

	//  if (isBlockInLoop(scope, currentBlock.blockId, trueCaseBlockRef.block)) {
	//     NES_DEBUG("1. found loop");
	//} else if (isBlockInLoop(scope, currentBlock.blockId, falseCaseBlockRef.block)) {
	//    NES_DEBUG("2. found loop");
	//} else {
	auto booleanValue = frame.getValue(createValueIdentifier(valueRef));
	auto ifOperation = std::make_unique<IfOperation>(booleanValue);
	auto trueCaseBlock = processBlock(scope + 1, trace->getBlock(trueCaseBlockRef.block));

	ifOperation->getTrueBlockInvocation().setBlock(trueCaseBlock);
	createBlockArguments(frame, ifOperation->getTrueBlockInvocation(), trueCaseBlockRef);

	auto falseCaseBlock = processBlock(scope + 1, trace->getBlock(falseCaseBlockRef.block));
	ifOperation->getFalseBlockInvocation().setBlock(falseCaseBlock);
	createBlockArguments(frame, ifOperation->getFalseBlockInvocation(), falseCaseBlockRef);
	currentIrBlock->addOperation(std::move(ifOperation));
}

std::vector<OperationIdentifier> TraceToIRConversionPhase::IRConversionContext::createBlockArguments(BlockRef val) {
	std::vector<OperationIdentifier> blockArgumentIdentifiers;
	for (auto& arg : val.arguments) {
		blockArgumentIdentifiers.emplace_back(createValueIdentifier(arg));
	}
	return blockArgumentIdentifiers;
}

void TraceToIRConversionPhase::IRConversionContext::createBlockArguments(ValueFrame& frame,
                                                                         BasicBlockInvocation& blockInvocation,
                                                                         BlockRef val) {
	for (auto& arg : val.arguments) {
		auto valueIdentifier = createValueIdentifier(arg);
		blockInvocation.addArgument(frame.getValue(valueIdentifier));
	}
}

OperationIdentifier TraceToIRConversionPhase::IRConversionContext::createValueIdentifier(InputVariant val) {
	if (holds_alternative<value_ref>(val)) {
		auto valueRef = std::get<value_ref>(val);
		return OperationIdentifier(valueRef.ref);
	} else
		return OperationIdentifier(0);
}

void TraceToIRConversionPhase::IRConversionContext::processAdd(int32_t, ValueFrame& frame,
                                                               compiler::ir::BasicBlock* currentBlock,
                                                               TraceOperation& operation) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto addOperation = std::make_unique<AddOperation>(resultIdentifier, leftInput, rightInput);
	frame.setValue(resultIdentifier, addOperation.get());
	currentBlock->addOperation(std::move(addOperation));
}

void TraceToIRConversionPhase::IRConversionContext::processSub(int32_t, ValueFrame& frame,
                                                               compiler::ir::BasicBlock* currentBlock,
                                                               TraceOperation& operation) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto subOperation = currentBlock->addOperation<SubOperation>(resultIdentifier, leftInput, rightInput);
	frame.setValue(resultIdentifier, subOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processMul(int32_t, ValueFrame& frame,
                                                               compiler::ir::BasicBlock* currentBlock,
                                                               TraceOperation& operation) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto mulOperation = currentBlock->addOperation<MulOperation>(resultIdentifier, leftInput, rightInput);
	frame.setValue(resultIdentifier, mulOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processDiv(int32_t, ValueFrame& frame, BasicBlock* currentBlock,
                                                               TraceOperation& operation) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto divOperation = currentBlock->addOperation<DivOperation>(resultIdentifier, leftInput, rightInput);
	frame.setValue(resultIdentifier, divOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processMod(int32_t, ValueFrame& frame, BasicBlock* currentBlock,
                                                               TraceOperation& operation) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto divOperation = currentBlock->addOperation<ModOperation>(resultIdentifier, leftInput, rightInput);
	frame.setValue(resultIdentifier, divOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processBinaryComp(
    int32_t, nautilus::tracing::TraceToIRConversionPhase::ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
    nautilus::tracing::TraceOperation& operation, compiler::ir::BinaryCompOperation::Type type) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto divOperation = currentBlock->addOperation<BinaryCompOperation>(resultIdentifier, leftInput, rightInput, type);
	frame.setValue(resultIdentifier, divOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processShift(
    int32_t, nautilus::tracing::TraceToIRConversionPhase::ValueFrame& frame, compiler::ir::BasicBlock* currentBlock,
    nautilus::tracing::TraceOperation& operation, compiler::ir::ShiftOperation::ShiftType type) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto divOperation = currentBlock->addOperation<ShiftOperation>(resultIdentifier, leftInput, rightInput, type);
	frame.setValue(resultIdentifier, divOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processNegate(int32_t, ValueFrame& frame, BasicBlock* currentBlock,
                                                                  TraceOperation& operation) {
	auto input = frame.getValue(createValueIdentifier(operation.input[0]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto negateOperation = currentBlock->addOperation<NegateOperation>(resultIdentifier, input);
	frame.setValue(resultIdentifier, negateOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processLessThan(int32_t, ValueFrame& frame,
                                                                    BasicBlock* currentBlock,
                                                                    TraceOperation& operation) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto compareOperation = currentBlock->addOperation<CompareOperation>(resultIdentifier, leftInput, rightInput,
	                                                                     CompareOperation::Comparator::LT);
	frame.setValue(resultIdentifier, compareOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processGreaterThan(int32_t, ValueFrame& frame,
                                                                       BasicBlock* currentBlock,
                                                                       TraceOperation& operation) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));

	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto compareOperation = currentBlock->addOperation<CompareOperation>(resultIdentifier, leftInput, rightInput,
	                                                                     CompareOperation::Comparator::GT);
	frame.setValue(resultIdentifier, compareOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processEquals(int32_t, ValueFrame& frame, BasicBlock* currentBlock,
                                                                  TraceOperation& operation,
                                                                  CompareOperation::Comparator comp) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto compareOperation = currentBlock->addOperation<CompareOperation>(resultIdentifier, leftInput, rightInput, comp);
	frame.setValue(resultIdentifier, compareOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processAnd(int32_t, ValueFrame& frame, BasicBlock* currentBlock,
                                                               TraceOperation& operation) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto andOperation = currentBlock->addOperation<AndOperation>(resultIdentifier, leftInput, rightInput);
	frame.setValue(resultIdentifier, andOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processOr(int32_t, ValueFrame& frame, BasicBlock* currentBlock,
                                                              TraceOperation& operation) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto orOperation = currentBlock->addOperation<OrOperation>(resultIdentifier, leftInput, rightInput);
	frame.setValue(resultIdentifier, orOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processLoad(int32_t, ValueFrame& frame, BasicBlock* currentBlock,
                                                                TraceOperation& operation) {
	// TODO add load data type
	// auto constOperation =
	// std::make_shared<LoadOperation>(createValueIdentifier(operation.result),
	//                                                                      createValueIdentifier(operation.input[0]),
	//                                                                      Operation::BasicType::VOID);
	// currentBlock->addOperation(constOperation);
	auto address = frame.getValue(createValueIdentifier(operation.input[0]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto resultType = operation.resultType;
	auto loadOperation = std::make_unique<LoadOperation>(resultIdentifier, address, resultType);
	frame.setValue(resultIdentifier, loadOperation.get());
	currentBlock->addOperation(std::move(loadOperation));
}

void TraceToIRConversionPhase::IRConversionContext::processStore(int32_t, ValueFrame& frame, BasicBlock* currentBlock,
                                                                 TraceOperation& operation) {
	auto address = frame.getValue(createValueIdentifier(operation.resultRef));
	auto value = frame.getValue(createValueIdentifier(operation.input[0]));
	currentBlock->addOperation<StoreOperation>(value, address);
}

void TraceToIRConversionPhase::IRConversionContext::processCall(int32_t, ValueFrame& frame, BasicBlock* currentBlock,
                                                                TraceOperation& operation) {

	auto inputArguments = std::vector<Operation*> {};
	auto functionCallTarget = std::get<FunctionCall>(operation.input[0]);

	for (uint32_t i = 0; i < functionCallTarget.arguments.size(); i++) {
		auto input = frame.getValue(createValueIdentifier(functionCallTarget.arguments[i]));
		inputArguments.emplace_back(input);
	}

	auto resultType = operation.resultType;
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto proxyCallOperation = currentBlock->addOperation<ProxyCallOperation>(
	    functionCallTarget.functionName, functionCallTarget.ptr, resultIdentifier, inputArguments, resultType);
	if (resultType != Type::v) {
		frame.setValue(resultIdentifier, proxyCallOperation);
	}
}

bool TraceToIRConversionPhase::IRConversionContext::isBlockInLoop(uint32_t parentBlockId, uint32_t currentBlockId) {
	if (currentBlockId == parentBlockId) {
		return true;
	}
	if (parentBlockId == 8)
		return false;
	if (currentBlockId == UINT32_MAX) {
		currentBlockId = parentBlockId;
	}
	auto currentBlock = trace->getBlock(currentBlockId);
	auto& terminationOp = currentBlock.operations.back();
	if (terminationOp.op == Op::CMP) {
		auto trueCaseBlockRef = get<BlockRef>(terminationOp.input[0]);
		auto falseCaseBlockRef = get<BlockRef>(terminationOp.input[1]);
		return currentBlock.type == Block::Type::ControlFlowMerge;
		// isBlockInLoop(parentBlockId, trueCaseBlockRef.block) || isBlockInLoop(parentBlockId,
		// falseCaseBlockRef.block);
	} else if (terminationOp.op == Op::JMP) {
		auto target = get<BlockRef>(terminationOp.input[0]);
		return isBlockInLoop(parentBlockId, target.block);
	}
	return false;
}

void TraceToIRConversionPhase::IRConversionContext::processConst(int32_t, TraceToIRConversionPhase::ValueFrame& frame,
                                                                 BasicBlock* currentBlock, TraceOperation& operation) {
	auto constant = &std::get<std::any>(operation.input[0]);
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto resultType = (operation.resultType);
	Operation* constOperation;
	if (constant->type() == typeid(int8_t)) {
		constOperation =
		    currentBlock->addOperation<ConstIntOperation>(resultIdentifier, any_cast<int8_t>(*constant), resultType);
	} else if (constant->type() == typeid(int16_t)) {
		constOperation =
		    currentBlock->addOperation<ConstIntOperation>(resultIdentifier, any_cast<int16_t>(*constant), resultType);
	} else if (constant->type() == typeid(int32_t)) {
		constOperation =
		    currentBlock->addOperation<ConstIntOperation>(resultIdentifier, any_cast<int32_t>(*constant), resultType);
	} else if (constant->type() == typeid(int64_t)) {
		constOperation =
		    currentBlock->addOperation<ConstIntOperation>(resultIdentifier, any_cast<int64_t>(*constant), resultType);
	} else if (constant->type() == typeid(uint8_t)) {
		constOperation =
		    currentBlock->addOperation<ConstIntOperation>(resultIdentifier, any_cast<uint8_t>(*constant), resultType);
	} else if (constant->type() == typeid(uint16_t)) {
		constOperation =
		    currentBlock->addOperation<ConstIntOperation>(resultIdentifier, any_cast<uint16_t>(*constant), resultType);

	} else if (constant->type() == typeid(uint32_t)) {
		constOperation =
		    currentBlock->addOperation<ConstIntOperation>(resultIdentifier, any_cast<uint32_t>(*constant), resultType);

	} else if (constant->type() == typeid(uint64_t)) {
		constOperation =
		    currentBlock->addOperation<ConstIntOperation>(resultIdentifier, any_cast<uint64_t>(*constant), resultType);

	} else if (constant->type() == typeid(float)) {
		constOperation =
		    currentBlock->addOperation<ConstFloatOperation>(resultIdentifier, any_cast<float>(*constant), resultType);

	} else if (constant->type() == typeid(double)) {
		constOperation =
		    currentBlock->addOperation<ConstFloatOperation>(resultIdentifier, any_cast<double>(*constant), resultType);

	} else if (constant->type() == typeid(bool)) {
		constOperation = currentBlock->addOperation<ConstBooleanOperation>(resultIdentifier, any_cast<bool>(*constant));
	} else if (constant->type() == typeid(void*)) {
		constOperation = currentBlock->addOperation<ConstPtrOperation>(resultIdentifier, any_cast<void*>(*constant));
	} else {
		throw NotImplementedException("Not constant implemented.");
	}

	frame.setValue(resultIdentifier, constOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processCast(int32_t, TraceToIRConversionPhase::ValueFrame& frame,
                                                                BasicBlock* currentBlock, TraceOperation& operation) {

	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto input = frame.getValue(createValueIdentifier(operation.input[0]));
	auto castOperation = currentBlock->addOperation<CastOperation>(resultIdentifier, input, operation.resultType);
	frame.setValue(resultIdentifier, castOperation);
}

} // namespace nautilus::tracing
