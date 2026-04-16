
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <algorithm>
#include <utility>

namespace nautilus::compiler::ir {
BasicBlock::BasicBlock(common::Arena& arena, BlockIdentifier identifier, std::vector<BasicBlockArgument*> arguments)
    : arena_(&arena), identifier(identifier), operations(), arguments(std::move(arguments)) {
}

void BasicBlock::addNextBlock(BasicBlock* nextBlock, std::span<Operation* const> ops) {
	auto* branchOp = arena_->create<BranchOperation>();
	auto& nextBlockIn = branchOp->getNextBlockInvocation();
	nextBlockIn.setBlock(nextBlock);
	for (auto* op : ops) {
		nextBlockIn.addArgument(*arena_, op);
	}
	addOperation(branchOp);
	// Register this block as a predecessor of the newly-targeted block.
	if (nextBlock != nullptr) {
		nextBlock->addPredecessor(this);
	}
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

void BasicBlock::replaceTerminatorOperation(Operation* newTerminatorOperation) {
	// Tear down the predecessor edges for the old terminator: each outgoing
	// invocation of the old terminator had `this` as a predecessor of its
	// target block; remove one such entry from each.
	if (!operations.empty()) {
		auto* oldTerm = operations.back();
		for (auto* inv : getSuccessorInvocations(*oldTerm)) {
			if (auto* target = const_cast<BasicBlock*>(inv->getBlock())) {
				target->removePredecessor(this);
			}
		}
	}
	operations.pop_back();
	operations.emplace_back(newTerminatorOperation);
	// Add predecessor edges for the new terminator's targets.
	for (auto* inv : getSuccessorInvocations(*newTerminatorOperation)) {
		if (auto* target = const_cast<BasicBlock*>(inv->getBlock())) {
			target->addPredecessor(this);
		}
	}
}

BasicBlock* BasicBlock::addOperation(Operation* operation) {
	operations.emplace_back(operation);
	return this;
}

void BasicBlock::replaceOperation(size_t operationIndex, Operation* operation) {
	// Replacing the terminator goes through `replaceTerminatorOperation` so
	// predecessor-list invariants stay in sync; intermediate operations are
	// plain SSA values with no CFG bookkeeping, so a pointer swap is enough.
	if (!operations.empty() && operationIndex == operations.size() - 1) {
		replaceTerminatorOperation(operation);
		return;
	}
	operations.at(operationIndex) = operation;
}

const std::vector<BasicBlock*>& BasicBlock::getPredecessors() const {
	return predecessors;
}

std::vector<BasicBlock*> BasicBlock::getSuccessors() {
	if (operations.empty()) {
		return {};
	}
	return ir::getSuccessors(*operations.back());
}

void BasicBlock::replaceSuccessor(BasicBlock* from, BasicBlock* to) {
	if (operations.empty() || from == to) {
		return;
	}
	auto* terminator = operations.back();
	for (auto* inv : getSuccessorInvocations(*terminator)) {
		if (inv->getBlock() == from) {
			inv->setBlock(to);
			// One invocation edge moved from `from` to `to`; reflect that
			// on the predecessor lists. Invocations are unique per edge,
			// so each matching invocation corresponds to exactly one
			// predecessor occurrence.
			if (from != nullptr) {
				from->removePredecessor(this);
			}
			if (to != nullptr) {
				to->addPredecessor(this);
			}
		}
	}
}

void BasicBlock::addPredecessor(BasicBlock* predecessor) {
	predecessors.push_back(predecessor);
}

void BasicBlock::removePredecessor(BasicBlock* predecessor) {
	auto it = std::find(predecessors.begin(), predecessors.end(), predecessor);
	if (it != predecessors.end()) {
		predecessors.erase(it);
	}
}

void BasicBlock::clearPredecessors() {
	predecessors.clear();
}

} // namespace nautilus::compiler::ir
