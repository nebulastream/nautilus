
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceOperation.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include <cassert>
#include <vector>

namespace nautilus::tracing {
using namespace compiler::ir;

namespace {

} // namespace

std::shared_ptr<IRGraph> TraceToIRConversionPhase::apply(std::shared_ptr<TraceModule> traceModule,
                                                         const compiler::CompilationUnitID& id) {
	auto ir = std::make_shared<compiler::ir::IRGraph>(id);

	// Process all functions in sorted order for deterministic IR output.
	for (const auto& functionName : traceModule->getFunctionNames()) {
		auto* trace = traceModule->getFunction(functionName);
		auto& attrs = traceModule->getFunctionAttributes(functionName);
		auto location = traceModule->getFunctionLocation(functionName);
		auto phaseContext = IRConversionContext(trace, ir, id);
		auto* functionOperation = phaseContext.processFunction(functionName, attrs, location);
		ir->addFunctionOperation(functionOperation);
		bindDefinition(*ir, *traceModule, functionName, functionOperation);
	}

	return ir;
}

std::shared_ptr<IRGraph> TraceToIRConversionPhase::apply(std::shared_ptr<TraceModule> traceModule,
                                                         common::ArenaPool& pool,
                                                         const compiler::CompilationUnitID& id) {
	auto ir = std::make_shared<compiler::ir::IRGraph>(pool.acquire(), id);

	for (const auto& functionName : traceModule->getFunctionNames()) {
		auto* trace = traceModule->getFunction(functionName);
		auto& attrs = traceModule->getFunctionAttributes(functionName);
		auto location = traceModule->getFunctionLocation(functionName);
		auto phaseContext = IRConversionContext(trace, ir, id);
		auto* functionOperation = phaseContext.processFunction(functionName, attrs, location);
		ir->addFunctionOperation(functionOperation);
		bindDefinition(*ir, *traceModule, functionName, functionOperation);
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
    : IRConversionSupport(trace, std::move(ir)) {
}

std::shared_ptr<IRGraph> TraceToIRConversionPhase::IRConversionContext::process() {
	processBlock(*trace->getBlocks().front());
	std::unordered_map<std::string, std::string> attributes = {{"entry", "true"}};
	auto* functionOperation = ir->getArena().create<FunctionOperation>(
	    "execute", std::move(currentBasicBlocks), std::vector<Type> {}, std::vector<std::string> {}, returnType,
	    collectAllocaSpecs(), std::move(attributes), std::move(currentRegionSpecs));
	ir->addFunctionOperation(functionOperation);
	// Single-trace path: the entry function has no NautilusFunctionDefinition
	// behind it (nothing calls it), so it interns with a null identity key and
	// simply gets a fresh entry.
	compiler::ir::CalleeDescriptor descriptor;
	descriptor.kind = compiler::ir::CalleeDescriptor::Kind::Internal;
	descriptor.customName = "execute";
	ir->defineFunction(ir->internCallee(descriptor), functionOperation);
	return ir;
}

FunctionOperation* TraceToIRConversionPhase::IRConversionContext::processFunction(
    const std::string& functionName, const std::unordered_map<std::string, std::string>& attributes,
    const SourceLocation& location) {
	// Clear state for this function
	currentBasicBlocks.clear();
	blockMap.clear();
	resetFunctionState();
	returnType = Type::v;

	// Process all blocks starting from the first block
	processBlock(*trace->getBlocks().front());

	// Create and return the function operation
	return ir->getArena().create<FunctionOperation>(functionName, std::move(currentBasicBlocks), std::vector<Type> {},
	                                                std::vector<std::string> {}, returnType, collectAllocaSpecs(),
	                                                attributes, std::move(currentRegionSpecs), location);
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
	// The trace block already names the innermost region containing all of its code
	// (the tracer merged the region seams away, widening the blocks it merged into),
	// and mapRegion preserves the nesting, so the IR block can take it over as is. The
	// block-cleanup passes widen it further when they merge blocks of different regions.
	irBasicBlockPtr->setRegionIndex(mapRegion(block.regionIndex));
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
	case Op::CMP: {
		processCMP(frame, currentBlock, currentIrBlock, operation);
		return;
	}
	case Op::JMP: {
		processJMP(frame, currentIrBlock, operation);
		return;
	}
	case Op::RETURN: {
		if (operation.input.empty()) {
			currentIrBlock->addTaggedOperation<ReturnOperation>(provenanceOf(operation));
			returnType = Type::v;
		} else {
			auto returnValue = frame.getValue(createValueIdentifier(operation.input[0]));
			currentIrBlock->addTaggedOperation<ReturnOperation>(provenanceOf(operation), returnValue);
			returnType = returnValue->getStamp();
		}
		return;
	}
	default: {
		if (!convertValueOperation(frame, currentIrBlock, operation)) {
			throw NotImplementedException("Operation type is not implemented.");
		}
	}
	}
}

void TraceToIRConversionPhase::IRConversionContext::processJMP(ValueFrame& frame, BasicBlock* block,
                                                               TraceOperation& operation) {
	const BlockRef& blockRef = *get<BlockRef*>(operation.input[0]);
	BasicBlockInvocation blockInvocation;
	createBlockArguments(frame, blockInvocation, blockRef);

	BasicBlock* targetBlock = nullptr;
	if (auto it = blockMap.find(blockRef.block); it != blockMap.end()) {
		targetBlock = it->second;
	} else {
		targetBlock = processBlock(trace->getBlock(blockRef.block));
		blockMap[blockRef.block] = targetBlock;
	}
	stampProvenance(*block->addNextBlock(targetBlock, blockInvocation.getArguments()), operation);
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
	stampProvenance(*ifOperation, operation);

	// IfOperation needs its true/false invocations wired before being
	// appended; we therefore can't use `addTaggedOperation` here without
	// breaking that ordering.
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

} // namespace nautilus::tracing
