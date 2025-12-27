
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/symbolic_execution/TraceTerminationException.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <nautilus/config.hpp>
#include <nautilus/exceptions/RuntimeException.hpp>
#include <nautilus/logging.hpp>

namespace nautilus::tracing {

ExecutionTrace::ExecutionTrace() : currentBlockIndex(0), currentOperationIndex(0), blocks() {
	createBlock();
}

Block& ExecutionTrace::getBlock(uint16_t blockIndex) {
	if (blockIndex >= blocks.size()) {
		throw RuntimeException("Block index out of bounds: " + std::to_string(blockIndex));
	}
	return blocks[blockIndex];
}

uint16_t ExecutionTrace::getCurrentBlockIndex() const {
	return currentBlockIndex;
}

Block& ExecutionTrace::getCurrentBlock() {
	if (currentBlockIndex >= blocks.size()) {
		throw RuntimeException("Current block index out of bounds: " + std::to_string(currentBlockIndex));
	}
	return blocks[currentBlockIndex];
}

void ExecutionTrace::setCurrentBlock(uint16_t index) {
	if (index >= blocks.size()) {
		throw RuntimeException("Cannot set current block to out of bounds index: " + std::to_string(index));
	}
	currentOperationIndex = 0;
	currentBlockIndex = index;
}

std::vector<Block>& ExecutionTrace::getBlocks() {
	return blocks;
}

bool ExecutionTrace::checkTag(Snapshot& snapshot) {
	// check if operation is in global map -> we have a repeating operation ->
	// this is a control-flow merge
	auto globalTabIter = globalTagMap.find(snapshot);
	if (globalTabIter != globalTagMap.end()) {
		auto& ref = globalTabIter->second;
		processControlFlowMerge(ref);
		return false;
	}

	// check if we visited the same operation in this execution -> loop
	auto localTagIter = localTagMap.find(snapshot);
	if (localTagIter != localTagMap.end()) {
		auto& ref = localTagIter->second;
		processControlFlowMerge(ref);
		return false;
	}
	return true;
}

void ExecutionTrace::addReturn(Snapshot& snapshot, Type resultType, const TypedValueRef& ref) {
	if (blocks.empty()) {
		createBlock();
	}
	auto& operations = blocks[currentBlockIndex].operations;
	auto op = Op::RETURN;
	if (ref.type == Type::v) {
		operations.emplace_back(snapshot, op, resultType, TypedValueRef(0, Type::v));
	} else {
		operations.emplace_back(snapshot, op, resultType, TypedValueRef(0, Type::v), ref);
	}
	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);

	returnRefs.emplace_back(operationIdentifier);
}

TypedValueRef& ExecutionTrace::addAssignmentOperation(Snapshot& snapshot, const TypedValueRef& targetRef,
                                                      const TypedValueRef& srcRef, Type resultType) {
	if (blocks.empty()) {
		createBlock();
	}
	auto& operations = blocks[currentBlockIndex].operations;
	auto op = ASSIGN;
	auto& operation = operations.emplace_back(snapshot, op, resultType, targetRef, srcRef);
	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
	return operation.resultRef;
}

void ExecutionTrace::addOperation(Snapshot& snapshot, Op& operation) {
	if (blocks.empty()) {
		createBlock();
	}
	auto& operations = blocks[currentBlockIndex].operations;
	operations.emplace_back(snapshot, operation, Type::v, TypedValueRef(0, Type::v));
}

void ExecutionTrace::addOperation(Snapshot& snapshot, Op& operation, InputVariant input0) {
	if (blocks.empty()) {
		createBlock();
	}
	auto& operations = blocks[currentBlockIndex].operations;
	operations.emplace_back(snapshot, operation, Type::v, TypedValueRef(0, Type::v), input0);
}

void ExecutionTrace::addOperation(Snapshot& snapshot, Op& operation, InputVariant input0, InputVariant input1) {
	if (blocks.empty()) {
		createBlock();
	}
	auto& operations = blocks[currentBlockIndex].operations;
	operations.emplace_back(snapshot, operation, Type::v, TypedValueRef(0, Type::v), input0, input1);
}

void ExecutionTrace::addOperation(Snapshot& snapshot, Op& operation, InputVariant input0, InputVariant input1,
                                  InputVariant input2) {
	if (blocks.empty()) {
		createBlock();
	}
	auto& operations = blocks[currentBlockIndex].operations;
	operations.emplace_back(snapshot, operation, Type::v, TypedValueRef(0, Type::v), input0, input1, input2);
}

TypedValueRef& ExecutionTrace::addOperationWithResult(Snapshot& snapshot, Op& operation, Type& resultType) {
	if (blocks.empty()) {
		createBlock();
	}

	auto& operations = blocks[currentBlockIndex].operations;
	auto& to = operations.emplace_back(snapshot, operation, resultType, TypedValueRef(getNextValueRef(), resultType));

	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
	return to.resultRef;
}

TypedValueRef& ExecutionTrace::addOperationWithResult(Snapshot& snapshot, Op& operation, Type& resultType,
                                                      InputVariant input0) {
	if (blocks.empty()) {
		createBlock();
	}

	auto& operations = blocks[currentBlockIndex].operations;
	auto& to =
	    operations.emplace_back(snapshot, operation, resultType, TypedValueRef(getNextValueRef(), resultType), input0);

	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
	return to.resultRef;
}

TypedValueRef& ExecutionTrace::addOperationWithResult(Snapshot& snapshot, Op& operation, Type& resultType,
                                                      InputVariant input0, InputVariant input1) {
	if (blocks.empty()) {
		createBlock();
	}

	auto& operations = blocks[currentBlockIndex].operations;
	auto& to = operations.emplace_back(snapshot, operation, resultType, TypedValueRef(getNextValueRef(), resultType),
	                                   input0, input1);

	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
	return to.resultRef;
}

TypedValueRef& ExecutionTrace::addOperationWithResult(Snapshot& snapshot, Op& operation, Type& resultType,
                                                      InputVariant input0, InputVariant input1, InputVariant input2) {
	if (blocks.empty()) {
		createBlock();
	}

	auto& operations = blocks[currentBlockIndex].operations;
	auto& to = operations.emplace_back(snapshot, operation, resultType, TypedValueRef(getNextValueRef(), resultType),
	                                   input0, input1, input2);

	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
	return to.resultRef;
}

// Adds a comparison operation to the execution trace
// This consists of a snapshot, the comparison input, two blocks for true and false branches, and the branch probability
void ExecutionTrace::addCmpOperation(Snapshot& snapshot, const TypedValueRef& condition, const float probability) {
	if (blocks.empty()) {
		createBlock();
	}

	// create if and else blocks
	auto trueBlock = createBlock();
	getBlock(trueBlock).predecessors.emplace_back(getCurrentBlockIndex());
	auto falseBlock = createBlock();
	getBlock(falseBlock).predecessors.emplace_back(getCurrentBlockIndex());
	auto& operations = blocks[currentBlockIndex].operations;
	auto trueBlockRefId = addBlockRef(BlockRef(trueBlock));
	auto falseBlockRefId = addBlockRef(BlockRef(falseBlock));
	operations.emplace_back(snapshot, CMP, Type::v, TypedValueRef(getNextValueRef(), Type::v), condition,
	                        trueBlockRefId, falseBlockRefId, probability);
	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
}

void ExecutionTrace::nextOperation() {
	this->currentOperationIndex++;
	auto& block = getCurrentBlock();
	if (currentOperationIndex >= block.operations.size()) {
		throw RuntimeException("Operation index out of bounds: " + std::to_string(currentOperationIndex));
	}
	auto& currentOp = block.operations[currentOperationIndex];
	if (currentOp.op == JMP) {
		auto blockRefId = std::get<BlockRefId>(currentOp.input[0]);
		auto& nextBlock = getBlockRef(blockRefId);
		setCurrentBlock(nextBlock.block);
	}
}

TraceOperation& ExecutionTrace::getCurrentOperation() {
	auto& block = getCurrentBlock();
	if (currentOperationIndex >= block.operations.size()) {
		throw RuntimeException("Current operation index out of bounds: " + std::to_string(currentOperationIndex));
	}
	while (block.operations[currentOperationIndex].op == JMP) {
		auto blockRefId = std::get<BlockRefId>(block.operations[currentOperationIndex].input[0]);
		auto& nextBlock = getBlockRef(blockRefId);
		setCurrentBlock(nextBlock.block);
		block = getCurrentBlock();
		if (currentOperationIndex >= block.operations.size()) {
			throw RuntimeException("Current operation index out of bounds after JMP: " +
			                       std::to_string(currentOperationIndex));
		}
	}
	return block.operations[currentOperationIndex];
}

uint16_t ExecutionTrace::createBlock() {
	auto& block = blocks.emplace_back(blocks.size());
	return block.blockId;
}

Block& ExecutionTrace::processControlFlowMerge(operation_identifier oi) {
	if (oi.blockIndex == currentBlockIndex) {
		throw RuntimeException("Invalid trace. This is maybe caused by a constant loop.");
	}

	// create new merge block
	auto mergedBlockId = createBlock();
	// perform a control flow merge and merge the current block with operations in
	// some other block.
	auto& referenceBlock = blocks[oi.blockIndex];
	auto& currentBlock = blocks[currentBlockIndex];

	auto& mergeBlock = getBlock(mergedBlockId);
	mergeBlock.type = Block::Type::ControlFlowMerge;

	// 1. move operation to new block
	// move everything from the reference block between opId and end to merge block
	for (uint32_t opIndex = oi.operationIndex; opIndex < referenceBlock.operations.size(); opIndex++) {
		auto& sourceOperation = referenceBlock.operations[opIndex];
		// Save values needed after move
		auto opType = sourceOperation.op;
		auto opTag = sourceOperation.tag;
		auto operationReference = mergeBlock.addOperation(std::move(sourceOperation));
		// update in global and local tag map

		if (opType == RETURN) {
			for (auto& returnRef : returnRefs) {
				if (returnRef.blockIndex == referenceBlock.blockId && returnRef.operationIndex == opIndex) {
					returnRef = operationReference;
				}
			}
		} else {
			globalTagMap[opTag] = operationReference;
			localTagMap[opTag] = operationReference;
		}
	}

	// remove content beyond opID
	referenceBlock.operations.erase(referenceBlock.operations.begin() + oi.operationIndex,
	                                referenceBlock.operations.end());

	// add jump from referenced block to merge block
	auto referenceBlockRefId = addBlockRef(BlockRef(mergedBlockId));
	referenceBlock.addOperation({Op::JMP, {referenceBlockRefId}});

	// add jump from current block to merge block
	auto currentBlockRefId = addBlockRef(BlockRef(mergedBlockId));
	currentBlock.addOperation({Op::JMP, {currentBlockRefId}});

	mergeBlock.predecessors.emplace_back(oi.blockIndex);
	mergeBlock.predecessors.emplace_back(currentBlockIndex);
	setCurrentBlock(mergedBlockId);

	// update predecessors of merge merge block
	auto& lastMergeOperation = mergeBlock.operations[mergeBlock.operations.size() - 1];
	if (lastMergeOperation.op == Op::CMP || lastMergeOperation.op == Op::JMP) {
		for (auto& input : lastMergeOperation.input) {
			if (auto blockRefId = std::get_if<BlockRefId>(&input)) {

				auto& blockPredecessor = getBlock(getBlockRef(*blockRefId).block).predecessors;
				std::replace(blockPredecessor.begin(), blockPredecessor.end(), oi.blockIndex, mergedBlockId);
				std::replace(blockPredecessor.begin(), blockPredecessor.end(), currentBlockIndex, mergedBlockId);
			}
		}
	}
	return mergeBlock;
}

TypedValueRef& ExecutionTrace::setArgument(Type type, size_t index) {
	++lastValueRef;
	uint16_t argRef = index + 1;
	auto& arguments = blocks[0].arguments;
	if (arguments.size() < argRef) {
		arguments.resize(argRef);
	}
	// arguments[index] = {argRef, type};
	arguments[index] = TypedValueRef(argRef, type);
	return arguments[index];
}

std::vector<operation_identifier> ExecutionTrace::getReturn() {
	return returnRefs;
}

uint16_t ExecutionTrace::getNextValueRef() {
	return ++lastValueRef;
}

operation_identifier ExecutionTrace::getNextOperationIdentifier() {
	currentOperationIndex = getCurrentBlock().operations.size() - 1;
	return {currentBlockIndex, (uint16_t) currentOperationIndex};
}

void ExecutionTrace::resetExecution() {
	currentBlockIndex = 0;
	currentOperationIndex = 0;
	globalTagMap.merge(localTagMap);
	localTagMap.clear();
}

const std::vector<TypedValueRef>& ExecutionTrace::getArguments() {
	return blocks[0].arguments;
}

void ExecutionTrace::addTag(Snapshot& snapshot, operation_identifier& identifier) {
	globalTagMap[snapshot] = identifier;
	localTagMap[snapshot] = identifier;
}

} // namespace nautilus::tracing

std::string nautilus::tracing::ExecutionTrace::toString() const {
	return fmt::to_string(*this);
}

namespace fmt {

// Forward declare formatters needed by ExecutionTrace formatter
template <>
struct formatter<nautilus::tracing::TypedValueRef> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::TypedValueRef& typeValRef,
	                   format_context& ctx) -> format_context::iterator {
		auto out = ctx.out();
		fmt::format_to(out, "${}", typeValRef.ref);
		return out;
	}
};

template <>
struct formatter<nautilus::ConstantLiteral> : formatter<std::string_view> {
	auto format(nautilus::ConstantLiteral c, format_context& ctx) const -> format_context::iterator;
};

template <>
struct formatter<nautilus::tracing::ExecutionTrace> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::ExecutionTrace& trace, format_context& ctx) -> format_context::iterator;
};

auto formatter<nautilus::tracing::ExecutionTrace>::format(const nautilus::tracing::ExecutionTrace& trace,
                                                          fmt::format_context& ctx) -> format_context::iterator {
	auto out = ctx.out();
	for (size_t i = 0; i < trace.blocks.size(); i++) {
		fmt::format_to(out, "B{}", i);
		// Format block with access to trace for lookups
		const auto& block = trace.blocks[i];
		fmt::format_to(out, "(");
		for (size_t j = 0; j < block.arguments.size(); j++) {
			if (j != 0) {
				fmt::format_to(out, ",");
			}
			fmt::format_to(out, "${}:{}", block.arguments[j].ref, toString(block.arguments[j].type));
		}
		fmt::format_to(out, ")");
		if (block.type == nautilus::tracing::Block::Type::ControlFlowMerge) {
			fmt::format_to(out, " ControlFlowMerge");
		}
		fmt::format_to(out, "\n");
		for (const auto& operation : block.operations) {
			// Format operation with access to trace
			fmt::format_to(out, "\t{}\t", toString(operation.op));
			fmt::format_to(out, "{}\t", operation.resultRef);
			for (const auto& opInput : operation.input) {
				if (auto inputRef = std::get_if<nautilus::tracing::TypedValueRef>(&opInput)) {
					fmt::format_to(out, "{}\t", *inputRef);
				} else if (auto blockRefId = std::get_if<nautilus::tracing::BlockRefId>(&opInput)) {
					const auto& blockRef = trace.blockRefs[blockRefId->id];
					fmt::format_to(out, "B{}(", blockRef.block);
					for (size_t k = 0; k < blockRef.arguments.size(); k++) {
						if (k != 0) {
							fmt::format_to(out, ",");
						}
						fmt::format_to(out, "{}", blockRef.arguments[k]);
					}
					fmt::format_to(out, ")\t");
				} else if (auto constantId = std::get_if<nautilus::tracing::ConstantLiteralId>(&opInput)) {
					auto& constant = trace.constantLiterals[constantId->id];
					fmt::format_to(out, "{}", constant);
				} else if (auto functionCallId = std::get_if<nautilus::tracing::FunctionCallId>(&opInput)) {
					const auto& functionCall = trace.functionCalls[functionCallId->id];

					if (nautilus::log::options::getLogAddresses()) {
						fmt::format_to(out, "{}(", functionCall.functionName);
					} else {
						fmt::format_to(out, "func_*(");
					}
					for (size_t k = 0; k < functionCall.arguments.size(); k++) {
						if (k != 0) {
							fmt::format_to(out, ",");
						}
						fmt::format_to(out, "{}", functionCall.arguments[k]);
					}
					fmt::format_to(out, ")\t");
				} else if (auto probability = std::get_if<nautilus::tracing::BranchProbability>(&opInput)) {
					// Branch probability is typically not printed in trace format
					(void) probability; // Suppress unused warning
				}
			}
			fmt::format_to(out, ":{}\n", toString(operation.resultType));
		}
	}
	return out;
}

} // namespace fmt
