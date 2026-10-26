#include "nautilus/nir/Rewriter.hpp"
#include <cassert>
#include <stdexcept>

namespace nautilus::nir {

Rewriter::Rewriter(const Module& src, Module& dst) : src_(src), dst_(dst) {
	assert(dst.insts.empty() && dst.functions.empty() && "the target of a rewrite must be empty");
	// names, constants, bindings, pointers, imports, data and attributes keep their numbering
	for (uint32_t i = 0; i < src.names.size(); ++i) {
		dst.internName(src.name(i));
	}
	dst.consts = src.consts;
	for (auto& s : src.signatures) {
		std::vector<TypeWord> params(src.extra.begin() + s.paramTypes, src.extra.begin() + s.paramTypes + s.paramCount);
		dst.internSignature(s.result, params);
	}
	for (auto& c : src.callees) {
		dst.internCallee(c);
	}
	for (auto& p : src.pointers) {
		dst.internPointer(p);
	}
	dst.bindings = src.bindings;
	dst.imports = src.imports;
	dst.data = src.data;
	dst.dataBytes = src.dataBytes;
	dst.functionAttrs = src.functionAttrs;
	dst.extensions = src.extensions;
	dst.module = src.module;
	dst.debugLocations = src.debugLocations;
	dst.debugRegions = src.debugRegions;
	dst.debugNames = src.debugNames;
	dst.debugNameBytes = src.debugNameBytes;
	dst.debugFunctions = src.debugFunctions;
	dst.debugParamNames = src.debugParamNames;
}

Ref Rewriter::map(Ref old) const {
	if (!isValueRef(old)) {
		return old;
	}
	assert(old < forward_.size());
	if (forward_[old] == NONE) {
		throw std::logic_error("rewrite: operand refers to an erased instruction");
	}
	return forward_[old];
}

Ref Rewriter::append(const Inst& inst) {
	auto& fn = dst_.functions.back();
	auto id = static_cast<uint32_t>(dst_.insts.size()) - fn.firstInst;
	dst_.insts.push_back(inst);
	if (level_ == Level::SSA) {
		dst_.blocks.back().count++;
	}
	return id;
}

void Rewriter::rewriteList(Inst& inst, const Inst& old) {
	auto* info = coreOpInfo(old.op);
	bool hasList = info && (info->c == OperandKind::Extra);
	bool cleanupInC = (old.op == static_cast<uint16_t>(Op::Div) || old.op == static_cast<uint16_t>(Op::Rem)) &&
	                  (old.flags & 2) && level_ == Level::PreSSA && old.c != NO_UNWIND;
	auto copyCleanup = [&](uint32_t offset) {
		uint32_t n = src_.extra[offset];
		return dst_.addExtra(std::span<const uint32_t>(src_.extra).subspan(offset, 1 + 2 * n));
	};
	if (hasList) {
		uint32_t len = 0;
		const uint32_t* l = src_.extra.data() + old.c;
		uint32_t unwindAt = NONE;
		switch (static_cast<Op>(old.op)) {
		case Op::Call:
			len = 1 + l[0] + 2;
			unwindAt = 1 + l[0];
			break;
		case Op::CallIndirect:
			len = 1 + l[0] + 3;
			unwindAt = 1 + l[0];
			break;
		case Op::Br:
			len = 1 + l[0];
			break;
		case Op::CondBr:
			len = 4 + l[1] + l[3 + l[1]];
			break;
		default:
			break;
		}
		inst.c = dst_.addExtra(std::span<const uint32_t>(src_.extra).subspan(old.c, len));
		if (unwindAt != NONE && level_ == Level::PreSSA && dst_.extra[inst.c + unwindAt] != NO_UNWIND) {
			dst_.extra[inst.c + unwindAt] = copyCleanup(dst_.extra[inst.c + unwindAt]);
		}
	} else if (cleanupInC) {
		inst.c = copyCleanup(old.c);
	}
	forEachRef(inst, dst_.extra.data(), level_, [&](Ref& r) { r = map(r); });
}

Ref Rewriter::copy(uint32_t oldId, const Inst& old) {
	Inst inst = old;
	rewriteList(inst, old);
	return emit(oldId, inst);
}

Ref Rewriter::emit(uint32_t oldId, const Inst& inst) {
	auto id = append(inst);
	if (level_ == Level::PreSSA) {
		blockLists_[blockOfInst_[oldId]].push_back(id);
	}
	forward_[oldId] = id;
	return id;
}

void Rewriter::replace(uint32_t oldId, Ref newRef) {
	forward_[oldId] = newRef;
}

void Rewriter::erase(uint32_t oldId) {
	forward_[oldId] = NONE;
}

void Rewriter::rewriteFunction(uint32_t f, const Visitor& visitor) {
	auto& sfn = src_.functions[f];
	srcFn_ = f;
	level_ = static_cast<Level>(sfn.level);
	forward_.assign(sfn.instCount, NONE);

	FunctionRecord fr = sfn;
	fr.firstInst = static_cast<uint32_t>(dst_.insts.size());
	fr.firstBlock = static_cast<uint32_t>(dst_.blocks.size());
	fr.firstAlloca = static_cast<uint32_t>(dst_.allocas.size());
	fr.instCount = 0;
	dst_.functions.push_back(fr);
	for (auto& a : src_.functionAllocas(f)) {
		dst_.allocas.push_back(a);
	}

	auto insts = src_.functionInsts(f);
	auto blocks = src_.functionBlocks(f);
	auto run = [&](uint32_t i) {
		if (!visitor || !visitor(*this, i, insts[i])) {
			copy(i, insts[i]);
		}
	};
	if (level_ == Level::SSA) {
		for (auto& b : blocks) {
			BlockRecord nb = b;
			nb.first = static_cast<uint32_t>(dst_.insts.size()) - fr.firstInst;
			nb.count = 0;
			dst_.blocks.push_back(nb);
			for (uint32_t i = b.first; i < b.first + b.count; ++i) {
				run(i);
			}
		}
	} else {
		blockOfInst_.assign(sfn.instCount, 0);
		blockLists_.assign(blocks.size(), {});
		for (uint32_t bi = 0; bi < blocks.size(); ++bi) {
			for (uint32_t k = 0; k < blocks[bi].count; ++k) {
				blockOfInst_[src_.extra[blocks[bi].first + k]] = bi;
			}
		}
		for (uint32_t i = 0; i < insts.size(); ++i) {
			run(i);
		}
		for (uint32_t bi = 0; bi < blocks.size(); ++bi) {
			BlockRecord nb = blocks[bi];
			nb.first = dst_.addExtra(blockLists_[bi]);
			nb.count = static_cast<uint32_t>(blockLists_[bi].size());
			dst_.blocks.push_back(nb);
		}
	}
	auto& d = dst_.functions.back();
	d.instCount = static_cast<uint32_t>(dst_.insts.size()) - d.firstInst;
	d.blockCount = static_cast<uint32_t>(blocks.size());
}

void Rewriter::rewriteAll(const Visitor& visitor) {
	for (uint32_t f = 0; f < src_.functions.size(); ++f) {
		rewriteFunction(f, visitor);
	}
}

} // namespace nautilus::nir
