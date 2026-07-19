
#include "fmt/core.h"
#include <algorithm>
#include <limits>
#include <nautilus/exceptions/RuntimeException.hpp>
#include <nautilus/tracing/ExecutionTrace.hpp>
#include <nautilus/tracing/TraceOperation.hpp>
#include <nautilus/tracing/phases/SSACreationPhase.hpp>
#include <unordered_map>

namespace nautilus::tracing {

namespace {

constexpr auto NO_BLOCK = std::numeric_limits<uint32_t>::max();
constexpr auto NOT_INDEXED = std::numeric_limits<size_t>::max();

class ArenaRewindGuard {
public:
	explicit ArenaRewindGuard(common::Arena& arena) : arena(arena), checkpoint(arena.checkpoint()) {
	}

	~ArenaRewindGuard() {
		arena.rewind(checkpoint);
	}

	ArenaRewindGuard(const ArenaRewindGuard&) = delete;
	ArenaRewindGuard& operator=(const ArenaRewindGuard&) = delete;

private:
	common::Arena& arena;
	common::Arena::Checkpoint checkpoint;
};

} // namespace

std::shared_ptr<ExecutionTrace> SSACreationPhase::apply(std::shared_ptr<ExecutionTrace> trace) {
	// Initialize a new context and perform the inference of the SSA values
	auto phaseContext = SSACreationPhaseContext(*trace);
	phaseContext.process();
	return trace;
}

std::shared_ptr<TraceModule> SSACreationPhase::apply(std::shared_ptr<TraceModule> traceModule) {
	// Get all function names in the module
	auto functionNames = traceModule->getFunctionNames();

	// Process each function's trace independently
	for (const auto& functionName : functionNames) {
		// Get the raw pointer to the execution trace
		auto* tracePtr = traceModule->getFunction(functionName);
		if (tracePtr == nullptr) {
			continue; // Skip if function not found (shouldn't happen)
		}

		// Apply SSA transformation to this function's trace
		auto phaseContext = SSACreationPhaseContext(*tracePtr);
		phaseContext.process();

		// The trace is modified in place, so no need to update the module
	}

	return traceModule;
}

SSACreationPhase::SSACreationPhaseContext::SSACreationPhaseContext(ExecutionTrace& trace) : trace(trace) {
}

Block& SSACreationPhase::SSACreationPhaseContext::getReturnBlock() {
	auto returns = trace.getReturn();
	if (returns.empty()) {
		// A well-formed trace always records at least one Return operation;
		// an empty list means every control-flow path through the traced
		// function's execution somehow missed recording one (e.g. a
		// bookkeeping bug elsewhere in the tracer for a rare control-flow
		// shape). Surface this as a catchable error instead of the
		// undefined behavior of calling front() on an empty vector.
		throw RuntimeException("Invalid trace: no Return operation was recorded.");
	}
	auto firstReturnOp = returns.front();
	if (returns.size() <= 1) {
		return trace.getBlock(firstReturnOp.blockIndex);
	}

	auto* defaultReturnOp = trace.getBlock(returns.front().blockIndex).operations[firstReturnOp.operationIndex];

	// add return block
	auto& returnBlock = trace.getBlock(trace.createBlock());
	// Allocate a copy of the default return op in the arena so that both
	// the return block and any subsequent in-place modifications of the
	// original operation stay independent.
	returnBlock.operations.push_back(cloneTraceOp(trace.getArena(), *defaultReturnOp));
	for (auto returnOp : returns) {
		auto& returnOpBlock = trace.getBlock(returnOp.blockIndex);
		auto* returnValue = returnOpBlock.operations[returnOp.operationIndex];
		// check if we have return values
		if (returnValue->input.empty()) {
			returnOpBlock.operations.erase(returnOpBlock.operations.cbegin() + returnOp.operationIndex);
		} else {
			auto snap = Snapshot();
			returnOpBlock.operations[returnOp.operationIndex] =
			    makeTraceOp(trace.getArena(), snap, ASSIGN, defaultReturnOp->resultType,
			                std::get<TypedValueRef>(defaultReturnOp->input[0]), returnValue->input[0]);
		}
		returnOpBlock.addOperation(
		    makeTraceOp(trace.getArena(), Op::JMP, trace.getArena().create<BlockRef>(returnBlock.blockId)));
		returnBlock.predecessors.emplace_back(returnOp.blockIndex);
	}

	return returnBlock;
	//  return trace->getBlock(bl);
}

void SSACreationPhase::SSACreationPhaseContext::process() {
	auto rootBlockNumberOfArguments = trace.getArguments().size();

	// Allocas no longer require hoisting: every Op::ALLOCA carries an index
	// into the trace's central allocaSpecs table, which is copied wholesale
	// onto the FunctionOperation by TraceToIRConversionPhase.  Backends emit
	// one real alloca per table entry in the function prologue, so the trace
	// op can sit anywhere in the CFG.

	//  In the first step we get the return block, which contains the return call.
	//  Starting with this block we trace all inputs

	// Merging all potential return blocks into a single (new) return block
	auto& returnBlock = getReturnBlock();
	const auto numberOfValueRefs = static_cast<size_t>(trace.lastValueRef) + 1;
	auto& arena = trace.getArena();
	// getReturnBlock may add durable trace objects. Everything allocated after
	// this point is phase-local scratch and is reclaimed on success or failure.
	ArenaRewindGuard scratchGuard(arena);

	auto* availabilityStorage =
	    static_cast<uint32_t*>(arena.allocate(numberOfValueRefs * sizeof(uint32_t), alignof(uint32_t)));
	std::fill_n(availabilityStorage, numberOfValueRefs, NO_BLOCK);
	availableInBlock = {availabilityStorage, numberOfValueRefs};

	auto* definitionBlockStorage =
	    static_cast<uint32_t*>(arena.allocate(numberOfValueRefs * sizeof(uint32_t), alignof(uint32_t)));
	std::fill_n(definitionBlockStorage, numberOfValueRefs, NO_BLOCK);
	uniqueDefinitionBlock = {definitionBlockStorage, numberOfValueRefs};

	const auto numberOfBlocks = trace.getBlocks().size();
	auto* processedBlockStorage =
	    static_cast<uint8_t*>(arena.allocate(numberOfBlocks * sizeof(uint8_t), alignof(uint8_t)));
	std::fill_n(processedBlockStorage, numberOfBlocks, uint8_t {0});
	processedBlocks = {processedBlockStorage, numberOfBlocks};

	auto* definitionRangeStorage = static_cast<AssignmentDefinitionRange*>(
	    arena.allocate(numberOfBlocks * sizeof(AssignmentDefinitionRange), alignof(AssignmentDefinitionRange)));
	assignmentDefinitionRanges = {definitionRangeStorage, numberOfBlocks};
	for (const auto* block : trace.getBlocks()) {
		if (block->blockId >= numberOfBlocks) {
			throw RuntimeException(fmt::format("Invalid trace: block id {} is outside the {}-block trace.", block->blockId,
			                                   numberOfBlocks));
		}
		assignmentDefinitionRanges[block->blockId] = {nullptr, NOT_INDEXED};
	}
	processBlock(returnBlock);
	// Eliminate all assign operations. We only needed them to create the SSA
	// from.
	removeAssignOperations();

	// check arguments
	if (rootBlockNumberOfArguments != trace.getBlocks().front()->arguments.size()) {
		throw RuntimeException(fmt::format("Wrong number of arguments in trace: expected {}, got {}\n",
		                                   rootBlockNumberOfArguments, trace.getBlocks().front()->arguments.size()));
	}
	// sort arguments
	std::sort(trace.getBlocks().front()->arguments.begin(), trace.getBlocks().front()->arguments.end());
}

void SSACreationPhase::SSACreationPhaseContext::processBlock(Block& startBlock) {
	// Single-pass traversal from the return block through predecessors.
	// Each block is visited exactly once. Non-local value references are
	// eagerly propagated upward through the predecessor chain by
	// propagateValue, eliminating the need to re-process any block.
	// Operations within each block are scanned forward so the availability
	// table directly represents definitions that precede the current use.
	std::vector<uint32_t> worklist;
	worklist.reserve(processedBlocks.size());
	worklist.push_back(startBlock.blockId);

	while (!worklist.empty()) {
		auto blockId = worklist.back();
		worklist.pop_back();

		if (blockId >= processedBlocks.size()) {
			throw RuntimeException(fmt::format("Invalid trace: predecessor block id {} is out of range.", blockId));
		}
		if (processedBlocks[blockId] != 0) {
			continue;
		}

		auto& block = trace.getBlock(blockId);
		for (const auto& argument : block.arguments) {
			validateValueRef(argument.ref);
			availableInBlock[argument.ref] = block.blockId;
		}

		// Process the inputs of all operations in the current block
		for (auto* operationPtr : block.operations) {
			auto& operation = *operationPtr;
			for (auto& input : operation.input) {
				forEachValueRef(input, [&](TypedValueRef ref) { processValueRef(block, ref); });
			}
			if (operation.resultRef.ref != 0) {
				validateValueRef(operation.resultRef.ref);
				availableInBlock[operation.resultRef.ref] = block.blockId;
			}
		}

		processedBlocks[block.blockId] = 1;

		// Add unprocessed predecessors to the worklist in reverse order so that
		// the stack (LIFO) pops them in the original left-to-right order,
		// matching the DFS traversal order of the previous recursive version.
		for (auto it = block.predecessors.rbegin(); it != block.predecessors.rend(); ++it) {
			if (*it >= processedBlocks.size()) {
				throw RuntimeException(fmt::format("Invalid trace: predecessor block id {} is out of range.", *it));
			}
			if (processedBlocks[*it] == 0) {
				worklist.push_back(*it);
			}
		}
	}
}

void SSACreationPhase::SSACreationPhaseContext::processValueRef(Block& block, TypedValueRef ref) {
	validateValueRef(ref.ref);
	if (availableInBlock[ref.ref] == block.blockId) {
		return;
	}

	// The valueRef references a different block. Eagerly propagate it upward
	// through the predecessor chain so every block that needs to pass this
	// value receives it without requiring the main loop to revisit any block.
	propagateValue(block, ref);
	availableInBlock[ref.ref] = block.blockId;
}

bool SSACreationPhase::SSACreationPhaseContext::isDefinedInBlock(uint32_t blockId, ValueRef ref) {
	if (blockId >= assignmentDefinitionRanges.size()) {
		throw RuntimeException(fmt::format("Invalid trace: definition lookup for block id {} is out of range.", blockId));
	}
	validateValueRef(ref);
	indexDefinitions(blockId);
	if (uniqueDefinitionBlock[ref] == blockId) {
		return true;
	}
	const auto& range = assignmentDefinitionRanges[blockId];
	return std::binary_search(range.data, range.data + range.count, ref);
}

void SSACreationPhase::SSACreationPhaseContext::indexDefinitions(uint32_t blockId) {
	auto& range = assignmentDefinitionRanges[blockId];
	if (range.count != NOT_INDEXED) {
		return;
	}

	auto& block = trace.getBlock(blockId);
	const auto assignmentCount = static_cast<size_t>(
	    std::count_if(block.operations.begin(), block.operations.end(), [](const auto* operation) {
		    return operation->op == Op::ASSIGN && operation->resultRef.ref != 0;
	    }));
	range.data = assignmentCount == 0
	                 ? nullptr
	                 : static_cast<ValueRef*>(
	                       trace.getArena().allocate(assignmentCount * sizeof(ValueRef), alignof(ValueRef)));
	auto* output = range.data;
	for (const auto* operation : block.operations) {
		const auto ref = operation->resultRef.ref;
		if (ref == 0) {
			continue;
		}
		validateValueRef(ref);
		if (operation->op == Op::ASSIGN) {
			*output++ = ref;
			continue;
		}
		auto& definingBlock = uniqueDefinitionBlock[ref];
		if (definingBlock != NO_BLOCK && definingBlock != blockId) {
			throw RuntimeException(fmt::format("Invalid trace: value ref ${} is defined by operations in B{} and B{}.",
			                                   ref, definingBlock, blockId));
		}
		definingBlock = blockId;
	}
	if (assignmentCount == 0) {
		range.count = 0;
	} else {
		std::sort(range.data, output);
		range.count = static_cast<size_t>(std::unique(range.data, output) - range.data);
	}
}

void SSACreationPhase::SSACreationPhaseContext::validateValueRef(ValueRef ref) const {
	if (ref >= availableInBlock.size()) {
		throw RuntimeException(
		    fmt::format("Invalid trace: value ref ${} exceeds the maximum value ref ${}.", ref, trace.lastValueRef));
	}
}

void SSACreationPhase::SSACreationPhaseContext::propagateValue(Block& block, TypedValueRef ref) {
	initializePropagation();
	const auto initialKey = (uint64_t(block.blockId) << 32) | ref.ref;
	if (!propagatedValues.insert(initialKey).second) {
		return;
	}
	block.arguments.emplace_back(ref);

	// Reuse the member worklist to avoid heap allocation per call.
	propWorklist.clear();
	propWorklist.push_back(block.blockId);

	while (!propWorklist.empty()) {
		auto curBlockId = propWorklist.back();
		propWorklist.pop_back();

		auto& curBlock = trace.getBlock(curBlockId);

		for (auto& predecessor : curBlock.predecessors) {
			if (predecessor >= processedBlocks.size()) {
				throw RuntimeException(fmt::format("Invalid trace: predecessor block id {} is out of range.", predecessor));
			}
			auto& predBlock = trace.getBlock(predecessor);
			if (predBlock.operations.empty()) {
				throw RuntimeException(
				    fmt::format("Invalid trace: predecessor B{} has no branch operation.", predecessor));
			}
			auto& lastOperation = *predBlock.operations.back();
			if (lastOperation.op == Op::JMP || lastOperation.op == Op::CMP) {
				for (auto& input : lastOperation.input) {
					if (auto* blockRefPtr = std::get_if<BlockRef*>(&input)) {
						BlockRef& blockRef = **blockRefPtr;
						if (blockRef.block == curBlockId) {
							// Each (block, value) pair is propagated once, so every matching
							// control-flow edge also receives the value exactly once.
							blockRef.arguments.emplace_back(ref);
						}
					}
				}
			}

			// If the value is defined by an operation in the predecessor, it is
			// locally available there and no further propagation is needed.
			if (isDefinedInBlock(predecessor, ref.ref)) {
				continue;
			}

			// O(1) check: if we have already propagated this value to this predecessor
			// (handles loops and diamond merges), skip re-queuing.
			uint64_t key = (uint64_t(predecessor) << 32) | ref.ref;
			if (!propagatedValues.insert(key).second) {
				continue;
			}

			// Value is not local in the predecessor: add as argument and
			// continue propagating upward.
			predBlock.arguments.emplace_back(ref);
			propWorklist.push_back(predecessor);
		}
	}
}

void SSACreationPhase::SSACreationPhaseContext::initializePropagation() {
	if (propagationInitialized) {
		return;
	}
	propagationInitialized = true;
	propWorklist.reserve(processedBlocks.size());
	size_t initialArgumentCount = 0;
	for (const auto* block : trace.getBlocks()) {
		initialArgumentCount += block->arguments.size();
	}
	propagatedValues.reserve(processedBlocks.size() * 2 + initialArgumentCount);
	// Existing block arguments are already propagated definitions. Seeding
	// their keys prevents an upstream walk from appending them a second time.
	for (const auto* block : trace.getBlocks()) {
		for (const auto& argument : block->arguments) {
			validateValueRef(argument.ref);
			propagatedValues.insert((uint64_t(block->blockId) << 32) | argument.ref);
		}
	}
}

void SSACreationPhase::SSACreationPhaseContext::removeAssignOperations() {
	// Iterate over all block and eliminate the ASSIGN operation.
	for (Block* blockPtr : trace.getBlocks()) {
		Block& block = *blockPtr;
		std::unordered_map<ValueRef, ValueRef> assignmentMap;
		for (auto* operationPtr : block.operations) {
			auto& operation = *operationPtr;
			if (operation.op == Op::ASSIGN) {
				const auto sourceRef = get<TypedValueRef>(operation.input[0]).ref;
				auto foundAssignment = assignmentMap.find(sourceRef);
				if (foundAssignment != assignmentMap.end()) {
					assignmentMap[operation.resultRef.ref] = foundAssignment->second;
				} else {
					assignmentMap[operation.resultRef.ref] = sourceRef;
				}
			} else {
				for (auto& input : operation.input) {
					forEachMutableValueRef(input, [&](TypedValueRef& valueRef) {
						auto foundAssignment = assignmentMap.find(valueRef.ref);
						if (foundAssignment != assignmentMap.end()) {
							valueRef.ref = foundAssignment->second;
						}
					});
				}
			}
		}
		std::erase_if(block.operations, [&](const auto* item) { return item->op == Op::ASSIGN; });
	}
}

} // namespace nautilus::tracing
