
#include "fmt/core.h"
#include <nautilus/exceptions/RuntimeException.hpp>
#include <nautilus/tracing/ExecutionTrace.hpp>
#include <nautilus/tracing/phases/SSACreationPhase.hpp>
#include <unordered_map>

namespace nautilus::tracing {

std::shared_ptr<ExecutionTrace> SSACreationPhase::apply(std::shared_ptr<ExecutionTrace> trace) {
	// Initialize a new context and perform the inference of the SSA values
	auto phaseContext = SSACreationPhaseContext(std::move(trace));
	return phaseContext.process();
}

SSACreationPhase::SSACreationPhaseContext::SSACreationPhaseContext(std::shared_ptr<ExecutionTrace> trace)
    : trace(std::move(trace)) {
}

Block& SSACreationPhase::SSACreationPhaseContext::getReturnBlock() {
	auto returns = trace->getReturn();
	auto firstReturnOp = returns.front();
	if (returns.size() <= 1) {
		return trace->getBlock(firstReturnOp.blockIndex);
	}

	auto defaultReturnOp = trace->getBlock(returns.front().blockIndex).operations[firstReturnOp.operationIndex];

	// add return block
	auto& returnBlock = trace->getBlock(trace->createBlock());
	returnBlock.operations.emplace_back(defaultReturnOp);
	for (auto returnOp : returns) {
		auto& returnOpBlock = trace->getBlock(returnOp.blockIndex);
		auto returnValue = returnOpBlock.operations[returnOp.operationIndex];
		// check if we have return values
		if (returnValue.input.empty()) {
			returnOpBlock.operations.erase(returnOpBlock.operations.cbegin() + returnOp.operationIndex);
		} else {
			auto snap = Snapshot();
			returnOpBlock.operations[returnOp.operationIndex] =
			    TraceOperation(snap, ASSIGN, defaultReturnOp.resultType,
			                   std::get<TypedValueRef>(defaultReturnOp.input[0]), {returnValue.input[0]});
		}
		returnOpBlock.addOperation({Op::JMP, std::vector<InputVariant> {BlockRef(returnBlock.blockId)}});
		returnBlock.predecessors.emplace_back(returnOp.blockIndex);
	}

	return returnBlock;
	//  return trace->getBlock(bl);
}

std::shared_ptr<ExecutionTrace> SSACreationPhase::SSACreationPhaseContext::process() {
	auto rootBlockNumberOfArguments = trace->getArguments().size();
	//  In the first step we get the return block, which contains the return call.
	//  Starting with this block we trace all inputs

	// Merging all potential return blocks into a single (new) return block
	auto& returnBlock = getReturnBlock();
	processBlock(returnBlock);
	// Eliminate all assign operations. We only needed them to create the SSA
	// from.
	removeAssignOperations();

	// check arguments
	if (rootBlockNumberOfArguments != trace->getBlocks().front().arguments.size()) {
		throw RuntimeException(fmt::format("Wrong number of arguments in trace: expected {}, got {}\n",
		                                   rootBlockNumberOfArguments, trace->getBlocks().front().arguments.size()));
	}
	// sort arguments
	std::sort(trace->getBlocks().front().arguments.begin(), trace->getBlocks().front().arguments.end());

	return std::move(trace);
}

bool SSACreationPhase::SSACreationPhaseContext::isLocalValueRef(Block& block, TypedValueRef& ref, Type,
                                                                uint32_t operationIndex) {
	// A value ref is defined in the local scope, if it is the result of an
	// operation before the operationIndex
	for (uint32_t i = 0; i < operationIndex; i++) {
		auto& resOperation = block.operations[i];
		if (resOperation.resultRef == ref) {
			return true;
		}
	}
	// check if the operation is defined in the block arguments
	return std::find(block.arguments.begin(), block.arguments.end(), ref) != block.arguments.end();
}

void SSACreationPhase::SSACreationPhaseContext::processBlock(Block& startBlock) {
	// Single-pass traversal from the return block through predecessors.
	// Each block is visited exactly once. Non-local value references are
	// eagerly propagated upward through the predecessor chain by
	// propagateValue, eliminating the need to re-process any block.
	// blockDefinitions entries are built lazily on first access.
	std::vector<uint16_t> worklist;
	worklist.push_back(startBlock.blockId);

	while (!worklist.empty()) {
		auto blockId = worklist.back();
		worklist.pop_back();

		if (processedBlocks.contains(blockId)) {
			continue;
		}

		auto& block = trace->getBlock(blockId);

		// Process the inputs of all operations in the current block
		for (int64_t i = block.operations.size() - 1; i >= 0; i--) {
			auto& operation = block.operations[i];
			// process input for each variable
			for (auto& input : operation.input) {
				if (auto* valueRef = std::get_if<TypedValueRef>(&input)) {
					processValueRef(block, *valueRef, operation.resultType, i);
				} else if (auto* blockRef = std::get_if<BlockRef>(&input)) {
					processBlockRef(block, *blockRef, i);
				} else if (auto* fcallRef = std::get_if<FunctionCall>(&input)) {
					for (auto valueRef : fcallRef->arguments) {
						processValueRef(block, valueRef, valueRef.type, i);
					}
				}
			}
		}

		processedBlocks.emplace(block.blockId);

		// Add unprocessed predecessors to the worklist in reverse order so that
		// the stack (LIFO) pops them in the original left-to-right order,
		// matching the DFS traversal order of the previous recursive version.
		for (auto it = block.predecessors.rbegin(); it != block.predecessors.rend(); ++it) {
			if (!processedBlocks.contains(*it)) {
				worklist.push_back(*it);
			}
		}
	}
}

void SSACreationPhase::SSACreationPhaseContext::processValueRef(Block& block, TypedValueRef& ref, Type ref_type,
                                                                uint32_t operationIndex) {
	if (isLocalValueRef(block, ref, ref_type, operationIndex)) {
		// variable is a local ref -> don't do anything as the value is defined in
		// the current block
	} else {
		// The valueRef references a different block. Eagerly propagate it upward
		// through the predecessor chain so every block that needs to pass this
		// value receives it without requiring the main loop to revisit any block.
		propagateValue(block, ref);
	}
}

const std::unordered_set<uint16_t>& SSACreationPhase::SSACreationPhaseContext::getOrBuildDefinitions(uint16_t blockId) {
	auto it = blockDefinitions.find(blockId);
	if (it != blockDefinitions.end()) {
		return it->second;
	}
	auto& defined = blockDefinitions[blockId];
	for (auto& op : trace->getBlock(blockId).operations) {
		defined.insert(op.resultRef.ref);
	}
	return defined;
}

void SSACreationPhase::SSACreationPhaseContext::propagateValue(Block& block, TypedValueRef ref) {
	block.addArgument(ref);
	propagatedValues.insert((uint32_t(block.blockId) << 16) | ref.ref);

	// Reuse the member worklist to avoid heap allocation per call.
	propWorklist.clear();
	propWorklist.push_back(block.blockId);

	while (!propWorklist.empty()) {
		auto curBlockId = propWorklist.back();
		propWorklist.pop_back();

		auto& curBlock = trace->getBlock(curBlockId);

		for (auto& predecessor : curBlock.predecessors) {
			auto& predBlock = trace->getBlock(predecessor);
			auto& lastOperation = predBlock.operations.back();
			if (lastOperation.op == Op::JMP || lastOperation.op == Op::CMP) {
				for (auto& input : lastOperation.input) {
					if (auto blockRef = std::get_if<BlockRef>(&input)) {
						if (blockRef->block == curBlockId) {
							blockRef->arguments.emplace_back(ref);
						}
					}
				}
			}

			// If the value is defined by an operation in the predecessor, it is
			// locally available there and no further propagation is needed.
			if (getOrBuildDefinitions(predecessor).contains(ref.ref)) {
				continue;
			}

			// O(1) check: if we have already propagated this value to this predecessor
			// (handles loops and diamond merges), skip re-queuing.
			uint32_t key = (uint32_t(predecessor) << 16) | ref.ref;
			if (propagatedValues.contains(key)) {
				continue;
			}

			// Value is not local in the predecessor: add as argument and
			// continue propagating upward.
			predBlock.addArgument(ref);
			propagatedValues.insert(key);
			propWorklist.push_back(predecessor);
		}
	}
}

void SSACreationPhase::SSACreationPhaseContext::processBlockRef(Block& block, BlockRef& blockRef,
                                                                uint32_t operationIndex) {
	// a block ref has a set of arguments, which are handled the same as all other
	// value references.
	for (auto& input : blockRef.arguments) {
		processValueRef(block, input, input.type, operationIndex);
	}
}

void SSACreationPhase::SSACreationPhaseContext::removeAssignOperations() {
	// Iterate over all block and eliminate the ASSIGN operation.
	for (Block& block : trace->getBlocks()) {
		std::unordered_map<uint16_t, uint16_t> assignmentMap;
		for (auto& operation : block.operations) {
			if (operation.op == Op::ASSIGN) {
				auto& valueRef = get<TypedValueRef>(operation.input[0]);
				auto foundAssignment = assignmentMap.find(valueRef.ref);
				if (foundAssignment != assignmentMap.end()) {
					assignmentMap[operation.resultRef.ref] = foundAssignment->second;
				} else {
					assignmentMap[operation.resultRef.ref] = get<TypedValueRef>(operation.input[0]).ref;
				}
			} else {
				for (auto& input : operation.input) {
					if (auto* valueRef = std::get_if<TypedValueRef>(&input)) {
						auto foundAssignment = assignmentMap.find(valueRef->ref);
						if (foundAssignment != assignmentMap.end()) {
							valueRef->ref = foundAssignment->second;
						}
					} else if (auto* blockRef = std::get_if<BlockRef>(&input)) {
						for (auto& blockArgument : blockRef->arguments) {
							auto foundAssignment = assignmentMap.find(blockArgument.ref);
							if (foundAssignment != assignmentMap.end()) {
								blockArgument.ref = foundAssignment->second;
							}
						}
					} else if (auto* fcallRef = std::get_if<FunctionCall>(&input)) {
						for (auto& funcArg : fcallRef->arguments) {
							auto foundAssignment = assignmentMap.find(funcArg.ref);
							if (foundAssignment != assignmentMap.end()) {
								funcArg.ref = foundAssignment->second;
							}
						}
					}
				}
			}
		}
		std::erase_if(block.operations, [&](const auto& item) { return item.op == Op::ASSIGN; });
	}
}

void SSACreationPhase::SSACreationPhaseContext::makeBlockArgumentsUnique() {
	for (Block& block : trace->getBlocks()) {
		std::unordered_map<uint16_t, uint16_t> blockArgumentMap;

		// iterate over all arguments of this block and create new ValRefs if the
		// argument ref is not local. for (uint64_t argIndex = 0; argIndex <
		// block.arguments.size(); argIndex++) {
		//    auto argRef = block.arguments[argIndex];
		//    if (argRef.blockId != block.blockId) {
		//        auto newLocalRef =
		//                ValueRef(block.blockId, block.operations.size() +
		//                blockArgumentMap.size() + 100,
		//                         argRef.type);
		//        blockArgumentMap[argRef] = newLocalRef;
		//        block.arguments[argIndex] = newLocalRef;
		//    }
		//}

		// set the new ValRefs to all depending on operations.
		for (uint64_t i = 0; i < block.operations.size(); i++) {
			auto& operation = block.operations[i];
			for (auto& input : operation.input) {
				if (auto* valueRef = std::get_if<TypedValueRef>(&input)) {
					auto foundAssignment = blockArgumentMap.find(valueRef->ref);
					if (foundAssignment != blockArgumentMap.end()) {
						// todo check assignment
						valueRef->ref = foundAssignment->second;
						// valueRef->blockId = foundAssignment->second.blockId;
						// valueRef->operationId = foundAssignment->second.operationId;
					}
				} else if (auto* blockRef = std::get_if<BlockRef>(&input)) {
					for (auto& blockArgument : blockRef->arguments) {
						auto foundAssignment = blockArgumentMap.find(blockArgument.ref);
						if (foundAssignment != blockArgumentMap.end()) {
							// valueRef = &foundAssignment->second;
							blockArgument.ref = foundAssignment->second;
							// blockArgument.blockId = foundAssignment->second.blockId;
							// blockArgument.operationId =
							// foundAssignment->second.operationId;
						}
					}
				}
			}
		}

		std::erase_if(block.operations, [&](const auto& item) { return item.op == Op::ASSIGN; });
	}
}

} // namespace nautilus::tracing
