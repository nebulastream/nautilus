
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
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

namespace {

/// Binds a traced body to the function-table entry its call sites minted.
///
/// A call to a Nautilus function is traced before that function's own body is,
/// so by the time we get here the id usually already exists -- keyed on the
/// definition pointer, never the name, so two functions sharing a name still
/// resolve to two entries. Functions nobody calls (the entry point) simply
/// intern here first.
void bindDefinition(compiler::ir::IRGraph& ir, TraceModule& traceModule, const std::string& functionName,
                    compiler::ir::FunctionOperation* functionOperation) {
	const auto* traceDefinition = traceModule.getFunctionDefinition(functionName);
	auto& table = ir.getFunctionTableMut();

	// One body can be denoted by several identities: a NautilusFunction may
	// share a name with a module-registered entry function, in which case the
	// trace loop traced the body once and recorded both. Call sites have
	// already minted an entry keyed on whichever identity they saw, so reuse
	// that entry rather than minting a second one -- two entries would carry
	// two emission names, and the call would look for a symbol the definition
	// was not emitted under.
	auto id = compiler::ir::INVALID_FUNCTION_ID;
	if (traceDefinition != nullptr) {
		for (const auto* definition : traceDefinition->definitions) {
			if (const auto existing = table.find(const_cast<void*>(definition));
			    existing != compiler::ir::INVALID_FUNCTION_ID) {
				id = existing;
				break;
			}
		}
	}

	if (id == compiler::ir::INVALID_FUNCTION_ID) {
		compiler::ir::CalleeDescriptor descriptor;
		descriptor.kind = compiler::ir::CalleeDescriptor::Kind::Internal;
		descriptor.customName = functionName;
		if (traceDefinition != nullptr && !traceDefinition->definitions.empty()) {
			descriptor.key = const_cast<void*>(traceDefinition->definitions.front());
		}
		id = ir.internCallee(descriptor);
	}

	// Point every remaining identity at that one entry.
	if (traceDefinition != nullptr) {
		for (const auto* definition : traceDefinition->definitions) {
			table.alias(const_cast<void*>(definition), id);
		}
	}
	ir.defineFunction(id, functionOperation);
}

} // namespace

std::shared_ptr<IRGraph> TraceToIRConversionPhase::apply(std::shared_ptr<TraceModule> traceModule,
                                                         const compiler::CompilationUnitID& id) {
	auto ir = std::make_shared<compiler::ir::IRGraph>(id);

	// Process all functions in sorted order for deterministic IR output.
	for (const auto& functionName : traceModule->getFunctionNames()) {
		auto* trace = traceModule->getFunction(functionName);
		auto& attrs = traceModule->getFunctionAttributes(functionName);
		auto phaseContext = IRConversionContext(trace, ir, id);
		auto* functionOperation = phaseContext.processFunction(functionName, attrs);
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
		auto phaseContext = IRConversionContext(trace, ir, id);
		auto* functionOperation = phaseContext.processFunction(functionName, attrs);
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
    : trace(trace), ir(std::move(ir)) {
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
    const std::string& functionName, const std::unordered_map<std::string, std::string>& attributes) {
	// Clear state for this function
	currentBasicBlocks.clear();
	blockMap.clear();
	currentRegionSpecs.clear();
	regionMap.clear();
	regionKeys.clear();
	returnType = Type::v;

	// Process all blocks starting from the first block
	processBlock(*trace->getBlocks().front());

	// Create and return the function operation
	return ir->getArena().create<FunctionOperation>(functionName, std::move(currentBasicBlocks), std::vector<Type> {},
	                                                std::vector<std::string> {}, returnType, collectAllocaSpecs(),
	                                                attributes, std::move(currentRegionSpecs));
}

size_t TraceToIRConversionPhase::IRConversionContext::RegionKeyHash::operator()(const RegionKey& key) const noexcept {
	auto hash = std::hash<const void*> {}(key.name);
	hash ^= std::hash<const void*> {}(key.file) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= std::hash<uint64_t> {}((static_cast<uint64_t>(key.line) << 32) | key.column) + 0x9e3779b9 + (hash << 6) +
	        (hash >> 2);
	hash ^= std::hash<uint32_t> {}(key.parent) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	return hash;
}

compiler::ir::RegionIndex TraceToIRConversionPhase::IRConversionContext::mapRegion(RegionIndex traceRegion) {
	if (traceRegion == NO_REGION || traceRegion >= trace->getRegions().size()) {
		return compiler::ir::NO_REGION;
	}
	if (auto it = regionMap.find(traceRegion); it != regionMap.end()) {
		return it->second;
	}
	const auto& spec = trace->getRegions()[traceRegion];
	// Resolve the enclosing chain first: a call site is only the same call site as
	// another if it also sits in the same enclosing region.
	const auto parent = mapRegion(spec.parent);
	const RegionKey key {spec.attributes.name, spec.attributes.location.file, spec.attributes.location.line,
	                     spec.attributes.location.column, parent};
	compiler::ir::RegionIndex index;
	if (auto it = regionKeys.find(key); it != regionKeys.end()) {
		index = it->second;
	} else if (currentRegionSpecs.size() >= compiler::ir::NO_REGION) {
		// One short of the sentinel is the last usable index; beyond it an operation
		// would have to claim a region it does not belong to, so it claims none.
		return compiler::ir::NO_REGION;
	} else {
		index = static_cast<compiler::ir::RegionIndex>(currentRegionSpecs.size());
		currentRegionSpecs.push_back(compiler::ir::RegionSpec {spec.attributes, parent});
		regionKeys[key] = index;
	}
	regionMap[traceRegion] = index;
	return index;
}

compiler::ir::OperationProvenance
TraceToIRConversionPhase::IRConversionContext::provenanceOf(const TraceOperation& operation) {
	return {operation.tag.getTag(), mapRegion(operation.regionIndex)};
}

void TraceToIRConversionPhase::IRConversionContext::stampProvenance(compiler::ir::Operation& irOperation,
                                                                    const TraceOperation& operation) {
	const auto provenance = provenanceOf(operation);
	irOperation.setSourceTag(provenance.sourceTag);
	irOperation.setRegionIndex(provenance.region);
}

std::vector<compiler::ir::AllocaSpec> TraceToIRConversionPhase::IRConversionContext::collectAllocaSpecs() const {
	std::vector<compiler::ir::AllocaSpec> specs;
	specs.reserve(trace->allocaSpecs.size());
	for (const auto& spec : trace->allocaSpecs) {
		specs.push_back({spec.size, spec.align});
	}
	return specs;
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
			currentIrBlock->addTaggedOperation<ReturnOperation>(provenanceOf(operation));
			returnType = Type::v;
		} else {
			auto returnValue = frame.getValue(createValueIdentifier(operation.input[0]));
			currentIrBlock->addTaggedOperation<ReturnOperation>(provenanceOf(operation), returnValue);
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
	case Op::CALL_WITH_EXCEPTION_HANDLING:
		processCall(frame, currentIrBlock, operation);
		return;
	case Op::INDIRECT_CALL:
	case Op::INDIRECT_CALL_WITH_EXCEPTION_HANDLING:
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
	auto operation =
	    currentBlock->addTaggedOperation<OpType>(provenanceOf(op), resultIdentifier, leftInput, rightInput);
	frame.setValue(resultIdentifier, operation);
}

template <typename OpType>
void TraceToIRConversionPhase::IRConversionContext::processUnaryOperator(ValueFrame& frame,
                                                                         compiler::ir::BasicBlock* currentBlock,
                                                                         TraceOperation& op) {
	auto input = frame.getValue(createValueIdentifier(op.input[0]));
	auto resultIdentifier = createValueIdentifier(op.resultRef);
	auto operation = currentBlock->addTaggedOperation<OpType>(provenanceOf(op), resultIdentifier, input);
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
	auto operation = currentBlock->addTaggedOperation<OpType>(provenanceOf(op), resultIdentifier, firstInput,
	                                                          secondInput, thirdInput, op.resultRef.type);
	frame.setValue(resultIdentifier, operation);
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

void TraceToIRConversionPhase::IRConversionContext::processBinaryComp(ValueFrame& frame,
                                                                      compiler::ir::BasicBlock* currentBlock,
                                                                      nautilus::tracing::TraceOperation& operation,
                                                                      compiler::ir::BinaryCompOperation::Type type) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto divOperation = currentBlock->addTaggedOperation<BinaryCompOperation>(provenanceOf(operation), resultIdentifier,
	                                                                          leftInput, rightInput, type);
	frame.setValue(resultIdentifier, divOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processShift(ValueFrame& frame,
                                                                 compiler::ir::BasicBlock* currentBlock,
                                                                 nautilus::tracing::TraceOperation& operation,
                                                                 compiler::ir::ShiftOperation::ShiftType type) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto divOperation = currentBlock->addTaggedOperation<ShiftOperation>(provenanceOf(operation), resultIdentifier,
	                                                                     leftInput, rightInput, type);
	frame.setValue(resultIdentifier, divOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processLogicalComperator(ValueFrame& frame,
                                                                             BasicBlock* currentBlock,
                                                                             TraceOperation& operation,
                                                                             CompareOperation::Comparator comp) {
	auto leftInput = frame.getValue(createValueIdentifier(operation.input[0]));
	auto rightInput = frame.getValue(createValueIdentifier(operation.input[1]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto compareOperation = currentBlock->addTaggedOperation<CompareOperation>(
	    provenanceOf(operation), resultIdentifier, leftInput, rightInput, comp);
	frame.setValue(resultIdentifier, compareOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processLoad(ValueFrame& frame, BasicBlock* currentBlock,
                                                                TraceOperation& operation) {
	auto address = frame.getValue(createValueIdentifier(operation.input[0]));
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto* loadOperation = currentBlock->addTaggedOperation<LoadOperation>(provenanceOf(operation), resultIdentifier,
	                                                                      address, operation.resultType);
	frame.setValue(resultIdentifier, loadOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processStore(ValueFrame& frame, BasicBlock* currentBlock,
                                                                 TraceOperation& operation) {
	auto address = frame.getValue(createValueIdentifier(operation.input[0]));
	auto value = frame.getValue(createValueIdentifier(operation.input[1]));
	currentBlock->addTaggedOperation<StoreOperation>(provenanceOf(operation), value, address);
}

compiler::ir::FunctionId
TraceToIRConversionPhase::IRConversionContext::internCallee(const FunctionCall& call, Type resultType,
                                                            const std::vector<Operation*>& arguments) {
	// Interning is idempotent, but building the descriptor is not free: it
	// copies three names and a parameter-type vector. Conversion walks every
	// call site, and a hot loop calls the same handful of functions over and
	// over, so resolve an already-known identity before paying for any of it.
	if (call.ptr != nullptr) {
		if (const auto existing = ir->getFunctionTable().find(call.ptr);
		    existing != compiler::ir::INVALID_FUNCTION_ID) {
			return existing;
		}
	}

	compiler::ir::CalleeDescriptor descriptor;
	descriptor.key = call.ptr;
	descriptor.resultType = resultType;
	descriptor.attrs = call.fnAttrs;
	descriptor.paramTypes.reserve(arguments.size());
	for (const auto* argument : arguments) {
		descriptor.paramTypes.push_back(argument->getStamp());
	}

	if (call.kind == CalleeKind::Internal) {
		// `ptr` is a NautilusFunctionDefinition, not code. Its name is one a
		// user chose, so it is a custom name rather than a symbol.
		descriptor.kind = compiler::ir::CalleeDescriptor::Kind::Internal;
		descriptor.customName = call.functionName;
	} else {
		descriptor.kind = compiler::ir::CalleeDescriptor::Kind::External;
		descriptor.mangledName = call.mangledName;
		// `functionName` is already rendered per the engine's naming options
		// (demangled, raw, or normalised), which is exactly the display name.
		descriptor.demangledName = call.functionName;
	}
	return ir->internCallee(descriptor);
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
	auto destructors = std::vector<CallOperation::Destructor> {};
	destructors.reserve(functionCallTarget.destructors.size());
	for (const auto& destructor : functionCallTarget.destructors) {
		destructors.push_back(CallOperation::Destructor {
		    .address = frame.getValue(createValueIdentifier(destructor.address)),
		    .functionSymbol = destructor.mangledName,
		    .functionName = destructor.functionName,
		    .functionPtr = destructor.ptr,
		});
	}
	const auto calleeId = internCallee(functionCallTarget, resultType, inputArguments);
	auto callOperation = currentBlock->addTaggedOperation<CallOperation>(
	    provenanceOf(operation), functionCallTarget.mangledName, functionCallTarget.functionName,
	    functionCallTarget.ptr, resultIdentifier, inputArguments, resultType, functionCallTarget.fnAttrs, calleeId,
	    std::move(destructors), operation.op == Op::CALL_WITH_EXCEPTION_HANDLING, functionCallTarget.captureFunc,
	    functionCallTarget.isNautilusCall);
	if (resultType != Type::v) {
		frame.setValue(resultIdentifier, callOperation);
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
	auto destructors = std::vector<IndirectCallOperation::Destructor> {};
	destructors.reserve(indirectCall.destructors.size());
	for (const auto& destructor : indirectCall.destructors) {
		destructors.push_back(IndirectCallOperation::Destructor {
		    .address = frame.getValue(createValueIdentifier(destructor.address)),
		    .functionSymbol = destructor.mangledName,
		    .functionName = destructor.functionName,
		    .functionPtr = destructor.ptr,
		});
	}
	auto indirectCallOp = currentBlock->addTaggedOperation<IndirectCallOperation>(
	    provenanceOf(operation), resultIdentifier, fnPtrOperand, inputArguments, resultType, indirectCall.fnAttrs,
	    std::move(destructors), operation.op == Op::INDIRECT_CALL_WITH_EXCEPTION_HANDLING, indirectCall.captureFunc);
	if (resultType != Type::v) {
		frame.setValue(resultIdentifier, indirectCallOp);
	}
}

void TraceToIRConversionPhase::IRConversionContext::processFuncAddr(ValueFrame& frame, BasicBlock* currentBlock,
                                                                    TraceOperation& operation) {
	const FunctionCall& functionCallTarget = *std::get<FunctionCall*>(operation.input[0]);
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	// Taking an address interns the same target a call to it would, so the two
	// share one table entry -- and so address-of resolves for every linkage,
	// not just for functions defined in this module.
	const auto calleeId = internCallee(functionCallTarget, Type::ptr, {});
	auto funcAddrOp = currentBlock->addTaggedOperation<FunctionAddressOfOperation>(
	    provenanceOf(operation), functionCallTarget.mangledName, functionCallTarget.functionName,
	    functionCallTarget.ptr, resultIdentifier, calleeId);
	frame.setValue(resultIdentifier, funcAddrOp);
}

void TraceToIRConversionPhase::IRConversionContext::processConst(ValueFrame& frame, BasicBlock* currentBlock,
                                                                 TraceOperation& operation) {
	auto constant = std::get<ConstantLiteral>(operation.input[0]);
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	auto resultType = operation.resultType;
	const auto provenance = provenanceOf(operation);
	Operation* constOperation;
	std::visit(
	    [&](auto&& value) {
		    using T = std::decay_t<decltype(value)>;
		    if constexpr (std::is_same_v<T, bool>) {
			    constOperation =
			        currentBlock->addTaggedOperation<ConstBooleanOperation>(provenance, resultIdentifier, value);
		    } else if constexpr (std::is_integral_v<T>) {
			    constOperation = currentBlock->addTaggedOperation<ConstIntOperation>(provenance, resultIdentifier,
			                                                                         value, resultType);
		    } else if constexpr (std::is_floating_point_v<T>) {
			    constOperation = currentBlock->addTaggedOperation<ConstFloatOperation>(provenance, resultIdentifier,
			                                                                           value, resultType);
		    } else if constexpr (std::is_pointer_v<T>) {
			    constOperation =
			        currentBlock->addTaggedOperation<ConstPtrOperation>(provenance, resultIdentifier, value);
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
	auto castOperation = currentBlock->addTaggedOperation<CastOperation>(provenanceOf(operation), resultIdentifier,
	                                                                     input, operation.resultType);
	frame.setValue(resultIdentifier, castOperation);
}

void TraceToIRConversionPhase::IRConversionContext::processAlloca(ValueFrame& frame, BasicBlock* currentBlock,
                                                                  TraceOperation& operation) {
	auto resultIdentifier = createValueIdentifier(operation.resultRef);
	AllocaIndex index = std::get<AllocaIndex>(operation.input[0]);
	auto allocaOperation =
	    currentBlock->addTaggedOperation<AllocaOperation>(provenanceOf(operation), resultIdentifier, index);
	frame.setValue(resultIdentifier, allocaOperation);
}

} // namespace nautilus::tracing
