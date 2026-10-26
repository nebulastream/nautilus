#include "TestModules.hpp"
#include "nautilus/nir/Canonicalize.hpp"
#include "nautilus/nir/Parser.hpp"
#include "nautilus/nir/Printer.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus::nir;

namespace {

Module parseOrDie(const std::string& text) {
	auto r = parse(text);
	std::string all;
	if (r.error) {
		all += r.error->str() + "\n";
	}
	for (auto& d : r.verifierDiagnostics) {
		all += d.str() + "\n";
	}
	INFO(all);
	REQUIRE(r.ok());
	return std::move(*r.module);
}

const char* const programA = R"(nir 1.0
module "one"
pointer #p0 = callee symbol "ext_b"
pointer #p1 = callee symbol "ext_a"
callee #c0 = external #p0 : (i64) -> i64 attrs(noUnwind)
callee #c1 = external #p1 : (i64) -> i64 attrs(noUnwind)
callee #c2 = internal @helper : (i64) -> i64

func export @zeta : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c1(%0)
  %2 = call #c0(%1)
  %3 = call #c2(%2)
  %4 = add %3, 42:i64
  ret %4
}

func @helper : (i64) -> i64 {
^b0(%0: i64):
  %1 = mul %0, 2:i64
  ret %1
}

func export @alpha : () -> i64 {
^b0:
  ret 7:i64
}
)";

// The same program with other table orders, duplicates, unused entries, another function order and another name.
const char* const programB = R"(nir 1.0
module "two"
binding #b0 = "unused" : i64 size 8 align 8
pointer #p0 = callee symbol "ext_a"
pointer #p1 = callee symbol "never_used"
pointer #p2 = callee symbol "ext_b"
callee #c0 = external #p2 : (i64) -> i64 attrs(noUnwind)
callee #c1 = external #p1 : (i64) -> i64 attrs(noUnwind)
callee #c2 = external #p0 : (i64) -> i64 attrs(noUnwind)
callee #c3 = internal @helper : (i64) -> i64
callee #c4 = external #p0 : (i64) -> i64 attrs(noUnwind)

func @unreferenced : () -> void {
^b0:
  ret
}

func @helper : (i64) -> i64 {
^b0(%0: i64):
  %1 = mul %0, 2:i64
  ret %1
}

func export @alpha : () -> i64 {
^b0:
  ret 7:i64
}

func export @zeta : (i64) -> i64 {
^b0(%0: i64):
  %1 = call #c4(%0)
  %2 = call #c0(%1)
  %3 = call #c3(%2)
  %4 = add %3, 42:i64
  ret %4
}
)";

CanonOptions anonymous() {
	CanonOptions o;
	o.anonymous = true;
	return o;
}

} // namespace

TEST_CASE("structurally equal modules have identical canonical sections", "[nir][canonical]") {
	auto a = parseOrDie(programA);
	auto b = parseOrDie(programB);
	REQUIRE_FALSE(sectionsEqual(a, b));
	auto ca = canonicalize(a, anonymous());
	auto cb = canonicalize(b, anonymous());
	REQUIRE(sectionsEqual(ca, cb));
	// the module name is part of the names section unless the module is canonicalized as anonymous
	REQUIRE_FALSE(sectionsEqual(canonicalize(a), canonicalize(b)));
}

TEST_CASE("canonicalization drops unused entries, interns duplicates and orders functions", "[nir][canonical]") {
	auto c = canonicalize(parseOrDie(programB), anonymous());
	REQUIRE(verify(c).empty());
	// exported functions by name, then the internal functions they reach; `unreferenced` is gone
	REQUIRE(c.functions.size() == 3);
	REQUIRE(c.name(c.functions[0].nameId) == "alpha");
	REQUIRE(c.name(c.functions[1].nameId) == "zeta");
	REQUIRE(c.name(c.functions[2].nameId) == "helper");
	REQUIRE(c.bindings.empty());
	REQUIRE(c.pointers.size() == 2); // never_used is dropped, the duplicate callee c4 is c2
	REQUIRE(c.callees.size() == 3);  // ext_a, ext_b, helper
	// pointers in scan order: zeta calls ext_a first
	REQUIRE(c.name(c.pointers[0].nameId) == "ext_a");
	REQUIRE(c.name(c.pointers[1].nameId) == "ext_b");
	// names in scan order: alpha, zeta, ext_a, ext_b, helper; nameBytes without padding or sharing
	std::string names;
	for (uint32_t i = 0; i < c.names.size(); ++i) {
		names += std::string(c.name(i)) + ",";
	}
	REQUIRE(names == "alpha,zeta,ext_a,ext_b,helper,");
	REQUIRE(std::string(c.nameBytes.begin(), c.nameBytes.end()) == "alphazetaext_aext_bhelper");
	// constants in scan order: alpha's 7, then 42 and 2
	REQUIRE(c.consts.at(0).bits == 7);
	REQUIRE(c.consts.at(1).bits == 42);
	REQUIRE(c.consts.at(2).bits == 2);
}

TEST_CASE("canonicalization is idempotent", "[nir][canonical]") {
	for (auto* text : {programA, programB}) {
		auto once = canonicalize(parseOrDie(text), anonymous());
		auto twice = canonicalize(once, anonymous());
		REQUIRE(sectionsEqual(once, twice));
	}
	Module m;
	test::buildAllSSA(m);
	m.module.processToken = 1;
	auto once = canonicalize(m);
	REQUIRE(verify(once).empty());
	REQUIRE(sectionsEqual(once, canonicalize(once)));
}

TEST_CASE("extra lists are written without gaps: signatures first, then per instruction", "[nir][canonical]") {
	auto c = canonicalize(parseOrDie(programA), anonymous());
	uint32_t words = 0;
	for (auto& s : c.signatures) {
		if (s.paramCount) {
			REQUIRE(s.paramTypes == words);
			words += s.paramCount;
		}
	}
	for (auto& f : c.functions) {
		for (auto& inst : c.functionInsts(static_cast<uint32_t>(&f - c.functions.data()))) {
			if (inst.op == static_cast<uint16_t>(Op::Call)) {
				REQUIRE(inst.c == words);
				words += 1 + c.extra[inst.c] + 2;
			}
		}
	}
	REQUIRE(words == c.extra.size());
}

TEST_CASE("the whole conformance-style module canonicalizes to a valid module", "[nir][canonical]") {
	Module m;
	test::buildAllSSA(m);
	m.module.processToken = 1;
	auto c = canonicalize(m);
	auto d = verify(c);
	std::string all;
	for (auto& x : d) {
		all += x.str() + "\n";
	}
	INFO(all);
	REQUIRE(d.empty());
	REQUIRE(c.insts.size() == m.insts.size());
	REQUIRE(c.allocas.size() == 1);
}

TEST_CASE("canonicalization refuses pre-SSA modules", "[nir][canonical]") {
	Module m;
	test::buildPreSSA(m);
	REQUIRE_THROWS_AS(canonicalize(m), std::invalid_argument);
}

TEST_CASE("unreferenced allocas are dropped and slots renumbered", "[nir][canonical]") {
	auto m = parseOrDie(R"(nir 1.0
func export @f : () -> ptr {
  alloca 8 align 8
  alloca 16 align 16
  alloca 4 align 4
^b0:
  %0 = alloca 2
  %1 = alloca 0
  %2 = alloca 2
  ret %1
}
)");
	auto c = canonicalize(m);
	REQUIRE(c.allocas.size() == 2);
	REQUIRE(c.allocas[0].size == 4); // slot 2 is visited first
	REQUIRE(c.allocas[1].size == 8);
	REQUIRE(c.insts[0].a == 0);
	REQUIRE(c.insts[1].a == 1);
	REQUIRE(c.insts[2].a == 0);
}

TEST_CASE("function string attributes are sorted by key", "[nir][canonical]") {
	// the verifier rejects unsorted records, so the module is parsed without it
	ParseOptions noVerify;
	noVerify.verify = false;
	auto parsed = parse(R"(nir 1.0
func export @f : () -> void {
  attr "b.key" = "2"
  attr "a.key" = "1"
^b0:
  ret
}
)",
	                    noVerify);
	REQUIRE(parsed.ok());
	REQUIRE_FALSE(verify(*parsed.module).empty());
	auto c = canonicalize(*parsed.module);
	REQUIRE(c.functionAttrs.size() == 2);
	REQUIRE(c.name(c.functionAttrs[0].keyNameId) == "a.key");
	REQUIRE(c.name(c.functionAttrs[1].keyNameId) == "b.key");
	REQUIRE(verify(c).empty());
}
