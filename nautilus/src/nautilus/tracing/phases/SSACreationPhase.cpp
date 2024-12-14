
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <nautilus/compiler/DumpHandler.hpp>
#include <nautilus/exceptions/RuntimeException.hpp>
#include <nautilus/tracing/ExecutionTrace.hpp>
#include <nautilus/tracing/phases/SSACreationPhase.hpp>
#include <random>
#include <ranges>
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


	auto createCompilationUnitID = []() -> compiler::CompilationUnitID {
		// Get the current time point
		// Create a timestamp string from the current time

		auto now = std::chrono::system_clock::now();
		std::string timestamp = fmt::format(fmt::runtime("{:%Y-%m-%d_%H-%M-%S}"), now);

		// Create a random device and generator
		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_int_distribution<> distribution(0, 15);

		// Generate a 7-character UUID
		std::string uuid;
		for (int i = 0; i < 7; ++i) {
			int random_number = distribution(generator);
			if (random_number < 10)
				uuid += std::to_string(random_number);
			else
				uuid += char('A' + random_number - 10);
		}

		// Concatenate timestamp and UUID
		return timestamp + "_#" + uuid;
	};

	compiler::CompilationUnitID compilationId = createCompilationUnitID();
	nautilus::engine::Options options;
	options.setOption("dump.file", true);
	options.setOption("dump.all", true);
	auto dumpHandler = compiler::DumpHandler(options, compilationId);
	// derive trace from function
	dumpHandler.dump("in_ssa_creation", "trace", [&]() { return trace->toString(); });


	// check arguments
	if (rootBlockNumberOfArguments != trace->getBlocks().front().arguments.size()) {
		throw RuntimeException(fmt::format("Wrong number of arguments in trace: expected {}, got {}\n",
		                                   rootBlockNumberOfArguments, trace->getBlocks().front().arguments.size()));
	}
	// sort arguments
	std::ranges::sort(trace->getBlocks().front().arguments);

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

void SSACreationPhase::SSACreationPhaseContext::processBlock(Block& block) {

	// Process the inputs of all operations in the current block
	for (int64_t i = block.operations.size() - 1; i >= 0; i--) {
		auto& operation = block.operations[i];
		// process input for each variable
		for (auto& input : operation.input) {
			if (auto* valueRef = std::get_if<TypedValueRef>(&input)) {
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

void SSACreationPhase::SSACreationPhaseContext::processValueRef(Block& block, TypedValueRef& ref, Type ref_type,
                                                                uint32_t operationIndex) {
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

void SSACreationPhase::SSACreationPhaseContext::processBlockRef(Block& block, BlockRef& blockRef,
                                                                uint32_t operationIndex) {
	// a block ref has a set of arguments, which are handled the same as all other
	// value references.
	for (auto& input : blockRef.arguments) {
		processValueRef(block, input, input.type, operationIndex);
	}
}

std::unordered_map<uint16_t, uint16_t> SSACreationPhase::SSACreationPhaseContext::creatingNewBlockIds(const std::set<uint16_t>& emptyBlocksToRemove) const {
	std::unordered_map<uint16_t, uint16_t> oldBlockIdsToNewBlockIds;
	uint16_t newBlockId = 0;
	// We have to iterate over all blocks and access a block via the block id, as a vector does not guarantee that the
	// block id is sorted
	std::vector<uint16_t> allBlockIds;
	for (const auto& block : trace->getBlocks()) {
		allBlockIds.push_back(block.blockId);
	}
	std::ranges::sort(allBlockIds);
	for (const auto blockId : allBlockIds) {
		const auto& block = trace->getBlock(blockId);
		if (not emptyBlocksToRemove.contains(block.blockId)) {
			oldBlockIdsToNewBlockIds[block.blockId] = newBlockId;
			++newBlockId;
		}
	}

	// For the empty blocks we have to set their new block id to the new block id of the next block
	for (const auto blockid : emptyBlocksToRemove) {
		oldBlockIdsToNewBlockIds[blockid] = oldBlockIdsToNewBlockIds[blockid + 1];
	}

	return oldBlockIdsToNewBlockIds;
}

void SSACreationPhase::SSACreationPhaseContext::removeAssignOperations() {
	// Iterate over all block and eliminate the ASSIGN operation.
	std::set<uint16_t> emptyBlocksToRemove;
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

		// If now a block has no more operations, we store it so that we can remove all empty blocks later
		if (block.operations.empty()) {
			emptyBlocksToRemove.insert(block.blockId);
		}
	}

	// Get the mapping of old block ids to new block ids. We need this to update the block references in the operations.
	auto oldBlockIdsToNewBlockIds = creatingNewBlockIds(emptyBlocksToRemove);

	// Remove all empty blocks
	std::erase_if(trace->getBlocks(), [&](const auto& item) { return emptyBlocksToRemove.contains(item.blockId); });

	// Now update all block references in the operations with its new block id
	for (Block& block : trace->getBlocks()) {
		block.blockId = oldBlockIdsToNewBlockIds[block.blockId];
		for (auto& operation : block.operations) {
			if (operation.op == Op::JMP || operation.op == Op::CMP) {
				for (auto& input : operation.input) {
					if (auto* blockRef = std::get_if<BlockRef>(&input)) {
						blockRef->block = oldBlockIdsToNewBlockIds[blockRef->block];

						blockRef->arguments.clear();
						const auto newBlockRefArgs = trace->getBlock(blockRef->block).arguments;
						for (const auto blockArgument : newBlockRefArgs) {
							blockRef->arguments.emplace_back(blockArgument);
						}
					}
				}
			}
		}
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
