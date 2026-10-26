#include "nautilus/nir/Canonicalize.hpp"
#include <algorithm>
#include <array>
#include <map>
#include <stdexcept>
#include <string>

namespace nautilus::nir {

namespace {

class Canon {
public:
	Canon(const Module& s, const CanonOptions& o) : s_(s), o_(o) {
		nameMap_.assign(s.names.size(), NONE);
		constMap_.assign(s.consts.size(), NONE);
		sigMap_.assign(s.signatures.size(), NONE);
		calleeMap_.assign(s.callees.size(), NONE);
		ptrMap_.assign(s.pointers.size(), NONE);
		importMap_.assign(s.imports.size(), NONE);
		bindingMap_.assign(s.bindings.size(), NONE);
		dataMap_.assign(s.data.size(), NONE);
		extMap_.assign(s.extensions.size(), NONE);
	}

	Module run() {
		for (auto& f : s_.functions) {
			if (f.level != static_cast<uint8_t>(Level::SSA)) {
				throw std::invalid_argument("canonicalization applies to SSA-level modules only");
			}
		}
		orderFunctions();
		for (uint32_t nf = 0; nf < order_.size(); ++nf) {
			scanFunction(nf, order_[nf]);
		}
		writeLists();
		d_.module.nameId = (o_.anonymous || s_.module.nameId == NONE) ? NONE : name(s_.module.nameId);
		d_.module.flags = 0;
		d_.module.processToken = s_.module.processToken;
		d_.rebuildIndexes();
		return std::move(d_);
	}

private:
	// ---- functions -------------------------------------------------------------------------------------------

	/// §7 step 2: exported functions by name, then the internal functions they reach, in first-use order.
	void orderFunctions() {
		for (uint32_t f = 0; f < s_.functions.size(); ++f) {
			if (s_.functions[f].visibility == static_cast<uint8_t>(Visibility::Exported)) {
				order_.push_back(f);
			}
		}
		std::sort(order_.begin(), order_.end(), [&](uint32_t a, uint32_t b) {
			return s_.name(s_.functions[a].nameId) < s_.name(s_.functions[b].nameId);
		});
		fnMap_.assign(s_.functions.size(), NONE);
		for (uint32_t i = 0; i < order_.size(); ++i) {
			fnMap_[order_[i]] = i;
		}
		for (size_t i = 0; i < order_.size(); ++i) {
			for (auto& inst : s_.functionInsts(order_[i])) {
				if (inst.op != static_cast<uint16_t>(Op::Call) && inst.op != static_cast<uint16_t>(Op::FuncAddr)) {
					continue;
				}
				auto& c = s_.callees[inst.a];
				if (c.kind == static_cast<uint8_t>(CalleeKind::Internal) && fnMap_[c.target] == NONE) {
					fnMap_[c.target] = static_cast<uint32_t>(order_.size());
					order_.push_back(c.target);
				}
			}
		}
	}

	// ---- table visits (§7.1): an entry is placed at its first visit, after its own fields -------------------

	uint32_t name(uint32_t old) {
		if (nameMap_[old] == NONE) {
			nameMap_[old] = d_.internName(s_.name(old));
		}
		return nameMap_[old];
	}

	Ref constant(Ref ref) {
		auto old = constIndexOf(ref);
		if (constMap_[old] == NONE) {
			auto& c = s_.consts.at(old);
			auto t = static_cast<TypeId>(c.type);
			auto prov = static_cast<Provenance>(c.provenance);
			Ref r;
			if (isExtension(t) && c.bits == 0 && !s_.consts.bytes().empty()) {
				// a wide constant of an extension type: its size is not known without the registry, so the bytes
				// up to the next wide constant (or the end) are taken
				r = d_.consts.internBytes(t, prov, wideBytes(old), alignOf(t));
			} else {
				r = d_.consts.intern(t, prov, c.bits);
			}
			constMap_[old] = constIndexOf(r);
		}
		return makeConstRef(constMap_[old]);
	}

	std::span<const std::byte> wideBytes(uint32_t constIndex) const {
		// the extent of a wide constant is the distance to the next wide constant's offset in constBytes
		auto& c = s_.consts.at(constIndex);
		uint32_t end = static_cast<uint32_t>(s_.consts.bytes().size());
		for (auto& other : s_.consts.records()) {
			if (other.byteOffset > c.byteOffset && other.byteOffset < end) {
				end = other.byteOffset;
			}
		}
		return s_.consts.bytes().subspan(c.byteOffset, end - c.byteOffset);
	}

	uint32_t alignOf(TypeId t) const {
		return o_.constAlign ? std::min<uint32_t>(o_.constAlign(t), 16) : 1;
	}

	uint32_t signature(uint32_t old) {
		if (sigMap_[old] == NONE) {
			auto& sg = s_.signatures[old];
			std::vector<TypeWord> params;
			for (uint32_t i = 0; i < sg.paramCount; ++i) {
				params.push_back(s_.paramType(sg, i));
			}
			sigMap_[old] = d_.internSignature(sg.result, params);
		}
		return sigMap_[old];
	}

	uint32_t pointer(uint32_t old) {
		if (ptrMap_[old] == NONE) {
			auto r = s_.pointers[old];
			if (r.mode != 0) {
				r.nameId = name(r.nameId);
			}
			ptrMap_[old] = d_.internPointer(r);
		}
		return ptrMap_[old];
	}

	uint32_t import(uint32_t old) {
		if (importMap_[old] == NONE) {
			auto r = s_.imports[old];
			r.moduleNameId = name(r.moduleNameId);
			r.functionNameId = name(r.functionNameId);
			r.signature = signature(r.signature);
			std::array<uint32_t, 4> key {r.moduleNameId, r.functionNameId, r.signature, r.attrs};
			auto it = importIndex_.find(key);
			if (it == importIndex_.end()) {
				it = importIndex_.emplace(key, d_.addImport(r)).first;
			}
			importMap_[old] = it->second;
		}
		return importMap_[old];
	}

	uint32_t extension(uint32_t old) {
		if (extMap_[old] == NONE) {
			auto r = s_.extensions[old];
			r.nameId = name(r.nameId);
			std::array<uint32_t, 2> key {r.nameId, r.kindAndVersion};
			auto it = extIndex_.find(key);
			if (it == extIndex_.end()) {
				d_.extensions.push_back(r);
				it = extIndex_.emplace(key, static_cast<uint32_t>(d_.extensions.size() - 1)).first;
			}
			extMap_[old] = it->second;
		}
		return extMap_[old];
	}

	uint32_t callee(uint32_t old) {
		if (calleeMap_[old] == NONE) {
			auto r = s_.callees[old];
			switch (static_cast<CalleeKind>(r.kind)) {
			case CalleeKind::Internal:
				r.target = fnMap_[r.target];
				break;
			case CalleeKind::External:
				r.target = pointer(r.target);
				break;
			case CalleeKind::Imported:
				r.target = import(r.target);
				break;
			case CalleeKind::Intrinsic:
				r.target = extension(r.target);
				break;
			}
			r.signature = signature(r.signature);
			calleeMap_[old] = d_.internCallee(r);
		}
		return calleeMap_[old];
	}

	uint32_t binding(uint32_t old) {
		if (bindingMap_[old] == NONE) {
			auto& b = s_.bindings[old];
			bindingMap_[old] = d_.addBinding(name(b.nameId), static_cast<TypeId>(b.type), b.size, b.align);
		}
		return bindingMap_[old];
	}

	uint32_t data(uint32_t old) {
		if (dataMap_[old] == NONE) {
			auto& r = s_.data[old];
			dataMap_[old] = d_.internData(std::span<const std::byte>(s_.dataBytes).subspan(r.byteOffset, r.size), r.align);
		}
		return dataMap_[old];
	}

	// ---- functions -------------------------------------------------------------------------------------------

	Ref value(Ref r) {
		return isConstRef(r) ? constant(r) : r;
	}

	void scanFunction(uint32_t nf, uint32_t of) {
		auto& old = s_.functions[of];
		FunctionRecord nr {};
		nr.nameId = name(old.nameId);
		nr.signature = signature(old.signature);
		nr.attrs = old.attrs;
		nr.level = old.level;
		nr.visibility = old.visibility;
		nr.firstInst = static_cast<uint32_t>(d_.insts.size());
		nr.firstBlock = static_cast<uint32_t>(d_.blocks.size());
		nr.firstAlloca = static_cast<uint32_t>(d_.allocas.size());

		// function string attributes, sorted by key bytes
		std::vector<const FunctionAttrRecord*> attrs;
		for (auto& r : s_.functionAttrs) {
			if (r.function == of) {
				attrs.push_back(&r);
			}
		}
		std::sort(attrs.begin(), attrs.end(),
		          [&](auto* a, auto* b) { return s_.name(a->keyNameId) < s_.name(b->keyNameId); });
		for (auto* r : attrs) {
			auto key = name(r->keyNameId);
			auto val = name(r->valueNameId);
			d_.functionAttrs.push_back(FunctionAttrRecord {nf, key, val, 0});
		}

		std::vector<uint32_t> allocaMap(old.allocaCount, NONE);
		for (auto& oi : s_.functionInsts(of)) {
			Inst ni {};
			ni.op = oi.op;
			ni.type = oi.type;
			ni.flags = oi.flags;
			auto* info = coreOpInfo(oi.op);
			if (info == nullptr) {
				throw std::invalid_argument("extension operations are canonicalized with the registry");
			}
			auto field = [&](OperandKind k, uint32_t v) -> uint32_t {
				switch (k) {
				case OperandKind::Value:
				case OperandKind::ValueOpt:
					return v == NONE ? v : value(v);
				case OperandKind::Callee:
					return callee(v);
				case OperandKind::Signature:
					return signature(v);
				case OperandKind::Alloca:
					if (allocaMap[v] == NONE) {
						allocaMap[v] = static_cast<uint32_t>(d_.allocas.size()) - nr.firstAlloca;
						d_.allocas.push_back(s_.allocas[old.firstAlloca + v]);
					}
					return allocaMap[v];
				case OperandKind::Binding:
					return binding(v);
				case OperandKind::Data:
					return data(v);
				case OperandKind::PointerOpt:
					return v == NONE ? v : pointer(v);
				default:
					return v; // blocks, immediates, unwind entries, unused fields
				}
			};
			ni.a = field(info->a, oi.a);
			ni.b = field(info->b, oi.b);
			ni.c = oi.c; // div/rem unwind entries are copied; `extra` offsets are rewritten by writeLists
			if (info->c == OperandKind::Extra) {
				lists_.push_back(list(oi));
			} else {
				lists_.emplace_back();
			}
			d_.insts.push_back(ni);
		}
		for (auto& b : s_.functionBlocks(of)) {
			BlockRecord nb {};
			nb.first = b.first;
			nb.count = b.count;
			nb.argCount = b.argCount;
			nb.flags = b.flags;
			d_.blocks.push_back(nb);
		}
		nr.instCount = old.instCount;
		nr.blockCount = old.blockCount;
		nr.allocaCount = static_cast<uint32_t>(d_.allocas.size()) - nr.firstAlloca;
		d_.functions.push_back(nr);
	}

	/// The extra list of an instruction with its references renumbered (§7 step 7).
	std::vector<uint32_t> list(const Inst& oi) {
		const uint32_t* l = s_.extra.data() + oi.c;
		std::vector<uint32_t> out;
		auto args = [&](const uint32_t* a, uint32_t n) {
			for (uint32_t i = 0; i < n; ++i) {
				out.push_back(value(a[i]));
			}
		};
		switch (static_cast<Op>(oi.op)) {
		case Op::Call:
		case Op::CallIndirect: {
			uint32_t argc = l[0];
			out.push_back(argc);
			args(l + 1, argc);
			out.push_back(l[1 + argc]);
			out.push_back(l[2 + argc] == NONE ? NONE : pointer(l[2 + argc]));
			if (static_cast<Op>(oi.op) == Op::CallIndirect) {
				out.push_back(l[3 + argc]);
			}
			break;
		}
		case Op::Br:
			out.push_back(l[0]);
			args(l + 1, l[0]);
			break;
		case Op::CondBr: {
			uint32_t tc = l[1];
			uint32_t fc = l[3 + tc];
			out.push_back(l[0]);
			out.push_back(tc);
			args(l + 2, tc);
			out.push_back(l[2 + tc]);
			out.push_back(fc);
			args(l + 4 + tc, fc);
			break;
		}
		default:
			break;
		}
		return out;
	}

	/// Writes the lists after the signatures' parameter lists, per function and per instruction in id order.
	void writeLists() {
		size_t k = 0;
		for (auto& f : d_.functions) {
			for (uint32_t i = 0; i < f.instCount; ++i, ++k) {
				auto& inst = d_.insts[f.firstInst + i];
				auto* info = coreOpInfo(inst.op);
				if (info->c == OperandKind::Extra) {
					inst.c = d_.addExtra(lists_[k]);
				}
			}
		}
	}

	const Module& s_;
	const CanonOptions& o_;
	Module d_;
	std::vector<uint32_t> order_, fnMap_;
	std::vector<uint32_t> nameMap_, constMap_, sigMap_, calleeMap_, ptrMap_, importMap_, bindingMap_, dataMap_, extMap_;
	std::map<std::array<uint32_t, 4>, uint32_t> importIndex_;
	std::map<std::array<uint32_t, 2>, uint32_t> extIndex_;
	std::vector<std::vector<uint32_t>> lists_; // per instruction in canonical order
};

} // namespace

Module canonicalize(const Module& m, const CanonOptions& options) {
	return Canon(m, options).run();
}

bool sectionsEqual(const Module& a, const Module& b) {
	auto bytes = [](const ConstPool& p) { return std::vector<std::byte>(p.bytes().begin(), p.bytes().end()); };
	return a.insts == b.insts && a.extra == b.extra &&
	       std::vector<ConstRecord>(a.consts.records().begin(), a.consts.records().end()) ==
	           std::vector<ConstRecord>(b.consts.records().begin(), b.consts.records().end()) &&
	       bytes(a.consts) == bytes(b.consts) && a.blocks == b.blocks && a.functions == b.functions &&
	       a.signatures == b.signatures && a.callees == b.callees && a.allocas == b.allocas &&
	       a.pointers == b.pointers && a.bindings == b.bindings && a.names == b.names && a.nameBytes == b.nameBytes &&
	       a.extensions == b.extensions && a.imports == b.imports && a.data == b.data && a.dataBytes == b.dataBytes &&
	       a.functionAttrs == b.functionAttrs && a.module == b.module;
}

} // namespace nautilus::nir
