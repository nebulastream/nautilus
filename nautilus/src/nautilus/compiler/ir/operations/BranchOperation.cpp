
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"

namespace nautilus::compiler::ir {

BranchOperation::BranchOperation() : Operation(OperationType::BranchOp, Type::v), basicBlock() {
}

const BasicBlockInvocation& BranchOperation::getNextBlockInvocation() const {
	return basicBlock;
}

BasicBlockInvocation& BranchOperation::getNextBlockInvocation() {
	return basicBlock;
}

bool BranchOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::BranchOp;
}

} // namespace nautilus::compiler::ir
