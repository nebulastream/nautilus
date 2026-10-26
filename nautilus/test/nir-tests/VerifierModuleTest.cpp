#include "TestModules.hpp"
#include "nautilus/nir/Verifier.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus::nir;

using test::has;
using test::negative;
using test::validModule;

TEST_CASE("a valid module has no module-level diagnostics", "[nir][verifier]") {
	auto m = validModule();
	auto d = verify(m);
	for (auto& x : d) {
		INFO(x.str());
	}
	REQUIRE(d.empty());
}

TEST_CASE("diagnostic codes", "[nir][verifier]") {
	REQUIRE(codeName(DiagCode::V001) == "NIR-V001");
	REQUIRE(codeName(DiagCode::P006) == "NIR-P006");
	REQUIRE(codeRule(DiagCode::V010) == "§6.1.6");
	Diagnostic d {DiagCode::V005, 1, 3, 2, "bad"};
	REQUIRE(d.str("f") == "NIR-V005: function 'f', %3, ^b2: bad");
	Diagnostic e {DiagCode::V030, NONE, NONE, NONE, "x"};
	REQUIRE(e.str() == "NIR-V030: x");
	// every code is listed once, with non-empty text
	std::set<std::string> seen;
#define NIR_DIAG(id, code, rule, check)                                                                                \
	REQUIRE(seen.insert(std::string(codeName(DiagCode::id))).second);                                                  \
	REQUIRE(codeName(DiagCode::id) == code);                                                                           \
	REQUIRE_FALSE(codeCheck(DiagCode::id).empty());
#include "nautilus/nir/Diagnostics.def"
#undef NIR_DIAG
	REQUIRE(seen.size() == 78);
}

TEST_CASE("NIR-V030 module record", "[nir][verifier]") {
	negative(DiagCode::V030, [](Module& m) { m.module.flags = 1; });
	negative(DiagCode::V030, [](Module& m) { m.module.nameId = 9999; });
}

TEST_CASE("NIR-V031 function records and names", "[nir][verifier]") {
	negative(DiagCode::V031, [](Module& m) { m.functions[0].level = 2; });
	negative(DiagCode::V031, [](Module& m) { m.functions[1].nameId = m.functions[0].nameId; });
	negative(DiagCode::V031, [](Module& m) { m.functions[0].nameId = m.internName(""); });
}

TEST_CASE("NIR-V032 callee signatures", "[nir][verifier]") {
	negative(DiagCode::V032, [](Module& m) {
		CalleeRecord c {};
		c.kind = static_cast<uint8_t>(CalleeKind::Internal);
		c.target = 1;
		c.signature = m.functions[0].signature; // function 1 has another signature
		m.internCallee(c);
	});
}

TEST_CASE("NIR-V033 internal callee attributes", "[nir][verifier]") {
	negative(DiagCode::V033, [](Module& m) {
		CalleeRecord c {};
		c.kind = static_cast<uint8_t>(CalleeKind::Internal);
		c.target = 0;
		c.signature = m.functions[0].signature;
		c.attrs = AttrNoUnwind;
		m.internCallee(c);
	});
	auto m = validModule();
	CalleeRecord c {};
	c.kind = static_cast<uint8_t>(CalleeKind::Internal);
	c.target = 0;
	c.signature = m.functions[0].signature;
	m.internCallee(c);
	REQUIRE(verify(m).empty());
}

TEST_CASE("NIR-V034 reserved attribute bits", "[nir][verifier]") {
	negative(DiagCode::V034, [](Module& m) { m.functions[0].attrs = 1u << 7; });
	negative(DiagCode::V034, [](Module& m) { m.callees[0].attrs = 1u << 8; });
}

TEST_CASE("NIR-V035 C ABI types", "[nir][verifier]") {
	auto ext = static_cast<TypeId>(16);
	auto build = [&](Module& m) {
		TypeWord p[] = {makeTypeWord(ext)};
		auto sig = m.internSignature(makeTypeWord(TypeId::Void), p);
		m.functions[0].signature = sig;
	};
	negative(DiagCode::V035, build);
	// with a registered C ABI type the module is valid
	auto m = validModule();
	build(m);
	VerifyOptions o;
	o.extensionHasCabi = [](TypeId) {
		return true;
	};
	REQUIRE_FALSE(has(verify(m, o), DiagCode::V035));
}

TEST_CASE("NIR-V036 type words", "[nir][verifier]") {
	negative(DiagCode::V036, [](Module& m) {
		TypeWord p[] = {makeTypeWord(TypeId::I8)}; // i8 needs an extension kind
		m.functions[0].signature = m.internSignature(makeTypeWord(TypeId::Void), p);
	});
	negative(DiagCode::V036, [](Module& m) {
		TypeWord p[] = {makeTypeWord(TypeId::Void)}; // void parameter
		m.functions[0].signature = m.internSignature(makeTypeWord(TypeId::Void), p);
	});
	negative(DiagCode::V036, [](Module& m) { m.bindings[0].type = 1u << 9; });
	negative(DiagCode::V036, [](Module& m) { m.bindings[0].type = 12; });
}

TEST_CASE("NIR-V037 and NIR-V038 imports", "[nir][verifier]") {
	auto addImport = [](Module& m, const char* mod, const char* fn) {
		m.addImport(ImportRecord {m.internName(mod), m.internName(fn), m.functions[0].signature, 0});
	};
	auto m = validModule();
	addImport(m, "lib", "f");
	REQUIRE(verify(m).empty());
	VerifyOptions deserialized;
	deserialized.deserialized = true;
	auto d = verify(m, deserialized);
	REQUIRE(has(d, DiagCode::V038));
	negative(DiagCode::V037, [&](Module& m2) {
		addImport(m2, "lib", "f");
		addImport(m2, "lib", "f");
	});
	negative(DiagCode::V037, [&](Module& m2) {
		m2.module.nameId = m2.internName("self");
		addImport(m2, "self", "f");
	});
}

TEST_CASE("NIR-V039 constants", "[nir][verifier]") {
	negative(DiagCode::V039, [](Module& m) { m.consts.intern(TypeId::Bool, Provenance::Literal, 2); });
	negative(DiagCode::V039, [](Module& m) { m.consts.intern(TypeId::I8, Provenance::Literal, 0x100); });
	negative(DiagCode::V039, [](Module& m) { m.consts.intern(TypeId::Ptr, Provenance::Literal, 0); });
	negative(DiagCode::V039, [](Module& m) { m.consts.intern(TypeId::Void, Provenance::Literal, 0); });
	negative(DiagCode::V039, [](Module& m) { m.consts.intern(TypeId::I32, static_cast<Provenance>(3), 0); });
	auto m = validModule();
	m.consts.intern(TypeId::I8, Provenance::Literal, 0xFF);
	m.consts.intern(TypeId::F32, Provenance::Literal, 0xFFFFFFFFull);
	REQUIRE(verify(m).empty());
}

TEST_CASE("NIR-V040 pointers and the process token", "[nir][verifier]") {
	negative(DiagCode::V040, [](Module& m) { m.module.processToken = 0; });
	negative(DiagCode::V040, [](Module& m) {
		// traced address in mode 0 must be RawProcessValue
		m.internPointer(PointerRecord {3, 0, 0, 0, 0, 0x5000, 0});
	});
	negative(DiagCode::V040, [](Module& m) { m.internPointer(PointerRecord {5, 0, 0, 0, 0, 1, 0}); });
	negative(DiagCode::V040, [](Module& m) { m.internPointer(PointerRecord {0, 1, 0, 0, 9999, 0, 0}); });
	negative(DiagCode::V040, [](Module& m) { m.internPointer(PointerRecord {0, 0, 1, 0, 0, 1, 0}); });
	// a portable-only module needs no token
	Module m;
	test::buildPreSSA(m);
	m.internPointer(PointerRecord {3, 1, 0, 0, m.internName("sym"), 0, 0});
	REQUIRE(verify(m).empty());
	m.module.processToken = 7;
	REQUIRE(has(verify(m), DiagCode::V040));
}

TEST_CASE("NIR-V041 bindings", "[nir][verifier]") {
	negative(DiagCode::V041, [](Module& m) { m.addBinding(m.bindings[0].nameId, TypeId::Void, 8, 8); });
	negative(DiagCode::V041, [](Module& m) { m.bindings[0].align = 3; });
}

TEST_CASE("NIR-V042 data objects", "[nir][verifier]") {
	negative(DiagCode::V042, [](Module& m) { m.data[0].size = 0; });
	negative(DiagCode::V042, [](Module& m) { m.data[0].align = 8192; });
	negative(DiagCode::V042, [](Module& m) { m.data[0].size = 1000; });
	negative(DiagCode::V042, [](Module& m) { m.data[0].flags = 1; });
	negative(DiagCode::V042, [](Module& m) {
		std::byte b[1] = {};
		m.internData(b, 1);
		m.data.back().align = 4;
		m.data.back().byteOffset = 5;
	});
}

TEST_CASE("NIR-V043 allocas", "[nir][verifier]") {
	negative(DiagCode::V043, [](Module& m) { m.allocas[0].size = 0; });
	negative(DiagCode::V043, [](Module& m) { m.allocas[0].align = 6; });
	negative(DiagCode::V043, [](Module& m) { m.allocas[0].align = 8192; });
}

TEST_CASE("NIR-V044 function attributes", "[nir][verifier]") {
	auto attr = [](Module& m, uint32_t fn, const char* key) {
		m.functionAttrs.push_back(FunctionAttrRecord {fn, m.internName(key), m.internName("v"), 0});
	};
	auto m = validModule();
	attr(m, 0, "gpu.kernel");
	attr(m, 0, "gpu.threads");
	attr(m, 1, "a.b");
	REQUIRE(verify(m).empty());
	negative(DiagCode::V044, [&](Module& m2) { attr(m2, 0, "nokey"); });
	negative(DiagCode::V044, [&](Module& m2) { attr(m2, 0, ".x"); });
	negative(DiagCode::V044, [&](Module& m2) {
		attr(m2, 0, "gpu.z");
		attr(m2, 0, "gpu.a"); // unsorted
	});
	negative(DiagCode::V044, [&](Module& m2) {
		attr(m2, 0, "gpu.a");
		attr(m2, 0, "gpu.a"); // repeated
	});
	negative(DiagCode::V044, [&](Module& m2) { attr(m2, 99, "gpu.a"); });
}

TEST_CASE("NIR-V045 destructor callees", "[nir][verifier]") {
	auto addDtor = [](Module& m, uint32_t sig) {
		CalleeRecord c {};
		c.kind = static_cast<uint8_t>(CalleeKind::External);
		c.target =
		    m.internPointer(PointerRecord {static_cast<uint8_t>(PointerKind::Destructor), 0, 0, 0, 0, 0x9000, 0});
		c.signature = sig;
		m.internCallee(c);
	};
	auto m = validModule();
	TypeWord p[] = {makeTypeWord(TypeId::Ptr)};
	addDtor(m, m.internSignature(makeTypeWord(TypeId::Void), p));
	REQUIRE(verify(m).empty());
	negative(DiagCode::V045, [&](Module& m2) { addDtor(m2, m2.functions[0].signature); });
}

TEST_CASE("NIR-V046 extension records", "[nir][verifier]") {
	negative(DiagCode::V046, [](Module& m) { m.extensions.push_back(ExtensionRecord {9999, 0}); });
	auto m = validModule();
	m.extensions.push_back(ExtensionRecord {m.internName("std.x@1"), 0});
	REQUIRE(verify(m).empty());
}

TEST_CASE("NIR-V007 and NIR-V008 table indices and extra bounds", "[nir][verifier]") {
	negative(DiagCode::V007, [](Module& m) { m.functions[0].signature = 999; });
	negative(DiagCode::V007, [](Module& m) { m.callees[0].signature = 999; });
	negative(DiagCode::V007, [](Module& m) { m.callees[0].target = 999; });
	negative(DiagCode::V008, [](Module& m) { m.signatures[0].paramTypes = 100000; });
}
