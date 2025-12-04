
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
	if (returns.empty()) {
		throw RuntimeException("No return operations found in trace");
	}

	auto firstReturnOp = returns.front();
	if (returns.size() <= 1) {
		return trace->getBlock(firstReturnOp.blockIndex);
	}

	// Validate first return operation
	auto& firstReturnBlock = trace->getBlock(firstReturnOp.blockIndex);
	if (firstReturnOp.operationIndex >= firstReturnBlock.operations.size()) {
		throw RuntimeException(fmt::format("Invalid operation index {} in block {}",
		                                   firstReturnOp.operationIndex, firstReturnOp.blockIndex));
	}
	auto defaultReturnOp = firstReturnBlock.operations[firstReturnOp.operationIndex];

	// add return block
	auto& returnBlock = trace->getBlock(trace->createBlock());
	returnBlock.operations.emplace_back(defaultReturnOp);

	for (auto returnOp : returns) {
		auto& returnOpBlock = trace->getBlock(returnOp.blockIndex);

		// Validate operation index
		if (returnOp.operationIndex >= returnOpBlock.operations.size()) {
			throw RuntimeException(fmt::format("Invalid operation index {} in block {}",
			                                   returnOp.operationIndex, returnOp.blockIndex));
		}

		auto returnValue = returnOpBlock.operations[returnOp.operationIndex];
		// check if we have return values
		if (returnValue.input.empty()) {
			// Safe erase: validate index before erasing
			if (returnOp.operationIndex < returnOpBlock.operations.size()) {
				returnOpBlock.operations.erase(returnOpBlock.operations.begin() + returnOp.operationIndex);
			}
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
	// Validate trace has blocks
	if (trace->getBlocks().empty()) {
		throw RuntimeException("Execution trace has no blocks");
	}

	auto rootBlockNumberOfArguments = trace->getArguments().size();
	//  In the first step we get the return block, which contains the return call.
	//  Starting with this block we trace all inputs

	// Merging all potential return blocks into a single (new) return block
	auto& returnBlock = getReturnBlock();
	processBlock(returnBlock);

	// Eliminate all assign operations. We only needed them to create the SSA form.
	removeAssignOperations();

	// Finally we make all block arguments unique to their local block.
	// As a result two blocks, can't use the same value references.
	makeBlockArgumentsUnique();

	// Validate root block exists
	if (trace->getBlocks().empty()) {
		throw RuntimeException("No blocks remain after SSA creation");
	}

	// check arguments
	auto& rootBlock = trace->getBlocks().front();
	if (rootBlockNumberOfArguments != rootBlock.arguments.size()) {
		throw RuntimeException(fmt::format("Wrong number of arguments in trace: expected {}, got {}",
		                                   rootBlockNumberOfArguments, rootBlock.arguments.size()));
	}

	// sort arguments
	std::sort(rootBlock.arguments.begin(), rootBlock.arguments.end());

	return std::move(trace);
}

bool SSACreationPhase::SSACreationPhaseContext::isLocalValueRef(Block& block, TypedValueRef& ref, Type,
                                                                uint32_t operationIndex) {
	// A value ref is defined in the local scope, if it is the result of an
	// operation before the operationIndex
	// Ensure we don't access beyond the valid operation range
	uint32_t maxIndex = std::min(operationIndex, static_cast<uint32_t>(block.operations.size()));
	for (uint32_t i = 0; i < maxIndex; i++) {
		auto& resOperation = block.operations[i];
		if (resOperation.resultRef == ref) {
			return true;
		}
	}
	// check if the operation is defined in the block arguments
	return std::find(block.arguments.begin(), block.arguments.end(), ref) != block.arguments.end();
}

void SSACreationPhase::SSACreationPhaseContext::processBlock(Block& block) {
	// Process the inputs of all operations in the current block
	// Iterate backwards to process operations in reverse order
	for (int64_t i = static_cast<int64_t>(block.operations.size()) - 1; i >= 0; i--) {
		auto& operation = block.operations[i];
		// process input for each variable
		for (auto& input : operation.input) {
			if (auto* valueRef = std::get_if<TypedValueRef>(&input)) {
				// set op type
				processValueRef(block, *valueRef, operation.resultType, i);
			} else if (auto* blockRef = std::get_if<BlockRef>(&input)) {
				processBlockRef(block, *blockRef, i);
			} else if (auto* fcallRef = std::get_if<FunctionCall>(&input)) {
				// Process function call arguments safely
				for (auto& valueRef : fcallRef->arguments) {
					processValueRef(block, valueRef, valueRef.type, i);
				}
			}
		}
	}

	// Mark this block as processed
	processedBlocks.emplace(block.blockId);

	// Recursively process the predecessors of this block
	// If the current block is a control-flow merge it may have multiple
	// predecessors. We avoid visiting them again by checking the processedBlocks set.
	for (auto pred : block.predecessors) {
		if (!processedBlocks.contains(pred)) {
			auto& predBlock = trace->getBlock(pred);
			processBlock(predBlock);
		}
	}
}

void SSACreationPhase::SSACreationPhaseContext::processValueRef(Block& block, TypedValueRef& ref, Type ref_type,
                                                                uint32_t operationIndex) {
	if (isLocalValueRef(block, ref, ref_type, operationIndex)) {
		// variable is a local ref -> don't do anything as the value is defined in
		// the current block
		return;
	}

	// The valueRef references a different block, so we have to add it to the
	// local arguments and append it to the predecessor calls
	block.addArgument(ref);

	// add to parameters in parent blocks
	for (auto& predecessor : block.predecessors) {
		// add to final call
		auto& predBlock = trace->getBlock(predecessor);

		// Validate that the predecessor block has operations
		if (predBlock.operations.empty()) {
			throw RuntimeException(fmt::format("Predecessor block {} has no operations", predecessor));
		}

		auto& lastOperation = predBlock.operations.back();
		if (lastOperation.op == Op::JMP || lastOperation.op == Op::CMP) {
			bool argumentAdded = false;
			for (auto& input : lastOperation.input) {
				if (auto blockRef = std::get_if<BlockRef>(&input)) {
					if (blockRef->block == block.blockId) {
						// Check if argument already exists to avoid duplicates
						auto it = std::find_if(blockRef->arguments.begin(), blockRef->arguments.end(),
						                       [&ref](const TypedValueRef& arg) { return arg.ref == ref.ref; });
						if (it == blockRef->arguments.end()) {
							blockRef->arguments.emplace_back(ref);
							argumentAdded = true;
						}
					}
				}
			}
			// Only mark for reprocessing if we actually added a new argument
			if (argumentAdded && processedBlocks.contains(predBlock.blockId)) {
				processedBlocks.erase(predBlock.blockId);
			}
		} else {
			throw RuntimeException(fmt::format("Last operation of predecessor block {} should be JMP or CMP, but is {}",
			                                   predecessor, static_cast<int>(lastOperation.op)));
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
	// Iterate over all blocks and eliminate the ASSIGN operations.
	// Build a mapping from assigned values to their source values, then replace all references.
	for (Block& block : trace->getBlocks()) {
		std::unordered_map<uint16_t, uint16_t> assignmentMap;

		// First pass: build the assignment map and handle transitive assignments
		for (auto& operation : block.operations) {
			if (operation.op == Op::ASSIGN) {
				// Validate ASSIGN operation has the expected input
				if (operation.input.empty()) {
					throw RuntimeException(fmt::format("ASSIGN operation in block {} has no inputs", block.blockId));
				}

				auto* valueRef = std::get_if<TypedValueRef>(&operation.input[0]);
				if (!valueRef) {
					throw RuntimeException(
					    fmt::format("ASSIGN operation in block {} has invalid input type", block.blockId));
				}

				// Handle transitive assignments: if the source is itself assigned, follow the chain
				auto foundAssignment = assignmentMap.find(valueRef->ref);
				if (foundAssignment != assignmentMap.end()) {
					assignmentMap[operation.resultRef.ref] = foundAssignment->second;
				} else {
					assignmentMap[operation.resultRef.ref] = valueRef->ref;
				}
			} else {
				// Second pass: replace all value references with their final assignments
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

		// Remove all ASSIGN operations now that references have been updated
		std::erase_if(block.operations, [](const auto& item) { return item.op == Op::ASSIGN; });
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
						valueRef->ref = foundAssignment->second;
					}
				} else if (auto* blockRef = std::get_if<BlockRef>(&input)) {
					for (auto& blockArgument : blockRef->arguments) {
						auto foundAssignment = blockArgumentMap.find(blockArgument.ref);
						if (foundAssignment != blockArgumentMap.end()) {
							blockArgument.ref = foundAssignment->second;
						}
					}
				}
			}
		}

		// Note: ASSIGN operations are already removed by removeAssignOperations()
		// No need to erase them again here
	}
}

} // namespace nautilus::tracing
