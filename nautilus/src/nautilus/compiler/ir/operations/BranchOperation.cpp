
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"

namespace nautilus::compiler::ir {

BranchOperation::BranchOperation() : Operation(OperationType::BranchOp, Type::v), basicBlock() {
}

BasicBlockInvocation& BranchOperation::getNextBlockInvocation() {
	return basicBlock;
}

std::string BranchOperation::toString() {
	std::string baseString = "br Block_" + basicBlock.getBlock()->getIdentifier() + "(";
	if (!basicBlock.getBlock()->getArguments().empty()) {
		baseString += basicBlock.getArguments().at(0)->getIdentifier().toString();
		for (int i = 1; i < (int) basicBlock.getArguments().size(); ++i) {
			baseString += ", " + basicBlock.getArguments().at(i)->getIdentifier().toString();
		}
	}
	return baseString + ")";
}

bool BranchOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::BranchOp;
}

} // namespace nautilus::compiler::ir
