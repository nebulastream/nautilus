
#include "nautilus/compiler/ir/Usages.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include <unordered_set>
#include <vector>

namespace nautilus::compiler::ir {

std::unordered_map<OperationIdentifier, uint32_t> countUsages(const BasicBlock* entryBlock) {
	std::unordered_map<OperationIdentifier, uint32_t> usageCounts;
	std::unordered_set<BlockIdentifier> visited;
	std::vector<const BasicBlock*> worklist;
	worklist.push_back(entryBlock);
	visited.insert(entryBlock->getIdentifier());

	auto countInput = [&usageCounts](const Operation* input) {
		if (input != nullptr) {
			usageCounts[input->getIdentifier()]++;
		}
	};

	while (!worklist.empty()) {
		const auto* block = worklist.back();
		worklist.pop_back();
		for (auto* opt : block->getOperations()) {
			for (auto* input : opt->getInputs()) {
				countInput(input);
			}
			if (auto* ifOp = dyn_cast<IfOperation>(opt)) {
				for (auto* input : ifOp->getTrueBlockInvocation().getInputs()) {
					countInput(input);
				}
				for (auto* input : ifOp->getFalseBlockInvocation().getInputs()) {
					countInput(input);
				}
				auto* trueBlk = ifOp->getTrueBlockInvocation().getBlock();
				auto* falseBlk = ifOp->getFalseBlockInvocation().getBlock();
				if (trueBlk != nullptr && visited.insert(trueBlk->getIdentifier()).second) {
					worklist.push_back(trueBlk);
				}
				if (falseBlk != nullptr && visited.insert(falseBlk->getIdentifier()).second) {
					worklist.push_back(falseBlk);
				}
			} else if (auto* brOp = dyn_cast<BranchOperation>(opt)) {
				for (auto* input : brOp->getNextBlockInvocation().getInputs()) {
					countInput(input);
				}
				auto* next = brOp->getNextBlockInvocation().getBlock();
				if (next != nullptr && visited.insert(next->getIdentifier()).second) {
					worklist.push_back(next);
				}
			}
		}
	}
	return usageCounts;
}

} // namespace nautilus::compiler::ir
