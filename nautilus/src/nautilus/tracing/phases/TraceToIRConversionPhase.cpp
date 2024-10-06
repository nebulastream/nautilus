
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include <vector>

namespace nautilus::tracing {
using namespace compiler::ir;

OperationIdentifier createValueIdentifier(TypedValueRef& val) {
	return {val.ref};
}

OperationIdentifier createValueIdentifier(InputVariant& val) {
	if (auto* valRef = std::get_if<TypedValueRef>(&val)) {
		return {valRef->ref};
	}
	throw NotImplementedException("wrong input variant");
}

std::shared_ptr<IRGraph> TraceToIRConversionPhase::apply(std::shared_ptr<ExecutionTrace> trace, const compiler::CompilationUnitID& id) {
	auto phaseContext = IRConversionContext(std::move(trace), id);
	return phaseContext.process();
}

TraceToIRConversionPhase::IRConversionContext::IRConversionContext(std::shared_ptr<ExecutionTrace> trace, const compiler::CompilationUnitID& id) : trace(trace), ir(std::make_shared<compiler::ir::IRGraph>(id)) {
}

std::shared_ptr<IRGraph> TraceToIRConversionPhase::IRConversionContext::process() {
	processBlock(trace->getBlocks().front());
	auto functionOperation = std::make_unique<FunctionOperation>("execute", currentBasicBlocks, std::vector<Type> {}, std::vector<std::string> {}, returnType);
	ir->addRootOperation(std::move(functionOperation));
	return ir;
}

BasicBlock* TraceToIRConversionPhase::IRConversionContext::processBlock(Block& block) {
	// create new frame and block
	ValueFrame blockFrame;
	std::vector<std::unique_ptr<BasicBlockArgument>> blockArguments;
	for (auto& arg : block.arguments) {
		auto argumentIdentifier = createValueIdentifier(arg);
		auto blockArgument = std::make_unique<BasicBlockArgument>(argumentIdentifier, arg.type);
		blockFrame.setValue(argumentIdentifier, blockArgument.get());
		blockArguments.emplace_back(std::move(blockArgument));
	}
	auto& irBasicBlock = currentBasicBlocks.emplace_back(std::make_unique<BasicBlock>(block.blockId, blockArguments));
	auto irBasicBlockPtr = irBasicBlock.get();

	blockMap[block.blockId] = irBasicBlockPtr;
	for (auto& operation : block.operations) {
		processOperation(blockFrame, block, irBasicBlockPtr, operation);
	}
	return irBasicBlockPtr;
}

void TraceToIRConversionPhase::IRConversionContext::processOperation(ValueFrame& frame, Block& currentBlock, BasicBlock*& currentIrBlock, TraceOperation& operation) {

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
		} else {
			auto returnValue = frame.getValue(createValueIdentifier(operation.input[0]));
			currentIrBlock->addOperation<ReturnOperation>(returnValue);
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
	default: {
		throw NotImplementedException("Operation type is not implemented.");
	}
	}
}

template <typename OpType>
void TraceToIRConversionPhase::IRConversionContext::processBinaryOperator(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& op) {
	auto leftInput = frame.getValue(createValueIdentifier(op.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(op.input[1]));
	auto resultIdentifier = createValueIdentifier(op.resultRef);
	auto operation = currentBlock->addOperation<OpType>(resultIdentifier, leftInput, rightInput);
	frame.setValue(resultIdentifier, operation);
}

template <typename OpType>
void TraceToIRConversionPhase::IRConversionContext::processUnaryOperator(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, TraceOperation& op) {
	auto input = frame.getValue(createValueIdentifier(op.input[0]));
	auto resultIdentifier = createValueIdentifier(op.resultRef);
	auto operation = currentBlock->addOperation<OpType>(resultIdentifier, input);
	frame.setValue(resultIdentifier, operation);
}

void TraceToIRConversionPhase::IRConversionContext::processJMP(ValueFrame& frame, BasicBlock* block, TraceOperation& operation) {
	auto blockRef = get<BlockRef>(operation.input[0]);
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

void TraceToIRConversionPhase::IRConversionContext::processCMP(ValueFrame& frame, Block&, BasicBlock* currentIrBlock, TraceOperation& operation) {
	auto valueRef = get<TypedValueRef>(operation.input[0]);
	auto trueCaseBlockRef = get<BlockRef>(operation.input[1]);
	auto falseCaseBlockRef = get<BlockRef>(operation.input[2]);

	auto booleanValue = frame.getValue(createValueIdentifier(valueRef));
	auto ifOperation = std::make_unique<IfOperation>(booleanValue);
	auto trueCaseBlock = processBlock(trace->getBlock(trueCaseBlockRef.block));

	ifOperation->getTrueBlockInvocation().setBlock(trueCaseBlock);
	createBlockArguments(frame, ifOperation->getTrueBlockInvocation(), trueCaseBlockRef);

	auto falseCaseBlock = processBlock(trace->getBlock(falseCaseBlockRef.block));
	ifOperation->getFalseBlockInvocation().setBlock(falseCaseBlock);
	createBlockArguments(frame, ifOperation->getFalseBlockInvocation(), falseCaseBlockRef);
	currentIrBlock->addOperation(std::move(ifOperation));
}

void TraceToIRConversionPhase::IRConversionContext::createBlockArguments(ValueFrame& frame, BasicBlockInvocation& blockInvocation, BlockRef val) {
	for (auto& arg : val.arguments) {
		auto valueIdentifier = createValueIdentifier(arg);
		blockInvocation.addArgument(frame.getValue(valueIdentifier));
	}
}

void TraceToIRConversionPhase::IRConversionContext::processBinaryComp(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, nautilus::tracing::TraceOperation& operation, compiler::ir::BinaryCompOperation::Type type) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto divOperation = currentBlock->addOperation<BinaryCompOperation>(resultIdentifier, leftInput, rightInput, type);
	frame.setValue(resultIdentifier, divOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processShift(ValueFrame& frame, compiler::ir::BasicBlock* currentBlock, nautilus::tracing::TraceOperation& operation, compiler::ir::ShiftOperation::ShiftType type) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto divOperation = currentBlock->addOperation<ShiftOperation>(resultIdentifier, leftInput, rightInput, type);
	frame.setValue(resultIdentifier, divOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processLogicalComperator(ValueFrame& frame, BasicBlock* currentBlock, TraceOperation& operation, CompareOperation::Comparator comp) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto compareOperation = currentBlock->addOperation<CompareOperation>(resultIdentifier, leftInput, rightInput, comp);
	frame.setValue(resultIdentifier, compareOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processLoad(ValueFrame& frame, BasicBlock* currentBlock, TraceOperation& operation) {
	auto address = frame.getValue(createValueIdentifier(operation.input[0]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto resultType = operation.resultType;
	auto loadOperation = std::make_unique<LoadOperation>(resultIdentifier, address, resultType);
	frame.setValue(resultIdentifier, loadOperation.get());
	currentBlock->addOperation(std::move(loadOperation));
}

void TraceToIRConversionPhase::IRConversionContext::processStore(ValueFrame& frame, BasicBlock* currentBlock, TraceOperation& operation) {
	auto address = frame.getValue(createValueIdentifier(operation.input[0]));
	auto value = frame.getValue(createValueIdentifier(operation.input[1]));
	currentBlock->addOperation<StoreOperation>(value, address);
}

void TraceToIRConversionPhase::IRConversionContext::processCall(ValueFrame& frame, BasicBlock* currentBlock, TraceOperation& operation) {
	auto functionCallTarget = std::get<FunctionCall>(operation.input[0]);
	auto inputArguments = std::vector<Operation*> {};
	for (auto& argument : functionCallTarget.arguments) {
		auto input = frame.getValue(createValueIdentifier(argument));
		inputArguments.emplace_back(input);
	}

	auto resultType = operation.resultType;
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto proxyCallOperation = currentBlock->addOperation<ProxyCallOperation>(functionCallTarget.mangledName, functionCallTarget.functionName, functionCallTarget.ptr, resultIdentifier, inputArguments, resultType);
	if (resultType != Type::v) {
		frame.setValue(resultIdentifier, proxyCallOperation);
	}
}

void TraceToIRConversionPhase::IRConversionContext::processConst(ValueFrame& frame, BasicBlock* currentBlock, TraceOperation& operation) {
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

void TraceToIRConversionPhase::IRConversionContext::processCast(ValueFrame& frame, BasicBlock* currentBlock, TraceOperation& operation) {
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto input = frame.getValue(createValueIdentifier(operation.input[0]));
	auto castOperation = currentBlock->addOperation<CastOperation>(resultIdentifier, input, operation.resultType);
	frame.setValue(resultIdentifier, castOperation);
}

} // namespace nautilus::tracing
