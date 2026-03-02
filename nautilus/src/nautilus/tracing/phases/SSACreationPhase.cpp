
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
	// Finally we make all block arguments unique to their local block.
	// As a result two blocks, can't use the same value references.
	makeBlockArgumentsUnique();

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
	// A value ref is defined in the local scope if it is the result of an operation before operationIndex.
	// Note: operationIndex is essential here — ASSIGN ops create forward definitions within a block
	// (e.g. "ADD $7 $2 $6" at index 1 followed by "ASSIGN $2 $7" at index 2), so we must only look
	// at ops that precede the current use.
	for (uint32_t i = 0; i < operationIndex; i++) {
		if (block.operations[i].resultRef == ref) {
			return true;
		}
	}
	return std::find(block.arguments.begin(), block.arguments.end(), ref) != block.arguments.end();
}

void SSACreationPhase::SSACreationPhaseContext::processBlock(Block& startBlock) {
	// Precompute the set of values defined by operations in each block.
	// This allows O(1) locality checks during value propagation.
	for (auto& block : trace->getBlocks()) {
		auto& defined = blockDefinitions[block.blockId];
		for (auto& op : block.operations) {
			defined.insert(op.resultRef.ref);
		}
	}

	// Single-pass DFS traversal from the return block through predecessors.
	// Each block is visited exactly once. All non-local refs discovered in a
	// block are batched and propagated upward in a single call to propagateBatch,
	// replacing the previous one-call-per-ref pattern.
	std::vector<uint16_t> worklist;
	worklist.push_back(startBlock.blockId);

	while (!worklist.empty()) {
		auto blockId = worklist.back();
		worklist.pop_back();

		if (processedBlocks.contains(blockId)) {
			continue;
		}

		auto& block = trace->getBlock(blockId);

		// Collect every non-local value reference used by this block.
		// Use a set to deduplicate: the same ref can appear in multiple ops.
		std::unordered_set<uint16_t> seenRefs;
		std::vector<TypedValueRef> nonLocalRefs;
		for (int64_t i = block.operations.size() - 1; i >= 0; i--) {
			auto& operation = block.operations[i];
			for (auto& input : operation.input) {
				if (auto* valueRef = std::get_if<TypedValueRef>(&input)) {
					if (!isLocalValueRef(block, *valueRef, operation.resultType, i) &&
					    seenRefs.insert(valueRef->ref).second) {
						nonLocalRefs.push_back(*valueRef);
					}
				} else if (auto* blockRef = std::get_if<BlockRef>(&input)) {
					for (auto& ref : blockRef->arguments) {
						if (!isLocalValueRef(block, ref, ref.type, i) && seenRefs.insert(ref.ref).second) {
							nonLocalRefs.push_back(ref);
						}
					}
				} else if (auto* fcallRef = std::get_if<FunctionCall>(&input)) {
					for (const auto& ref : fcallRef->arguments) {
						auto copy = ref;
						if (!isLocalValueRef(block, copy, copy.type, i) && seenRefs.insert(copy.ref).second) {
							nonLocalRefs.push_back(copy);
						}
					}
				}
			}
		}

		// Propagate all non-local refs upward in one backward traversal.
		if (!nonLocalRefs.empty()) {
			propagateBatch(block, nonLocalRefs);
		}

		processedBlocks.emplace(block.blockId);

		// Push unprocessed predecessors in reverse order so the LIFO stack pops
		// them left-to-right, matching the original recursive DFS order.
		for (auto it = block.predecessors.rbegin(); it != block.predecessors.rend(); ++it) {
			if (!processedBlocks.contains(*it)) {
				worklist.push_back(*it);
			}
		}
	}
}

void SSACreationPhase::SSACreationPhaseContext::propagateBatch(Block& startBlock,
                                                               const std::vector<TypedValueRef>& refs) {
	// Add all non-local refs to the starting block's arguments.
	// Callers have already confirmed these refs are absent, so emplace directly.
	for (const auto& ref : refs) {
		startBlock.arguments.emplace_back(ref);
	}

	// pending maps blockId → refs still needing upward propagation from that block.
	// An entry's presence means the block is scheduled in workQueue.
	std::unordered_map<uint16_t, std::vector<TypedValueRef>> pending;
	pending.emplace(startBlock.blockId, refs);

	std::vector<uint16_t> workQueue = {startBlock.blockId};

	while (!workQueue.empty()) {
		auto curBlockId = workQueue.back();
		workQueue.pop_back();

		auto pendingIt = pending.find(curBlockId);
		if (pendingIt == pending.end()) {
			continue;
		}
		// Move out before modifying pending to avoid iterator invalidation.
		auto refsToPropagate = std::move(pendingIt->second);
		pending.erase(pendingIt);

		auto& curBlock = trace->getBlock(curBlockId);

		for (auto predecessorId : curBlock.predecessors) {
			auto& predBlock = trace->getBlock(predecessorId);

			// Update the predecessor's branch instruction that targets curBlock.
			auto& lastOp = predBlock.operations.back();
			if (lastOp.op == Op::JMP || lastOp.op == Op::CMP) {
				for (auto& input : lastOp.input) {
					if (auto* blockRef = std::get_if<BlockRef>(&input)) {
						if (blockRef->block == curBlockId) {
							for (const auto& ref : refsToPropagate) {
								blockRef->arguments.emplace_back(ref);
							}
						}
					}
				}
			}

			// Determine which refs still need to propagate through the predecessor.
			auto defIt = blockDefinitions.find(predecessorId);
			std::vector<TypedValueRef> refsForPred;
			for (const auto& ref : refsToPropagate) {
				// Stop if the value is defined by an operation in the predecessor.
				if (defIt != blockDefinitions.end() && defIt->second.contains(ref.ref)) {
					continue;
				}
				// Stop if already a block argument (previously propagated).
				if (std::find(predBlock.arguments.begin(), predBlock.arguments.end(), ref) !=
				    predBlock.arguments.end()) {
					continue;
				}
				refsForPred.push_back(ref);
			}

			if (!refsForPred.empty()) {
				// Add directly — we just confirmed absence, so no double-scan needed.
				for (const auto& ref : refsForPred) {
					predBlock.arguments.emplace_back(ref);
				}
				// Schedule for further upward propagation, merging if already queued.
				auto [it, inserted] = pending.try_emplace(predecessorId);
				if (inserted) {
					it->second = std::move(refsForPred);
					workQueue.push_back(predecessorId);
				} else {
					// Predecessor already queued from another branch: merge new refs.
					auto& existing = it->second;
					for (const auto& ref : refsForPred) {
						if (std::find(existing.begin(), existing.end(), ref) == existing.end()) {
							existing.push_back(ref);
						}
					}
				}
			}
		}
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
