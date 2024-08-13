
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <memory>
#include <utility>

namespace nautilus::compiler::ir {
BasicBlock::BasicBlock(const std::string& identifier, int32_t scopeLevel, std::vector<std::unique_ptr<Operation>>& operations, std::vector<std::unique_ptr<BasicBlockArgument>>& arguments)
    : identifier(identifier), scopeLevel(scopeLevel), numLoopBackEdges(0), operations(std::move(operations)), arguments(std::move(arguments)) {
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

const std::string& BasicBlock::getIdentifier() const {
	return identifier;
}

void BasicBlock::setIdentifier(const std::string& identifier) {
	this->identifier = identifier;
}

uint32_t BasicBlock::getScopeLevel() const {
	return scopeLevel;
}

void BasicBlock::setScopeLevel(uint32_t scopeLevel) {
	this->scopeLevel = scopeLevel;
}

uint32_t BasicBlock::getNumLoopBackEdges() {
	return numLoopBackEdges;
}

void BasicBlock::incrementNumLoopBackEdge() {
	++this->numLoopBackEdges;
}

bool BasicBlock::isLoopHeaderBlock() {
	return numLoopBackEdges > 0;
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
	operations.emplace_back(std::move(loopOperation));
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

/* void BasicBlock::replaceOperation(size_t operationIndex, Operation
 *operation) { operations.at(operationIndex) = operation;
 }

 void BasicBlock::removeOperation(Operation *operation) {
     auto pos = std::find(operations.begin(), operations.end(), operation);
     operations.erase();
 }

 void BasicBlock::removeArgument(BasicBlockArgument* argument) {
     arguments.erase(std::find(arguments.begin(), arguments.end(), argument));
 }

 void BasicBlock::addOperationBefore(Operation *before,
 std::unique_ptr<Operation>& operation) { auto position =
 std::find(operations.begin(), operations.end(), before);
     operations.insert(position, std::move(operation));
 }

 [[nodiscard]] std::pair<const BasicBlock *, const BasicBlock *>
 BasicBlock::getNextBlocks() {
     // std::pair<std::shared_ptr<BasicBlock>, std::shared_ptr<BasicBlock>>
 nextBlocks; if (operations.back()->getOperationType() ==
 Operation::OperationType::BranchOp) { auto branchOp =
 std::static_pointer_cast<BranchOperation>(operations.back()); return
 std::make_pair(branchOp->getNextBlockInvocation().getBlock(), nullptr); } else
 if (operations.back()->getOperationType() == Operation::OperationType::IfOp) {
         auto ifOp = std::static_pointer_cast<IfOperation>(operations.back());
         return std::make_pair(ifOp->getTrueBlockInvocation().getBlock(),
                               ifOp->getFalseBlockInvocation().getBlock());
     } else if (operations.back()->getOperationType() ==
 Operation::OperationType::LoopOp) { auto loopOp =
 std::static_pointer_cast<LoopOperation>(operations.back()); return
 std::make_pair(loopOp->getLoopBodyBlock().getBlock(),
 loopOp->getLoopFalseBlock().getBlock()); } else if
 (operations.back()->getOperationType() == Operation::OperationType::ReturnOp) {
         return {};
     } else {
         throw NotImplementedException(
                 "BasicBlock::getNextBlocks: Tried to get next block for
 unsupported operation type.");
     }
 }*/

} // namespace nautilus::compiler::ir
