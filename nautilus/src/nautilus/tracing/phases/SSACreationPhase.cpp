
#include <nautilus/exceptions/RuntimeException.hpp>
#include <nautilus/logging.hpp>
#include <nautilus/tracing/ExecutionTrace.hpp>
#include <nautilus/tracing/phases/SSACreationPhase.hpp>
#include <unordered_map>

namespace nautilus::tracing {

std::shared_ptr<ExecutionTrace> SSACreationPhase::apply(std::shared_ptr<ExecutionTrace> trace) {
	// Initialize a new context and perform the inference of the SSA values
	auto phaseContext = SSACreationPhaseContext(std::move(trace));
	return phaseContext.process();
};

SSACreationPhase::SSACreationPhaseContext::SSACreationPhaseContext(std::shared_ptr<ExecutionTrace> trace) : trace(std::move(trace)) {
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
		auto snap = Snapshot();
		returnOpBlock.operations[returnOp.operationIndex] = TraceOperation(snap, ASSIGN, defaultReturnOp.resultType, defaultReturnOp.resultRef, {returnValue.resultRef});
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
		throw RuntimeException(fmt::format("Wrong number of arguments in trace: expected {}, got {}\n", rootBlockNumberOfArguments, trace->getBlocks().front().arguments.size()));
	}
	// sort arguments
	std::sort(trace->getBlocks().front().arguments.begin(), trace->getBlocks().front().arguments.end());

	return std::move(trace);
}

bool SSACreationPhase::SSACreationPhaseContext::isLocalValueRef(Block& block, value_ref& ref, Type, uint32_t operationIndex) {
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

void SSACreationPhase::SSACreationPhaseContext::processBlock(Block& block) {

	// Process the inputs of all operations in the current block
	for (int64_t i = block.operations.size() - 1; i >= 0; i--) {
		auto& operation = block.operations[i];
		// process input for each variable
		if (operation.op == RETURN && operation.resultType != Type::v) {
			processValueRef(block, operation.resultRef, operation.resultType, i);
		}
		for (auto& input : operation.input) {
			if (auto* valueRef = std::get_if<value_ref>(&input)) {
				// set op type
				processValueRef(block, *valueRef, operation.resultType, i);
			} else if (auto* blockRef = std::get_if<BlockRef>(&input)) {
				processBlockRef(block, *blockRef, i);
			} else if (auto* fcallRef = std::get_if<FunctionCall>(&input)) {
				for (auto valueRef : fcallRef->arguments) {
					processValueRef(block, valueRef, valueRef.type, i);
				}
			}
		}

		if (operation.op == STORE) {
			processValueRef(block, operation.resultRef, operation.resultType, i);
		}
	}
	processedBlocks.emplace(block.blockId);
	// Recursively process the predecessors of this block
	// If the current block is a control-flow merge it may have multiple
	// predecessors. We avoid visiting them again by checking the processedBlocks
	// set.
	for (auto pred : block.predecessors) {
		auto& predBlock = trace->getBlock(pred);
		if (!processedBlocks.contains(pred)) {
			processBlock(predBlock);
		}
	}
}

void SSACreationPhase::SSACreationPhaseContext::processValueRef(Block& block, value_ref& ref, Type ref_type, uint32_t operationIndex) {
	if (isLocalValueRef(block, ref, ref_type, operationIndex)) {
		// variable is a local ref -> don't do anything as the value is defined in
		// the current block
	} else {
		// The valeRef references a different block, so we have to add it to the
		// local arguments and append it to the pre-predecessor calls
		block.addArgument(ref);
		// add to parameters in parent blocks
		for (auto& predecessor : block.predecessors) {
			// add to final call
			auto& predBlock = trace->getBlock(predecessor);
			auto& lastOperation = predBlock.operations.back();
			if (lastOperation.op == Op::JMP || lastOperation.op == Op::CMP) {
				for (auto& input : lastOperation.input) {
					if (auto blockRef = std::get_if<BlockRef>(&input)) {
						if (blockRef->block == block.blockId) {
							// TODO check if we contain the type already.
							blockRef->arguments.emplace_back(ref);
							// we changed the block an arguments, thus we have to revisit it.
							if (processedBlocks.contains(predBlock.blockId)) {
								processedBlocks.erase(predBlock.blockId);
							}
						}
					}
				}
			} else {
				// NES_ERROR(trace->toString());
				// NES_THROW_RUNTIME_ERROR("Last operation of pred block should be JMP
				// or CMP");
			}
		}
	}
}

void SSACreationPhase::SSACreationPhaseContext::processBlockRef(Block& block, BlockRef& blockRef, uint32_t operationIndex) {
	// a block ref has a set of arguments, which are handled the same as all other
	// value references.
	for (auto& input : blockRef.arguments) {
		processValueRef(block, input, input.type, operationIndex);
	}
};

void SSACreationPhase::SSACreationPhaseContext::removeAssignOperations() {
	// Iterate over all block and eliminate the ASSIGN operation.
	for (Block& block : trace->getBlocks()) {
		std::unordered_map<uint16_t, uint16_t> assignmentMap;
		for (uint64_t i = 0; i < block.operations.size(); i++) {
			auto& operation = block.operations[i];
			if (operation.op == Op::CMP) {
				if (auto valueRef = &operation.resultRef) {
					auto foundAssignment = assignmentMap.find(valueRef->ref);
					if (foundAssignment != assignmentMap.end()) {
						// todo check assignment
						valueRef->ref = foundAssignment->second;
					}
				}
			}
			if (operation.op == Op::ASSIGN) {
				auto valueRef = get<value_ref>(operation.input[0]);
				auto foundAssignment = assignmentMap.find(valueRef.ref);
				if (foundAssignment != assignmentMap.end()) {
					assignmentMap[operation.resultRef.ref] = assignmentMap[valueRef.ref];
				} else {
					assignmentMap[operation.resultRef.ref] = get<value_ref>(operation.input[0]).ref;
				}
			} else if (operation.op == Op::RETURN) {
				auto foundAssignment = assignmentMap.find(operation.resultRef.ref);
				if (foundAssignment != assignmentMap.end()) {
					operation.resultRef.ref = foundAssignment->second;
				}
			} else {
				for (auto& input : operation.input) {
					if (auto* valueRef = std::get_if<value_ref>(&input)) {
						auto foundAssignment = assignmentMap.find(valueRef->ref);
						if (foundAssignment != assignmentMap.end()) {
							// todo check assignment
							valueRef->ref = foundAssignment->second;
							// valueRef->blockId = foundAssignment->second.blockId;
							// valueRef->operationId = foundAssignment->second.operationId;
						}
					} else if (auto* blockRef = std::get_if<BlockRef>(&input)) {
						for (auto& blockArgument : blockRef->arguments) {
							auto foundAssignment = assignmentMap.find(blockArgument.ref);
							if (foundAssignment != assignmentMap.end()) {
								// valueRef = &foundAssignment->second;
								blockArgument.ref = foundAssignment->second;
								// blockArgument.operationId =
								// foundAssignment->second.operationId;
							}
						}
					} else if (auto* fcallRef = std::get_if<FunctionCall>(&input)) {
						for (auto& funcArg : fcallRef->arguments) {
							auto foundAssignment = assignmentMap.find(funcArg.ref);
							if (foundAssignment != assignmentMap.end()) {
								// valueRef = &foundAssignment->second;
								funcArg.ref = foundAssignment->second;
								// blockArgument.operationId =
								// foundAssignment->second.operationId;
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
				if (auto* valueRef = std::get_if<value_ref>(&input)) {
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
