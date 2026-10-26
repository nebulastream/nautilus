#include "nautilus/nir/Printer.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/nir/Operands.hpp"
#include <bit>
#include <charconv>
#include <cmath>
#include <cstdio>
#include <ostream>
#include <sstream>

namespace nautilus::nir {

std::string typeName(TypeId t) {
	switch (t) {
	case TypeId::Void:
		return "void";
	case TypeId::Bool:
		return "bool";
	case TypeId::I8:
		return "i8";
	case TypeId::I16:
		return "i16";
	case TypeId::I32:
		return "i32";
	case TypeId::I64:
		return "i64";
	case TypeId::F32:
		return "f32";
	case TypeId::F64:
		return "f64";
	case TypeId::Ptr:
		return "ptr";
	default:
		// an extension type prints as `!` and its registered name (13 §9.1); one the registry does not know keeps a
		// placeholder
		if (auto* info = ext::ExtensionRegistry::instance().type(static_cast<uint8_t>(t))) {
			return "!" + info->name;
		}
		return "!ext.t" + std::to_string(static_cast<unsigned>(t));
	}
}

namespace {

std::string hex(uint64_t v) {
	char buf[24];
	std::snprintf(buf, sizeof buf, "0x%llx", static_cast<unsigned long long>(v));
	return buf;
}

/// `"…"` with the escapes of §9.3.
std::string quote(std::string_view s) {
	std::string out = "\"";
	for (unsigned char c : s) {
		if (c == '"' || c == '\\') {
			out += '\\';
			out += static_cast<char>(c);
		} else if (c < 0x20 || c == 0x7F) {
			char buf[8];
			std::snprintf(buf, sizeof buf, "\\x%02x", c);
			out += buf;
		} else {
			out += static_cast<char>(c);
		}
	}
	return out + "\"";
}

bool isBareName(std::string_view s) {
	if (s.empty()) {
		return false;
	}
	auto start = [](char c) {
		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' || c == '$';
	};
	if (!start(s[0])) {
		return false;
	}
	for (char c : s) {
		if (!start(c) && !(c >= '0' && c <= '9') && c != '.') {
			return false;
		}
	}
	return true;
}

std::string fname(std::string_view s) {
	return "@" + (isBareName(s) ? std::string(s) : quote(s));
}

const char* const attrNames[] = {"noArgRead", "noArgWrite", "noOtherRead", "noOtherWrite",
                                 "noUnwind",  "willReturn", "declared"};

std::string attrsText(uint32_t attrs) {
	std::string out;
	for (uint32_t bit = 0; bit < 7; ++bit) {
		if (attrs & (1u << bit)) {
			out += out.empty() ? "" : ", ";
			out += attrNames[bit];
		}
	}
	return out.empty() ? "" : " attrs(" + out + ")";
}

class Printer {
public:
	Printer(const Module& m, std::ostream& os) : m_(m), os_(os) {
	}

	void run() {
		os_ << "nir 1.0\n";
		if (m_.module.nameId != NONE) {
			os_ << "module " << quote(m_.name(m_.module.nameId)) << "\n";
		}
		for (auto& e : m_.extensions) {
			os_ << "extension " << quote(m_.name(e.nameId)) << "\n";
		}
		for (uint32_t i = 0; i < m_.imports.size(); ++i) {
			auto& r = m_.imports[i];
			os_ << "import #i" << i << " = " << quote(m_.name(r.moduleNameId))
			    << " :: " << quote(m_.name(r.functionNameId)) << " : " << signature(r.signature) << attrsText(r.attrs)
			    << "\n";
		}
		for (uint32_t i = 0; i < m_.pointers.size(); ++i) {
			pointer(i);
		}
		for (uint32_t i = 0; i < m_.bindings.size(); ++i) {
			auto& b = m_.bindings[i];
			os_ << "binding #b" << i << " = " << quote(m_.name(b.nameId)) << " : "
			    << typeName(static_cast<TypeId>(b.type)) << " size " << b.size << " align " << b.align << "\n";
		}
		for (uint32_t i = 0; i < m_.data.size(); ++i) {
			auto& d = m_.data[i];
			os_ << "data #d" << i << " align " << d.align << " = x\"";
			for (uint32_t k = 0; k < d.size; ++k) {
				char buf[4];
				std::snprintf(buf, sizeof buf, "%02x", static_cast<unsigned>(m_.dataBytes[d.byteOffset + k]));
				os_ << buf;
			}
			os_ << "\"\n";
		}
		for (uint32_t i = 0; i < m_.callees.size(); ++i) {
			callee(i);
		}
		for (uint32_t f = 0; f < m_.functions.size(); ++f) {
			os_ << "\n";
			function(f);
		}
	}

private:
	// ---- tables ----------------------------------------------------------------------------------------------

	static const char* pointerKind(uint8_t k) {
		static const char* const names[] = {"callee", "thunk", "destructor", "address", "payload"};
		return names[k];
	}

	void pointer(uint32_t i) {
		auto& p = m_.pointers[i];
		os_ << "pointer #p" << i << " = " << pointerKind(p.kind) << " ";
		switch (p.mode) {
		case 0:
			os_ << "local " << hex(p.address);
			break;
		case 1:
			os_ << "symbol " << quote(m_.name(p.nameId));
			break;
		default:
			os_ << "image " << quote(m_.name(p.nameId)) << " + " << hex(p.address);
			break;
		}
		if (p.provenance == 1) {
			os_ << " audited";
		} else if (p.provenance == 2) {
			os_ << " raw";
		}
		os_ << "\n";
	}

	void callee(uint32_t i) {
		auto& c = m_.callees[i];
		os_ << "callee #c" << i << " = ";
		switch (static_cast<CalleeKind>(c.kind)) {
		case CalleeKind::Internal:
			os_ << "internal " << fname(m_.name(m_.functions[c.target].nameId));
			break;
		case CalleeKind::External:
			os_ << "external #p" << c.target;
			break;
		case CalleeKind::Imported:
			os_ << "imported #i" << c.target;
			break;
		case CalleeKind::Intrinsic: {
			// the name without its "@version", which the `extension` line carries
			std::string_view name = m_.name(m_.extensions[c.target].nameId), bare;
			uint32_t version;
			os_ << "intrinsic " << (ext::splitNameVersion(name, bare, version) ? bare : name);
		} break;
		}
		os_ << " : " << signature(c.signature) << attrsText(c.attrs) << "\n";
	}

	std::string ptype(TypeWord w) const {
		auto s = typeName(typeOf(w));
		switch (extKindOf(w)) {
		case ExtKind::Zext:
			return s + " zext";
		case ExtKind::Sext:
			return s + " sext";
		default:
			return s;
		}
	}

	std::string signature(uint32_t index) const {
		auto& s = m_.signatures[index];
		std::string out = "(";
		for (uint32_t i = 0; i < s.paramCount; ++i) {
			out += (i ? ", " : "") + ptype(m_.paramType(s, i));
		}
		return out + ") -> " + ptype(s.result);
	}

	// ---- constants and operands ------------------------------------------------------------------------------

	std::string constant(uint32_t index) const {
		auto& c = m_.consts.at(index);
		auto t = static_cast<TypeId>(c.type);
		std::string lit;
		switch (t) {
		case TypeId::Bool:
			lit = c.bits ? "true" : "false";
			break;
		case TypeId::I8:
			lit = std::to_string(static_cast<int8_t>(c.bits));
			break;
		case TypeId::I16:
			lit = std::to_string(static_cast<int16_t>(c.bits));
			break;
		case TypeId::I32:
			lit = std::to_string(static_cast<int32_t>(c.bits));
			break;
		case TypeId::I64:
			lit = std::to_string(static_cast<int64_t>(c.bits));
			break;
		case TypeId::F32:
			lit = floatText(std::bit_cast<float>(static_cast<uint32_t>(c.bits)), static_cast<uint32_t>(c.bits), 23, 8);
			break;
		case TypeId::F64:
			lit = floatText(std::bit_cast<double>(c.bits), c.bits, 52, 11);
			break;
		default: {
			lit = "x\"";
			// an extension-type constant has the registered size; a type the registry does not know shows 16 bytes
			auto* typeInfo = ext::ExtensionRegistry::instance().type(static_cast<uint8_t>(t));
			const uint32_t size = typeInfo ? typeInfo->size : 16;
			for (uint32_t i = 0; i < size && c.byteOffset + i < m_.consts.bytes().size(); ++i) {
				char buf[4];
				std::snprintf(buf, sizeof buf, "%02x", static_cast<unsigned>(m_.consts.bytes()[c.byteOffset + i]));
				lit += buf;
			}
			lit += "\"";
		}
		}
		std::string out = lit + ":" + typeName(t);
		if (c.provenance == 1) {
			out += "[audited]";
		} else if (c.provenance == 2) {
			out += "[raw]";
		}
		return out;
	}

	template <class F>
	static std::string floatText(F value, uint64_t bits, int mantissaBits, int exponentBits) {
		bool negative = (bits >> (mantissaBits + exponentBits)) & 1;
		if (std::isnan(value)) {
			uint64_t trailing = bits & ((1ull << mantissaBits) - 1);
			return std::string(negative ? "-" : "") + "nan(" + hex(trailing) + ")";
		}
		if (std::isinf(value)) {
			return negative ? "-inf" : "inf";
		}
		char buf[64];
		auto r = std::to_chars(buf, buf + sizeof buf, value);
		return std::string(buf, r.ptr);
	}

	std::string operand(Ref r) const {
		return isConstRef(r) ? constant(constIndexOf(r)) : "%" + std::to_string(r);
	}

	std::string args(const uint32_t* list, uint32_t count) const {
		std::string out = "(";
		for (uint32_t i = 0; i < count; ++i) {
			out += (i ? ", " : "") + operand(list[i]);
		}
		return out + ")";
	}

	// ---- functions -------------------------------------------------------------------------------------------

	void function(uint32_t f) {
		auto& fn = m_.functions[f];
		auto insts = m_.functionInsts(f);
		auto blocks = m_.functionBlocks(f);
		level_ = static_cast<Level>(fn.level);
		curF_ = f;
		os_ << "func";
		if (fn.visibility == 1) {
			os_ << " export";
		}
		if (level_ == Level::PreSSA) {
			os_ << " pressa";
		}
		os_ << " " << fname(m_.name(fn.nameId)) << " : " << signature(fn.signature) << attrsText(fn.attrs) << " {\n";
		for (auto& a : m_.functionAllocas(f)) {
			os_ << "  alloca " << a.size << " align " << a.align << "\n";
		}
		for (auto& r : m_.functionAttrs) {
			if (r.function == f) {
				os_ << "  attr " << quote(m_.name(r.keyNameId)) << " = " << quote(m_.name(r.valueNameId)) << "\n";
			}
		}
		for (uint32_t b = 0; b < blocks.size(); ++b) {
			block(f, b, insts, blocks[b]);
		}
		os_ << "}\n";
	}

	void block(uint32_t f, uint32_t b, std::span<const Inst> insts, const BlockRecord& blk) {
		// block arguments: the leading blockargs (SSA) or the parameters in block 0 (pre-SSA)
		std::vector<uint32_t> ids;
		for (uint32_t k = 0; k < blk.count; ++k) {
			ids.push_back(level_ == Level::SSA ? blk.first + k : m_.extra[blk.first + k]);
		}
		size_t skipped = 0;
		os_ << "^b" << b;
		std::string argText;
		if (level_ == Level::SSA) {
			skipped = blk.argCount;
		} else if (b == 0) {
			while (skipped < ids.size() && insts[ids[skipped]].op == static_cast<uint16_t>(Op::BlockArg)) {
				skipped++;
			}
		}
		if (skipped != 0) {
			os_ << "(";
			for (size_t k = 0; k < skipped; ++k) {
				os_ << (k ? ", " : "") << "%" << ids[k] << ": " << typeName(static_cast<TypeId>(insts[ids[k]].type));
			}
			os_ << ")";
		}
		if (blk.flags & BLOCK_LANDING_PAD) {
			os_ << " pad";
		}
		os_ << ":\n";
		for (size_t k = skipped; k < ids.size(); ++k) {
			os_ << "  " << instruction(ids[k], insts[ids[k]]) << "\n";
		}
		(void) f;
	}

	std::string unwind(uint32_t u) const {
		if (u == NO_UNWIND) {
			return "";
		}
		if (u == NO_CLEANUP) {
			return " unwind caller";
		}
		if (level_ == Level::SSA) {
			return " unwind ^b" + std::to_string(u);
		}
		std::string out = " cleanup [";
		uint32_t n = m_.extra[u];
		for (uint32_t k = 0; k < n; ++k) {
			out += (k ? ", " : "");
			out += "(#c" + std::to_string(m_.extra[u + 1 + 2 * k]) + ", " + operand(m_.extra[u + 2 + 2 * k]) + ")";
		}
		return out + "]";
	}

	TypeId typeOfRef(Ref r) const {
		return isConstRef(r) ? static_cast<TypeId>(m_.consts.at(constIndexOf(r)).type) : TypeId::Void;
	}

	/// Whether the operand is known to be a float (constants only; values are looked up by the caller).
	bool isFloatOperand(std::span<const Inst> insts, Ref r) const {
		if (isConstRef(r)) {
			return isFloat(typeOfRef(r));
		}
		return r < insts.size() && isFloat(static_cast<TypeId>(insts[r].type));
	}

	std::string instruction(uint32_t id, const Inst& inst) const {
		auto& fn = *curFunction();
		auto insts = std::span<const Inst>(m_.insts).subspan(fn.firstInst, fn.instCount);
		std::string lhs;
		if (inst.type != static_cast<uint8_t>(TypeId::Void)) {
			lhs = "%" + std::to_string(id) + " = ";
		} else if (level_ == Level::PreSSA) {
			lhs = "[" + std::to_string(id) + "] ";
		}
		if (inst.op >= FIRST_EXTENSION_OP) {
			return lhs + extensionInstruction(inst);
		}
		auto op = static_cast<Op>(inst.op);
		auto* info = coreOpInfo(inst.op);
		std::string name = info ? info->name : "?op" + std::to_string(inst.op);
		auto A = [&] {
			return operand(inst.a);
		};
		auto B = [&] {
			return operand(inst.b);
		};
		std::string text;
		switch (op) {
		case Op::Add:
		case Op::Sub:
		case Op::Mul:
		case Op::And:
		case Op::Or:
		case Op::BAnd:
		case Op::BOr:
		case Op::BXor:
		case Op::Shl:
			text = name + " " + A() + ", " + B();
			break;
		case Op::FNeg:
		case Op::Not:
		case Op::BNot:
			text = name + " " + A();
			break;
		case Op::Div:
		case Op::Rem: {
			bool fl = isFloatOperand(insts, inst.a);
			std::string mn = name;
			if (!fl) {
				mn += (inst.flags & 1) ? ".u" : ".s";
				if (inst.flags & 2) {
					mn += ".checked";
				}
			}
			text = mn + " " + A() + ", " + B() + ((inst.flags & 2) ? unwind(inst.c) : "");
			break;
		}
		case Op::Shr:
			text = std::string(inst.flags & 1 ? "shr.u " : "shr.s ") + A() + ", " + B();
			break;
		case Op::Cmp: {
			static const char* const preds[] = {"eq", "ne", "lt", "le", "gt", "ge", "?6", "?7"};
			std::string mn = std::string("cmp.") + preds[inst.flags & 7];
			bool ordered = (inst.flags & 7) >= 2 && (inst.flags & 7) <= 5;
			if (ordered && isInteger(typeOfOperand(insts, inst.a))) {
				mn += (inst.flags & 8) ? ".u" : ".s";
			}
			text = mn + " " + A() + ", " + B();
			break;
		}
		case Op::Select:
			text = name + " " + A() + ", " + B() + ", " + operand(inst.c);
			break;
		case Op::Cast: {
			static const char* const kinds[] = {"sext",   "zext",  "trunc",   "sitofp",  "uitofp",   "fptosi",
			                                    "fptoui", "fpext", "fptrunc", "bitcast", "ptrtoint", "inttoptr"};
			auto k = inst.flags & 0xF;
			text = std::string("cast.") + (k < 12 ? kinds[k] : "?") + " " + A() + " : " +
			       typeName(static_cast<TypeId>(inst.type));
			break;
		}
		case Op::Load:
			text = "load " + A() + " : " + typeName(static_cast<TypeId>(inst.type)) + align(inst.flags);
			break;
		case Op::Store:
			text = "store " + A() + ", " + B() + align(inst.flags);
			break;
		case Op::Alloca:
			text = "alloca " + std::to_string(inst.a);
			break;
		case Op::Call: {
			const uint32_t* l = m_.extra.data() + inst.c;
			text = "call #c" + std::to_string(inst.a) + args(l + 1, l[0]) + unwind(l[1 + l[0]]);
			if (l[2 + l[0]] != NONE) {
				text += " thunk #p" + std::to_string(l[2 + l[0]]);
			}
			break;
		}
		case Op::CallIndirect: {
			const uint32_t* l = m_.extra.data() + inst.c;
			text = "call.indirect " + A() + args(l + 1, l[0]) + " : " + signature(inst.b) + attrsText(l[3 + l[0]]) +
			       unwind(l[1 + l[0]]);
			if (l[2 + l[0]] != NONE) {
				text += " thunk #p" + std::to_string(l[2 + l[0]]);
			}
			break;
		}
		case Op::FuncAddr:
			text = "funcaddr #c" + std::to_string(inst.a);
			break;
		case Op::BindAddr:
			text = "bind.addr #b" + std::to_string(inst.a);
			break;
		case Op::DataAddr:
			text = "data.addr #d" + std::to_string(inst.a);
			break;
		case Op::PtrConst:
			text = inst.a == NONE ? "ptr.const null" : "ptr.const #p" + std::to_string(inst.a);
			break;
		case Op::BlockArg:
			text = "blockarg " + std::to_string(inst.a) + " : " + typeName(static_cast<TypeId>(inst.type));
			break;
		case Op::Br: {
			const uint32_t* l = m_.extra.data() + inst.c;
			text = "br ^b" + std::to_string(inst.a) + (l[0] ? args(l + 1, l[0]) : "");
			break;
		}
		case Op::CondBr: {
			const uint32_t* l = m_.extra.data() + inst.c;
			uint32_t tc = l[1];
			uint32_t fc = l[3 + tc];
			text = "condbr " + A() + ", ^b" + std::to_string(inst.b) + (tc ? args(l + 2, tc) : "") + ", ^b" +
			       std::to_string(l[2 + tc]) + (fc ? args(l + 4 + tc, fc) : "");
			if (l[0] != NONE) {
				text += " prob " + std::to_string(l[0]);
			}
			break;
		}
		case Op::Ret:
			text = inst.a == NONE ? "ret" : "ret " + A();
			break;
		case Op::Resume:
			text = "resume";
			break;
		case Op::Assign:
			text = "assign " + A() + ", " + B();
			break;
		case Op::Var:
			text = "var " + A();
			break;
		}
		return lhs + text;
	}

	/// An extension instruction, generically from its operand shape (13 §5.11, §9.3): `name [flags(F)] f0, f1, ... [:
	/// T]`.
	std::string extensionInstruction(const Inst& inst) const {
		auto* info = ext::ExtensionRegistry::instance().op(inst.op);
		if (info == nullptr) {
			return "?op" + std::to_string(inst.op);
		}
		std::string text = info->name;
		if (inst.flags != 0) {
			text += " flags(" + std::to_string(inst.flags) + ")";
		}
		std::string fields;
		auto field = [&](ext::FieldKind kind, uint32_t value) {
			if (kind == ext::FieldKind::Unused) {
				return;
			}
			fields += fields.empty() ? " " : ", ";
			switch (kind) {
			case ext::FieldKind::Value:
			case ext::FieldKind::Const:
				fields += operand(value);
				break;
			case ext::FieldKind::Imm:
				fields += std::to_string(value);
				break;
			case ext::FieldKind::List: {
				const uint32_t* l = m_.extra.data() + value;
				fields += "[";
				for (uint32_t k = 0; k < l[0]; ++k) {
					fields += k ? ", " : "";
					fields += extensionWord(info->shape, k, l[1 + k]);
				}
				fields += "]";
				break;
			}
			case ext::FieldKind::Unused:
				break;
			}
		};
		field(info->shape.a, inst.a);
		field(info->shape.b, inst.b);
		field(info->shape.c, inst.c);
		text += fields;
		if (inst.type != static_cast<uint8_t>(TypeId::Void)) {
			text += " : " + typeName(static_cast<TypeId>(inst.type));
		}
		return text;
	}

	/// Word `k` of an extension payload: the prefix kinds, then the repeated group.
	std::string extensionWord(const ext::OperandShape& shape, uint32_t k, uint32_t w) const {
		ext::WordKind kind = k < shape.prefix.size() ? shape.prefix[k]
		                     : shape.group.empty()   ? ext::WordKind::Imm
		                                             : shape.group[(k - shape.prefix.size()) % shape.group.size()];
		switch (kind) {
		case ext::WordKind::Value:
		case ext::WordKind::Const:
			return operand(w);
		case ext::WordKind::Pointer:
			return "#p" + std::to_string(w);
		case ext::WordKind::Type:
			return typeName(static_cast<TypeId>(w & 0xFF));
		case ext::WordKind::Imm:
			break;
		}
		return std::to_string(w);
	}

	TypeId typeOfOperand(std::span<const Inst> insts, Ref r) const {
		if (isConstRef(r)) {
			return typeOfRef(r);
		}
		return r < insts.size() ? static_cast<TypeId>(insts[r].type) : TypeId::Void;
	}

	static std::string align(uint8_t flags) {
		auto k = flags & 0xF;
		return k == 0 ? "" : " align " + std::to_string(1u << (k - 1));
	}

	const FunctionRecord* curFunction() const {
		return &m_.functions[curF_];
	}

	const Module& m_;
	std::ostream& os_;
	Level level_ = Level::SSA;
	uint32_t curF_ = 0;
};

} // namespace

void print(const Module& m, std::ostream& os) {
	Printer p(m, os);
	p.run();
}

std::string print(const Module& m) {
	std::ostringstream os;
	print(m, os);
	return os.str();
}

} // namespace nautilus::nir
