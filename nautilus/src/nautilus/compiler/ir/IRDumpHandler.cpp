
#include "nautilus/compiler/ir/IRDumpHandler.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <iostream>

namespace nautilus::compiler::ir {

IRDumpHandler::~IRDumpHandler() = default;

IRDumpHandler::IRDumpHandler(std::ostream& out, const std::string& fileName) : out(out), fileName(fileName){
}

std::shared_ptr<IRDumpHandler> IRDumpHandler::create(std::ostream& out) {
	return std::make_shared<IRDumpHandler>(out, "");
}
std::shared_ptr<IRDumpHandler> IRDumpHandler::create(std::ostream& out, const std::string& fileName) {
	return std::make_shared<IRDumpHandler>(out, fileName);
}

const BasicBlock* IRDumpHandler::getNextLowerOrEqualLevelBasicBlock(const BasicBlock* thenBlock, int ifParentBlockLevel) {
	auto& terminatorOp = thenBlock->getOperations().back();
	if (terminatorOp->getOperationType() == Operation::OperationType::BranchOp) {
		auto branchOp = dynamic_cast<BranchOperation*>(terminatorOp.get());
		if (branchOp->getNextBlockInvocation().getBlock()->getScopeLevel() <= (uint32_t) ifParentBlockLevel) {
			return branchOp->getNextBlockInvocation().getBlock();
		} else {
			return getNextLowerOrEqualLevelBasicBlock(branchOp->getNextBlockInvocation().getBlock(), ifParentBlockLevel);
		}
	} else if (terminatorOp->getOperationType() == Operation::OperationType::IfOp) {
		auto ifOp = dynamic_cast<IfOperation*>(terminatorOp.get());
		if (ifOp->getFalseBlockInvocation().getBlock() != nullptr) {
			return getNextLowerOrEqualLevelBasicBlock(ifOp->getFalseBlockInvocation().getBlock(), ifParentBlockLevel);
		} else {
			return getNextLowerOrEqualLevelBasicBlock(ifOp->getTrueBlockInvocation().getBlock(), ifParentBlockLevel);
		}
	} else { // ReturnOp todo changed #3234
		return nullptr;
	}
}

void IRDumpHandler::dumpHelper(Operation* terminatorOp, int32_t) {
	terminatorOp->setLocation(compiler::ir::FileLineLocation{fileName, currentLine, 0});
	switch (terminatorOp->getOperationType()) {
	case Operation::OperationType::BranchOp: {
		auto branchOp = static_cast<BranchOperation*>(terminatorOp);
		dumpHelper(branchOp->getNextBlockInvocation().getBlock());
		break;
	}
	case Operation::OperationType::IfOp: {
		auto ifOp = static_cast<IfOperation*>(terminatorOp);
		auto lastTerminatorOp = getNextLowerOrEqualLevelBasicBlock(ifOp->getTrueBlockInvocation().getBlock(),
		                                                           ifOp->getTrueBlockInvocation().getBlock()->getScopeLevel() - 1); // todo can lead to error #3234
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

void IRDumpHandler::dumpHelper(const BasicBlock* basicBlock) {
	if (!visitedBlocks.contains(basicBlock->getIdentifier())) {
		visitedBlocks.emplace(basicBlock->getIdentifier());
		currentLine++;
		out << '\n' << "Block_" << basicBlock->getIdentifier() << '(';
		auto& blockArguments = basicBlock->getArguments();
		if (!blockArguments.empty()) {
			auto& first = blockArguments.front();
			first->setLocation(compiler::ir::FileLineLocation{fileName, currentLine, 0});
			out << first->getIdentifier().toString() << ":" << toString(first->getStamp());
			for (int i = 1; i < (int) blockArguments.size(); ++i) {
				auto& current = blockArguments.at(i);
				current->setLocation(compiler::ir::FileLineLocation{fileName, currentLine, 0});
				out << ", " << current->getIdentifier().toString() << ":" << toString(current->getStamp());
			}
		}
		out << "):" << '\n';
		currentLine++;
		for (auto& operation : basicBlock->getOperations()) {
			operation->setLocation(compiler::ir::FileLineLocation{fileName, currentLine, 0});
			out << std::string(4, ' ') << operation->toString() << " :" << toString(operation->getStamp()) << std::endl;
			currentLine++;
		}
		auto& terminatorOp = basicBlock->getOperations().back();
		dumpHelper(terminatorOp.get(), basicBlock->getScopeLevel());
	}
}

void IRDumpHandler::dump(FunctionOperation* funcOp) {
	out << funcOp->toString() << " {";
	currentLine = 2;
	dumpHelper(funcOp->getFunctionBasicBlock());
	out << "}\n";
}

} // namespace nautilus::compiler::ir
