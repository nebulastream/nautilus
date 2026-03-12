
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
	// check if operation was already seen -> control-flow merge or loop
	auto tagIter = tagMap.find(snapshot);
	if (tagIter != tagMap.end()) {
		processControlFlowMerge(tagIter->second);
		return false;
	}
	return true;
}

void ExecutionTrace::addReturn(Snapshot& snapshot, Type resultType, const TypedValueRef& ref) {
	auto& operations = blocks[currentBlockIndex].operations;
	auto op = Op::RETURN;
	if (ref.type == Type::v) {
		operations.emplace_back(op, resultType, TypedValueRef(0, Type::v), std::vector<InputVariant> {});
	} else {
		operations.emplace_back(op, resultType, TypedValueRef(0, Type::v), std::vector<InputVariant> {ref});
	}
	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);

	returnRefs.emplace_back(operationIdentifier);
}

TypedValueRef& ExecutionTrace::addAssignmentOperation(Snapshot& snapshot, const TypedValueRef& targetRef,
                                                      const TypedValueRef& srcRef, Type resultType) {
	auto& operations = blocks[currentBlockIndex].operations;
	auto op = ASSIGN;
	auto& operation = operations.emplace_back(op, resultType, targetRef, std::vector<InputVariant> {srcRef});
	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
	return operation.resultRef;
}

TypedValueRef& ExecutionTrace::addOperationWithResult(Snapshot& snapshot, Op& operation, Type& resultType,
                                                      std::vector<InputVariant> inputs) {
	auto& operations = blocks[currentBlockIndex].operations;
	auto& to =
	    operations.emplace_back(operation, resultType, TypedValueRef(getNextValueRef(), resultType), std::move(inputs));

	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
	return to.resultRef;
}

// Adds a comparison operation to the execution trace
// This consists of a snapshot, the comparison input, two blocks for true and false branches, and the branch probability
void ExecutionTrace::addCmpOperation(Snapshot& snapshot, const TypedValueRef& condition, const double probability) {
	// create if and else blocks
	auto trueBlock = createBlock();
	getBlock(trueBlock).predecessors.emplace_back(getCurrentBlockIndex());
	auto falseBlock = createBlock();
	getBlock(falseBlock).predecessors.emplace_back(getCurrentBlockIndex());
	auto& operations = blocks[currentBlockIndex].operations;
	auto op = CMP;
	operations.emplace_back(
	    op, Type::v, TypedValueRef(getNextValueRef(), Type::v),
	    std::vector<InputVariant> {condition, BlockRef(trueBlock), BlockRef(falseBlock), probability});
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
		auto& nextBlock = std::get<BlockRef>(currentOp.input[0]);
		setCurrentBlock(nextBlock.block);
	}
}

TraceOperation& ExecutionTrace::getCurrentOperation() {
	if (currentOperationIndex >= getCurrentBlock().operations.size()) {
		throw RuntimeException("Current operation index out of bounds: " + std::to_string(currentOperationIndex));
	}
	while (getCurrentBlock().operations[currentOperationIndex].op == JMP) {
		auto& nextBlock = std::get<BlockRef>(getCurrentBlock().operations[currentOperationIndex].input[0]);
		setCurrentBlock(nextBlock.block);
		if (currentOperationIndex >= getCurrentBlock().operations.size()) {
			throw RuntimeException("Current operation index out of bounds after JMP: " +
			                       std::to_string(currentOperationIndex));
		}
	}
	return getCurrentBlock().operations[currentOperationIndex];
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

	// 1. move operations to new block
	// move everything from the reference block between opId and end to merge block
	for (uint32_t opIndex = oi.operationIndex; opIndex < referenceBlock.operations.size(); opIndex++) {
		mergeBlock.addOperation(std::move(referenceBlock.operations[opIndex]));
	}

	// update return refs that pointed into the moved range
	for (auto& returnRef : returnRefs) {
		if (returnRef.blockIndex == referenceBlock.blockId && returnRef.operationIndex >= oi.operationIndex) {
			returnRef = {mergedBlockId, returnRef.operationIndex - oi.operationIndex};
		}
	}

	// update tag map entries that pointed into the moved range
	for (auto& [snapshot, id] : tagMap) {
		if (id.blockIndex == referenceBlock.blockId && id.operationIndex >= oi.operationIndex) {
			id = {mergedBlockId, id.operationIndex - oi.operationIndex};
		}
	}

	// remove content beyond opID
	referenceBlock.operations.erase(referenceBlock.operations.begin() + oi.operationIndex,
	                                referenceBlock.operations.end());

	// add jump from referenced block to merge block
	auto mergeBlockRef = BlockRef(mergedBlockId);
	referenceBlock.addOperation({Op::JMP, {mergeBlockRef}});

	// add jump from current block to merge block
	currentBlock.addOperation({Op::JMP, {mergeBlockRef}});

	mergeBlock.predecessors.emplace_back(oi.blockIndex);
	mergeBlock.predecessors.emplace_back(currentBlockIndex);
	setCurrentBlock(mergedBlockId);

	// update predecessors of merge merge block
	auto& lastMergeOperation = mergeBlock.operations[mergeBlock.operations.size() - 1];
	if (lastMergeOperation.op == Op::CMP || lastMergeOperation.op == Op::JMP) {
		for (auto& input : lastMergeOperation.input) {
			if (auto blockRef = std::get_if<BlockRef>(&input)) {
				auto& blockPredecessor = getBlock(blockRef->block).predecessors;
				std::replace(blockPredecessor.begin(), blockPredecessor.end(), oi.blockIndex, mergedBlockId);
				std::replace(blockPredecessor.begin(), blockPredecessor.end(), currentBlockIndex, mergedBlockId);
			}
		}
	}
	return mergeBlock;
}

TypedValueRef& ExecutionTrace::setArgument(Type type, size_t index) {
	++lastValueRef;
	ValueRef argRef = index + 1;
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

ValueRef ExecutionTrace::getNextValueRef() {
	return ++lastValueRef;
}

operation_identifier ExecutionTrace::getNextOperationIdentifier() {
	currentOperationIndex = getCurrentBlock().operations.size() - 1;
	return {currentBlockIndex, currentOperationIndex};
}

void ExecutionTrace::resetExecution() {
	currentBlockIndex = 0;
	currentOperationIndex = 0;
}

const std::vector<TypedValueRef>& ExecutionTrace::getArguments() {
	return blocks[0].arguments;
}

void ExecutionTrace::addTag(Snapshot& snapshot, operation_identifier& identifier) {
	tagMap[snapshot] = identifier;
}

} // namespace nautilus::tracing

std::string nautilus::tracing::ExecutionTrace::toString() const {
	return fmt::to_string(*this);
}

namespace fmt {
template <>
struct formatter<nautilus::tracing::ExecutionTrace> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::ExecutionTrace& trace, format_context& ctx) -> format_context::iterator;
};

template <>
struct formatter<nautilus::tracing::Block> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::Block& trace, format_context& ctx) -> format_context::iterator;
};

template <>
struct formatter<nautilus::tracing::TraceOperation> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::TraceOperation& trace, format_context& ctx) -> format_context::iterator;
};

auto formatter<nautilus::tracing::ExecutionTrace>::format(const nautilus::tracing::ExecutionTrace& trace,
                                                          fmt::format_context& ctx) -> format_context::iterator {
	auto out = ctx.out();
	for (size_t i = 0; i < trace.blocks.size(); i++) {
		fmt::format_to(out, "B{}{}", i, trace.blocks[i]);
	}
	return out;
}

auto formatter<nautilus::tracing::Block>::format(const nautilus::tracing::Block& block,
                                                 format_context& ctx) -> format_context::iterator {
	auto out = ctx.out();
	fmt::format_to(out, "(");
	for (size_t i = 0; i < block.arguments.size(); i++) {
		if (i != 0) {
			fmt::format_to(out, ",");
		}
		fmt::format_to(out, "${}:{}", block.arguments[i].ref, toString(block.arguments[i].type));
	}
	fmt::format_to(out, ")");
	if (block.type == nautilus::tracing::Block::Type::ControlFlowMerge) {
		fmt::format_to(out, " ControlFlowMerge");
	}
	fmt::format_to(out, "\n");
	for (const auto& operation : block.operations) {
		fmt::format_to(out, "{}\n", operation);
	}
	return out;
}

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
struct formatter<nautilus::tracing::BlockRef> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::BlockRef& ref, format_context& ctx) -> format_context::iterator {
		auto out = ctx.out();
		fmt::format_to(out, "B{}(", ref.block);
		for (size_t i = 0; i < ref.arguments.size(); i++) {
			if (i != 0) {
				fmt::format_to(out, ",");
			}
			fmt::format_to(out, "{}", ref.arguments[i]);
		}
		fmt::format_to(out, ")");
		return out;
	}
};

template <>
struct formatter<nautilus::tracing::FunctionCall> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::FunctionCall& call, format_context& ctx) -> format_context::iterator {
		auto out = ctx.out();
		if (nautilus::log::options::getLogAddresses()) {
			fmt::format_to(out, "{}(", call.functionName);
		} else {
			fmt::format_to(out, "func_*(");
		}

		for (size_t i = 0; i < call.arguments.size(); i++) {
			if (i != 0) {
				fmt::format_to(out, ",");
			}
			fmt::format_to(out, "{}", call.arguments[i]);
		}
		fmt::format_to(out, ")");
		return out;
	}
};

template <>
struct formatter<nautilus::ConstantLiteral> : formatter<std::string_view> {
	auto format(nautilus::ConstantLiteral c, format_context& ctx) const -> format_context::iterator;
};

auto formatter<nautilus::tracing::TraceOperation>::format(const nautilus::tracing::TraceOperation& operation,
                                                          format_context& ctx) -> format_context::iterator {
	auto out = ctx.out();
	fmt::format_to(out, "\t{}\t", toString(operation.op));
	fmt::format_to(out, "{}\t", operation.resultRef);
	for (const auto& opInput : operation.input) {
		if (auto inputRef = std::get_if<nautilus::tracing::TypedValueRef>(&opInput)) {
			fmt::format_to(out, "{}\t", *inputRef);
		} else if (auto blockRef = std::get_if<nautilus::tracing::BlockRef>(&opInput)) {
			fmt::format_to(out, "{}\t", *blockRef);
		} else if (auto fCall = std::get_if<nautilus::tracing::FunctionCall>(&opInput)) {
			fmt::format_to(out, "{}\t", *fCall);
		} else if (auto constant = std::get_if<nautilus::ConstantLiteral>(&opInput)) {
			fmt::format_to(out, "{}", *constant);
		}
	}
	fmt::format_to(out, ":{}", toString(operation.resultType));
	return out;
}

} // namespace fmt
