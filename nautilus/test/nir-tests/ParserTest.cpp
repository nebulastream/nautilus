#include "TestModules.hpp"
#include "nautilus/nir/Parser.hpp"
#include "nautilus/nir/Printer.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus::nir;

namespace {

ParseResult parseOk(const std::string& text) {
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
	return r;
}

DiagCode errorOf(const std::string& text) {
	auto r = parse(text);
	INFO(text);
	REQUIRE(r.error.has_value());
	return r.error->code;
}

const char* const example1 = R"(nir 1.0

func export @divideFirst : (ptr, i64) -> i64 {
^b0(%0: ptr, %1: i64):
  %2 = cmp.lt.s %1, 0:i64
  condbr %2, ^b1, ^b2(%1) prob 429496729
^b1:
  ret 0:i64
^b2(%5: i64):
  %6 = load %0 : i64
  %7 = div.s.checked %6, %5 unwind caller
  ret %7
}
)";

const char* const example2 = R"(nir 1.0
module "nes.q1"
pointer #p0 = callee symbol "process"
pointer #p1 = thunk symbol "process$thunk"
pointer #p2 = destructor symbol "Buffer::~Buffer"
callee #c0 = external #p0 : (ptr) -> i64
callee #c1 = external #p2 : (ptr) -> void attrs(noUnwind)

func export @run : (ptr) -> i64 {
  alloca 16 align 8
^b0(%0: ptr):
  %1 = alloca 0
  %2 = call #c0(%1) unwind ^b1 thunk #p1
  call #c1(%1)
  ret %2
^b1 pad:
  call #c1(%1)
  resume
}
)";

} // namespace

TEST_CASE("the spec examples parse, verify and print back unchanged", "[nir][parser]") {
	for (auto* text : {example1, example2}) {
		auto r = parseOk(text);
		REQUIRE(print(*r.module) == text);
	}
}

TEST_CASE("print(parse(print(m))) equals print(m)", "[nir][parser][roundtrip]") {
	Module m;
	test::buildAllSSA(m);
	test::buildPreSSA(m);
	m.module.nameId = m.internName("all");
	auto text = print(m);
	auto r = parseOk(text);
	REQUIRE(print(*r.module) == text);
	// the parsed module has the same instructions, blocks and functions
	REQUIRE(r.module->functions.size() == m.functions.size());
	REQUIRE(r.module->insts.size() == m.insts.size());
	REQUIRE(r.module->blocks.size() == m.blocks.size());
	REQUIRE(r.module->callees.size() == m.callees.size());
	REQUIRE(r.module->callees[0].kind == m.callees[0].kind);
	REQUIRE(r.module->pointers == m.pointers);
	REQUIRE(r.module->module.processToken != 0); // the module has ProcessLocal pointers
}

TEST_CASE("pre-SSA modules keep ids, blocks and block lists", "[nir][parser][roundtrip]") {
	Module m;
	test::buildPreSSA(m);
	auto r = parseOk(print(m));
	REQUIRE(r.module->insts.size() == m.insts.size());
	for (size_t i = 0; i < m.insts.size(); ++i) {
		auto& x = r.module->insts[i];
		auto& y = m.insts[i];
		REQUIRE(x.op == y.op);
		REQUIRE(x.type == y.type);
		REQUIRE(x.flags == y.flags);
		REQUIRE(x.a == y.a);
	}
	auto& a = r.module->blocks;
	auto& b = m.blocks;
	REQUIRE(a.size() == b.size());
	for (size_t i = 0; i < a.size(); ++i) {
		REQUIRE(a[i].count == b[i].count);
		for (uint32_t k = 0; k < a[i].count; ++k) {
			REQUIRE(r.module->extra[a[i].first + k] == m.extra[b[i].first + k]);
		}
	}
}

TEST_CASE("pre-SSA ids may be out of order across blocks", "[nir][parser]") {
	auto r = parseOk(R"(nir 1.0
func pressa @f : (i64) -> i64 {
^b0(%0: i64):
  [4] br ^b1
^b1:
  %1 = var %0
  [2] assign %1, %0
  [3] ret %1
}
)");
	REQUIRE(r.module->insts.size() == 5);
	REQUIRE(r.module->insts[4].op == static_cast<uint16_t>(Op::Br));
}

TEST_CASE("literals are interpreted by their type", "[nir][parser]") {
	auto r = parseOk(R"(nir 1.0
func @f : () -> void {
^b0:
  %0 = add 255:i8, 0x7f:i8
  %1 = add -128:i8, 1:i8
  %2 = add 1.5:f64, 0x1.8p+1:f64
  %3 = add 3:f32, -0:f32
  %4 = fneg inf:f64
  %5 = fneg -nan(0x1):f32
  %6 = and true:bool, false:bool
  %7 = add 1e-3:f64, 1:f64
  %8 = add 18446744073709551615:i64, -9223372036854775808:i64
  ret
}
)");
	auto& m = *r.module;
	auto bits = [&](Ref ref) {
		return m.consts.record(ref).bits;
	};
	REQUIRE(bits(m.insts[0].a) == 0xFF);
	REQUIRE(bits(m.insts[1].a) == 0x80);
	REQUIRE(bits(m.insts[2].b) == std::bit_cast<uint64_t>(3.0));
	REQUIRE(bits(m.insts[3].b) == 0x80000000u); // -0.0f
	REQUIRE(bits(m.insts[4].a) == 0x7FF0000000000000ull);
	REQUIRE(bits(m.insts[5].a) == 0xFF800001u);
	REQUIRE(bits(m.insts[8].a) == ~0ull);
	// printing normalizes
	REQUIRE(print(m).find("add -1:i8, 127:i8") != std::string::npos);
}

TEST_CASE("NIR-P001 lexical errors", "[nir][parser]") {
	REQUIRE(errorOf("nir 1.0\n$$") == DiagCode::P002); // $ starts a name
	REQUIRE(errorOf("nir 1.0 \x01") == DiagCode::P001);
	REQUIRE(errorOf("nir 1.0\nmodule \"abc") == DiagCode::P001);
	REQUIRE(errorOf("nir 1.0\nmodule \"a\\q\"") == DiagCode::P001);
	REQUIRE(errorOf("nir 1.0\nfunc @f : () -> void {\n^b0:\n %0 = add 1:i64, 1:i64 \n  ret\n}\n^") == DiagCode::P001);
	REQUIRE(errorOf("nir 1.0\ndata #d0 align 1 = x\"0\"") == DiagCode::P001);
	REQUIRE(errorOf("nir 1.0\nfunc @f : () -> void {\n^b0:\n  %0 = add 1:i64[bogus], 1:i64\n  ret\n}\n") ==
	        DiagCode::P001);
}

TEST_CASE("NIR-P002 syntax errors", "[nir][parser]") {
	REQUIRE(errorOf("") == DiagCode::P002);
	REQUIRE(errorOf("nir 2.0") == DiagCode::P002);
	REQUIRE(errorOf("nir 1.1") == DiagCode::P002);
	REQUIRE(errorOf("nir 1.99999999999999999999") == DiagCode::P002); // found by the fuzzer: stoi threw
	REQUIRE(errorOf("nir 1.") == DiagCode::P002);
	REQUIRE(errorOf("nir 1.x") == DiagCode::P002);
	REQUIRE(errorOf("nir 1.0\nfunc @f : () -> void {\n^b0:\n  frobnicate\n}\n") == DiagCode::P002);
	REQUIRE(errorOf("nir 1.0\nfunc @f : () -> void {\n^b0:\n  ret\n") == DiagCode::P002);
	REQUIRE(errorOf("nir 1.0\nfunc @f : () -> void {\n}\n") == DiagCode::P002);
	REQUIRE(errorOf("nir 1.0\ncallee #c0 = internal @f : ()") == DiagCode::P002);
	REQUIRE(errorOf("nir 1.0\nfunc @f : () -> void {\n^b0:\n  %0 = ret\n}\n") == DiagCode::P002);
}

TEST_CASE("NIR-P003 undefined or out-of-order references", "[nir][parser]") {
	REQUIRE(errorOf("nir 1.0\ncallee #c1 = external #p0 : () -> void") == DiagCode::P003);
	REQUIRE(errorOf("nir 1.0\nfunc @f : () -> void {\n^b0:\n  br ^b5\n}\n") == DiagCode::P003);
	REQUIRE(errorOf("nir 1.0\nfunc @f : () -> void {\n^b1:\n  ret\n}\n") == DiagCode::P003);
	REQUIRE(errorOf("nir 1.0\nfunc @f : () -> void {\n^b0:\n  call #c3()\n  ret\n}\n") == DiagCode::P003);
	REQUIRE(errorOf("nir 1.0\ncallee #c0 = internal @missing : () -> void") == DiagCode::P003);
	REQUIRE(errorOf("nir 1.0\nfunc @f : () -> void {\n^b0:\n  %0 = add %9, 1:i64\n  ret\n}\n") == DiagCode::P003);
	REQUIRE(errorOf("nir 1.0\nfunc @f : () -> void {\n^b0:\n  %0 = ptr.const #p4\n  ret\n}\n") == DiagCode::P003);
}

TEST_CASE("NIR-P004 printed ids match positions", "[nir][parser]") {
	REQUIRE(errorOf("nir 1.0\nfunc @f : () -> void {\n^b0:\n  %5 = add 1:i64, 1:i64\n  ret\n}\n") == DiagCode::P004);
	REQUIRE(errorOf("nir 1.0\nfunc pressa @f : () -> void {\n^b0:\n  [1] ret\n}\n") == DiagCode::P004);
	REQUIRE(errorOf("nir 1.0\nfunc pressa @f : () -> void {\n^b0:\n  [0] ret\n  [0] ret\n}\n") == DiagCode::P004);
	auto ok = parse("nir 1.0\nfunc @f : () -> void {\n^b0:\n  [0] ret\n}\n");
	REQUIRE(ok.ok()); // [N] is accepted for void SSA instructions if it equals the position
}

TEST_CASE("NIR-P006 literals out of range", "[nir][parser]") {
	auto body = [](const std::string& lit) {
		return "nir 1.0\nfunc @f : () -> void {\n^b0:\n  %0 = add " + lit + ", " + lit + "\n  ret\n}\n";
	};
	REQUIRE(errorOf(body("256:i8")) == DiagCode::P006);
	REQUIRE(errorOf(body("-129:i8")) == DiagCode::P006);
	REQUIRE(errorOf(body("18446744073709551616:i64")) == DiagCode::P006);
	REQUIRE(errorOf(body("1.5:i32")) == DiagCode::P006);
	REQUIRE(errorOf(body("1:bool")) == DiagCode::P006);
	REQUIRE(errorOf(body("0:ptr")) == DiagCode::P006);
	REQUIRE(errorOf(body("nan(0x0):f64")) == DiagCode::P006);
	REQUIRE(errorOf(body("nan(0x1000000):f32")) == DiagCode::P006);
	REQUIRE(errorOf(body("abc:f64")) == DiagCode::P002);
}

TEST_CASE("the verifier runs on parsed modules", "[nir][parser]") {
	auto r = parse("nir 1.0\nfunc @f : () -> i64 {\n^b0:\n  ret\n}\n");
	REQUIRE_FALSE(r.error);
	REQUIRE_FALSE(r.ok());
	REQUIRE(r.verifierDiagnostics[0].code == DiagCode::V010);
	ParseOptions o;
	o.verify = false;
	REQUIRE(parse("nir 1.0\nfunc @f : () -> i64 {\n^b0:\n  ret\n}\n", o).ok());
}

TEST_CASE("comments and whitespace are insignificant", "[nir][parser]") {
	auto r = parseOk("nir 1.0 ; header\nmodule \"m\"\n\nfunc @f : () -> void { ^b0: ret } ; done\n");
	REQUIRE(print(*r.module) == "nir 1.0\nmodule \"m\"\n\nfunc @f : () -> void {\n^b0:\n  ret\n}\n");
}
