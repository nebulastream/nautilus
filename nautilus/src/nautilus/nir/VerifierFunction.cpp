#include "nautilus/nir/Operands.hpp"
#include "nautilus/nir/Verifier.hpp"
#include <algorithm>
#include <functional>
#include <string>

namespace nautilus::nir {

namespace {

/// Checks the rules of 13-nir-spec.md §3, §5 and §6.1 for one function. The module tables have been checked before.
class FunctionChecker {
public:
	FunctionChecker(const Module& m, const VerifyOptions& o, uint32_t f, std::vector<Diagnostic>& out)
	    : m_(m), o_(o), f_(f), fn_(m.functions[f]), out_(out), level_(static_cast<Level>(fn_.level)),
	      insts_(m.functionInsts(f)), blocks_(m.functionBlocks(f)), sig_(m.signatures[fn_.signature]) {
	}

	void run() {
		if (blocks_.empty()) {
			report(DiagCode::V001, "the function has no block");
			return;
		}
		if (!buildBlockMap()) {
			return;
		}
		for (uint32_t b = 0; b < blocks_.size(); ++b) {
			terminators(b);
		}
		for (uint32_t i = 0; i < insts_.size(); ++i) {
			instruction(i);
		}
		for (uint32_t b = 0; b < blocks_.size(); ++b) {
			blockArgs(b);
		}
		if (level_ == Level::SSA) {
			ssaLevel();
		} else if (clean()) {
			// the analyses read operands and lists, so they run only on a function without structural errors
			definiteDefinitions();
		}
	}

	/// Intervals of `extra` this function's instructions own: [begin, end).
	std::vector<std::pair<uint32_t, uint32_t>> extraRanges;

private:
	void report(DiagCode code, std::string message, uint32_t inst = NONE, uint32_t block = NONE) {
		reported_++;
		if (out_.size() < o_.maxDiagnostics) {
			out_.push_back(Diagnostic {code, f_, inst, block, std::move(message)});
		}
	}

	// ---- structure -------------------------------------------------------------------------------------------

	/// Fills blockOf_ and returns false if block membership cannot be read.
	bool buildBlockMap() {
		blockOf_.assign(insts_.size(), NONE);
		bool ok = true;
		for (uint32_t b = 0; b < blocks_.size(); ++b) {
			auto& blk = blocks_[b];
			if (blk.pad != 0 || (blk.flags & ~BLOCK_LANDING_PAD) != 0) {
				report(DiagCode::V004, "reserved block flags or padding are set", NONE, b);
			}
			if (level_ == Level::SSA) {
				if (uint64_t(blk.first) + blk.count > insts_.size()) {
					report(DiagCode::V067, "the block is out of the function's instruction range", NONE, b);
					ok = false;
					continue;
				}
				for (uint32_t i = blk.first; i < blk.first + blk.count; ++i) {
					blockOf_[i] = b;
				}
			} else {
				if (uint64_t(blk.first) + blk.count > m_.extra.size()) {
					report(DiagCode::V008, "the block's instruction list is out of bounds of extra", NONE, b);
					ok = false;
					continue;
				}
				extraRanges.push_back({blk.first, blk.first + blk.count});
				for (uint32_t k = 0; k < blk.count; ++k) {
					auto id = m_.extra[blk.first + k];
					if (id >= insts_.size()) {
						report(DiagCode::V054, "the block lists an instruction id out of range", NONE, b);
						ok = false;
					} else if (blockOf_[id] != NONE) {
						report(DiagCode::V054, "the instruction belongs to more than one block", id, b);
						ok = false;
					} else {
						blockOf_[id] = b;
					}
				}
			}
		}
		if (ok) {
			for (uint32_t i = 0; i < insts_.size(); ++i) {
				if (blockOf_[i] == NONE) {
					report(level_ == Level::SSA ? DiagCode::V067 : DiagCode::V054,
					       "the instruction belongs to no block", i);
					ok = false;
				}
			}
		}
		return ok;
	}

	uint32_t blockInst(uint32_t b, uint32_t k) const {
		auto& blk = blocks_[b];
		return level_ == Level::SSA ? blk.first + k : m_.extra[blk.first + k];
	}

	void terminators(uint32_t b) {
		auto& blk = blocks_[b];
		if (blk.count == 0) {
			report(DiagCode::V002, "the block is empty", NONE, b);
			return;
		}
		for (uint32_t k = 0; k < blk.count; ++k) {
			auto id = blockInst(b, k);
			auto* info = coreOpInfo(insts_[id].op);
			bool term = info && (info->flags & OpTerminator);
			if (k + 1 == blk.count && !term) {
				report(DiagCode::V002, "the block does not end in a terminator", id, b);
			} else if (k + 1 != blk.count && term) {
				report(DiagCode::V002, "a terminator is not the last instruction of its block", id, b);
			}
		}
	}

	void blockArgs(uint32_t b) {
		auto& blk = blocks_[b];
		if (level_ == Level::PreSSA) {
			if (blk.argCount != 0) {
				report(DiagCode::V052, "a pre-SSA block has no block arguments", NONE, b);
			}
			if (blk.flags & BLOCK_LANDING_PAD) {
				report(DiagCode::V053, "a pre-SSA block is not a landing pad", NONE, b);
			}
			return;
		}
		if (blk.argCount > blk.count) {
			report(DiagCode::V020, "argCount exceeds the block's instructions", NONE, b);
			return;
		}
		for (uint32_t k = 0; k < blk.count; ++k) {
			auto& inst = insts_[blk.first + k];
			bool isArg = inst.op == static_cast<uint16_t>(Op::BlockArg);
			if (k < blk.argCount) {
				if (!isArg || inst.a != k) {
					report(DiagCode::V020, "the first argCount instructions must be blockargs numbered by position",
					       blk.first + k, b);
				}
			} else if (isArg) {
				report(DiagCode::V020, "blockarg outside the leading argCount instructions", blk.first + k, b);
			}
		}
	}

	/// No diagnostic was reported for this function so far.
	bool clean() const {
		return reported_ == 0;
	}

	bool isPad(uint32_t b) const {
		return b < blocks_.size() && (blocks_[b].flags & BLOCK_LANDING_PAD) != 0;
	}

	/// The unwind entry of an instruction if it carries a block id, else NONE.
	uint32_t unwindBlock(const Inst& inst) const {
		uint32_t unwind = NO_UNWIND;
		switch (static_cast<Op>(inst.op)) {
		case Op::Div:
		case Op::Rem:
			if (inst.flags & 2) {
				unwind = inst.c;
			}
			break;
		case Op::Call:
		case Op::CallIndirect: {
			uint32_t len = 0;
			if (listLength(inst, len)) {
				unwind = m_.extra[inst.c + 1 + m_.extra[inst.c]];
			}
			break;
		}
		default:
			break;
		}
		return level_ == Level::SSA && unwind < blocks_.size() ? unwind : NONE;
	}

	/// Successor blocks of block b: branch targets and unwind edges.
	void successors(uint32_t b, std::vector<uint32_t>& out) const {
		auto& blk = blocks_[b];
		for (uint32_t k = 0; k < blk.count; ++k) {
			auto& inst = insts_[blockInst(b, k)];
			auto u = unwindBlock(inst);
			if (u != NONE) {
				out.push_back(u);
			}
			uint32_t len = 0;
			if (inst.op == static_cast<uint16_t>(Op::Br) && inst.a < blocks_.size()) {
				out.push_back(inst.a);
			} else if (inst.op == static_cast<uint16_t>(Op::CondBr) && listLength(inst, len)) {
				if (inst.b < blocks_.size()) {
					out.push_back(inst.b);
				}
				uint32_t f = m_.extra[inst.c + 2 + m_.extra[inst.c + 1]];
				if (f < blocks_.size()) {
					out.push_back(f);
				}
			}
		}
	}

	void ssaLevel() {
		// NIR-V066 entry block
		if (isPad(0)) {
			report(DiagCode::V066, "the entry block is not a landing pad", NONE, 0);
		}
		if (blocks_[0].argCount != sig_.paramCount) {
			report(DiagCode::V066, "the entry block's arguments equal the signature's parameters", NONE, 0);
		} else {
			for (uint32_t k = 0; k < sig_.paramCount; ++k) {
				if (static_cast<TypeId>(insts_[blocks_[0].first + k].type) != typeOf(m_.paramType(sig_, k))) {
					report(DiagCode::V066, "entry argument " + std::to_string(k) + " differs from the parameter type",
					       blocks_[0].first + k, 0);
				}
			}
		}
		// NIR-V064 landing pads
		std::vector<uint32_t> succ;
		for (uint32_t b = 0; b < blocks_.size(); ++b) {
			bool pad = isPad(b);
			if (pad && blocks_[b].argCount != 0) {
				report(DiagCode::V064, "a landing pad has no arguments", NONE, b);
			}
			auto& blk = blocks_[b];
			for (uint32_t k = 0; k < blk.count; ++k) {
				auto id = blk.first + k;
				auto& inst = insts_[id];
				auto u = unwindBlock(inst);
				if (u != NONE && !isPad(u)) {
					report(DiagCode::V064, "an unwind entry names a block that is not a landing pad", id, b);
				}
				bool br = inst.op == static_cast<uint16_t>(Op::Br);
				if (br && inst.a < blocks_.size() && isPad(inst.a) && !pad) {
					report(DiagCode::V064, "only landing pads branch to landing pads", id, b);
				}
				if (inst.op == static_cast<uint16_t>(Op::CondBr)) {
					if (pad) {
						report(DiagCode::V064, "condbr does not occur in landing pads", id, b);
					} else {
						uint32_t len = 0;
						if (listLength(inst, len)) {
							uint32_t f = m_.extra[inst.c + 2 + m_.extra[inst.c + 1]];
							if (isPad(inst.b) || isPad(f)) {
								report(DiagCode::V064, "only unwind entries enter landing pads", id, b);
							}
						}
					}
				}
				if (pad && inst.op == static_cast<uint16_t>(Op::Ret)) {
					report(DiagCode::V064, "ret does not occur in landing pads", id, b);
				}
				if (pad && br) {
					if (inst.a >= blocks_.size() || !isPad(inst.a)) {
						report(DiagCode::V064, "a pad's br targets another pad", id, b);
					}
					if (m_.extra[inst.c] != 0) {
						report(DiagCode::V064, "a pad's br has no arguments", id, b);
					}
				}
			}
		}
		// pad br edges form no cycle: every pad has at most one out edge, so follow it
		for (uint32_t start = 0; start < blocks_.size(); ++start) {
			uint32_t cur = start;
			for (size_t steps = 0; isPad(cur) && steps <= blocks_.size(); ++steps) {
				auto& blk = blocks_[cur];
				if (blk.count == 0) {
					break;
				}
				const Inst& last = insts_[blk.first + blk.count - 1];
				if (last.op != static_cast<uint16_t>(Op::Br) || last.a >= blocks_.size()) {
					break;
				}
				cur = last.a;
				if (cur == start) {
					report(DiagCode::V064, "the br edges between landing pads form a cycle", NONE, start);
					break;
				}
			}
		}
		// NIR-V063 reachability
		std::vector<uint8_t> seen(blocks_.size(), 0);
		std::vector<uint32_t> work {0};
		seen[0] = 1;
		while (!work.empty()) {
			auto b = work.back();
			work.pop_back();
			succ.clear();
			successors(b, succ);
			for (auto s : succ) {
				if (!seen[s]) {
					seen[s] = 1;
					work.push_back(s);
				}
			}
		}
		bool allReachable = true;
		for (uint32_t b = 0; b < blocks_.size(); ++b) {
			if (!seen[b]) {
				report(DiagCode::V063, "the block is not reachable from the entry block", NONE, b);
				allReachable = false;
			}
		}
		if (allReachable && clean()) {
			dominance();
			layout();
		}
	}

	// ---- control flow graph, dominance, layout (SSA) ---------------------------------------------------------

	void buildCfg() {
		succ_.assign(blocks_.size(), {});
		pred_.assign(blocks_.size(), {});
		for (uint32_t b = 0; b < blocks_.size(); ++b) {
			successors(b, succ_[b]);
			for (auto t : succ_[b]) {
				pred_[t].push_back(b);
			}
		}
	}

	/// Immediate dominators by the iterative algorithm of Cooper, Harvey and Kennedy over reverse post-order.
	void computeDominators() {
		buildCfg();
		auto n = static_cast<uint32_t>(blocks_.size());
		std::vector<uint32_t> order; // post-order
		std::vector<uint8_t> state(n, 0);
		std::vector<std::pair<uint32_t, size_t>> stack {{0, 0}};
		state[0] = 1;
		while (!stack.empty()) {
			auto& [b, k] = stack.back();
			if (k < succ_[b].size()) {
				auto t = succ_[b][k++];
				if (!state[t]) {
					state[t] = 1;
					stack.push_back({t, 0});
				}
			} else {
				order.push_back(b);
				stack.pop_back();
			}
		}
		rpoNumber_.assign(n, NONE);
		for (uint32_t i = 0; i < order.size(); ++i) {
			rpoNumber_[order[i]] = static_cast<uint32_t>(order.size()) - 1 - i;
		}
		idom_.assign(n, NONE);
		idom_[0] = 0;
		auto intersect = [&](uint32_t a, uint32_t b) {
			while (a != b) {
				while (rpoNumber_[a] > rpoNumber_[b]) {
					a = idom_[a];
				}
				while (rpoNumber_[b] > rpoNumber_[a]) {
					b = idom_[b];
				}
			}
			return a;
		};
		for (bool changed = true; changed;) {
			changed = false;
			for (auto it = order.rbegin(); it != order.rend(); ++it) {
				auto b = *it;
				if (b == 0) {
					continue;
				}
				uint32_t d = NONE;
				for (auto p : pred_[b]) {
					if (idom_[p] == NONE) {
						continue;
					}
					d = d == NONE ? p : intersect(p, d);
				}
				if (d != idom_[b]) {
					idom_[b] = d;
					changed = true;
				}
			}
		}
	}

	bool dominates(uint32_t a, uint32_t b) const {
		while (true) {
			if (a == b) {
				return true;
			}
			if (b == 0 || idom_[b] == NONE) {
				return false;
			}
			b = idom_[b];
		}
	}

	/// NIR-V061: every use is dominated by its definition, unwind edges included.
	void dominance() {
		computeDominators();
		// unwind edges: instruction id -> pad block
		std::vector<std::pair<uint32_t, uint32_t>> unwinds;
		for (uint32_t i = 0; i < insts_.size(); ++i) {
			auto u = unwindBlock(insts_[i]);
			if (u != NONE) {
				unwinds.push_back({i, u});
			}
		}
		for (uint32_t u = 0; u < insts_.size(); ++u) {
			auto ub = blockOf_[u];
			forEachRef(insts_[u], m_.extra.data(), level_, [&](const uint32_t& r) {
				if (!isValueRef(r) || r >= insts_.size()) {
					return;
				}
				auto db = blockOf_[r];
				if (db == ub) {
					if (r >= u) {
						report(DiagCode::V061, "a value is used before its definition", u, ub);
					}
				} else if (!dominates(db, ub)) {
					report(DiagCode::V061, "the definition %" + std::to_string(r) + " does not dominate the use", u,
					       ub);
				} else if (isPad(ub)) {
					// within the block of a throwing instruction, the definition must precede it, and the
					// throwing instruction's own result is not available in the pad
					for (auto [t, pad] : unwinds) {
						if (pad == ub && blockOf_[t] == db && r >= t) {
							report(DiagCode::V061,
							       "the definition %" + std::to_string(r) +
							           " does not precede the instruction that unwinds to this pad",
							       u, ub);
							break;
						}
					}
				}
			});
		}
	}

	/// NIR-V062: forward edges go forward, reducible loops are contiguous, pads come last in order of first reference.
	void layout() {
		auto n = static_cast<uint32_t>(blocks_.size());
		// strongly connected components of the non-pad subgraph, to find irreducible regions
		std::vector<uint32_t> comp(n, NONE);
		{
			std::vector<uint32_t> index(n, NONE), low(n, 0), st;
			std::vector<uint8_t> on(n, 0);
			uint32_t counter = 0, comps = 0;
			std::function<void(uint32_t)> visit = [&](uint32_t v) {
				index[v] = low[v] = counter++;
				st.push_back(v);
				on[v] = 1;
				for (auto w : succ_[v]) {
					if (isPad(w)) {
						continue;
					}
					if (index[w] == NONE) {
						visit(w);
						low[v] = std::min(low[v], low[w]);
					} else if (on[w]) {
						low[v] = std::min(low[v], index[w]);
					}
				}
				if (low[v] == index[v]) {
					while (true) {
						auto w = st.back();
						st.pop_back();
						on[w] = 0;
						comp[w] = comps;
						if (w == v) {
							break;
						}
					}
					comps++;
				}
			};
			for (uint32_t v = 0; v < n; ++v) {
				if (!isPad(v) && index[v] == NONE) {
					visit(v);
				}
			}
		}
		std::vector<uint32_t> entries(n, 0); // per component: number of entry blocks
		std::vector<uint32_t> compSize(n, 0);
		for (uint32_t b = 0; b < n; ++b) {
			if (isPad(b)) {
				continue;
			}
			compSize[comp[b]]++;
			bool entry = b == 0;
			for (auto p : pred_[b]) {
				entry |= comp[p] != comp[b];
			}
			entries[comp[b]] += entry;
		}
		auto irreducible = [&](uint32_t b) {
			return compSize[comp[b]] > 1 && entries[comp[b]] > 1;
		};

		for (uint32_t s = 0; s < n; ++s) {
			if (isPad(s)) {
				continue;
			}
			for (auto t : succ_[s]) {
				if (isPad(t) || t > s || dominates(t, s)) {
					continue;
				}
				// an edge to an earlier (or the same) block whose target does not dominate the source
				if (!(irreducible(s) && comp[s] == comp[t])) {
					report(DiagCode::V062, "a forward edge goes to an earlier block", NONE, s);
				}
			}
		}
		// reducible natural loops are contiguous
		for (uint32_t s = 0; s < n; ++s) {
			if (isPad(s)) {
				continue;
			}
			for (auto h : succ_[s]) {
				if (isPad(h) || !dominates(h, s) || irreducible(h)) {
					continue;
				}
				std::vector<uint8_t> in(n, 0);
				std::vector<uint32_t> work {s};
				in[h] = 1;
				in[s] = 1;
				while (!work.empty()) {
					auto x = work.back();
					work.pop_back();
					if (x == h) {
						continue;
					}
					for (auto p : pred_[x]) {
						if (!in[p] && !isPad(p)) {
							in[p] = 1;
							work.push_back(p);
						}
					}
				}
				uint32_t lo = n, hi = 0, count = 0;
				for (uint32_t x = 0; x < n; ++x) {
					if (in[x]) {
						lo = std::min(lo, x);
						hi = std::max(hi, x);
						count++;
					}
				}
				if (hi - lo + 1 != count) {
					report(DiagCode::V062, "the blocks of the loop headed by this block are not contiguous", NONE, h);
				}
			}
		}
		// pads: after all other blocks, in order of first reference
		uint32_t firstPad = n;
		for (uint32_t b = 0; b < n; ++b) {
			if (isPad(b)) {
				firstPad = std::min(firstPad, b);
			}
		}
		for (uint32_t b = firstPad; b < n; ++b) {
			if (!isPad(b)) {
				report(DiagCode::V062, "a landing pad precedes a block that is not a pad", NONE, b);
				break;
			}
		}
		std::vector<uint32_t> firstRef;
		std::vector<uint8_t> referenced(n, 0);
		for (uint32_t i = 0; i < insts_.size(); ++i) {
			uint32_t target = unwindBlock(insts_[i]);
			if (target == NONE && insts_[i].op == static_cast<uint16_t>(Op::Br) && isPad(blockOf_[i]) &&
			    insts_[i].a < n) {
				target = insts_[i].a;
			}
			if (target != NONE && isPad(target) && !referenced[target]) {
				referenced[target] = 1;
				firstRef.push_back(target);
			}
		}
		if (!std::is_sorted(firstRef.begin(), firstRef.end())) {
			report(DiagCode::V062, "landing pads are not in the order of their first reference");
		}
	}

	// ---- pre-SSA: definitions reach every use ----------------------------------------------------------------

	/// NIR-V051: a forward must-analysis of the instructions that have executed on every path.
	void definiteDefinitions() {
		auto n = static_cast<uint32_t>(blocks_.size());
		auto words = (insts_.size() + 63) / 64;
		std::vector<std::vector<uint64_t>> in(n, std::vector<uint64_t>(words, ~0ull)), out = in;
		in[0].assign(words, 0);
		buildCfg();
		auto transfer = [&](uint32_t b, std::vector<uint64_t>& set, bool check) {
			auto& blk = blocks_[b];
			for (uint32_t k = 0; k < blk.count; ++k) {
				auto id = m_.extra[blk.first + k];
				const Inst& inst = insts_[id];
				if (check) {
					forEachRef(inst, m_.extra.data(), level_, [&](const uint32_t& r) {
						if (isValueRef(r) && r < insts_.size() && r != id && !((set[r / 64] >> (r % 64)) & 1)) {
							report(DiagCode::V051,
							       "%" + std::to_string(r) + " is not defined on every path to this use", id, b);
						}
					});
				}
				if (inst.type != static_cast<uint8_t>(TypeId::Void)) {
					set[id / 64] |= 1ull << (id % 64);
				}
			}
		};
		for (bool changed = true; changed;) {
			changed = false;
			for (uint32_t b = 0; b < n; ++b) {
				if (b != 0) {
					std::vector<uint64_t> meet(words, ~0ull);
					for (auto p : pred_[b]) {
						for (size_t w = 0; w < words; ++w) {
							meet[w] &= out[p][w];
						}
					}
					in[b] = meet;
				}
				auto set = in[b];
				transfer(b, set, false);
				if (set != out[b]) {
					out[b] = std::move(set);
					changed = true;
				}
			}
		}
		for (uint32_t b = 0; b < n; ++b) {
			if (pred_[b].empty() && b != 0) {
				continue; // unreachable
			}
			auto set = in[b];
			transfer(b, set, true);
		}
	}

	// ---- operands --------------------------------------------------------------------------------------------

	/// The type of a value reference, or false if the reference is invalid (reported by V005).
	bool refType(Ref r, TypeId& t) const {
		if (r == NONE || isReservedRef(r)) {
			return false;
		}
		if (isConstRef(r)) {
			if (constIndexOf(r) >= m_.consts.size()) {
				return false;
			}
			t = static_cast<TypeId>(m_.consts.at(constIndexOf(r)).type);
			return true;
		}
		if (r >= insts_.size()) {
			return false;
		}
		t = static_cast<TypeId>(insts_[r].type);
		return t != TypeId::Void;
	}

	/// Checks that `r` is a valid value reference and returns its type through `t`.
	bool useRef(uint32_t inst, Ref r, TypeId& t, const char* what) {
		if (r == NONE || isReservedRef(r)) {
			report(DiagCode::V005, std::string(what) + " is NONE or a reserved reference", inst);
			return false;
		}
		if (isConstRef(r)) {
			if (constIndexOf(r) >= m_.consts.size()) {
				report(DiagCode::V005, std::string(what) + " names a constant out of range", inst);
				return false;
			}
		} else if (r >= insts_.size()) {
			report(DiagCode::V005, std::string(what) + " names an instruction out of range", inst);
			return false;
		} else if (insts_[r].type == static_cast<uint8_t>(TypeId::Void)) {
			report(DiagCode::V005, std::string(what) + " names an instruction without a result", inst);
			return false;
		}
		return refType(r, t);
	}

	void expectType(uint32_t inst, TypeId have, bool ok, const char* what) {
		(void) have;
		if (!ok) {
			report(DiagCode::V006, std::string(what), inst);
		}
	}

	/// The length of the extra list of `inst`, false if the list is out of bounds.
	bool listLength(const Inst& inst, uint32_t& len) const {
		if (inst.c >= m_.extra.size()) {
			return false;
		}
		const uint32_t* l = m_.extra.data() + inst.c;
		uint64_t avail = m_.extra.size() - inst.c;
		switch (static_cast<Op>(inst.op)) {
		case Op::Call:
			len = 0;
			if (l[0] > avail) {
				return false;
			}
			len = 1 + l[0] + 2;
			break;
		case Op::CallIndirect:
			if (l[0] > avail) {
				return false;
			}
			len = 1 + l[0] + 3;
			break;
		case Op::Br:
			if (l[0] > avail) {
				return false;
			}
			len = 1 + l[0];
			break;
		case Op::CondBr: {
			if (avail < 2 || l[1] > avail) {
				return false;
			}
			if (uint64_t(l[1]) + 4 > avail || l[3 + l[1]] > avail) {
				return false;
			}
			len = 4 + l[1] + l[3 + l[1]];
			break;
		}
		default:
			return false;
		}
		return len <= avail;
	}

	void instruction(uint32_t i) {
		const Inst& inst = insts_[i];
		auto* info = coreOpInfo(inst.op);
		if (info == nullptr) {
			if (!(inst.op >= FIRST_EXTENSION_OP && o_.isExtensionOp && o_.isExtensionOp(inst.op))) {
				report(DiagCode::V003, "unknown opcode " + std::to_string(inst.op), i);
			}
			return;
		}
		auto op = static_cast<Op>(inst.op);
		auto type = static_cast<TypeId>(inst.type);
		if (level_ == Level::SSA && (op == Op::Assign || op == Op::Var)) {
			report(DiagCode::V060, "assign and var occur only at the pre-SSA level", i);
			return;
		}

		// unused fields and reserved flag bits
		auto unused = [&](OperandKind k, uint32_t v, const char* name) {
			if (k == OperandKind::Unused && v != 0) {
				report(DiagCode::V004, std::string("unused operand field ") + name + " is not 0", i);
			}
		};
		unused(info->a, inst.a, "a");
		unused(info->b, inst.b, "b");
		unused(info->c, inst.c, "c");
		uint8_t flagMask = 0;
		switch (op) {
		case Op::Div:
		case Op::Rem:
			flagMask = 3;
			break;
		case Op::Shr:
			flagMask = 1;
			break;
		case Op::Cmp:
		case Op::Cast:
		case Op::Load:
		case Op::Store:
			flagMask = 0x0F;
			break;
		default:
			break;
		}
		if ((inst.flags & ~flagMask) != 0) {
			report(DiagCode::V004, "reserved flag bits are set", i);
		}
		if (type != TypeId::Void && !isScalar(type) && !isExtension(type)) {
			report(DiagCode::V006, "the result type is not a valid type", i);
			return;
		}

		TypeId ta = TypeId::Void, tb = TypeId::Void, tc = TypeId::Void;
		auto A = [&] {
			return useRef(i, inst.a, ta, "operand a");
		};
		auto B = [&] {
			return useRef(i, inst.b, tb, "operand b");
		};
		auto C = [&] {
			return useRef(i, inst.c, tc, "operand c");
		};
		auto resultIs = [&](TypeId t) {
			if (type != t) {
				report(DiagCode::V006, "the result type does not match the operation", i);
			}
		};
		auto sameBinary = [&](auto pred, const char* what) {
			bool a = A(), b = B();
			if (a && b) {
				if (ta != tb || !pred(ta)) {
					report(DiagCode::V006, what, i);
				}
				resultIs(ta);
			}
		};

		switch (op) {
		case Op::Add:
		case Op::Sub: {
			bool a = A(), b = B();
			if (a && b) {
				bool ptrArith = ta == TypeId::Ptr && tb == TypeId::I64;
				if (!ptrArith && (ta != tb || !(isInteger(ta) || isFloat(ta)))) {
					report(DiagCode::V006, "operands must be equal integer or float types, or ptr and i64", i);
				}
				resultIs(ta);
			}
			break;
		}
		case Op::Mul:
			sameBinary([](TypeId t) { return isInteger(t) || isFloat(t); }, "operands must be equal numeric types");
			break;
		case Op::Div:
		case Op::Rem:
			divRem(i, inst, type);
			break;
		case Op::FNeg:
			if (A()) {
				if (!isFloat(ta)) {
					report(DiagCode::V006, "fneg needs a float", i);
				}
				resultIs(ta);
			}
			break;
		case Op::And:
		case Op::Or:
			sameBinary([](TypeId t) { return t == TypeId::Bool; }, "operands must be bool");
			break;
		case Op::Not:
			if (A()) {
				if (ta != TypeId::Bool) {
					report(DiagCode::V006, "not needs a bool", i);
				}
				resultIs(TypeId::Bool);
			}
			break;
		case Op::BAnd:
		case Op::BOr:
		case Op::BXor:
		case Op::Shl:
		case Op::Shr:
			sameBinary([](TypeId t) { return isInteger(t); }, "operands must be equal integer types");
			break;
		case Op::BNot:
			if (A()) {
				if (!isInteger(ta)) {
					report(DiagCode::V006, "bnot needs an integer", i);
				}
				resultIs(ta);
			}
			break;
		case Op::Cmp:
			compare(i, inst, type);
			break;
		case Op::Select: {
			bool a = A(), b = B(), c = C();
			if (a && ta != TypeId::Bool) {
				report(DiagCode::V006, "the condition of select must be bool", i);
			}
			if (b && c) {
				if (tb != tc) {
					report(DiagCode::V006, "select operands differ in type", i);
				}
				resultIs(tb);
			}
			break;
		}
		case Op::Cast:
			cast(i, inst, type);
			break;
		case Op::Load:
		case Op::Store:
			memory(i, inst, type, op == Op::Store);
			break;
		case Op::Alloca:
			resultIs(TypeId::Ptr);
			if (inst.a >= fn_.allocaCount) {
				report(DiagCode::V007, "alloca slot out of range", i);
			}
			break;
		case Op::Call:
		case Op::CallIndirect:
		case Op::FuncAddr:
			call(i, inst, type);
			break;
		case Op::BindAddr:
			resultIs(TypeId::Ptr);
			if (inst.a >= m_.bindings.size()) {
				report(DiagCode::V007, "binding index out of range", i);
			}
			break;
		case Op::DataAddr:
			resultIs(TypeId::Ptr);
			if (inst.a >= m_.data.size()) {
				report(DiagCode::V007, "data object index out of range", i);
			}
			break;
		case Op::PtrConst:
			resultIs(TypeId::Ptr);
			if (inst.a != NONE) {
				if (inst.a >= m_.pointers.size()) {
					report(DiagCode::V007, "pointers index out of range", i);
				} else if (m_.pointers[inst.a].kind != static_cast<uint8_t>(PointerKind::TracedAddress)) {
					report(DiagCode::V007, "ptr.const needs a pointers entry of kind traced address", i);
				}
			}
			break;
		case Op::BlockArg:
			if (type == TypeId::Void) {
				report(DiagCode::V006, "blockarg needs a non-void type", i);
			}
			blockArgPlacement(i, inst);
			break;
		case Op::Br:
		case Op::CondBr:
			branch(i, inst, type);
			break;
		case Op::Ret:
			ret(i, inst, type);
			break;
		case Op::Resume:
			resultIs(TypeId::Void);
			if (level_ == Level::PreSSA || !isPad(blockOf_[i])) {
				report(DiagCode::V065, "resume occurs only in landing pads", i);
			}
			break;
		case Op::Assign:
			resultIs(TypeId::Void);
			if (isConstRef(inst.a)) {
				report(DiagCode::V050, "assign targets a variable, never a constant", i);
			} else if (A() && inst.a >= i) {
				report(DiagCode::V050, "assign targets a variable defined earlier", i);
			}
			if (A() && B() && ta != tb) {
				report(DiagCode::V050, "assign operands differ in type", i);
			}
			break;
		case Op::Var:
			if (A()) {
				resultIs(ta);
			}
			break;
		}
	}

	void blockArgPlacement(uint32_t i, const Inst& inst) {
		if (level_ == Level::PreSSA) {
			if (i >= sig_.paramCount || inst.a != i) {
				report(DiagCode::V020, "pre-SSA blockargs are the function's parameters, numbered by position", i);
			}
		}
		// SSA placement is checked per block (blockArgs)
	}

	void divRem(uint32_t i, const Inst& inst, TypeId type) {
		TypeId ta = TypeId::Void, tb = TypeId::Void;
		bool a = useRef(i, inst.a, ta, "operand a"), b = useRef(i, inst.b, tb, "operand b");
		if (a && b) {
			if (ta != tb || !(isInteger(ta) || isFloat(ta))) {
				report(DiagCode::V006, "operands must be equal integer or float types", i);
			}
			if (type != ta) {
				report(DiagCode::V006, "the result type does not match the operation", i);
			}
			bool checked = (inst.flags & 2) != 0;
			if (level_ == Level::PreSSA && isInteger(ta) && (!checked || inst.c == NO_UNWIND)) {
				report(DiagCode::V055, "a pre-SSA integer div/rem is checked and has a cleanup list", i);
			}
			if (isFloat(ta)) {
				if (inst.flags != 0 || inst.c != 0) {
					report(DiagCode::V016, "float div/rem has no flags and c = 0", i);
				}
			} else if (checked) {
				if (inst.c == NO_UNWIND) {
					report(DiagCode::V016, "checked div/rem needs an unwind entry other than NO_UNWIND", i);
				} else {
					unwindEntry(i, inst.c);
				}
			} else if (inst.c != 0) {
				report(DiagCode::V016, "unchecked div/rem has c = 0", i);
			}
		}
	}

	void compare(uint32_t i, const Inst& inst, TypeId type) {
		TypeId ta = TypeId::Void, tb = TypeId::Void;
		bool a = useRef(i, inst.a, ta, "operand a"), b = useRef(i, inst.b, tb, "operand b");
		if (type != TypeId::Bool) {
			report(DiagCode::V006, "cmp yields bool", i);
		}
		auto pred = inst.flags & 7;
		bool unsignedBit = (inst.flags & 8) != 0;
		if (inst.flags & 0xF0) {
			return; // V004 already reported
		}
		if (pred > 5) {
			report(DiagCode::V017, "comparison predicate out of range", i);
		}
		if (a && b) {
			if (ta != tb || !isScalar(ta)) {
				report(DiagCode::V006, "cmp operands must be equal scalar types", i);
				return;
			}
			bool ordered = pred >= 2 && pred <= 5;
			if (unsignedBit && !(ordered && isInteger(ta))) {
				report(DiagCode::V017, "the unsigned bit applies to ordered integer predicates only", i);
			}
			if (ta == TypeId::Bool && pred > 1) {
				report(DiagCode::V017, "bool allows only eq and ne", i);
			}
		}
	}

	void cast(uint32_t i, const Inst& inst, TypeId type) {
		TypeId ta = TypeId::Void;
		if (!useRef(i, inst.a, ta, "operand a")) {
			return;
		}
		if (inst.flags & 0xF0) {
			return; // V004
		}
		auto kind = inst.flags & 0xF;
		bool ok = false;
		auto intLike = [](TypeId t) {
			return isInteger(t) || t == TypeId::Bool;
		};
		auto bits = [](TypeId t) {
			return t == TypeId::Bool ? 1u : sizeOf(t) * 8;
		};
		switch (kind) {
		case 0:
		case 1:
			ok = intLike(ta) && isInteger(type) && bits(type) > bits(ta);
			break;
		case 2:
			ok = isInteger(ta) && isInteger(type) && bits(type) < bits(ta);
			break;
		case 3:
		case 4:
			ok = isInteger(ta) && isFloat(type);
			break;
		case 5:
		case 6:
			ok = isFloat(ta) && isInteger(type);
			break;
		case 7:
			ok = ta == TypeId::F32 && type == TypeId::F64;
			break;
		case 8:
			ok = ta == TypeId::F64 && type == TypeId::F32;
			break;
		case 9: {
			auto in = [](TypeId t) {
				return t == TypeId::I32 || t == TypeId::F32 || t == TypeId::I64 || t == TypeId::F64;
			};
			ok = in(ta) && in(type) && ta != type && sizeOf(ta) == sizeOf(type);
			break;
		}
		case 10:
			ok = ta == TypeId::Ptr && type == TypeId::I64;
			break;
		case 11:
			ok = ta == TypeId::I64 && type == TypeId::Ptr;
			break;
		default:
			break;
		}
		if (!ok) {
			report(DiagCode::V018, "the cast kind does not allow this source and result type", i);
		}
	}

	void memory(uint32_t i, const Inst& inst, TypeId type, bool isStore) {
		TypeId ta = TypeId::Void, tb = TypeId::Void;
		if (useRef(i, inst.a, ta, "operand a") && ta != TypeId::Ptr) {
			report(DiagCode::V006, "the address must be a ptr", i);
		}
		TypeId valueType = type;
		if (isStore) {
			if (type != TypeId::Void) {
				report(DiagCode::V006, "store has no result", i);
			}
			if (!useRef(i, inst.b, tb, "operand b")) {
				return;
			}
			valueType = tb;
		} else if (type == TypeId::Void) {
			report(DiagCode::V006, "load needs a non-void result type", i);
			return;
		}
		auto k = inst.flags & 0xF;
		if (k > 13) {
			report(DiagCode::V019, "alignment code out of range", i);
		} else if (k != 0 && isCore(valueType) && sizeOf(valueType) != 0 && (1u << (k - 1)) == alignOf(valueType)) {
			report(DiagCode::V019, "a natural alignment must be written as 0", i);
		}
	}

	void unwindEntry(uint32_t i, uint32_t unwind) {
		if (unwind == NO_UNWIND) {
			return;
		}
		if (level_ == Level::SSA) {
			if (unwind != NO_CLEANUP && unwind >= blocks_.size()) {
				report(DiagCode::V012, "the unwind entry is neither NO_UNWIND, NO_CLEANUP nor a block id", i);
			}
		} else {
			if (unwind == NO_CLEANUP) {
				report(DiagCode::V012, "NO_CLEANUP does not occur at the pre-SSA level", i);
			} else if (unwind >= m_.extra.size() || uint64_t(unwind) + 1 + 2ull * m_.extra[unwind] > m_.extra.size()) {
				report(DiagCode::V008, "the cleanup list is out of bounds of extra", i);
			} else {
				extraRanges.push_back({unwind, unwind + 1 + 2 * m_.extra[unwind]});
				cleanupEntries(i, unwind);
			}
		}
	}

	void cleanupEntries(uint32_t i, uint32_t offset) {
		uint32_t n = m_.extra[offset];
		for (uint32_t k = 0; k < n; ++k) {
			TypeId t = TypeId::Void;
			auto callee = m_.extra[offset + 1 + 2 * k];
			bool dtor = callee < m_.callees.size() &&
			            m_.callees[callee].kind == static_cast<uint8_t>(CalleeKind::External) &&
			            m_.callees[callee].target < m_.pointers.size() &&
			            m_.pointers[m_.callees[callee].target].kind == static_cast<uint8_t>(PointerKind::Destructor);
			if (!dtor) {
				report(DiagCode::V056, "a cleanup entry names a callee that is not a destructor callee", i);
			}
			if (useRef(i, m_.extra[offset + 2 + 2 * k], t, "cleanup object") && t != TypeId::Ptr) {
				report(DiagCode::V056, "a cleanup object must be a ptr", i);
			}
		}
	}

	void call(uint32_t i, const Inst& inst, TypeId type) {
		auto op = static_cast<Op>(inst.op);
		if (op == Op::FuncAddr) {
			if (type != TypeId::Ptr) {
				report(DiagCode::V006, "funcaddr yields ptr", i);
			}
			if (inst.a >= m_.callees.size()) {
				report(DiagCode::V007, "callee index out of range", i);
			} else {
				auto& c = m_.callees[inst.a];
				if (c.kind == static_cast<uint8_t>(CalleeKind::Intrinsic)) {
					report(DiagCode::V015, "funcaddr names an intrinsic", i);
				} else if (c.signature < m_.signatures.size() && !sigCAbi(c.signature)) {
					report(DiagCode::V035, "the target of funcaddr uses a type without a C ABI", i);
				}
			}
			return;
		}
		uint32_t len = 0;
		if (!listLength(inst, len)) {
			report(DiagCode::V008, "the call's extra list is out of bounds", i);
			return;
		}
		extraRanges.push_back({inst.c, inst.c + len});
		const uint32_t* l = m_.extra.data() + inst.c;
		uint32_t argc = l[0];
		uint32_t unwind = l[1 + argc];
		uint32_t thunk = l[2 + argc];
		uint32_t sigIdx = NONE;
		uint32_t attrs = 0;
		bool external = false;
		if (op == Op::Call) {
			if (inst.a >= m_.callees.size()) {
				report(DiagCode::V007, "callee index out of range", i);
				return;
			}
			auto& c = m_.callees[inst.a];
			sigIdx = c.signature;
			attrs = c.attrs;
			external = c.kind == static_cast<uint8_t>(CalleeKind::External);
		} else {
			TypeId ta = TypeId::Void;
			if (useRef(i, inst.a, ta, "callee address") && ta != TypeId::Ptr) {
				report(DiagCode::V006, "call.indirect needs a ptr callee", i);
			}
			if (inst.b >= m_.signatures.size()) {
				report(DiagCode::V007, "signature index out of range", i);
				return;
			}
			sigIdx = inst.b;
			attrs = l[3 + argc];
			external = true;
			if ((attrs >> 7) != 0) {
				report(DiagCode::V034, "reserved attribute bits are set in the attrs word", i);
			}
			if (!sigCAbi(sigIdx)) {
				report(DiagCode::V035, "call.indirect uses a type without a C ABI", i);
			}
		}
		if (sigIdx >= m_.signatures.size()) {
			return;
		}
		auto& sig = m_.signatures[sigIdx];
		if (type != typeOf(sig.result)) {
			report(DiagCode::V006, "the result type differs from the signature's", i);
		}
		if (argc != sig.paramCount) {
			report(DiagCode::V011, "argument count differs from the signature's", i);
		} else {
			for (uint32_t k = 0; k < argc; ++k) {
				TypeId t = TypeId::Void;
				if (useRef(i, l[1 + k], t, "call argument") && t != typeOf(m_.paramType(sig, k))) {
					report(DiagCode::V011, "argument " + std::to_string(k) + " differs in type from the signature's",
					       i);
				}
			}
		}
		// unwind entry and thunk
		unwindEntry(i, unwind);
		if (level_ == Level::SSA && isPad(blockOf_[i]) && unwind != NO_UNWIND && unwind != NO_CLEANUP) {
			report(DiagCode::V065, "calls in landing pads have NO_UNWIND or NO_CLEANUP", i);
		}
		if ((attrs & AttrNoUnwind) && unwind != NO_UNWIND) {
			report(DiagCode::V013, "a noUnwind call must have NO_UNWIND", i);
		}
		bool needsThunk = external && unwind != NO_UNWIND;
		if (needsThunk) {
			if (thunk >= m_.pointers.size() ||
			    m_.pointers[thunk].kind != static_cast<uint8_t>(PointerKind::CaptureThunk)) {
				report(DiagCode::V014, "a may-throw external or indirect call needs a capture thunk", i);
			}
		} else if (thunk != NONE) {
			report(DiagCode::V014, "this call must not name a thunk", i);
		}
	}

	bool sigCAbi(uint32_t sig) const {
		auto cabi = [&](TypeWord w) {
			auto t = typeOf(w);
			return !isExtension(t) || (o_.extensionHasCabi && o_.extensionHasCabi(t));
		};
		auto& s = m_.signatures[sig];
		if (!cabi(s.result)) {
			return false;
		}
		for (uint32_t i = 0; i < s.paramCount; ++i) {
			if (!cabi(m_.paramType(s, i))) {
				return false;
			}
		}
		return true;
	}

	void branch(uint32_t i, const Inst& inst, TypeId type) {
		if (type != TypeId::Void) {
			report(DiagCode::V006, "a branch has no result", i);
		}
		uint32_t len = 0;
		if (!listLength(inst, len)) {
			report(DiagCode::V008, "the branch's extra list is out of bounds", i);
			return;
		}
		extraRanges.push_back({inst.c, inst.c + len});
		const uint32_t* l = m_.extra.data() + inst.c;
		auto checkEdge = [&](uint32_t target, const uint32_t* args, uint32_t argc) {
			if (target >= blocks_.size()) {
				report(DiagCode::V007, "branch target out of range", i);
				return;
			}
			if (level_ == Level::PreSSA) {
				if (argc != 0) {
					report(DiagCode::V052, "pre-SSA branch argument lists are empty", i);
				}
				return;
			}
			auto& tb = blocks_[target];
			if (argc != tb.argCount) {
				report(DiagCode::V009, "argument count differs from the target block's blockargs", i);
				return;
			}
			if (uint64_t(tb.first) + tb.argCount > insts_.size()) {
				return;
			}
			for (uint32_t k = 0; k < argc; ++k) {
				TypeId t = TypeId::Void;
				if (useRef(i, args[k], t, "branch argument") && t != static_cast<TypeId>(insts_[tb.first + k].type)) {
					report(DiagCode::V009, "argument " + std::to_string(k) + " differs in type from the blockarg", i);
				}
			}
		};
		if (static_cast<Op>(inst.op) == Op::Br) {
			checkEdge(inst.a, l + 1, l[0]);
		} else {
			TypeId ta = TypeId::Void;
			if (useRef(i, inst.a, ta, "condition") && ta != TypeId::Bool) {
				report(DiagCode::V006, "the condition must be bool", i);
			}
			uint32_t tc = l[1];
			checkEdge(inst.b, l + 2, tc);
			checkEdge(l[2 + tc], l + 4 + tc, l[3 + tc]);
		}
	}

	void ret(uint32_t i, const Inst& inst, TypeId type) {
		if (type != TypeId::Void) {
			report(DiagCode::V006, "ret has no result", i);
		}
		auto want = typeOf(sig_.result);
		if (inst.a == NONE) {
			if (want != TypeId::Void) {
				report(DiagCode::V010, "ret without a value in a function that returns one", i);
			}
			return;
		}
		TypeId t = TypeId::Void;
		if (useRef(i, inst.a, t, "return value")) {
			if (want == TypeId::Void) {
				report(DiagCode::V010, "ret with a value in a void function", i);
			} else if (t != want) {
				report(DiagCode::V010, "the returned value differs in type from the signature's result", i);
			}
		}
	}

	const Module& m_;
	const VerifyOptions& o_;
	uint32_t f_;
	const FunctionRecord& fn_;
	std::vector<Diagnostic>& out_;
	Level level_;
	std::span<const Inst> insts_;
	std::span<const BlockRecord> blocks_;
	const SignatureRecord& sig_;
	size_t reported_ = 0;
	std::vector<uint32_t> blockOf_;
	std::vector<std::vector<uint32_t>> succ_, pred_;
	std::vector<uint32_t> idom_, rpoNumber_;
};

} // namespace

void verifyFunctions(const Module& m, const VerifyOptions& options, std::vector<Diagnostic>& out) {
	auto report = [&](DiagCode c, std::string msg, uint32_t f = NONE) {
		if (out.size() < options.maxDiagnostics) {
			out.push_back(Diagnostic {c, f, NONE, NONE, std::move(msg)});
		}
	};
	// NIR-V067: the instruction, block and alloca ranges are contiguous and in functions order
	uint64_t inst = 0, block = 0, alloca = 0;
	bool layoutOk = true;
	for (uint32_t f = 0; f < m.functions.size(); ++f) {
		auto& fn = m.functions[f];
		if (fn.firstInst != inst || fn.firstBlock != block || fn.firstAlloca != alloca) {
			report(DiagCode::V067, "the function's ranges are not contiguous or not in functions order", f);
			layoutOk = false;
		}
		inst = uint64_t(fn.firstInst) + fn.instCount;
		block = uint64_t(fn.firstBlock) + fn.blockCount;
		alloca = uint64_t(fn.firstAlloca) + fn.allocaCount;
		if (fn.signature >= m.signatures.size()) {
			layoutOk = false; // V007 from the module tables
		}
	}
	if (inst != m.insts.size() || block != m.blocks.size() || alloca != m.allocas.size()) {
		report(DiagCode::V067, "the functions do not cover all instructions, blocks and allocas");
		layoutOk = false;
	}
	if (!layoutOk) {
		return;
	}
	std::vector<std::pair<uint32_t, uint32_t>> ranges;
	for (uint32_t f = 0; f < m.functions.size(); ++f) {
		FunctionChecker c(m, options, f, out);
		c.run();
		ranges.insert(ranges.end(), c.extraRanges.begin(), c.extraRanges.end());
	}
	for (uint32_t s = 0; s < m.signatures.size(); ++s) {
		auto& sig = m.signatures[s];
		if (sig.paramCount != 0) {
			ranges.push_back({sig.paramTypes, sig.paramTypes + sig.paramCount});
		}
	}
	// NIR-V008: lists do not overlap (§3.3)
	std::sort(ranges.begin(), ranges.end());
	for (size_t k = 1; k < ranges.size(); ++k) {
		if (ranges[k].first < ranges[k - 1].second) {
			report(DiagCode::V008, "extra lists overlap at word " + std::to_string(ranges[k].first));
			break;
		}
	}
}

} // namespace nautilus::nir
