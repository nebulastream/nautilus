#include "nautilus/compiler/ir/passes/Dominators.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include <algorithm>

namespace nautilus::compiler::ir {

namespace {

/// Post-order DFS over successor edges, starting at `entry`. Iterative (an
/// explicit stack) to avoid recursion depth tracking the function's block
/// count. Successor order is `BasicBlock::getSuccessors()`'s order (true
/// branch before false branch for `IfOp`), so the result is deterministic
/// across constructions of the same graph (D3).
std::vector<const BasicBlock*> postOrder(BasicBlock* entry) {
	std::vector<const BasicBlock*> order;
	if (entry == nullptr) {
		return order;
	}
	std::unordered_map<const BasicBlock*, bool> visited;
	// Each stack frame is a block plus how many of its successors have
	// already been pushed, so we can resume expanding it after a child
	// finishes instead of requiring recursion.
	struct Frame {
		BasicBlock* block;
		std::vector<BasicBlock*> successors;
		size_t next = 0;
	};
	std::vector<Frame> stack;
	visited.emplace(entry, true);
	stack.push_back({entry, entry->getSuccessors(), 0});

	while (!stack.empty()) {
		Frame& frame = stack.back();
		if (frame.next < frame.successors.size()) {
			auto* succ = frame.successors[frame.next++];
			if (succ != nullptr && !visited.contains(succ)) {
				visited.emplace(succ, true);
				stack.push_back({succ, succ->getSuccessors(), 0});
			}
			continue;
		}
		order.push_back(frame.block);
		stack.pop_back();
	}
	return order;
}

} // namespace

Dominators::Dominators(const FunctionOperation& fn) {
	auto* entry = fn.getEntryBlock();
	auto post = postOrder(entry);
	// Reverse post-order is simply the post-order sequence reversed.
	rpo_.assign(post.rbegin(), post.rend());
	rpoIndex_.reserve(rpo_.size());
	for (size_t i = 0; i < rpo_.size(); ++i) {
		rpoIndex_.emplace(rpo_[i], i);
	}
	if (rpo_.empty()) {
		return;
	}

	idom_.assign(rpo_.size(), rpo_.size()); // rpo_.size() means "undefined"
	idom_[0] = 0;                           // entry is its own immediate dominator

	auto intersect = [&](size_t a, size_t b) {
		while (a != b) {
			while (a > b) {
				a = idom_[a];
			}
			while (b > a) {
				b = idom_[b];
			}
		}
		return a;
	};

	bool changed = true;
	while (changed) {
		changed = false;
		// Skip the entry block (index 0): it has no predecessors to fold in
		// and stays its own idom for the whole computation.
		for (size_t i = 1; i < rpo_.size(); ++i) {
			const auto* block = rpo_[i];
			// `block` is reachable (it is in the RPO), so it was discovered via
			// some successor edge; its actual predecessor list may contain
			// predecessors outside the reachable set only if the CFG wiring is
			// already inconsistent, which is outside what this analysis has to
			// tolerate. Deduplicate predecessor *blocks* -- a block targeted
			// twice by the same predecessor (the shared-target-if shape)
			// contributes one dominance edge, not two.
			size_t newIdom = rpo_.size();
			for (auto* rawPred : block->getPredecessors()) {
				auto it = rpoIndex_.find(rawPred);
				if (it == rpoIndex_.end()) {
					continue; // predecessor unreachable itself; ignore.
				}
				const size_t predIdx = it->second;
				if (idom_[predIdx] == rpo_.size()) {
					continue; // predecessor not processed yet this round.
				}
				newIdom = (newIdom == rpo_.size()) ? predIdx : intersect(newIdom, predIdx);
			}
			if (newIdom != rpo_.size() && idom_[i] != newIdom) {
				idom_[i] = newIdom;
				changed = true;
			}
		}
	}
}

bool Dominators::isReachable(const BasicBlock* block) const {
	return rpoIndex_.contains(block);
}

bool Dominators::dominates(const BasicBlock* a, const BasicBlock* b) const {
	auto bIt = rpoIndex_.find(b);
	if (bIt == rpoIndex_.end()) {
		return false; // unreachable target: dominated by nothing, not even itself.
	}
	if (a == b) {
		return true;
	}
	auto aIt = rpoIndex_.find(a);
	if (aIt == rpoIndex_.end()) {
		return false;
	}
	size_t cur = bIt->second;
	const size_t target = aIt->second;
	while (cur != 0) {
		cur = idom_[cur];
		if (cur == target) {
			return true;
		}
	}
	return false;
}

const std::vector<const BasicBlock*>& Dominators::reversePostOrder() const {
	return rpo_;
}

} // namespace nautilus::compiler::ir
