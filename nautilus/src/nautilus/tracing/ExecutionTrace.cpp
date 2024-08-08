
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/symbolic_execution/TraceTerminationException.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

namespace nautilus::tracing {

ExecutionTrace::ExecutionTrace() : currentBlockIndex(0), currentOperationIndex(0), blocks() {
	createBlock();
};

bool ExecutionTrace::checkTag(Snapshot& snapshot) {
	// check if operation is in global map -> we have a repeating operation -> this is a control-flow merge
	// std::cout << "\n checkTag \n" << std::endl;
	// std::cout << *this << std::endl;
	auto globalTabIter = globalTagMap.find(snapshot);
	if (globalTabIter != globalTagMap.end()) {
		auto& ref = globalTabIter->second;
		processControlFlowMerge(ref);
		return false;
	}

	auto localTagIter = localTagMap.find(snapshot);
	if (localTagIter != localTagMap.end()) {
		// TODO #3500 Fix handling of repeated operations
		auto& ref = localTagIter->second;
		// add loop iteration to tag
		processControlFlowMerge(ref);
		return false;
	}
	return true;
}

void ExecutionTrace::addReturn(Snapshot& snapshot, Type type, value_ref ref) {
	// std::cout << "\n Add Return:" <<  std::endl;
	// std::cout << *this << std::endl;
	if (blocks.empty()) {
		createBlock();
	}
	auto& operations = blocks[currentBlockIndex].operations;
	auto op = Op::RETURN;
	operations.emplace_back(snapshot, op, type, ref, std::vector<InputVariant> {});
	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);

	// returnRefs
	returnRefs.emplace_back(operationIdentifier);
}

void ExecutionTrace::addAssignmentOperation(Snapshot& snapshot, nautilus::tracing::value_ref targetRef, nautilus::tracing::value_ref srcRef, Type resultType) {
	// std::cout << "\n Add Assign:" << std::endl;
	// std::cout << targetRef.ref << " - " << srcRef.ref << std::endl;
	// std::cout << *this << std::endl;
	if (blocks.empty()) {
		createBlock();
	}
	auto& operations = blocks[currentBlockIndex].operations;
	auto op = ASSIGN;

	operations.emplace_back(snapshot, op, resultType, targetRef, std::vector<InputVariant> {srcRef});
	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
}

void ExecutionTrace::addOperation(Snapshot& snapshot, Op& operation, Type& resultType, nautilus::tracing::value_ref targetRef, nautilus::tracing::value_ref srcRef) {
	// std::cout << "\n Add Operation:" << operation << std::endl;
	// std::cout << targetRef.ref << " - " << srcRef.ref << std::endl;
	// std::cout << *this << std::endl;
	if (blocks.empty()) {
		createBlock();
	}
	auto& operations = blocks[currentBlockIndex].operations;
	operations.emplace_back(snapshot, operation, resultType, targetRef, std::vector<InputVariant> {srcRef});
}

value_ref ExecutionTrace::addOperationWithResult(Snapshot& snapshot, Op& operation, Type& resultType, std::vector<InputVariant>&& inputs) {
	// std::cout << "\n Add Operation:" << operation << std::endl;
	// std::cout << *this << std::endl;
	if (blocks.empty()) {
		createBlock();
	}

	auto& operations = blocks[currentBlockIndex].operations;
	auto& to = operations.emplace_back(snapshot, operation, resultType, value_ref(getNextValueRef(), resultType), std::forward<std::vector<InputVariant>>(inputs));

	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
	return to.resultRef;
}

void ExecutionTrace::addCmpOperation(Snapshot& snapshot, nautilus::tracing::value_ref inputs) {
	if (blocks.empty()) {
		createBlock();
	}

	// create if and else blocks
	auto trueBlock = createBlock();
	getBlock(trueBlock).predecessors.emplace_back(getCurrentBlockIndex());
	auto falseBlock = createBlock();
	getBlock(falseBlock).predecessors.emplace_back(getCurrentBlockIndex());

	auto operationInputs = std::vector<InputVariant> {inputs, BlockRef(trueBlock), BlockRef(falseBlock)};

	auto& operations = blocks[currentBlockIndex].operations;
	operations.emplace_back(snapshot, CMP, Type::v, value_ref(getNextValueRef(), Type::v), std::forward<std::vector<InputVariant>>(operationInputs));
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
	auto currentOp = getCurrentBlock().operations[currentOperationIndex];
	while (currentOp.op == JMP) {
		auto& nextBlock = std::get<BlockRef>(currentOp.input[0]);
		setCurrentBlock(nextBlock.block);
		currentOp = getCurrentBlock().operations[currentOperationIndex];
	}
	return getCurrentBlock().operations[currentOperationIndex];
}

uint16_t ExecutionTrace::createBlock() {
	// add first block
	if (blocks.empty()) {
		// add arguments to first block
		blocks.emplace_back(blocks.size());
		// blocks[0].arguments = arguments;
		return blocks.size() - 1;
	}
	blocks.emplace_back(blocks.size());
	return blocks.size() - 1;
}

Block& ExecutionTrace::processControlFlowMerge(operation_identifier oi) {
	if (oi.blockIndex == currentBlockIndex) {
		throw RuntimeException("Invalid trace. This is maybe caused by a constant loop.");
	}

	// std::cout << "\n processControlFlowMerge \n" << std::endl;
	// std::cout << oi.blockIndex << " - " << oi.operationIndex << std::endl;
	// std::cout << *this << std::endl;

	// create new merge block
	auto mergedBlockId = createBlock();
	// perform a control flow merge and merge the current block with operations in some other block.
	auto& referenceBlock = blocks[oi.blockIndex];
	auto& currentBlock = blocks[currentBlockIndex];

	auto& mergeBlock = getBlock(mergedBlockId);
	mergeBlock.type = Block::Type::ControlFlowMerge;

	// 1. move operation to new block
	// copy everything from the reference block between opId and end to merge block;
	for (uint32_t opIndex = oi.operationIndex; opIndex < referenceBlock.operations.size(); opIndex++) {
		auto sourceOperation = referenceBlock.operations[opIndex];
		auto operationReference = mergeBlock.addOperation(std::move(sourceOperation));
		// update in global and local tag map

		if (sourceOperation.op == RETURN) {
			for (size_t i = 0; i < returnRefs.size(); i++) {
				if (returnRefs[i].blockIndex == referenceBlock.blockId && returnRefs[i].operationIndex == opIndex) {
					returnRefs[i] = operationReference;
				}
			}
		} else {
			globalTagMap[sourceOperation.tag] = operationReference;
			localTagMap[sourceOperation.tag] = operationReference;
		}
	}

	// remove content beyond opID
	referenceBlock.operations.erase(referenceBlock.operations.begin() + oi.operationIndex, referenceBlock.operations.end());

	// add jump from referenced block to merge block
	auto mergeBlockRef = BlockRef(mergedBlockId);
	referenceBlock.addOperation({Op::JMP, std::vector<InputVariant> {mergeBlockRef}});

	// add jump from current block to merge block
	currentBlock.addOperation({Op::JMP, std::vector<InputVariant> {mergeBlockRef}});

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

value_ref ExecutionTrace::setArgument(Type type, size_t index) {
	++lastValueRef;
	uint16_t argRef = index + 1;
	auto& arguments = blocks[0].arguments;
	if (arguments.size() < argRef) {
		arguments.resize(argRef);
	}
	// arguments[index] = {argRef, type};
	arguments[index] = value_ref(argRef, type);
	return arguments[index];
};

void ExecutionTrace::destruct(nautilus::tracing::value_ref) {
	// variableBitset[inputs] = false;
}

std::vector<operation_identifier> ExecutionTrace::getReturn() {
	return returnRefs;
}

uint16_t ExecutionTrace::getNextValueRef() {
	auto ref = ++lastValueRef;
	return ref;
}

operation_identifier ExecutionTrace::getNextOperationIdentifier() {
	currentOperationIndex = getCurrentBlock().operations.size() - 1;
	return {currentBlockIndex, (uint16_t) currentOperationIndex};
}

void ExecutionTrace::resetExecution() {
	// variableBitset.reset();
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

std::string ExecutionTrace::toString() const {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

std::ostream& operator<<(std::ostream& os, const ExecutionTrace& executionTrace) {
	for (size_t i = 0; i < executionTrace.blocks.size(); i++) {
		os << "B" << i;
		os << executionTrace.blocks[i];
	}
	return os;
}

} // namespace nautilus::tracing
