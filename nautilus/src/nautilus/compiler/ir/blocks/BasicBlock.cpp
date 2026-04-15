
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <utility>

namespace nautilus::compiler::ir {
BasicBlock::BasicBlock(common::Arena& arena, BlockIdentifier identifier, std::vector<BasicBlockArgument*> arguments)
    : arena_(&arena), identifier(identifier), operations(), arguments(std::move(arguments)) {
}

void BasicBlock::addNextBlock(BasicBlock* nextBlock, const std::vector<Operation*>& ops) {
	auto* branchOp = arena_->create<BranchOperation>();
	auto& nextBlockIn = branchOp->getNextBlockInvocation();
	nextBlockIn.setBlock(nextBlock);
	for (auto op : ops) {
		nextBlockIn.addArgument(op);
	}
	addOperation(branchOp);
}

BasicBlock::~BasicBlock() = default;

BlockIdentifier BasicBlock::getIdentifier() const {
	return identifier;
}

const std::vector<Operation*>& BasicBlock::getOperations() const {
	return operations;
}

[[nodiscard]] Operation* BasicBlock::getOperationAt(size_t index) {
	return operations.at(index);
}

Operation* BasicBlock::getTerminatorOp() {
	return operations.back();
}

const std::vector<BasicBlockArgument*>& BasicBlock::getArguments() const {
	return arguments;
}

uint64_t BasicBlock::getIndexOfArgument(Operation* arg) {
	for (uint64_t i = 0; i < arguments.size(); i++) {
		if (arguments[i] == arg) {
			return i;
		}
	}
	return -1;
}

void BasicBlock::replaceTerminatorOperation(Operation* loopOperation) {
	operations.pop_back();
	operations.emplace_back(loopOperation);
}

BasicBlock* BasicBlock::addOperation(Operation* operation) {
	operations.emplace_back(operation);
	return this;
}

} // namespace nautilus::compiler::ir
