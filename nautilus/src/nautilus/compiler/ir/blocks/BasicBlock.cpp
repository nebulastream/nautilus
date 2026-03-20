
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <algorithm>
#include <memory>
#include <utility>

namespace nautilus::compiler::ir {
BasicBlock::BasicBlock(uint16_t identifier, std::vector<std::unique_ptr<BasicBlockArgument>>& arguments)
    : identifier(identifier), operations(), arguments(std::move(arguments)) {
}

void BasicBlock::addNextBlock(BasicBlock* nextBlock, const std::vector<Operation*>& ops) {
	auto branchOp = std::make_unique<BranchOperation>();
	auto& nextBlockIn = branchOp->getNextBlockInvocation();
	nextBlockIn.setBlock(nextBlock);
	for (auto op : ops) {
		nextBlockIn.addArgument(op);
	}
	addOperation(std::move(branchOp));
	// add this block as a predecessor to the next block
	// Todo #3167 : can we use this to replace the addPredecessor pass? (also:
	// addTrueBlock, and addFalseBlock)
	// nextBlock->addPredecessor(shared_from_this());
}

BasicBlock::~BasicBlock() = default;

const std::string BasicBlock::getIdentifier() const {
	return std::to_string(identifier);
}

const std::vector<std::unique_ptr<Operation>>& BasicBlock::getOperations() const {
	return operations;
}

[[nodiscard]] Operation* BasicBlock::getOperationAt(size_t index) {
	return operations.at(index).get();
}

Operation* BasicBlock::getTerminatorOp() {
	return operations.back().get();
}

const std::vector<std::unique_ptr<BasicBlockArgument>>& BasicBlock::getArguments() const {
	return arguments;
}

uint64_t BasicBlock::getIndexOfArgument(Operation* arg) {
	for (uint64_t i = 0; i < arguments.size(); i++) {
		if (arguments[i].get() == arg) {
			return i;
		}
	}
	return -1;
}

void BasicBlock::replaceTerminatorOperation(Operation* loopOperation) {
	operations.pop_back();
	operations.emplace_back(loopOperation);
}

BasicBlock* BasicBlock::addOperation(std::unique_ptr<Operation> operation) {
	operations.emplace_back(std::move(operation));
	return this;
}

void BasicBlock::addPredecessor(BasicBlock* predecessor) {
	this->predecessors.emplace_back(predecessor);
}

std::vector<BasicBlock*>& BasicBlock::getPredecessors() {
	return predecessors;
}

void BasicBlock::removeOperation(Operation* operation) {
	auto pos = std::find_if(operations.begin(), operations.end(),
	                        [operation](const std::unique_ptr<Operation>& op) { return op.get() == operation; });
	if (pos != operations.end()) {
		operations.erase(pos);
	}
}

void BasicBlock::addOperationBefore(Operation* before, std::unique_ptr<Operation>& operation) {
	auto position = std::find_if(operations.begin(), operations.end(),
	                             [before](const std::unique_ptr<Operation>& op) { return op.get() == before; });
	operations.insert(position, std::move(operation));
}

void BasicBlock::removeArgumentAt(uint64_t index) {
	if (index < arguments.size()) {
		arguments.erase(arguments.begin() + static_cast<std::ptrdiff_t>(index));
	}
}

} // namespace nautilus::compiler::ir
