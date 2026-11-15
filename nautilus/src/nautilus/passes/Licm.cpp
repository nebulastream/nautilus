#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/passes/Cfg.hpp"
#include "nautilus/passes/Passes.hpp"
#include <algorithm>
#include <set>

namespace nautilus::passes {

using nir::NONE;
using nir::Op;
using nir::Ref;

namespace {

/// Whether the instruction may move to the preheader: it has a result, no effect, cannot throw, and reads no memory.
/// Loads, stores and calls with effects stay where they are (no alias analysis); a call with all the guarantee bits
/// of 13 §4.6.2 (`pure` and `noUnwind`) may be hoisted, as the freedom to optimize states.
bool hoistable(const MInst& mi, const nir::Module& m) {
	const auto& inst = mi.inst;
	if (static_cast<nir::TypeId>(inst.type) == nir::TypeId::Void) {
		return false;
	}
	if (inst.op >= nir::FIRST_EXTENSION_OP) {
		auto* info = ext::ExtensionRegistry::instance().op(inst.op);
		return info != nullptr && info->effects == ext::EffectClass::None;
	}
	const auto* info = nir::coreOpInfo(inst.op);
	if (info == nullptr || (info->flags & (nir::OpTerminator | nir::OpPreSSAOnly)) != 0) {
		return false;
	}
	switch (static_cast<Op>(inst.op)) {
	case Op::BlockArg:
	case Op::Alloca:
	case Op::Load:
	case Op::Store:
	case Op::CallIndirect:
		return false;
	case Op::Div:
	case Op::Rem:
		// a float division never throws, and an integer one whose check was dropped cannot (13 §5.10)
		return (inst.flags & 2) == 0;
	case Op::Call: {
		const auto& callee = m.callees[inst.a];
		return nir::attrsImply(callee.attrs, nir::GUARANTEE_MASK) && mi.list[1 + mi.list[0]] == nir::NO_UNWIND;
	}
	default:
		return (info->flags & nir::OpPure) != 0;
	}
}

/// Hoists the invariant instructions of one loop. Returns how many moved. Invalidates the CFG.
size_t hoistFromLoop(MutableFunction& fn, const Cfg& cfg, const Loop& loop, const nir::Module& m) {
	std::vector<uint32_t> defBlock(fn.nextUid, NONE);
	for (uint32_t b = 0; b < fn.blocks.size(); ++b) {
		for (const auto& mi : fn.blocks[b].insts) {
			defBlock[mi.uid] = b;
		}
	}
	std::vector<char> hoisted(fn.nextUid, 0);
	auto available = [&](Ref r) {
		if (!nir::isValueRef(r)) {
			return true; // a constant
		}
		return hoisted[r] || (defBlock[r] != NONE && !loop.inBody[defBlock[r]]);
	};

	// body blocks in layout order, instructions in order: a definition comes before its uses, so one scan finds every
	// instruction whose operands are invariant
	std::vector<std::pair<uint32_t, uint32_t>> found; // block uid, instruction uid
	for (uint32_t b : loop.body) {
		for (const auto& mi : fn.blocks[b].insts) {
			if (!hoistable(mi, m)) {
				continue;
			}
			bool invariant = true;
			forEachValue(mi, [&](const Ref& r) { invariant = invariant && available(r); });
			if (invariant) {
				hoisted[mi.uid] = 1;
				found.emplace_back(fn.blocks[b].uid, mi.uid);
			}
		}
	}
	if (found.empty()) {
		return 0;
	}
	const uint32_t preheaderUid = ensurePreheader(fn, cfg, loop);
	if (preheaderUid == NONE) {
		return 0;
	}
	const auto index = fn.blockIndex();
	std::set<uint32_t> blocks;
	for (auto [blockUid, instUid] : found) {
		(void) instUid;
		blocks.insert(blockUid);
	}
	// copy the instructions out in the order of discovery (definitions before uses), then drop them from their blocks
	std::vector<const MInst*> where(fn.nextUid, nullptr);
	for (uint32_t blockUid : blocks) {
		for (const auto& mi : fn.blocks[index[blockUid]].insts) {
			if (mi.uid < hoisted.size() && hoisted[mi.uid]) {
				where[mi.uid] = &mi;
			}
		}
	}
	std::vector<MInst> moved;
	moved.reserve(found.size());
	for (auto [blockUid, instUid] : found) {
		(void) blockUid;
		moved.push_back(*where[instUid]);
	}
	for (uint32_t blockUid : blocks) {
		std::erase_if(fn.blocks[index[blockUid]].insts, [&](const MInst& mi) { return hoisted[mi.uid] != 0; });
	}
	auto& pre = fn.blocks[index[preheaderUid]].insts;
	pre.insert(pre.end() - 1, std::make_move_iterator(moved.begin()), std::make_move_iterator(moved.end()));
	return found.size();
}

class LicmPass final : public Pass {
public:
	[[nodiscard]] std::string_view name() const override {
		return "licm";
	}

	std::optional<nir::Module> run(const nir::Module& in, PassContext& context) override {
		// a function without an edge to an earlier block has no loop
		std::vector<uint32_t> candidates;
		for (uint32_t f = 0; f < in.functions.size(); ++f) {
			if (hasBackEdge(in, f)) {
				candidates.push_back(f);
			}
		}
		if (candidates.empty()) {
			return std::nullopt;
		}
		nir::Module out = copyForRewrite(in);
		size_t total = 0;
		for (uint32_t f = 0; f < in.functions.size(); ++f) {
			auto fn = decode(in, f);
			size_t n = 0;
			if (std::find(candidates.begin(), candidates.end(), f) != candidates.end()) {
				n = hoistLoops(fn, in);
			}
			total += n;
			encode(fn, out);
		}
		if (total == 0) {
			return std::nullopt;
		}
		context.changes = total;
		return out;
	}

private:
	static bool hasBackEdge(const nir::Module& m, uint32_t f) {
		auto blocks = m.functionBlocks(f);
		auto insts = m.functionInsts(f);
		for (uint32_t b = 0; b < blocks.size(); ++b) {
			if (blocks[b].count == 0) {
				continue;
			}
			const auto& t = insts[blocks[b].first + blocks[b].count - 1];
			if (t.op == static_cast<uint16_t>(Op::Br) && t.a <= b) {
				return true;
			}
			if (t.op == static_cast<uint16_t>(Op::CondBr)) {
				const uint32_t* l = m.extra.data() + t.c;
				if (t.b <= b || l[2 + l[1]] <= b) {
					return true;
				}
			}
		}
		return false;
	}

	static size_t hoistLoops(MutableFunction& fn, const nir::Module& m) {
		size_t total = 0;
		std::set<uint32_t> done; // header uids already processed
		while (true) {
			Cfg cfg(fn);
			auto loops = findLoops(cfg);
			const Loop* next = nullptr;
			for (const auto& loop : loops) {
				if (!done.count(fn.blocks[loop.header].uid)) {
					next = &loop;
					break;
				}
			}
			if (next == nullptr) {
				return total;
			}
			done.insert(fn.blocks[next->header].uid);
			total += hoistFromLoop(fn, cfg, *next, m);
		}
	}
};

} // namespace

std::unique_ptr<Pass> makeLicmPass() {
	return std::make_unique<LicmPass>();
}

} // namespace nautilus::passes
