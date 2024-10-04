
#include "nautilus/compiler/ir/IRDumpHandler.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <iostream>

namespace nautilus::compiler::ir {

NESIRDumpHandler::~NESIRDumpHandler() = default;

NESIRDumpHandler::NESIRDumpHandler(std::ostream& out) : out(out) {
}

std::shared_ptr<NESIRDumpHandler> NESIRDumpHandler::create(std::ostream& out) {
	return std::make_shared<NESIRDumpHandler>(out);
}

const BasicBlock* NESIRDumpHandler::getNextLowerOrEqualLevelBasicBlock(const BasicBlock* thenBlock) {
	auto& terminatorOp = thenBlock->getOperations().back();
	if (terminatorOp->getOperationType() == Operation::OperationType::BranchOp) {
		auto branchOp = dynamic_cast<BranchOperation*>(terminatorOp.get());
		return getNextLowerOrEqualLevelBasicBlock(branchOp->getNextBlockInvocation().getBlock());
	} else if (terminatorOp->getOperationType() == Operation::OperationType::IfOp) {
		auto ifOp = dynamic_cast<IfOperation*>(terminatorOp.get());
		if (ifOp->getFalseBlockInvocation().getBlock() != nullptr) {
			return getNextLowerOrEqualLevelBasicBlock(ifOp->getFalseBlockInvocation().getBlock());
		} else {
			return getNextLowerOrEqualLevelBasicBlock(ifOp->getTrueBlockInvocation().getBlock());
		}
	} else { // ReturnOp todo changed #3234
		return nullptr;
	}
}

void NESIRDumpHandler::dumpHelper(Operation* terminatorOp) {
	switch (terminatorOp->getOperationType()) {
	case Operation::OperationType::BranchOp: {
		auto branchOp = static_cast<BranchOperation*>(terminatorOp);
		dumpHelper(branchOp->getNextBlockInvocation().getBlock());
		break;
	}
	case Operation::OperationType::IfOp: {
		auto ifOp = static_cast<IfOperation*>(terminatorOp);
		auto lastTerminatorOp = getNextLowerOrEqualLevelBasicBlock(ifOp->getTrueBlockInvocation().getBlock());
		dumpHelper(ifOp->getTrueBlockInvocation().getBlock());
		dumpHelper(ifOp->getFalseBlockInvocation().getBlock());
		if (lastTerminatorOp) {
			dumpHelper(lastTerminatorOp);
		}
		break;
	}
	case Operation::OperationType::ReturnOp:
		break;
	default:
		break;
	}
}

void NESIRDumpHandler::dumpHelper(const BasicBlock* basicBlock) {
	if (!visitedBlocks.contains(basicBlock->getIdentifier())) {
		// int32_t indent = basicBlock->getScopeLevel() + 1;
		visitedBlocks.emplace(basicBlock->getIdentifier());
		out << '\n' << "Block_" << basicBlock->getIdentifier() << '(';
		if (basicBlock->getArguments().size() > 0) {
			out << basicBlock->getArguments().at(0)->getIdentifier().toString() << ":" << toString(basicBlock->getArguments().at(0)->getStamp());
			for (int i = 1; i < (int) basicBlock->getArguments().size(); ++i) {
				out << ", " << basicBlock->getArguments().at(i)->getIdentifier().toString() << ":" << toString(basicBlock->getArguments().at(i)->getStamp());
			}
		}
		out << "):" << '\n';
		for (auto& operation : basicBlock->getOperations()) {
			out << std::string(4, ' ') << operation->toString() << " :" << toString(operation->getStamp()) << std::endl;
		}
		auto& terminatorOp = basicBlock->getOperations().back();
		dumpHelper(terminatorOp.get());
	}
}

void NESIRDumpHandler::dump(const std::unique_ptr<FunctionOperation>& funcOp) {
	out << funcOp->toString() << " {";
	dumpHelper(funcOp->getFunctionBasicBlock());
	out << "}\n";
}

} // namespace nautilus::compiler::ir
