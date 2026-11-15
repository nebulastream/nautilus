#include "nautilus/passes/MutableFunction.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include <algorithm>
#include <stdexcept>
#include <string>

namespace nautilus::passes {

using nir::Inst;
using nir::NONE;
using nir::Op;
using nir::Ref;

uint16_t MBlock::argCount() const {
	uint16_t n = 0;
	while (n < insts.size() && insts[n].inst.op == static_cast<uint16_t>(Op::BlockArg)) {
		++n;
	}
	return n;
}

size_t MutableFunction::instCount() const {
	size_t n = 0;
	for (const auto& b : blocks) {
		n += b.insts.size();
	}
	return n;
}

std::vector<uint32_t> MutableFunction::blockIndex() const {
	std::vector<uint32_t> index(nextBlockUid, NONE);
	for (uint32_t i = 0; i < blocks.size(); ++i) {
		index[blocks[i].uid] = i;
	}
	return index;
}

size_t listLength(const Inst& inst, const uint32_t* extra) {
	if (inst.op >= nir::FIRST_EXTENSION_OP) {
		auto* info = ext::ExtensionRegistry::instance().op(inst.op);
		if (info != nullptr && info->shape.c == ext::FieldKind::List) {
			return 1 + extra[inst.c];
		}
		return 0;
	}
	const uint32_t* l = extra + inst.c;
	switch (static_cast<Op>(inst.op)) {
	case Op::Call:
		return 1 + l[0] + 2;
	case Op::CallIndirect:
		return 1 + l[0] + 3;
	case Op::Br:
		return 1 + l[0];
	case Op::CondBr:
		return 4 + l[1] + l[3 + l[1]];
	default:
		return 0;
	}
}

nir::Module copyForRewrite(const nir::Module& in) {
	nir::Module out = in.copyTables();
	// the signatures keep their parameter types in `extra`, which does not come along with the tables
	for (auto& sig : out.signatures) {
		const auto words = std::span<const uint32_t>(in.extra.data() + sig.paramTypes, sig.paramCount);
		sig.paramTypes = sig.paramCount == 0 ? 0 : out.addExtra(words);
	}
	out.rebuildIndexes();
	out.debugInstLocations.clear();
	out.debugInstRegions.clear();
	return out;
}

MutableFunction decode(const nir::Module& m, uint32_t f) {
	const auto& fr = m.functions[f];
	MutableFunction fn;
	fn.record = fr;
	auto allocas = m.functionAllocas(f);
	fn.allocas.assign(allocas.begin(), allocas.end());
	fn.nextUid = fr.instCount;
	fn.nextBlockUid = fr.blockCount;
	fn.hasLocations = m.debugInstLocations.size() == m.insts.size() && !m.insts.empty();
	fn.hasRegions = m.debugInstRegions.size() == m.insts.size() && !m.insts.empty();
	auto insts = m.functionInsts(f);
	auto blocks = m.functionBlocks(f);
	fn.blocks.reserve(blocks.size());
	for (uint32_t b = 0; b < blocks.size(); ++b) {
		MBlock mb;
		mb.uid = b;
		mb.flags = blocks[b].flags;
		mb.insts.reserve(blocks[b].count);
		for (uint32_t i = blocks[b].first; i < blocks[b].first + blocks[b].count; ++i) {
			MInst mi;
			mi.inst = insts[i];
			mi.uid = i;
			if (auto n = listLength(insts[i], m.extra.data()); n != 0) {
				mi.list.assign(m.extra.begin() + insts[i].c, m.extra.begin() + insts[i].c + n);
				mi.inst.c = 0;
			}
			if (fn.hasLocations) {
				mi.location = m.debugInstLocations[fr.firstInst + i];
			}
			if (fn.hasRegions) {
				mi.region = m.debugInstRegions[fr.firstInst + i];
			}
			mb.insts.push_back(std::move(mi));
		}
		fn.blocks.push_back(std::move(mb));
	}
	return fn;
}

std::vector<uint32_t> successors(const MBlock& block) {
	if (block.insts.empty()) {
		return {};
	}
	const auto& t = block.terminator();
	switch (static_cast<Op>(t.inst.op)) {
	case Op::Br:
		return {t.inst.a};
	case Op::CondBr:
		return {t.inst.b, t.list[2 + t.list[1]]};
	default:
		return {};
	}
}

void substitute(MutableFunction& fn, const std::vector<Ref>& subst) {
	auto resolve = [&](Ref r) {
		// follow chains; a replacement is never part of a cycle (it is defined before the use it replaces)
		for (size_t steps = 0; nir::isValueRef(r) && r < subst.size() && subst[r] != NONE && steps <= subst.size();
		     ++steps) {
			r = subst[r];
		}
		return r;
	};
	for (auto& b : fn.blocks) {
		for (auto& mi : b.insts) {
			forEachValue(mi, [&](Ref& r) { r = resolve(r); });
		}
	}
}

void encode(const MutableFunction& fn, nir::Module& out) {
	const auto nBlocks = static_cast<uint32_t>(fn.blocks.size());
	auto index = fn.blockIndex();

	// the blocks reachable from the entry block, landing pads through the unwind entries and `br` of reachable blocks
	std::vector<char> reachable(nBlocks, 0);
	std::vector<uint32_t> work {0};
	reachable[0] = 1;
	auto reach = [&](uint32_t uid) {
		if (uid == NONE || uid >= index.size() || index[uid] == NONE) {
			throw std::logic_error("encode: a block operand names a block that does not exist");
		}
		if (!reachable[index[uid]]) {
			reachable[index[uid]] = 1;
			work.push_back(index[uid]);
		}
	};
	while (!work.empty()) {
		const uint32_t b = work.back();
		work.pop_back();
		for (const auto& mi : fn.blocks[b].insts) {
			forEachBlock(mi, [&](const uint32_t& uid) { reach(uid); });
		}
	}

	// the order: reachable blocks that are not pads in layout order, then the pads in the order of their first
	// reference
	std::vector<uint32_t> order;
	order.reserve(nBlocks);
	std::vector<char> placed(nBlocks, 0);
	for (uint32_t b = 0; b < nBlocks; ++b) {
		if (reachable[b] && !fn.blocks[b].isPad()) {
			order.push_back(b);
			placed[b] = 1;
		}
	}
	const size_t firstPad = order.size();
	auto scanForPads = [&](uint32_t b) {
		for (const auto& mi : fn.blocks[b].insts) {
			forEachBlock(mi, [&](const uint32_t& uid) {
				const uint32_t target = index[uid];
				if (fn.blocks[target].isPad() && !placed[target]) {
					placed[target] = 1;
					order.push_back(target);
				}
			});
		}
	};
	for (size_t i = 0; i < firstPad; ++i) {
		scanForPads(order[i]);
	}
	for (size_t i = firstPad; i < order.size(); ++i) {
		scanForPads(order[i]);
	}

	// ids: the position of an instruction in the new function
	std::vector<uint32_t> idOf(fn.nextUid, NONE);
	std::vector<uint32_t> newBlock(fn.nextBlockUid, NONE);
	uint32_t next = 0;
	for (uint32_t pos = 0; pos < order.size(); ++pos) {
		const auto& block = fn.blocks[order[pos]];
		newBlock[block.uid] = pos;
		for (const auto& mi : block.insts) {
			idOf[mi.uid] = next++;
		}
	}

	nir::FunctionRecord record = fn.record;
	record.firstInst = static_cast<uint32_t>(out.insts.size());
	record.instCount = next;
	record.firstBlock = static_cast<uint32_t>(out.blocks.size());
	record.blockCount = static_cast<uint32_t>(order.size());
	record.firstAlloca = static_cast<uint32_t>(out.allocas.size());
	record.allocaCount = static_cast<uint32_t>(fn.allocas.size());
	out.functions.push_back(record);
	out.allocas.insert(out.allocas.end(), fn.allocas.begin(), fn.allocas.end());
	out.insts.reserve(out.insts.size() + next);

	uint32_t first = 0;
	for (uint32_t pos = 0; pos < order.size(); ++pos) {
		const auto& block = fn.blocks[order[pos]];
		out.blocks.push_back(
		    nir::BlockRecord {first, static_cast<uint32_t>(block.insts.size()), block.argCount(), block.flags, 0});
		first += static_cast<uint32_t>(block.insts.size());
		for (const auto& mi : block.insts) {
			MInst copy = mi;
			forEachValue(copy, [&](Ref& r) {
				if (nir::isValueRef(r)) {
					if (r >= idOf.size() || idOf[r] == NONE) {
						throw std::logic_error("encode: value %" + std::to_string(r) + " has no definition");
					}
					r = idOf[r];
				}
			});
			forEachBlock(copy, [&](uint32_t& uid) { uid = newBlock[uid]; });
			if (!copy.list.empty()) {
				copy.inst.c = out.addExtra(copy.list);
			}
			out.insts.push_back(copy.inst);
			if (fn.hasLocations) {
				out.debugInstLocations.push_back(copy.location);
			}
			if (fn.hasRegions) {
				out.debugInstRegions.push_back(copy.region);
			}
		}
	}
}

} // namespace nautilus::passes
