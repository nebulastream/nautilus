#include "nautilus/passes/Cfg.hpp"
#include <algorithm>

namespace nautilus::passes {

using nir::NONE;
using nir::Op;

Cfg::Cfg(const MutableFunction& fn) {
	const auto n = static_cast<uint32_t>(fn.blocks.size());
	auto index = fn.blockIndex();
	succs.assign(n, {});
	preds.assign(n, {});
	for (uint32_t b = 0; b < n; ++b) {
		if (fn.blocks[b].isPad()) {
			continue;
		}
		for (uint32_t uid : successors(fn.blocks[b])) {
			const uint32_t t = index[uid];
			if (t == NONE || fn.blocks[t].isPad()) {
				continue;
			}
			succs[b].push_back(t);
			preds[t].push_back(b);
		}
	}

	// reverse post-order by an iterative depth-first search from the entry block
	rpoNumber.assign(n, NONE);
	std::vector<uint32_t> post;
	post.reserve(n);
	if (n != 0) {
		std::vector<char> seen(n, 0);
		std::vector<std::pair<uint32_t, uint32_t>> stack;
		stack.emplace_back(0, 0);
		seen[0] = 1;
		while (!stack.empty()) {
			auto& [b, next] = stack.back();
			if (next < succs[b].size()) {
				const uint32_t s = succs[b][next++];
				if (!seen[s]) {
					seen[s] = 1;
					stack.emplace_back(s, 0);
				}
			} else {
				post.push_back(b);
				stack.pop_back();
			}
		}
	}
	rpo.assign(post.rbegin(), post.rend());
	for (uint32_t i = 0; i < rpo.size(); ++i) {
		rpoNumber[rpo[i]] = i;
	}

	// immediate dominators (Cooper, Harvey, Kennedy)
	idom.assign(n, NONE);
	if (n != 0) {
		idom[0] = 0;
		bool changed = true;
		while (changed) {
			changed = false;
			for (size_t i = 1; i < rpo.size(); ++i) {
				const uint32_t b = rpo[i];
				uint32_t candidate = NONE;
				for (uint32_t p : preds[b]) {
					if (idom[p] == NONE) {
						continue;
					}
					if (candidate == NONE) {
						candidate = p;
						continue;
					}
					uint32_t x = p, y = candidate;
					while (x != y) {
						while (rpoNumber[x] > rpoNumber[y]) {
							x = idom[x];
						}
						while (rpoNumber[y] > rpoNumber[x]) {
							y = idom[y];
						}
					}
					candidate = x;
				}
				if (candidate != idom[b]) {
					idom[b] = candidate;
					changed = true;
				}
			}
		}
	}
}

bool Cfg::dominates(uint32_t a, uint32_t b) const {
	if (!reachable(a) || !reachable(b)) {
		return false;
	}
	while (true) {
		if (a == b) {
			return true;
		}
		if (b == 0) {
			return false;
		}
		b = idom[b];
	}
}

std::vector<Loop> findLoops(const Cfg& cfg) {
	const uint32_t n = cfg.size();
	std::vector<Loop> loops;
	std::vector<uint32_t> loopOfHeader(n, NONE);
	for (uint32_t s = 0; s < n; ++s) {
		if (!cfg.reachable(s)) {
			continue;
		}
		for (uint32_t h : cfg.succs[s]) {
			if (!cfg.dominates(h, s)) {
				continue;
			}
			if (loopOfHeader[h] == NONE) {
				loopOfHeader[h] = static_cast<uint32_t>(loops.size());
				loops.emplace_back();
				loops.back().header = h;
			}
			loops[loopOfHeader[h]].latches.push_back(s);
		}
	}
	for (auto& loop : loops) {
		loop.inBody.assign(n, 0);
		loop.inBody[loop.header] = 1;
		std::vector<uint32_t> work;
		for (uint32_t l : loop.latches) {
			if (!loop.inBody[l]) {
				loop.inBody[l] = 1;
				work.push_back(l);
			}
		}
		while (!work.empty()) {
			const uint32_t b = work.back();
			work.pop_back();
			for (uint32_t p : cfg.preds[b]) {
				if (cfg.reachable(p) && !loop.inBody[p]) {
					loop.inBody[p] = 1;
					work.push_back(p);
				}
			}
		}
		for (uint32_t b = 0; b < n; ++b) {
			if (loop.inBody[b]) {
				loop.body.push_back(b);
			}
		}
		for (uint32_t p : cfg.preds[loop.header]) {
			if (cfg.reachable(p) && !loop.inBody[p]) {
				loop.outsidePreds.push_back(p);
			}
		}
	}
	std::stable_sort(loops.begin(), loops.end(),
	                 [](const Loop& a, const Loop& b) { return a.body.size() < b.body.size(); });
	return loops;
}

uint32_t ensurePreheader(MutableFunction& fn, const Cfg& cfg, const Loop& loop) {
	(void) cfg;
	const uint32_t h = loop.header;
	if (h == 0 || loop.outsidePreds.empty()) {
		return NONE;
	}
	for (uint32_t p : loop.outsidePreds) {
		if (p >= h) {
			return NONE;
		}
	}
	const uint32_t headerUid = fn.blocks[h].uid;
	if (loop.outsidePreds.size() == 1) {
		auto& pred = fn.blocks[loop.outsidePreds[0]];
		if (pred.terminator().inst.op == static_cast<uint16_t>(Op::Br)) {
			return pred.uid;
		}
	}

	MBlock q;
	q.uid = fn.freshBlockUid();
	const uint16_t argc = fn.blocks[h].argCount();
	MInst br;
	br.inst.op = static_cast<uint16_t>(Op::Br);
	br.inst.a = headerUid;
	br.uid = fn.freshUid();
	br.list.push_back(argc);
	for (uint16_t j = 0; j < argc; ++j) {
		MInst arg;
		arg.inst.op = static_cast<uint16_t>(Op::BlockArg);
		arg.inst.type = fn.blocks[h].insts[j].inst.type;
		arg.inst.a = j;
		arg.uid = fn.freshUid();
		q.insts.push_back(arg);
		br.list.push_back(arg.uid);
	}
	q.insts.push_back(std::move(br));
	for (uint32_t pi : loop.outsidePreds) {
		auto& t = fn.blocks[pi].terminator();
		forEachBlock(t, [&](uint32_t& target) {
			if (target == headerUid) {
				target = q.uid;
			}
		});
	}
	const uint32_t qUid = q.uid;
	fn.blocks.insert(fn.blocks.begin() + h, std::move(q));
	return qUid;
}

} // namespace nautilus::passes
