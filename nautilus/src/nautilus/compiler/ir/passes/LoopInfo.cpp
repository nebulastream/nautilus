
#include "nautilus/compiler/ir/passes/LoopInfo.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/passes/Dominators.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <cstdint>

namespace nautilus::compiler::ir {

bool containsLoop(const FunctionOperation& fn) {
	enum class Color : uint8_t { White, Grey, Black };
	std::unordered_map<const BasicBlock*, Color> color;
	color.reserve(fn.getBasicBlocks().size());
	struct Frame {
		BasicBlock* block;
		std::vector<BasicBlock*> successors;
		size_t next = 0;
	};
	std::vector<Frame> stack;
	// Every block is a root in turn, so a cycle among blocks the entry cannot
	// reach is found too -- the caller is asking about the function's text,
	// not about what a later unreachable-block sweep will leave of it.
	for (auto* root : fn.getBasicBlocks()) {
		if (root == nullptr || color[root] != Color::White) {
			continue;
		}
		color[root] = Color::Grey;
		stack.push_back({root, root->getSuccessors(), 0});
		while (!stack.empty()) {
			Frame& frame = stack.back();
			if (frame.next < frame.successors.size()) {
				auto* succ = frame.successors[frame.next++];
				if (succ == nullptr) {
					continue;
				}
				auto& c = color[succ];
				if (c == Color::Grey) {
					return true; // an edge back onto the walk's own stack
				}
				if (c == Color::White) {
					c = Color::Grey;
					stack.push_back({succ, succ->getSuccessors(), 0});
				}
				continue;
			}
			color[frame.block] = Color::Black;
			stack.pop_back();
		}
	}
	return false;
}

namespace {

/// Finds the natural-loop back edge / preheader edge into @p header, if header
/// has exactly the shape the passes handle: exactly two predecessor edges, one
/// of which is a back edge (header dominates its source) and one a forward
/// edge (the preheader). Returns false (leaving the out-params untouched) for
/// anything else -- multiple latches, multiple preheaders, irreducible control
/// flow, etc. are all conservatively skipped.
bool findSimpleLoopEdges(BasicBlock* header, const Dominators& dom, BasicBlock*& latch, BasicBlockInvocation*& latchInv,
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
		const bool isBack = dom.dominates(header, p);
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
/// plus every reachable block from which latch can be reached without passing
/// through header -- the standard backward walk over predecessor edges from
/// the latch, stopping at the header. O(body + its incoming edges).
std::unordered_set<BasicBlock*> computeLoopBody(BasicBlock* header, BasicBlock* latch, const Dominators& dom) {
	std::unordered_set<BasicBlock*> body;
	body.insert(header);
	std::vector<BasicBlock*> frontier;
	if (body.insert(latch).second) {
		frontier.push_back(latch);
	}
	while (!frontier.empty()) {
		auto* b = frontier.back();
		frontier.pop_back();
		for (auto* p : b->getPredecessors()) {
			if (p == nullptr || !dom.isReachable(p)) {
				continue;
			}
			if (body.insert(p).second) {
				frontier.push_back(p);
			}
		}
	}
	return body;
}

} // namespace

std::vector<NaturalLoop> findNaturalLoops(FunctionOperation& fn) {
	std::vector<NaturalLoop> loops;
	const Dominators dom(fn);
	for (auto* header : fn.getBasicBlocks()) {
		BasicBlock* latch = nullptr;
		BasicBlockInvocation* latchInv = nullptr;
		BasicBlock* preheader = nullptr;
		BasicBlockInvocation* preheaderInv = nullptr;
		if (!findSimpleLoopEdges(header, dom, latch, latchInv, preheader, preheaderInv)) {
			continue;
		}
		auto body = computeLoopBody(header, latch, dom);
		// The header dominates the latch, so every path from the preheader to
		// the latch runs through the header and the preheader cannot be in the
		// body; kept as a guard against a CFG whose predecessor lists are stale.
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
