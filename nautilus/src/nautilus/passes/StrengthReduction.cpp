#include "nautilus/passes/Cfg.hpp"
#include "nautilus/passes/Passes.hpp"
#include <algorithm>
#include <set>

namespace nautilus::passes {

using nir::NONE;
using nir::Op;
using nir::Ref;
using nir::TypeId;

namespace {

/// `base + iv * stride` in a counted loop: `iv` is an `i64` argument of the loop header that the one back edge updates
/// by a constant, `stride` a constant, `base` a pointer defined outside the loop, and the multiplication has no other
/// use. The pointer becomes a loop-carried value that the back edge advances by `step * stride`.
struct Candidate {
	uint32_t ivSlot;
	uint32_t mulUid;
	uint32_t addUid;
	Ref base;
	Ref stride;
	int64_t step;
	Ref stepConst;
};

MInst makeBinary(Op op, TypeId type, Ref a, Ref b, uint32_t uid, uint32_t location) {
	MInst mi;
	mi.inst.op = static_cast<uint16_t>(op);
	mi.inst.type = static_cast<uint8_t>(type);
	mi.inst.a = a;
	mi.inst.b = b;
	mi.uid = uid;
	mi.location = location;
	return mi;
}

/// The integer value of an `i64` constant reference, if it is one.
std::optional<int64_t> i64Constant(const nir::Module& m, Ref r) {
	if (!nir::isConstRef(r)) {
		return std::nullopt;
	}
	const auto& rec = m.consts.record(r);
	if (static_cast<TypeId>(rec.type) != TypeId::I64) {
		return std::nullopt;
	}
	return static_cast<int64_t>(rec.bits);
}

/// Appends `value` to the argument list of the edge from `term` to `target` (the one arm that enters it).
void appendEdgeArg(MInst& term, uint32_t target, Ref value) {
	if (term.inst.op == static_cast<uint16_t>(Op::Br)) {
		term.list.push_back(value);
		++term.list[0];
		return;
	}
	const uint32_t tc = term.list[1];
	if (term.inst.b == target) {
		term.list.insert(term.list.begin() + 2 + tc, value);
		++term.list[1];
	} else {
		++term.list[3 + tc];
		term.list.push_back(value);
	}
}

/// Strength-reduces the loop; returns the number of pointers it made loop-carried. Invalidates the CFG.
size_t reduceLoop(MutableFunction& fn, const Cfg& cfg, const Loop& loop, nir::Module& out) {
	const nir::Module& m = out; // constants only: the tables are those of the input
	if (loop.latches.size() != 1) {
		return 0;
	}
	const uint32_t latch = loop.latches[0];
	const uint32_t headerUid = fn.blocks[loop.header].uid;
	// the one back edge: the arm of the latch's terminator that enters the header, and its argument list
	auto& latchTerm = fn.blocks[latch].terminator();
	uint32_t* argList = nullptr; // [argc, args...] of the back edge
	size_t arms = 0;
	if (latchTerm.inst.op == static_cast<uint16_t>(Op::Br)) {
		arms = latchTerm.inst.a == headerUid ? 1 : 0;
		argList = latchTerm.list.data();
	} else if (latchTerm.inst.op == static_cast<uint16_t>(Op::CondBr)) {
		const uint32_t tc = latchTerm.list[1];
		if (latchTerm.inst.b == headerUid) {
			++arms;
			argList = latchTerm.list.data() + 1; // [tc, args...]
		}
		if (latchTerm.list[2 + tc] == headerUid) {
			++arms;
			argList = latchTerm.list.data() + 3 + tc; // [fc, args...]
		}
	}
	if (arms != 1 || argList == nullptr) {
		return 0;
	}

	std::vector<uint32_t> defBlock(fn.nextUid, NONE), defIndex(fn.nextUid, NONE);
	std::vector<uint32_t> useCount(fn.nextUid, 0);
	for (uint32_t b = 0; b < fn.blocks.size(); ++b) {
		for (uint32_t i = 0; i < fn.blocks[b].insts.size(); ++i) {
			const auto& mi = fn.blocks[b].insts[i];
			defBlock[mi.uid] = b;
			defIndex[mi.uid] = i;
			forEachValue(mi, [&](const Ref& r) {
				if (nir::isValueRef(r)) {
					++useCount[r];
				}
			});
		}
	}
	auto outside = [&](Ref r) {
		return defBlock[r] != NONE && !loop.inBody[defBlock[r]];
	};

	std::vector<Candidate> candidates;
	const uint16_t argc = fn.blocks[loop.header].argCount();
	for (uint32_t j = 0; j < argc && j < argList[0]; ++j) {
		const auto& argInst = fn.blocks[loop.header].insts[j];
		if (static_cast<TypeId>(argInst.inst.type) != TypeId::I64) {
			continue;
		}
		const uint32_t iv = argInst.uid;
		// the update: add iv, step
		const Ref updated = argList[1 + j];
		if (!nir::isValueRef(updated) || defBlock[updated] == NONE || !loop.inBody[defBlock[updated]]) {
			continue;
		}
		const auto& update = fn.blocks[defBlock[updated]].insts[defIndex[updated]];
		if (update.inst.op != static_cast<uint16_t>(Op::Add) || static_cast<TypeId>(update.inst.type) != TypeId::I64) {
			continue;
		}
		Ref stepRef = NONE;
		if (update.inst.a == iv) {
			stepRef = update.inst.b;
		} else if (update.inst.b == iv) {
			stepRef = update.inst.a;
		}
		auto step = i64Constant(m, stepRef);
		if (!step) {
			continue;
		}
		for (uint32_t b : loop.body) {
			for (const auto& mul : fn.blocks[b].insts) {
				if (mul.inst.op != static_cast<uint16_t>(Op::Mul) ||
				    static_cast<TypeId>(mul.inst.type) != TypeId::I64 || useCount[mul.uid] != 1) {
					continue;
				}
				Ref strideRef = NONE;
				if (mul.inst.a == iv) {
					strideRef = mul.inst.b;
				} else if (mul.inst.b == iv) {
					strideRef = mul.inst.a;
				}
				auto stride = i64Constant(m, strideRef);
				if (!stride || *stride <= 0) {
					continue;
				}
				// its one user: add base, mul with a loop-invariant pointer base
				for (uint32_t b2 : loop.body) {
					for (const auto& add : fn.blocks[b2].insts) {
						if (add.inst.op == static_cast<uint16_t>(Op::Add) &&
						    static_cast<TypeId>(add.inst.type) == TypeId::Ptr && add.inst.b == mul.uid &&
						    nir::isValueRef(add.inst.a) && outside(add.inst.a)) {
							candidates.push_back(
							    Candidate {j, mul.uid, add.uid, add.inst.a, strideRef, *step, stepRef});
						}
					}
				}
			}
		}
	}
	if (candidates.empty()) {
		return 0;
	}

	const uint32_t latchUid = fn.blocks[latch].uid;
	const uint32_t preheaderUid = ensurePreheader(fn, cfg, loop);
	if (preheaderUid == NONE) {
		return 0;
	}
	const auto index = fn.blockIndex();
	auto& header = fn.blocks[index[headerUid]];
	auto& pre = fn.blocks[index[preheaderUid]];
	auto& latchBlock = fn.blocks[index[latchUid]];

	std::vector<Ref> subst(fn.nextUid, NONE);
	std::set<uint32_t> dead;
	for (const auto& c : candidates) {
		const auto location = fn.blocks[defBlock[c.addUid]].insts[defIndex[c.addUid]].location;
		// the loop-carried pointer: a new last argument of the header
		const uint32_t position = header.argCount();
		MInst arg;
		arg.inst.op = static_cast<uint16_t>(Op::BlockArg);
		arg.inst.type = static_cast<uint8_t>(TypeId::Ptr);
		arg.inst.a = position;
		arg.uid = fn.freshUid();
		header.insts.insert(header.insts.begin() + position, arg);
		// entering the loop: base + iv0 * stride, in the preheader
		MInst& preBr = pre.terminator();
		const Ref iv0 = preBr.list[1 + c.ivSlot];
		const uint64_t strideBits = static_cast<uint64_t>(i64Constant(m, c.stride).value());
		Ref initial = c.base;
		if (nir::isConstRef(iv0)) {
			// a constant first index: the offset is a constant (and none at all for index 0)
			const uint64_t offset = static_cast<uint64_t>(i64Constant(m, iv0).value()) * strideBits;
			if (offset != 0) {
				const auto provenance =
				    nir::leastTrusted(static_cast<nir::Provenance>(m.consts.record(iv0).provenance),
				                      static_cast<nir::Provenance>(m.consts.record(c.stride).provenance));
				auto addInit = makeBinary(Op::Add, TypeId::Ptr, c.base,
				                          out.consts.intern(TypeId::I64, provenance, offset), fn.freshUid(), location);
				initial = addInit.uid;
				pre.insts.insert(pre.insts.end() - 1, addInit);
			}
		} else {
			auto mulInit = makeBinary(Op::Mul, TypeId::I64, iv0, c.stride, fn.freshUid(), location);
			auto addInit = makeBinary(Op::Add, TypeId::Ptr, c.base, mulInit.uid, fn.freshUid(), location);
			initial = addInit.uid;
			pre.insts.insert(pre.insts.end() - 1, {mulInit, addInit});
		}
		appendEdgeArg(pre.terminator(), headerUid, initial);
		// the back edge: the pointer advances by step * stride
		const auto provenance = nir::leastTrusted(static_cast<nir::Provenance>(m.consts.record(c.stepConst).provenance),
		                                          static_cast<nir::Provenance>(m.consts.record(c.stride).provenance));
		const Ref advance = out.consts.intern(TypeId::I64, provenance, static_cast<uint64_t>(c.step) * strideBits);
		auto addNext = makeBinary(Op::Add, TypeId::Ptr, arg.uid, advance, fn.freshUid(), location);
		const uint32_t nextUid = addNext.uid;
		latchBlock.insts.insert(latchBlock.insts.end() - 1, addNext);
		appendEdgeArg(latchBlock.terminator(), headerUid, nextUid);
		subst[c.addUid] = arg.uid;
		dead.insert(c.addUid);
		dead.insert(c.mulUid);
	}
	for (auto& block : fn.blocks) {
		std::erase_if(block.insts, [&](const MInst& mi) { return mi.uid < subst.size() && dead.count(mi.uid) != 0; });
	}
	substitute(fn, subst);
	return candidates.size();
}

class StrengthReductionPass final : public Pass {
public:
	[[nodiscard]] std::string_view name() const override {
		return "strengthReduction";
	}

	std::optional<nir::Module> run(const nir::Module& in, PassContext& context) override {
		std::vector<uint32_t> candidates;
		for (uint32_t f = 0; f < in.functions.size(); ++f) {
			if (hasLoopWithMul(in, f)) {
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
			if (std::find(candidates.begin(), candidates.end(), f) != candidates.end()) {
				total += reduceLoops(fn, out);
			}
			encode(fn, out);
		}
		if (total == 0) {
			return std::nullopt;
		}
		context.changes = total;
		return out;
	}

private:
	/// A cheap test before the function is decoded: it has a `mul` and an edge to an earlier block.
	static bool hasLoopWithMul(const nir::Module& m, uint32_t f) {
		bool mul = false;
		for (const auto& inst : m.functionInsts(f)) {
			mul = mul || inst.op == static_cast<uint16_t>(Op::Mul);
		}
		if (!mul) {
			return false;
		}
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

	static size_t reduceLoops(MutableFunction& fn, nir::Module& out) {
		size_t total = 0;
		std::set<uint32_t> done;
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
			total += reduceLoop(fn, cfg, *next, out);
		}
	}
};

} // namespace

std::unique_ptr<Pass> makeStrengthReductionPass() {
	return std::make_unique<StrengthReductionPass>();
}

} // namespace nautilus::passes
