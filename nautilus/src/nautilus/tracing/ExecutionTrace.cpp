
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/symbolic_execution/TraceTerminationException.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <nautilus/config.hpp>
#include <nautilus/logging.hpp>

namespace nautilus::tracing {

ExecutionTrace::ExecutionTrace() : currentBlockIndex(0), currentOperationIndex(0), blocks() {
	createBlock();
}

Block& ExecutionTrace::getBlock(uint16_t blockIndex) {
	return blocks[blockIndex];
}

uint16_t ExecutionTrace::getCurrentBlockIndex() const {
	return currentBlockIndex;
}

Block& ExecutionTrace::getCurrentBlock() {
	return blocks[currentBlockIndex];
}

void ExecutionTrace::setCurrentBlock(uint16_t index) {
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
		operations.emplace_back(snapshot, op, resultType, TypedValueRef(0, Type::v), std::vector<InputVariant> {});
	} else {
		operations.emplace_back(snapshot, op, resultType, TypedValueRef(0, Type::v), std::vector<InputVariant> {ref});
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
	auto& operation = operations.emplace_back(snapshot, op, resultType, targetRef, std::vector<InputVariant> {srcRef});
	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
	return operation.resultRef;
}

void ExecutionTrace::addOperation(Snapshot& snapshot, Op& operation, std::vector<InputVariant>&& inputs) {
	if (blocks.empty()) {
		createBlock();
	}
	auto& operations = blocks[currentBlockIndex].operations;
	operations.emplace_back(snapshot, operation, Type::v, TypedValueRef(0, Type::v),
	                        std::vector<InputVariant> {inputs});
}

TypedValueRef& ExecutionTrace::addOperationWithResult(Snapshot& snapshot, Op& operation, Type& resultType,
                                                      std::vector<InputVariant>&& inputs) {
	if (blocks.empty()) {
		createBlock();
	}

	auto& operations = blocks[currentBlockIndex].operations;
	auto& to = operations.emplace_back(snapshot, operation, resultType, TypedValueRef(getNextValueRef(), resultType),
	                                   std::forward<std::vector<InputVariant>>(inputs));

	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
	return to.resultRef;
}

void ExecutionTrace::addCmpOperation(Snapshot& snapshot, const TypedValueRef& inputs) {
	if (blocks.empty()) {
		createBlock();
	}

	// create if and else blocks
	auto trueBlock = createBlock();
	getBlock(trueBlock).predecessors.emplace_back(getCurrentBlockIndex());
	auto falseBlock = createBlock();
	getBlock(falseBlock).predecessors.emplace_back(getCurrentBlockIndex());
	auto& operations = blocks[currentBlockIndex].operations;
	operations.emplace_back(snapshot, CMP, Type::v, TypedValueRef(getNextValueRef(), Type::v),
	                        std::vector<InputVariant> {inputs, BlockRef(trueBlock), BlockRef(falseBlock)});
	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
}

void ExecutionTrace::nextOperation() {
	this->currentOperationIndex++;
	auto& currentOp = getCurrentBlock().operations[currentOperationIndex];
	if (currentOp.op == JMP) {
		auto& nextBlock = std::get<BlockRef>(currentOp.input[0]);
		setCurrentBlock(nextBlock.block);
	}
}

TraceOperation& ExecutionTrace::getCurrentOperation() {
	auto& currentOp = getCurrentBlock().operations[currentOperationIndex];
	while (currentOp.op == JMP) {
		auto& nextBlock = std::get<BlockRef>(currentOp.input[0]);
		setCurrentBlock(nextBlock.block);
		currentOp = getCurrentBlock().operations[currentOperationIndex];
	}
	return currentOp;
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
	// copy everything from the reference block between opId and end to merge
	// block;
	for (uint32_t opIndex = oi.operationIndex; opIndex < referenceBlock.operations.size(); opIndex++) {
		auto sourceOperation = referenceBlock.operations[opIndex];
		auto operationReference = mergeBlock.addOperation(std::move(sourceOperation));
		// update in global and local tag map

		if (sourceOperation.op == RETURN) {
			for (auto& returnRef : returnRefs) {
				if (returnRef.blockIndex == referenceBlock.blockId && returnRef.operationIndex == opIndex) {
					returnRef = operationReference;
				}
			}
		} else {
			globalTagMap[sourceOperation.tag] = operationReference;
			localTagMap[sourceOperation.tag] = operationReference;
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
