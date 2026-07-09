
#include "nautilus/compiler/ir/passes/LoopInfo.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"

namespace nautilus::compiler::ir {

const std::unordered_set<BasicBlock*>& Reachability::from(BasicBlock* start) {
	auto it = cache_.find(start);
	if (it != cache_.end()) {
		return it->second;
	}
	std::unordered_set<BasicBlock*> visited;
	std::vector<BasicBlock*> frontier = start->getSuccessors();
	while (!frontier.empty()) {
		auto* b = frontier.back();
		frontier.pop_back();
		if (b == nullptr || !visited.insert(b).second) {
			continue;
		}
		for (auto* s : b->getSuccessors()) {
			frontier.push_back(s);
		}
	}
	return cache_.emplace(start, std::move(visited)).first->second;
}

namespace {

/// Finds the natural-loop back edge / preheader edge into @p header, if header
/// has exactly the shape the passes handle: exactly two predecessor edges, one
/// of which is a back edge (header is reachable from it) and one a forward edge
/// (the preheader). Returns false (leaving the out-params untouched) for
/// anything else -- multiple latches, multiple preheaders, irreducible control
/// flow, etc. are all conservatively skipped.
bool findSimpleLoopEdges(BasicBlock* header, Reachability& reach, BasicBlock*& latch, BasicBlockInvocation*& latchInv,
                         BasicBlock*& preheader, BasicBlockInvocation*& preheaderInv) {
	const auto& preds = header->getPredecessors();
	if (preds.size() != 2) {
		return false;
	}
	BasicBlock* back = nullptr;
	BasicBlock* fwd = nullptr;
	for (auto* p : preds) {
		if (p == nullptr) {
			return false;
		}
		const bool isBack = reach.from(header).contains(p);
		if (isBack) {
			if (back != nullptr) {
				return false;
			}
			back = p;
		} else {
			if (fwd != nullptr) {
				return false;
			}
			fwd = p;
		}
	}
	if (back == nullptr || fwd == nullptr) {
		return false;
	}

	auto findSingleInvocationTo = [&](BasicBlock* pred, BasicBlockInvocation*& out) {
		BasicBlockInvocation* found = nullptr;
		int count = 0;
		for (auto* inv : getSuccessorInvocations(*pred->getTerminatorOp())) {
			if (inv->getBlock() == header) {
				found = inv;
				count++;
			}
		}
		if (count != 1) {
			return false;
		}
		out = found;
		return true;
	};
	if (!findSingleInvocationTo(back, latchInv) || !findSingleInvocationTo(fwd, preheaderInv)) {
		return false;
	}
	if (latchInv->getArguments().size() != header->getArguments().size() ||
	    preheaderInv->getArguments().size() != header->getArguments().size()) {
		return false;
	}
	latch = back;
	preheader = fwd;
	return true;
}

/// Computes the natural-loop body for the (header, latch) pair: header itself
/// plus every block reachable from header that can also reach latch.
std::unordered_set<BasicBlock*> computeLoopBody(BasicBlock* header, BasicBlock* latch, Reachability& reach) {
	std::unordered_set<BasicBlock*> body;
	body.insert(header);
	for (auto* b : reach.from(header)) {
		if (b == latch || reach.from(b).contains(latch)) {
			body.insert(b);
		}
	}
	body.insert(latch);
	return body;
}

} // namespace

std::vector<NaturalLoop> findNaturalLoops(FunctionOperation& fn) {
	std::vector<NaturalLoop> loops;
	Reachability reach;
	for (auto* header : fn.getBasicBlocks()) {
		BasicBlock* latch = nullptr;
		BasicBlockInvocation* latchInv = nullptr;
		BasicBlock* preheader = nullptr;
		BasicBlockInvocation* preheaderInv = nullptr;
		if (!findSimpleLoopEdges(header, reach, latch, latchInv, preheader, preheaderInv)) {
			continue;
		}
		auto body = computeLoopBody(header, latch, reach);
		// A preheader that is itself part of the loop body means the "forward"
		// edge is not really outside the loop -- skip such irreducible shapes.
		if (body.contains(preheader)) {
			continue;
		}
		loops.push_back(NaturalLoop {header, preheader, latch, preheaderInv, latchInv, std::move(body)});
	}
	return loops;
}

std::unordered_set<Operation*> computeIvAliases(BasicBlock* header, BasicBlockArgument* ivArg,
                                                const std::unordered_set<BasicBlock*>& loopBody) {
	std::unordered_set<Operation*> aliases;
	aliases.insert(ivArg);
	std::vector<std::pair<BasicBlock*, Operation*>> frontier = {{header, ivArg}};
	while (!frontier.empty()) {
		auto [block, val] = frontier.back();
		frontier.pop_back();
		auto* terminator = block->getTerminatorOp();
		if (terminator == nullptr) {
			continue;
		}
		for (auto* inv : getSuccessorInvocations(*terminator)) {
			auto* target = const_cast<BasicBlock*>(inv->getBlock());
			if (target == nullptr || !loopBody.contains(target)) {
				continue;
			}
			const auto args = inv->getArguments();
			const auto& targetArgs = target->getArguments();
			for (size_t k = 0; k < args.size() && k < targetArgs.size(); k++) {
				if (args[k] == val && aliases.insert(targetArgs[k]).second) {
					frontier.emplace_back(target, targetArgs[k]);
				}
			}
		}
	}
	return aliases;
}

std::unordered_map<Operation*, bool> computeHeaderPassThroughMap(BasicBlock* header, BasicBlockInvocation* latchInv,
                                                                 const std::unordered_set<BasicBlock*>& loopBody) {
	std::unordered_map<Operation*, bool> result;
	const auto& headerArgs = header->getArguments();
	const auto latchArgs = latchInv->getArguments();
	for (size_t k = 0; k < headerArgs.size() && k < latchArgs.size(); k++) {
		auto aliases = computeIvAliases(header, headerArgs[k], loopBody);
		if (aliases.contains(latchArgs[k])) {
			for (auto* a : aliases) {
				result[a] = true;
			}
		}
	}
	return result;
}

bool isLoopInvariant(Operation* op, const std::unordered_map<Operation*, BasicBlock*>& definingBlock,
                     const std::unordered_set<BasicBlock*>& loopBody,
                     const std::unordered_map<Operation*, bool>& passThroughMap) {
	if (passThroughMap.contains(op)) {
		return true;
	}
	auto it = definingBlock.find(op);
	if (it == definingBlock.end()) {
		return true;
	}
	return !loopBody.contains(it->second);
}

std::unordered_map<Operation*, BasicBlock*> computeDefiningBlocks(FunctionOperation& fn) {
	std::unordered_map<Operation*, BasicBlock*> definingBlock;
	for (auto* block : fn.getBasicBlocks()) {
		for (auto* arg : block->getArguments()) {
			definingBlock[arg] = block;
		}
		for (auto* op : block->getOperations()) {
			definingBlock[op] = block;
		}
	}
	return definingBlock;
}

} // namespace nautilus::compiler::ir
