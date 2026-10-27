#include "nautilus/nir/Verifier.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include <algorithm>
#include <set>
#include <string>

namespace nautilus::nir {

namespace {

constexpr bool isPow2(uint32_t v) {
	return v != 0 && (v & (v - 1)) == 0;
}

class Checker {
public:
	Checker(const Module& m, const VerifyOptions& o, std::vector<Diagnostic>& out) : m_(m), o_(o), out_(out) {
	}

	void report(DiagCode code, std::string message, uint32_t function = NONE, uint32_t inst = NONE,
	            uint32_t block = NONE) {
		if (out_.size() < o_.maxDiagnostics) {
			out_.push_back(Diagnostic {code, function, inst, block, std::move(message)});
		}
	}

	bool nameOk(uint32_t id) const {
		return id < m_.names.size();
	}
	bool cAbi(TypeWord w) const {
		auto t = typeOf(w);
		if (!isExtension(t)) {
			return true;
		}
		if (o_.extensionHasCabi) {
			return o_.extensionHasCabi(t);
		}
		auto* info = ext::ExtensionRegistry::instance().type(static_cast<uint8_t>(t));
		return info != nullptr && info->available && !info->cabi.empty();
	}
	bool sigCAbi(uint32_t sig) const {
		auto& s = m_.signatures[sig];
		if (!cAbi(s.result)) {
			return false;
		}
		for (uint32_t i = 0; i < s.paramCount; ++i) {
			if (!cAbi(m_.paramType(s, i))) {
				return false;
			}
		}
		return true;
	}

	void moduleRecord() {
		auto& r = m_.module;
		if (r.flags != 0) {
			report(DiagCode::V030, "the module record has reserved flags set");
		}
		if (r.nameId != NONE && !nameOk(r.nameId)) {
			report(DiagCode::V030, "the module name is not in the names section");
		}
	}

	/// Returns false if the signature table is not safe to read.
	bool signatures() {
		bool ok = true;
		for (uint32_t i = 0; i < m_.signatures.size(); ++i) {
			auto& s = m_.signatures[i];
			std::string where = "signature #" + std::to_string(i);
			if (s.paramCount != 0 && (uint64_t(s.paramTypes) + s.paramCount > m_.extra.size())) {
				report(DiagCode::V008, where + ": parameter list is out of bounds of the extra section");
				ok = false;
				continue;
			}
			if (!isValidTypeWord(s.result)) {
				report(DiagCode::V036, where + ": invalid result type word");
			}
			for (uint32_t p = 0; p < s.paramCount; ++p) {
				auto w = m_.paramType(s, p);
				if (!isValidTypeWord(w) || typeOf(w) == TypeId::Void) {
					report(DiagCode::V036, where + ": invalid type word of parameter " + std::to_string(p));
				}
			}
		}
		return ok;
	}

	void functions() {
		std::set<std::string_view> seen;
		for (uint32_t i = 0; i < m_.functions.size(); ++i) {
			auto& f = m_.functions[i];
			if (f.level > 1 || f.visibility > 1 || f.pad != 0) {
				report(DiagCode::V031, "level, visibility or padding out of range", i);
			}
			if (!nameOk(f.nameId) || m_.name(f.nameId).empty()) {
				report(DiagCode::V031, "function name is missing or empty", i);
			} else if (!seen.insert(m_.name(f.nameId)).second) {
				report(DiagCode::V031, "duplicate function name '" + std::string(m_.name(f.nameId)) + "'", i);
			}
			if (f.signature >= m_.signatures.size()) {
				report(DiagCode::V007, "signature index out of range", i);
			} else if (f.visibility == 1 && !sigCAbi(f.signature)) {
				report(DiagCode::V035, "exported function uses a type without a C ABI", i);
			}
			if ((f.attrs >> 7) != 0) {
				report(DiagCode::V034, "reserved attribute bits are set", i);
			}
		}
	}

	void callees() {
		for (uint32_t i = 0; i < m_.callees.size(); ++i) {
			auto& c = m_.callees[i];
			std::string where = "callee #" + std::to_string(i);
			if (c.pad[0] || c.pad[1] || c.pad[2]) {
				report(DiagCode::V004, where + ": padding is not 0");
			}
			if ((c.attrs >> 7) != 0) {
				report(DiagCode::V034, where + ": reserved attribute bits are set");
			}
			if (c.signature >= m_.signatures.size()) {
				report(DiagCode::V007, where + ": signature index out of range");
				continue;
			}
			switch (static_cast<CalleeKind>(c.kind)) {
			case CalleeKind::Internal:
				if (c.target >= m_.functions.size()) {
					report(DiagCode::V007, where + ": function index out of range");
					break;
				}
				if (m_.functions[c.target].signature != c.signature) {
					report(DiagCode::V032, where + ": signature differs from the function's");
				}
				if ((m_.functions[c.target].attrs & GUARANTEE_MASK) != (c.attrs & GUARANTEE_MASK)) {
					report(DiagCode::V033, where + ": attributes differ from the function's");
				}
				break;
			case CalleeKind::External:
				if (c.target >= m_.pointers.size()) {
					report(DiagCode::V007, where + ": pointers index out of range");
					break;
				}
				if (m_.pointers[c.target].kind != static_cast<uint8_t>(PointerKind::Callee) &&
				    m_.pointers[c.target].kind != static_cast<uint8_t>(PointerKind::Destructor)) {
					report(DiagCode::V007, where + ": pointer is neither a callee nor a destructor");
					break;
				}
				if (!sigCAbi(c.signature)) {
					report(DiagCode::V035, where + ": external callee uses a type without a C ABI");
				}
				if (m_.pointers[c.target].kind == static_cast<uint8_t>(PointerKind::Destructor)) {
					auto& s = m_.signatures[c.signature];
					if (s.paramCount != 1 || m_.paramType(s, 0) != makeTypeWord(TypeId::Ptr) ||
					    s.result != makeTypeWord(TypeId::Void)) {
						report(DiagCode::V045, where + ": destructor callee must have signature (ptr) -> void");
					}
				}
				break;
			case CalleeKind::Imported:
				if (c.target >= m_.imports.size()) {
					report(DiagCode::V007, where + ": import index out of range");
					break;
				}
				if (m_.imports[c.target].signature != c.signature) {
					report(DiagCode::V032, where + ": signature differs from the import's");
				}
				break;
			case CalleeKind::Intrinsic:
				if (c.target >= m_.extensions.size()) {
					report(DiagCode::V007, where + ": extension index out of range");
				}
				break;
			default:
				report(DiagCode::V007, where + ": unknown callee kind");
			}
		}
	}

	void imports() {
		std::set<std::pair<uint32_t, uint32_t>> seen;
		for (uint32_t i = 0; i < m_.imports.size(); ++i) {
			auto& r = m_.imports[i];
			std::string where = "import #" + std::to_string(i);
			if (!nameOk(r.moduleNameId) || !nameOk(r.functionNameId)) {
				report(DiagCode::V007, where + ": name index out of range");
				continue;
			}
			if (r.signature >= m_.signatures.size()) {
				report(DiagCode::V007, where + ": signature index out of range");
			}
			if ((r.attrs >> 7) != 0) {
				report(DiagCode::V034, where + ": reserved attribute bits are set");
			}
			if (!seen.insert({r.moduleNameId, r.functionNameId}).second) {
				report(DiagCode::V037, where + ": duplicate import");
			}
			if (m_.module.nameId != NONE && nameOk(m_.module.nameId) &&
			    m_.name(r.moduleNameId) == m_.name(m_.module.nameId)) {
				report(DiagCode::V037, where + ": a module cannot import from itself");
			}
		}
		if (o_.deserialized && !m_.imports.empty()) {
			report(DiagCode::V038, "a deserialized image has no imports");
		}
	}

	void constants() {
		for (uint32_t i = 0; i < m_.consts.size(); ++i) {
			auto& c = m_.consts.at(i);
			std::string where = "constant #" + std::to_string(i);
			auto t = static_cast<TypeId>(c.type);
			if (c.pad != 0 || c.provenance > 2) {
				report(DiagCode::V039, where + ": padding or provenance out of range");
			}
			if (t == TypeId::Void || t == TypeId::Ptr || (t > TypeId::Ptr && !isExtension(t))) {
				report(DiagCode::V039, where + ": invalid constant type");
				continue;
			}
			if (isExtension(t)) {
				continue; // size comes from the registry
			}
			if (c.byteOffset != 0) {
				report(DiagCode::V039, where + ": byteOffset is set for a type of at most 8 bytes");
			}
			auto bits = sizeOf(t) * 8;
			if (t == TypeId::Bool) {
				bits = 1;
			}
			if (bits < 64 && (c.bits >> bits) != 0) {
				report(DiagCode::V039, where + ": unused upper bits are not 0");
			}
		}
	}

	void pointers() {
		bool processLocal = false;
		for (uint32_t i = 0; i < m_.pointers.size(); ++i) {
			auto& p = m_.pointers[i];
			std::string where = "pointer #" + std::to_string(i);
			if (p.kind > 4 || p.mode > 2 || p.pad != 0 || p.reserved != 0) {
				report(DiagCode::V040, where + ": kind, mode, padding or reserved field out of range");
				continue;
			}
			if (p.mode == 0) {
				processLocal = true;
			}
			bool traced = p.kind == static_cast<uint8_t>(PointerKind::TracedAddress);
			if (traced) {
				if (p.provenance > 2 || (p.provenance == 2) != (p.mode == 0)) {
					report(DiagCode::V040, where + ": RawProcessValue exactly in mode 0");
				}
			} else if (p.provenance != 0) {
				report(DiagCode::V040, where + ": provenance applies to traced addresses only");
			}
			if (p.mode == 0 && p.nameId != 0) {
				report(DiagCode::V040, where + ": nameId is unused in mode 0");
			}
			if (p.mode == 1 && p.address != 0) {
				report(DiagCode::V040, where + ": address is unused in mode 1");
			}
			if (p.mode != 0 && !nameOk(p.nameId)) {
				report(DiagCode::V040, where + ": name index out of range");
			}
		}
		for (auto& c : m_.consts.records()) {
			processLocal |= c.provenance == 2;
		}
		if (processLocal != (m_.module.processToken != 0)) {
			report(DiagCode::V040, processLocal ? "a process-local module needs a processToken"
			                                    : "processToken is set on a module that is not process-local");
		}
	}

	void bindings() {
		std::set<uint32_t> names;
		for (uint32_t i = 0; i < m_.bindings.size(); ++i) {
			auto& b = m_.bindings[i];
			std::string where = "binding #" + std::to_string(i);
			if (!nameOk(b.nameId)) {
				report(DiagCode::V041, where + ": name index out of range");
			} else if (!names.insert(b.nameId).second) {
				report(DiagCode::V041, where + ": duplicate binding name");
			}
			if ((b.type >> 8) != 0 || (b.type > uint32_t(TypeId::Ptr) && b.type < 16)) {
				report(DiagCode::V036, where + ": invalid type field");
			}
			if (!isPow2(b.align)) {
				report(DiagCode::V041, where + ": alignment is not a power of two");
			}
		}
	}

	void data() {
		for (uint32_t i = 0; i < m_.data.size(); ++i) {
			auto& d = m_.data[i];
			std::string where = "data object #" + std::to_string(i);
			if (d.size == 0 || !isPow2(d.align) || d.align > 4096 || d.flags != 0) {
				report(DiagCode::V042, where + ": size, alignment or flags invalid");
				continue;
			}
			if (d.byteOffset % d.align != 0) {
				report(DiagCode::V042, where + ": byteOffset is not aligned");
			}
			if (uint64_t(d.byteOffset) + d.size > m_.dataBytes.size()) {
				report(DiagCode::V042, where + ": contents are out of bounds of dataBytes");
			}
		}
	}

	void allocas() {
		for (uint32_t i = 0; i < m_.allocas.size(); ++i) {
			auto& a = m_.allocas[i];
			if (a.size == 0 || !isPow2(a.align) || a.align > 4096) {
				report(DiagCode::V043, "alloca #" + std::to_string(i) + ": size or alignment invalid");
			}
		}
	}

	void functionAttrs() {
		const FunctionAttrRecord* prev = nullptr;
		for (uint32_t i = 0; i < m_.functionAttrs.size(); ++i) {
			auto& r = m_.functionAttrs[i];
			std::string where = "function attribute #" + std::to_string(i);
			if (r.function >= m_.functions.size() || !nameOk(r.keyNameId) || !nameOk(r.valueNameId) || r.pad != 0) {
				report(DiagCode::V044, where + ": index out of range or padding set");
				prev = nullptr;
				continue;
			}
			auto key = m_.name(r.keyNameId);
			auto dot = key.find('.');
			if (dot == std::string_view::npos || dot == 0 || dot + 1 == key.size()) {
				report(DiagCode::V044, where + ": key '" + std::string(key) + "' is not <plugin>.<key>");
			}
			if (prev != nullptr) {
				auto pk = m_.name(prev->keyNameId);
				bool ordered = prev->function < r.function || (prev->function == r.function && pk < key);
				if (!ordered) {
					report(DiagCode::V044, where + ": records are not sorted by (function, key) or a key repeats");
				}
			}
			prev = &r;
		}
	}

	void extensions() {
		for (uint32_t i = 0; i < m_.extensions.size(); ++i) {
			if (!nameOk(m_.extensions[i].nameId)) {
				report(DiagCode::V046, "extension #" + std::to_string(i) + ": name index out of range");
			}
		}
	}

private:
	const Module& m_;
	const VerifyOptions& o_;
	std::vector<Diagnostic>& out_;
};

} // namespace

void verifyModuleTables(const Module& m, const VerifyOptions& options, std::vector<Diagnostic>& out) {
	Checker c(m, options, out);
	// every name is read through its record, so the records come first
	for (uint32_t i = 0; i < m.names.size(); ++i) {
		if (uint64_t(m.names[i].offset) + m.names[i].length > m.nameBytes.size()) {
			c.report(DiagCode::V007, "name #" + std::to_string(i) + " is out of bounds of nameBytes");
			return;
		}
	}
	c.moduleRecord();
	// the other tables read signatures; stop early if they cannot be read safely
	if (!c.signatures()) {
		return;
	}
	c.functions();
	c.callees();
	c.imports();
	c.constants();
	c.pointers();
	c.bindings();
	c.data();
	c.allocas();
	c.functionAttrs();
	c.extensions();
}

std::vector<Diagnostic> verify(const Module& m, const VerifyOptions& options) {
	std::vector<Diagnostic> out;
	verifyModuleTables(m, options, out);
	if (out.empty()) {
		// function bodies refer to the tables, so they are read only once the tables are sound
		verifyFunctions(m, options, out);
	}
	return out;
}

} // namespace nautilus::nir
