
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"

namespace nautilus::compiler::ir {

std::vector<BasicBlockInvocation*> getSuccessorInvocations(Operation& op) {
	using OT = Operation::OperationType;
	switch (op.getOperationType()) {
	case OT::BranchOp: {
		auto* branch = as<BranchOperation>(&op);
		return {&branch->getNextBlockInvocation()};
	}
	case OT::IfOp: {
		auto* ifOp = as<IfOperation>(&op);
		return {&ifOp->getTrueBlockInvocation(), &ifOp->getFalseBlockInvocation()};
	}
	case OT::ReturnOp:
		return {};
	default:
		// Non-terminator ops have no successor invocations.
		return {};
	}
}

std::vector<BasicBlock*> getSuccessors(Operation& op) {
	auto invocations = getSuccessorInvocations(op);
	std::vector<BasicBlock*> out;
	out.reserve(invocations.size());
	for (auto* inv : invocations) {
		// `getBlock()` returns `const BasicBlock*`; we need the mutable
		// pointer for CFG rewiring, which is the only caller of this helper.
		out.push_back(const_cast<BasicBlock*>(inv->getBlock()));
	}
	return out;
}

void rebuildPredecessorLists(IRGraph& ir) {
	// Pass 1: clear every block's predecessor list.
	for (auto* fn : ir.getFunctionOperations()) {
		for (auto* block : fn->getBasicBlocks()) {
			block->clearPredecessors();
		}
	}
	// Pass 2: for every block whose terminator has outgoing invocations,
	// add an entry in each target's predecessor list.
	for (auto* fn : ir.getFunctionOperations()) {
		for (auto* block : fn->getBasicBlocks()) {
			if (block->getOperations().empty()) {
				continue;
			}
			auto* terminator = block->getTerminatorOp();
			for (auto* inv : getSuccessorInvocations(*terminator)) {
				auto* target = const_cast<BasicBlock*>(inv->getBlock());
				if (target != nullptr) {
					target->addPredecessor(block);
				}
			}
		}
	}
}

} // namespace nautilus::compiler::ir
