#include "nautilus/compiler/backends/cpp/CPPNirLowering.hpp"
#include "nautilus/common/ExceptionTransport.hpp"
#include "nautilus/compiler/backends/nir/NirRuntime.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/nir/Inst.hpp"
#include <cstdint>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace nautilus::compiler::cpp {

using nir::CalleeKind;
using N = nir::Op;
using nir::TypeId;

namespace {

/// How a value is held inside a generated function: integers and pointers as unsigned integers of their width, so that
/// arithmetic wraps and the signed view is an explicit cast.
std::string valueType(TypeId t) {
	switch (t) {
	case TypeId::Bool:
		return "bool";
	case TypeId::I8:
		return "uint8_t";
	case TypeId::I16:
		return "uint16_t";
	case TypeId::I32:
		return "uint32_t";
	case TypeId::I64:
	case TypeId::Ptr:
		return "uint64_t";
	case TypeId::F32:
		return "float";
	case TypeId::F64:
		return "double";
	default:
		throw NotImplementedException("cpp: extension types are lowered with the extension opcode");
	}
}

std::string signedType(TypeId t) {
	switch (t) {
	case TypeId::I8:
		return "int8_t";
	case TypeId::I16:
		return "int16_t";
	case TypeId::I32:
		return "int32_t";
	case TypeId::I64:
	case TypeId::Ptr:
		return "int64_t";
	default:
		throw RuntimeException("cpp: a signed view of a type that is not an integer");
	}
}

/// The width an operation on a narrow integer is carried out in: wide enough that the promotion rules never produce a
/// signed overflow.
std::string wideType(TypeId t) {
	return nir::sizeOf(t) <= 4 ? "uint32_t" : "uint64_t";
}

unsigned bitWidth(TypeId t) {
	return nir::sizeOf(t) * 8;
}

/// The type of a value at a native boundary (a signature): what the C ABI sees, including the extension kind of a
/// narrow integer (13 §4.5.1).
std::string abiType(nir::TypeWord word) {
	const auto kind = nir::extKindOf(word);
	switch (nir::typeOf(word)) {
	case TypeId::Void:
		return "void";
	case TypeId::Bool:
		return "bool";
	case TypeId::I8:
		return kind == nir::ExtKind::Zext ? "uint8_t" : "int8_t";
	case TypeId::I16:
		return kind == nir::ExtKind::Zext ? "uint16_t" : "int16_t";
	case TypeId::I32:
		return kind == nir::ExtKind::Zext ? "uint32_t" : "int32_t";
	case TypeId::I64:
		return "int64_t";
	case TypeId::F32:
		return "float";
	case TypeId::F64:
		return "double";
	case TypeId::Ptr:
		return "uint8_t*";
	default:
		throw NotImplementedException("cpp: extension types in a signature are lowered with the extension opcode");
	}
}

/// `expression` of the ABI type `word` as the internal representation of a value of that type.
std::string fromAbi(nir::TypeWord word, const std::string& expression) {
	const auto type = nir::typeOf(word);
	if (type == TypeId::Ptr) {
		return "(uint64_t)(uintptr_t)(" + expression + ")";
	}
	if (type == TypeId::Bool || nir::isFloat(type)) {
		return "(" + valueType(type) + ")(" + expression + ")";
	}
	return "(" + valueType(type) + ")(" + expression + ")";
}

/// A value of the internal representation as the ABI type `word`.
std::string toAbi(nir::TypeWord word, const std::string& value) {
	if (nir::typeOf(word) == TypeId::Ptr) {
		return "(uint8_t*)(uintptr_t)" + value;
	}
	return "(" + abiType(word) + ")" + value;
}

std::string hex(uint64_t value) {
	std::ostringstream out;
	out << "0x" << std::hex << value << "ULL";
	return out.str();
}

class Lowering {
public:
	Lowering(const nir::Module& module, const nirbackend::HostEnvironment& host) : m_(module), host_(host) {
	}

	std::string run() {
		for (uint32_t f = 0; f < m_.functions.size(); ++f) {
			if (m_.functions[f].level != static_cast<uint8_t>(nir::Level::SSA)) {
				throw RuntimeException("cpp: function #" + std::to_string(f) + " is not an SSA function");
			}
		}
		const auto needsCapture = !nirbackend::functionsNeedingCapture(m_).empty();

		std::ostringstream functions;
		for (uint32_t f = 0; f < m_.functions.size(); ++f) {
			lowerFunction(f, functions);
		}

		std::ostringstream out;
		out << "\n#include <cstdint>\n";
		if (needsCapture) {
			// The generated library cannot link against nautilus, so the helpers of the exception frame are baked in
			// as raw addresses resolved in the host process.
			out << "#include <exception>\n\n";
			out << "namespace nautilus { namespace compiler {\n";
			out << "struct ExceptionFrame { std::exception_ptr pending; ExceptionFrame* parent = nullptr; };\n";
			out << "}}\n\n";
			out << "static nautilus::compiler::ExceptionFrame* (*nautilus_current_exception_frame)() = "
			    << "(nautilus::compiler::ExceptionFrame* (*)())(uintptr_t)"
			    << reinterpret_cast<void*>(&nautilus::compiler::currentExceptionFrame) << ";\n";
			out << "static bool (*nautilus_has_pending_exception)() = (bool (*)())(uintptr_t)"
			    << reinterpret_cast<void*>(&nautilus::compiler::hasPendingException) << ";\n\n";
		}
		out << helpers_.str() << "\n";
		// forward declarations: functions call each other regardless of definition order
		for (uint32_t f = 0; f < m_.functions.size(); ++f) {
			out << prototype(f, true) << ";\n";
		}
		out << "\n" << functions.str();
		return out.str();
	}

private:
	const nir::Module& m_;
	const nirbackend::HostEnvironment& host_;
	std::ostringstream helpers_;
	std::set<std::string> helperNames_;

	// per function
	uint32_t f_ = 0;
	std::span<const nir::Inst> insts_;
	std::span<const nir::BlockRecord> blocks_;
	bool usesExit_ = false;

	static std::string functionName(uint32_t f) {
		return "nfn_" + std::to_string(f);
	}

	std::string signatureTypes(const nir::SignatureRecord& sig) const {
		std::string types;
		for (uint32_t p = 0; p < sig.paramCount; ++p) {
			types += (p == 0 ? "" : ",") + abiType(m_.paramType(sig, p));
		}
		return types;
	}

	/// The declaration of a function. Every function is a symbol of the library under its exact name, which need not be
	/// a C identifier and may clash with a name of the C library, so the C name is `nfn_<index>` and the symbol an
	/// assembler label: the engine looks module functions up by name, not only the entry point.
	std::string prototype(uint32_t f, bool declaration) const {
		const auto& record = m_.functions[f];
		const auto& sig = m_.signatures[record.signature];
		std::string text = "extern \"C\" " + abiType(sig.result) + " " + functionName(f) + "(";
		for (uint32_t p = 0; p < sig.paramCount; ++p) {
			text += (p == 0 ? "" : ", ") + abiType(m_.paramType(sig, p));
			if (!declaration) {
				text += " p" + std::to_string(p);
			}
		}
		text += ")";
		if (declaration) {
			text += " __asm__(\"" + std::string(m_.name(record.nameId)) + "\")";
		}
		return text;
	}

	// ---- values ---------------------------------------------------------------------------------------------

	static std::string var(uint32_t id) {
		return "v" + std::to_string(id);
	}

	TypeId typeOfRef(nir::Ref r) const {
		return nir::isConstRef(r) ? static_cast<TypeId>(m_.consts.record(r).type) : static_cast<TypeId>(insts_[r].type);
	}

	std::string constant(nir::Ref r) const {
		const auto& record = m_.consts.record(r);
		const auto type = static_cast<TypeId>(record.type);
		switch (type) {
		case TypeId::Bool:
			return record.bits != 0 ? "true" : "false";
		case TypeId::I8:
			return "(uint8_t)" + hex(record.bits & 0xFF);
		case TypeId::I16:
			return "(uint16_t)" + hex(record.bits & 0xFFFF);
		case TypeId::I32:
			return "(uint32_t)" + hex(record.bits & 0xFFFFFFFFull);
		case TypeId::I64:
			return "(uint64_t)" + hex(record.bits);
		case TypeId::F32:
			// the bit pattern, so that NaN payloads and the sign of zero survive
			return "__builtin_bit_cast(float, (uint32_t)" + hex(record.bits & 0xFFFFFFFFull) + ")";
		case TypeId::F64:
			return "__builtin_bit_cast(double, (uint64_t)" + hex(record.bits) + ")";
		default:
			throw NotImplementedException("cpp: a constant of an extension type");
		}
	}

	/// The expression for an operand.
	std::string val(nir::Ref r) const {
		return nir::isConstRef(r) ? constant(r) : var(r);
	}

	/// An integer operand as its signed view.
	std::string asSigned(nir::Ref r) const {
		return "(" + signedType(typeOfRef(r)) + ")" + val(r);
	}

	// ---- helpers emitted once -------------------------------------------------------------------------------

	void ensureHelper(const std::string& name, const std::string& definition) {
		if (helperNames_.insert(name).second) {
			helpers_ << definition << "\n";
		}
	}

	/// The saturating conversion of D11: NaN gives 0, a value out of range gives the minimum or the maximum.
	std::string saturatingHelper(bool isSigned, TypeId to, TypeId from) {
		const unsigned bits = bitWidth(to);
		const std::string source = from == TypeId::F32 ? "float" : "double";
		const std::string result = (isSigned ? "int" : "uint") + std::to_string(bits) + "_t";
		const std::string name = std::string("nautilus_") + (isSigned ? "f2s" : "f2u") + std::to_string(bits) + "_" +
		                         (from == TypeId::F32 ? "f32" : "f64");
		// the limits of the integer are exact in a float or double: -2^(bits-1), 0, and 2^bits resp. 2^(bits-1)
		const std::string two =
		    isSigned ? "(" + source + ")" + std::to_string(uint64_t {1} << (bits - 1)) + "ULL"
		             : "(" + source + ")" + std::to_string(bits == 64 ? 0 : (uint64_t {1} << bits)) + "ULL";
		std::ostringstream def;
		def << "static inline " << result << " " << name << "(" << source << " v) {\n";
		def << "  if (v != v) return 0;\n";
		if (isSigned) {
			const std::string hi =
			    bits == 64 ? "9223372036854775808.0" : std::to_string(uint64_t {1} << (bits - 1)) + ".0";
			def << "  if (v <= (" << source << ")-" << hi << ") return (" << result << ")((uint" << bits << "_t)1 << "
			    << (bits - 1) << ");\n";
			def << "  if (v >= (" << source << ")" << hi << ") return (" << result << ")(((uint" << bits << "_t)1 << "
			    << (bits - 1) << ") - 1);\n";
		} else {
			const std::string hi = bits == 64 ? "18446744073709551616.0" : std::to_string(uint64_t {1} << bits) + ".0";
			def << "  if (v <= (" << source << ")0) return 0;\n";
			def << "  if (v >= (" << source << ")" << hi << ") return (" << result << ")~(" << result << ")0;\n";
		}
		def << "  return (" << result << ")v;\n}\n";
		(void) two;
		ensureHelper(name, def.str());
		return name;
	}

	// ---- control flow -------------------------------------------------------------------------------------

	static std::string label(uint32_t block) {
		return "B" + std::to_string(block);
	}

	/// Where an exception raised at an instruction with unwind entry `unwind` continues: its landing pad, or the exit
	/// of the function.
	std::string unwindLabel(uint32_t unwind) {
		if (unwind == nir::NO_CLEANUP || unwind == nir::NO_UNWIND) {
			usesExit_ = true;
			return "exceptional_exit";
		}
		return label(unwind);
	}

	/// The arguments of an edge as a parallel copy into the parameters of `target`: an argument may be a parameter of
	/// the target itself, so all are read before any is written.
	void edge(std::ostringstream& out, uint32_t target, std::span<const uint32_t> args) {
		std::vector<std::pair<uint32_t, std::string>> copies;
		for (size_t i = 0; i < args.size(); ++i) {
			const uint32_t param = blocks_[target].first + static_cast<uint32_t>(i);
			const auto source = val(args[i]);
			if (source != var(param)) {
				copies.emplace_back(param, source);
			}
		}
		if (copies.empty()) {
			return;
		}
		out << "{\n";
		for (size_t i = 0; i < copies.size(); ++i) {
			out << valueType(static_cast<TypeId>(insts_[copies[i].first].type)) << " t" << i << " = "
			    << copies[i].second << ";\n";
		}
		for (size_t i = 0; i < copies.size(); ++i) {
			out << var(copies[i].first) << " = t" << i << ";\n";
		}
		out << "}\n";
	}

	/// A call expression of the ABI types of a signature, with its arguments converted.
	struct CallParts {
		std::string pointerType; // R(*)(A, B)
		std::string arguments;   // converted arguments, comma separated
		nir::TypeWord result;
	};

	CallParts callParts(const nir::SignatureRecord& sig, const uint32_t* list) {
		CallParts parts;
		parts.result = sig.result;
		parts.pointerType = abiType(sig.result) + "(*)(" + signatureTypes(sig) + ")";
		const uint32_t argc = list[0];
		for (uint32_t i = 0; i < argc; ++i) {
			parts.arguments += (i == 0 ? "" : ", ") + toAbi(m_.paramType(sig, i), val(list[1 + i]));
		}
		return parts;
	}

	/// Emits `result = call` (or a captured call when the instruction may throw), and the check that continues at the
	/// pad when an exception is pending.
	void emitCall(std::ostringstream& out, uint32_t id, const nir::Inst& inst, const std::string& callExpression,
	              nir::TypeWord resultWord, uint32_t unwind) {
		const bool hasResult = nir::typeOf(resultWord) != TypeId::Void;
		const std::string assign = hasResult ? var(id) + " = " + fromAbi(resultWord, callExpression) : callExpression;
		if (unwind == nir::NO_UNWIND) {
			out << assign << ";\n";
			return;
		}
		out << "try {\n" << assign << ";\n} catch (...) {\n";
		out << "auto* frame = nautilus_current_exception_frame();\n";
		out << "if (frame && !frame->pending) { frame->pending = std::current_exception(); }\n";
		if (hasResult) {
			out << var(id) << " = (" << valueType(static_cast<TypeId>(inst.type)) << ")0;\n";
		}
		out << "}\n";
		out << "if (nautilus_has_pending_exception()) {\ngoto " << unwindLabel(unwind) << ";\n}\n";
	}

	// ---- instructions ---------------------------------------------------------------------------------------

	void lowerInstruction(std::ostringstream& out, uint32_t id) {
		const nir::Inst& inst = insts_[id];
		const auto op = static_cast<N>(inst.op);
		const auto type = static_cast<TypeId>(inst.type);
		const auto result = var(id);
		auto assign = [&](const std::string& expression) {
			out << result << " = " << expression << ";\n";
		};
		switch (op) {
		case N::Add:
		case N::Sub:
		case N::Mul: {
			const char* symbol = op == N::Add ? "+" : op == N::Sub ? "-" : "*";
			if (nir::isFloat(type)) {
				assign("(" + valueType(type) + ")(" + val(inst.a) + " " + symbol + " " + val(inst.b) + ")");
			} else {
				const auto wide = wideType(type);
				assign("(" + valueType(type) + ")((" + wide + ")" + val(inst.a) + " " + symbol + " (" + wide + ")" +
				       val(inst.b) + ")");
			}
			return;
		}
		case N::Div:
		case N::Rem:
			return lowerDivision(out, id, inst);
		case N::FNeg:
			assign("-" + val(inst.a));
			return;
		case N::And:
			assign(val(inst.a) + " && " + val(inst.b));
			return;
		case N::Or:
			assign(val(inst.a) + " || " + val(inst.b));
			return;
		case N::Not:
			assign("!" + val(inst.a));
			return;
		case N::BAnd:
		case N::BOr:
		case N::BXor: {
			const char* symbol = op == N::BAnd ? "&" : op == N::BOr ? "|" : "^";
			assign("(" + valueType(type) + ")(" + val(inst.a) + " " + symbol + " " + val(inst.b) + ")");
			return;
		}
		case N::BNot:
			assign("(" + valueType(type) + ")~" + val(inst.a));
			return;
		case N::Shl:
		case N::Shr: {
			// the amount is taken modulo the bit width (D10)
			const auto amount = "(uint32_t)(" + val(inst.b) + " & " + std::to_string(bitWidth(type) - 1) + ")";
			if (op == N::Shl) {
				assign("(" + valueType(type) + ")((" + wideType(type) + ")" + val(inst.a) + " << " + amount + ")");
			} else if ((inst.flags & 1) != 0) {
				assign("(" + valueType(type) + ")((" + wideType(type) + ")" + val(inst.a) + " >> " + amount + ")");
			} else {
				assign("(" + valueType(type) + ")(" + asSigned(inst.a) + " >> " + amount + ")");
			}
			return;
		}
		case N::Cmp:
			return lowerCompare(out, id, inst);
		case N::Select:
			assign(val(inst.a) + " ? " + val(inst.b) + " : " + val(inst.c));
			return;
		case N::Cast:
			return lowerCast(out, id, inst);
		case N::Load: {
			const auto t = valueType(type);
			const auto address = "(const void*)(uintptr_t)" + val(inst.a);
			if (type == TypeId::Bool) {
				out << "{ uint8_t raw; __builtin_memcpy(&raw, " << address << ", 1); " << result << " = raw != 0; }\n";
			} else {
				out << "__builtin_memcpy(&" << result << ", " << address << ", sizeof(" << t << "));\n";
			}
			return;
		}
		case N::Store: {
			const auto valueTypeOf = typeOfRef(inst.b);
			const auto address = "(void*)(uintptr_t)" + val(inst.a);
			if (valueTypeOf == TypeId::Bool) {
				out << "{ uint8_t raw = " << val(inst.b) << " ? 1 : 0; __builtin_memcpy(" << address
				    << ", &raw, 1); }\n";
			} else {
				out << "{ " << valueType(valueTypeOf) << " stored = " << val(inst.b) << "; __builtin_memcpy(" << address
				    << ", &stored, sizeof(stored)); }\n";
			}
			return;
		}
		case N::Alloca:
			assign("(uint64_t)(uintptr_t)alloca_buf_" + std::to_string(inst.a));
			return;
		case N::Call:
			return lowerCall(out, id, inst);
		case N::CallIndirect: {
			const uint32_t* list = m_.extra.data() + inst.c;
			const auto& sig = m_.signatures[inst.b];
			const auto parts = callParts(sig, list);
			const auto expression =
			    "((" + parts.pointerType + ")(uintptr_t)" + val(inst.a) + ")(" + parts.arguments + ")";
			emitCall(out, id, inst, expression, sig.result, list[1 + list[0]]);
			return;
		}
		case N::FuncAddr:
			return lowerFuncAddr(out, id, inst);
		case N::BindAddr: {
			void* address = host_.bindingAddress ? host_.bindingAddress(m_, inst.a) : nullptr;
			if (address == nullptr) {
				throw RuntimeException("cpp: the host supplies no address for binding #b" + std::to_string(inst.a));
			}
			assign("(uint64_t)" + hex(reinterpret_cast<uint64_t>(address)));
			return;
		}
		case N::DataAddr: {
			const auto& data = m_.data.at(inst.a);
			const auto* address = m_.dataBytes.data() + data.byteOffset;
			assign("(uint64_t)" + hex(reinterpret_cast<uint64_t>(address)));
			return;
		}
		case N::PtrConst: {
			void* address = inst.a == nir::NONE ? nullptr : nirbackend::resolvePointer(m_, inst.a, host_);
			assign("(uint64_t)" + hex(reinterpret_cast<uint64_t>(address)));
			return;
		}
		case N::Br: {
			const uint32_t* list = m_.extra.data() + inst.c;
			edge(out, inst.a, std::span<const uint32_t>(list + 1, list[0]));
			out << "goto " << label(inst.a) << ";\n";
			return;
		}
		case N::CondBr: {
			const uint32_t* list = m_.extra.data() + inst.c;
			const uint32_t trueArgc = list[1];
			const uint32_t falseBlock = list[2 + trueArgc];
			const uint32_t falseArgc = list[3 + trueArgc];
			out << "if (" << val(inst.a) << ") {\n";
			edge(out, inst.b, std::span<const uint32_t>(list + 2, trueArgc));
			out << "goto " << label(inst.b) << ";\n} else {\n";
			edge(out, falseBlock, std::span<const uint32_t>(list + 4 + trueArgc, falseArgc));
			out << "goto " << label(falseBlock) << ";\n}\n";
			return;
		}
		case N::Ret: {
			const auto& sig = m_.signatures[m_.functions[f_].signature];
			if (inst.a == nir::NONE) {
				out << "return;\n";
			} else {
				out << "return " << toAbi(sig.result, val(inst.a)) << ";\n";
			}
			return;
		}
		case N::Resume:
			usesExit_ = true;
			out << "goto exceptional_exit;\n";
			return;
		default:
			throw NotImplementedException("cpp: extension operations are lowered with the extension opcode");
		}
	}

	void lowerCompare(std::ostringstream& out, uint32_t id, const nir::Inst& inst) {
		static const char* const symbols[] = {"==", "!=", "<", "<=", ">", ">="};
		const unsigned predicate = inst.flags & 7;
		if (predicate > 5) {
			throw RuntimeException("cpp: an invalid compare predicate");
		}
		const auto type = typeOfRef(inst.a);
		const bool ordered = predicate >= 2;
		std::string left = val(inst.a), right = val(inst.b);
		// signed order needs the signed view; a pointer orders as unsigned (13 §5.4)
		if (ordered && nir::isInteger(type) && (inst.flags & 8) == 0) {
			left = asSigned(inst.a);
			right = asSigned(inst.b);
		}
		out << var(id) << " = " << left << " " << symbols[predicate] << " " << right << ";\n";
	}

	void lowerDivision(std::ostringstream& out, uint32_t id, const nir::Inst& inst) {
		const auto type = static_cast<TypeId>(inst.type);
		const bool isDiv = static_cast<N>(inst.op) == N::Div;
		const auto result = var(id);
		if (nir::isFloat(type)) {
			if (isDiv) {
				out << result << " = " << val(inst.a) << " / " << val(inst.b) << ";\n";
			} else {
				out << result << " = " << (type == TypeId::F32 ? "__builtin_fmodf" : "__builtin_fmod") << "("
				    << val(inst.a) << ", " << val(inst.b) << ");\n";
			}
			return;
		}
		const bool isUnsigned = (inst.flags & 1) != 0;
		const bool checked = (inst.flags & 2) != 0;
		const auto a = val(inst.a), b = val(inst.b);
		if (checked) {
			// the divisor is zero, or the signed minimum is divided by -1: throw through the capture, then continue at
			// the pad (13 §5.10)
			std::string bad = b + " == 0";
			if (!isUnsigned) {
				const auto bits = bitWidth(type);
				bad += " || (" + a + " == (" + valueType(type) + ")" + hex(uint64_t {1} << (bits - 1)) + " && " + b +
				       " == (" + valueType(type) + ")" + hex(bits == 64 ? ~uint64_t {0} : (uint64_t {1} << bits) - 1) +
				       ")";
			}
			out << "if (" << bad << ") {\n";
			out << "try { ((void(*)())(uintptr_t)" << reinterpret_cast<void*>(nirbackend::throwArithTarget())
			    << ")(); } catch (...) {\n";
			out << "auto* frame = nautilus_current_exception_frame();\n";
			out << "if (frame && !frame->pending) { frame->pending = std::current_exception(); }\n}\n";
			out << "goto " << unwindLabel(inst.c) << ";\n}\n";
		}
		const char* symbol = isDiv ? "/" : "%";
		if (isUnsigned) {
			out << result << " = (" << valueType(type) << ")((" << wideType(type) << ")" << a << " " << symbol << " ("
			    << wideType(type) << ")" << b << ");\n";
		} else {
			out << result << " = (" << valueType(type) << ")(" << asSigned(inst.a) << " " << symbol << " "
			    << asSigned(inst.b) << ");\n";
		}
	}

	void lowerCast(std::ostringstream& out, uint32_t id, const nir::Inst& inst) {
		const auto kind = inst.flags & 15;
		const auto from = typeOfRef(inst.a);
		const auto to = static_cast<TypeId>(inst.type);
		const auto source = val(inst.a);
		const auto target = valueType(to);
		const auto result = var(id);
		switch (kind) {
		case 0: // sext
			if (from == TypeId::Bool) {
				out << result << " = (" << target << ")(" << source << " ? -1 : 0);\n";
			} else {
				out << result << " = (" << target << ")(int64_t)" << asSigned(inst.a) << ";\n";
			}
			return;
		case 1: // zext
			out << result << " = (" << target << ")(uint64_t)" << source << ";\n";
			return;
		case 2: // trunc
		case 7: // fpext
		case 8: // fptrunc
			out << result << " = (" << target << ")" << source << ";\n";
			return;
		case 3: // sitofp
			out << result << " = (" << target << ")" << asSigned(inst.a) << ";\n";
			return;
		case 4: // uitofp
			out << result << " = (" << target << ")" << source << ";\n";
			return;
		case 5: // fptosi
		case 6: // fptoui: saturating (D11)
			out << result << " = (" << target << ")" << saturatingHelper(kind == 5, to, from) << "(" << source
			    << ");\n";
			return;
		case 9: // bitcast
			out << result << " = __builtin_bit_cast(" << target << ", " << source << ");\n";
			return;
		case 10: // ptrtoint
		case 11: // inttoptr
			out << result << " = (" << target << ")" << source << ";\n";
			return;
		default:
			throw RuntimeException("cpp: an invalid cast kind");
		}
	}

	void lowerCall(std::ostringstream& out, uint32_t id, const nir::Inst& inst) {
		const auto& callee = m_.callees.at(inst.a);
		const auto& sig = m_.signatures[callee.signature];
		const uint32_t* list = m_.extra.data() + inst.c;
		const auto parts = callParts(sig, list);
		const uint32_t unwind = list[1 + list[0]];
		std::string expression;
		switch (static_cast<CalleeKind>(callee.kind)) {
		case CalleeKind::Internal:
			expression = functionName(callee.target) + "(" + parts.arguments + ")";
			break;
		case CalleeKind::External:
			expression = "((" + parts.pointerType + ")" +
			             hex(reinterpret_cast<uint64_t>(nirbackend::resolvePointer(m_, callee.target, host_))) + ")(" +
			             parts.arguments + ")";
			break;
		case CalleeKind::Intrinsic: {
			const auto* info = ext::ExtensionRegistry::instance().intrinsic(callee.target);
			if (info == nullptr) {
				throw RuntimeException("cpp: an intrinsic callee that is not registered");
			}
			// no backend lowering yet: call the fallback
			expression = "((" + parts.pointerType + ")" + hex(reinterpret_cast<uint64_t>(info->fallback)) + ")(" +
			             parts.arguments + ")";
			break;
		}
		case CalleeKind::Imported:
			throw NotImplementedException("cpp: imported callees come with modules and linking (M4.6)");
		}
		emitCall(out, id, inst, expression, sig.result, unwind);
	}

	void lowerFuncAddr(std::ostringstream& out, uint32_t id, const nir::Inst& inst) {
		const auto& callee = m_.callees.at(inst.a);
		switch (static_cast<CalleeKind>(callee.kind)) {
		case CalleeKind::Internal:
			out << var(id) << " = (uint64_t)(uintptr_t)&" << functionName(callee.target) << ";\n";
			return;
		case CalleeKind::External:
			out << var(id) << " = (uint64_t)"
			    << hex(reinterpret_cast<uint64_t>(nirbackend::resolvePointer(m_, callee.target, host_))) << ";\n";
			return;
		case CalleeKind::Imported:
			throw NotImplementedException("cpp: imported callees come with modules and linking (M4.6)");
		case CalleeKind::Intrinsic:
			throw RuntimeException("cpp: an intrinsic has no address");
		}
	}

	// ---- functions ------------------------------------------------------------------------------------------

	void lowerFunction(uint32_t f, std::ostringstream& out) {
		f_ = f;
		insts_ = m_.functionInsts(f);
		blocks_ = m_.functionBlocks(f);
		usesExit_ = false;
		const auto& record = m_.functions[f];
		const auto& sig = m_.signatures[record.signature];

		std::ostringstream body;
		for (uint32_t b = 0; b < blocks_.size(); ++b) {
			body << label(b) << ":\n";
			const auto& block = blocks_[b];
			for (uint32_t k = block.argCount; k < block.count; ++k) {
				lowerInstruction(body, block.first + k);
			}
		}

		out << prototype(f, false) << " {\n";
		// every value is declared up front, so that a jump never crosses a declaration
		for (uint32_t i = 0; i < insts_.size(); ++i) {
			const auto type = static_cast<TypeId>(insts_[i].type);
			const auto op = static_cast<N>(insts_[i].op);
			const bool producesValue = type != TypeId::Void && op != N::Br && op != N::CondBr && op != N::Ret &&
			                           op != N::Resume && op != N::Store && op != N::Assign;
			if (producesValue) {
				out << valueType(type) << " " << var(i) << ";\n";
			}
		}
		uint32_t slot = 0;
		for (const auto& alloca : m_.functionAllocas(f)) {
			out << "alignas(" << std::max<uint32_t>(alloca.align, 1) << ") uint8_t alloca_buf_" << slot++ << "["
			    << std::max<uint32_t>(alloca.size, 1) << "];\n";
		}
		// the parameters are the arguments of the entry block
		for (uint32_t p = 0; p < blocks_[0].argCount; ++p) {
			out << var(blocks_[0].first + p) << " = " << fromAbi(m_.paramType(sig, p), "p" + std::to_string(p))
			    << ";\n";
		}
		out << body.str();
		if (usesExit_) {
			out << "exceptional_exit:\n";
			if (nir::typeOf(sig.result) == TypeId::Void) {
				out << "return;\n";
			} else {
				out << "return " << toAbi(sig.result, "(" + valueType(nir::typeOf(sig.result)) + ")0") << ";\n";
			}
		}
		out << "}\n\n";
	}
};

} // namespace

std::string lowerNirToCpp(const nir::Module& module, const nirbackend::HostEnvironment& host) {
	return Lowering(module, host).run();
}

} // namespace nautilus::compiler::cpp
