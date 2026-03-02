
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

bool SSACreationPhase::SSACreationPhaseContext::isLocalValueRef(uint16_t blockId, uint16_t ref,
                                                                uint32_t operationIndex) const {
	// Check if ref is defined by an operation before operationIndex in this block (O(1) lookup)
	auto blockIt = blockFirstDefIndex.find(blockId);
	if (blockIt != blockFirstDefIndex.end()) {
		auto refIt = blockIt->second.find(ref);
		if (refIt != blockIt->second.end() && refIt->second < operationIndex) {
			return true;
		}
	}
	// Check if the operation is defined in the block arguments (O(1) lookup)
	auto argIt = blockArgSets.find(blockId);
	return argIt != blockArgSets.end() && argIt->second.contains(ref);
}

void SSACreationPhase::SSACreationPhaseContext::addBlockArgument(Block& block, TypedValueRef ref) {
	if (blockArgSets[block.blockId].insert(ref.ref).second) {
		block.arguments.emplace_back(ref);
	}
}

void SSACreationPhase::SSACreationPhaseContext::processBlock(Block& startBlock) {
	// Precompute: for each block, map from valueRef to its first definition index.
	// Also initialize the argument set from existing block arguments.
	for (auto& block : trace->getBlocks()) {
		auto& defIndex = blockFirstDefIndex[block.blockId];
		for (uint32_t i = 0; i < block.operations.size(); i++) {
			auto ref = block.operations[i].resultRef.ref;
			if (!defIndex.contains(ref)) {
				defIndex[ref] = i;
			}
		}
		auto& argSet = blockArgSets[block.blockId];
		for (auto& arg : block.arguments) {
			argSet.insert(arg.ref);
		}
	}

	// Single-pass traversal from the return block through predecessors.
	// Each block is visited exactly once. Non-local value references are
	// eagerly propagated upward through the predecessor chain by
	// propagateValue, eliminating the need to re-process any block.
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
					processValueRef(block, *valueRef, i);
				} else if (auto* blockRef = std::get_if<BlockRef>(&input)) {
					processBlockRef(block, *blockRef, i);
				} else if (auto* fcallRef = std::get_if<FunctionCall>(&input)) {
					for (auto valueRef : fcallRef->arguments) {
						processValueRef(block, valueRef, i);
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

void SSACreationPhase::SSACreationPhaseContext::processValueRef(Block& block, TypedValueRef& ref,
                                                                uint32_t operationIndex) {
	if (!isLocalValueRef(block.blockId, ref.ref, operationIndex)) {
		// The valueRef references a different block. Eagerly propagate it upward
		// through the predecessor chain so every block that needs to pass this
		// value receives it without requiring the main loop to revisit any block.
		propagateValue(block, ref);
	}
}

void SSACreationPhase::SSACreationPhaseContext::propagateValue(Block& block, TypedValueRef ref) {
	addBlockArgument(block, ref);

	// Iterative worklist: propagate the value upward through predecessors
	// until we reach blocks where the value is locally defined.
	std::vector<uint16_t> propWorklist;
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
			auto defIt = blockFirstDefIndex.find(predecessor);
			if (defIt != blockFirstDefIndex.end() && defIt->second.contains(ref.ref)) {
				continue;
			}

			// Try to add as block argument; if already present, skip
			// (handles loops and diamond merges in O(1)).
			if (!blockArgSets[predecessor].insert(ref.ref).second) {
				continue;
			}

			// Value was newly added as argument, continue propagating upward.
			predBlock.arguments.emplace_back(ref);
			propWorklist.push_back(predecessor);
		}
	}
}

void SSACreationPhase::SSACreationPhaseContext::processBlockRef(Block& block, BlockRef& blockRef,
                                                                uint32_t operationIndex) {
	// a block ref has a set of arguments, which are handled the same as all other
	// value references.
	for (auto& input : blockRef.arguments) {
		processValueRef(block, input, operationIndex);
	}
}

void SSACreationPhase::SSACreationPhaseContext::removeAssignOperations() {
	// Single-pass: resolve assignment chains, replace references, and compact in one traversal.
	for (Block& block : trace->getBlocks()) {
		std::unordered_map<uint16_t, uint16_t> assignmentMap;
		size_t writeIdx = 0;
		for (size_t readIdx = 0; readIdx < block.operations.size(); readIdx++) {
			auto& operation = block.operations[readIdx];
			if (operation.op == Op::ASSIGN) {
				auto& valueRef = get<TypedValueRef>(operation.input[0]);
				auto foundAssignment = assignmentMap.find(valueRef.ref);
				if (foundAssignment != assignmentMap.end()) {
					assignmentMap[operation.resultRef.ref] = foundAssignment->second;
				} else {
					assignmentMap[operation.resultRef.ref] = valueRef.ref;
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
				if (writeIdx != readIdx) {
					block.operations[writeIdx] = std::move(operation);
				}
				writeIdx++;
			}
		}
		block.operations.erase(block.operations.begin() + writeIdx, block.operations.end());
	}
}

} // namespace nautilus::tracing
