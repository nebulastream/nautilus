#include "TestModules.hpp"
#include "nautilus/nir/Printer.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus::nir;

namespace {

/// The first example of 13-nir-spec.md §9.
Module divideFirst() {
	Module m;
	Builder b(m);
	TypeWord params[] = {test::tw(TypeId::Ptr), test::tw(TypeId::I64)};
	auto sig = m.internSignature(test::tw(TypeId::I64), params);
	auto zero = m.consts.intern(TypeId::I64, Provenance::Literal, 0);
	b.beginFunction(m.internName("divideFirst"), sig, Visibility::Exported, Level::SSA);
	b.beginBlock(2);
	auto p = b.emit(Op::BlockArg, TypeId::Ptr, 0, 0);
	auto n = b.emit(Op::BlockArg, TypeId::I64, 0, 1);
	auto c = b.emit(Op::Cmp, TypeId::Bool, 2, n, zero);
	uint32_t cond[] = {429496729, 0, 2, 1, n};
	b.emit(Op::CondBr, TypeId::Void, 0, c, 1, m.addExtra(cond));
	b.beginBlock();
	b.emit(Op::Ret, TypeId::Void, 0, zero);
	b.beginBlock(1);
	auto d = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	auto l = b.emit(Op::Load, TypeId::I64, 0, p);
	auto q = b.emit(Op::Div, TypeId::I64, 2, l, d, NO_CLEANUP);
	b.emit(Op::Ret, TypeId::Void, 0, q);
	b.endFunction();
	return m;
}

/// The second example: tables, a destructor and a landing pad.
Module withTables() {
	Module m;
	m.module.nameId = m.internName("nes.q1");
	auto p0 = m.internPointer(PointerRecord {0, 1, 0, 0, m.internName("process"), 0, 0});
	auto p1 = m.internPointer(PointerRecord {1, 1, 0, 0, m.internName("process$thunk"), 0, 0});
	auto p2 = m.internPointer(PointerRecord {2, 1, 0, 0, m.internName("Buffer::~Buffer"), 0, 0});
	TypeWord ptrParam[] = {test::tw(TypeId::Ptr)};
	auto sigI = m.internSignature(test::tw(TypeId::I64), ptrParam);
	auto sigV = m.internSignature(test::tw(TypeId::Void), ptrParam);
	auto c0 = m.internCallee(CalleeRecord {static_cast<uint8_t>(CalleeKind::External), {}, p0, sigI, 0});
	auto c1 = m.internCallee(CalleeRecord {static_cast<uint8_t>(CalleeKind::External), {}, p2, sigV, AttrNoUnwind});
	Builder b(m);
	b.beginFunction(m.internName("run"), sigI, Visibility::Exported, Level::SSA);
	b.addAlloca(16, 8);
	b.beginBlock(1);
	b.emit(Op::BlockArg, TypeId::Ptr, 0, 0);
	auto a = b.emit(Op::Alloca, TypeId::Ptr, 0, 0);
	uint32_t call0[] = {1, a, 1, p1};
	auto r = b.emit(Op::Call, TypeId::I64, 0, c0, 0, m.addExtra(call0));
	uint32_t call1[] = {1, a, NO_UNWIND, NONE};
	b.emit(Op::Call, TypeId::Void, 0, c1, 0, m.addExtra(call1));
	b.emit(Op::Ret, TypeId::Void, 0, r);
	b.beginBlock(0, BLOCK_LANDING_PAD);
	uint32_t call2[] = {1, a, NO_UNWIND, NONE};
	b.emit(Op::Call, TypeId::Void, 0, c1, 0, m.addExtra(call2));
	b.emit(Op::Resume, TypeId::Void, 0);
	b.endFunction();
	return m;
}

} // namespace

TEST_CASE("the printer reproduces the first example of the spec", "[nir][printer]") {
	auto m = divideFirst();
	REQUIRE(verify(m).empty());
	REQUIRE(print(m) == R"(nir 1.0

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
)");
}

TEST_CASE("the printer reproduces the second example of the spec", "[nir][printer]") {
	auto m = withTables();
	REQUIRE(print(m) == R"(nir 1.0
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
)");
}

TEST_CASE("the printer covers every opcode", "[nir][printer]") {
	Module m;
	test::buildAllSSA(m);
	test::buildPreSSA(m);
	auto text = print(m);
	INFO(text);
	for (auto name : {"add",      "sub",           "mul",      "div.u.checked", "rem.s",     "fneg",      "and",
	                  "or",       "not",           "band",     "bor",           "bxor",      "bnot",      "shl",
	                  "shr.u",    "cmp.lt.u",      "select",   "cast.sitofp",   "load",      "store",     "alloca",
	                  "call #c0", "call.indirect", "funcaddr", "bind.addr",     "data.addr", "ptr.const", "br ^b2",
	                  "condbr",   "ret",           "resume",   "assign",        "var",       "cleanup []"}) {
		INFO(name);
		REQUIRE(text.find(name) != std::string::npos);
	}
	// pre-SSA: void instructions print their id
	REQUIRE(text.find("[3] assign %1, %2") != std::string::npos);
	REQUIRE(text.find("func export pressa @pre") != std::string::npos);
}

TEST_CASE("constants print as the spec says", "[nir][printer]") {
	Module m;
	Builder b(m);
	auto sig = m.internSignature(test::tw(TypeId::Void), {});
	b.beginFunction(m.internName("c"), sig, Visibility::Internal, Level::SSA);
	b.beginBlock();
	auto neg = m.consts.intern(TypeId::I8, Provenance::Literal, 0xFF);
	auto t = m.consts.intern(TypeId::Bool, Provenance::Literal, 1);
	auto f = m.consts.intern(TypeId::F64, Provenance::AuditedInvariant, std::bit_cast<uint64_t>(1.5));
	auto nan = m.consts.intern(TypeId::F32, Provenance::Literal, 0xFFC00001u);
	auto inf = m.consts.intern(TypeId::F64, Provenance::RawProcessValue, std::bit_cast<uint64_t>(-HUGE_VAL));
	b.emit(Op::Add, TypeId::I8, 0, neg, neg);
	b.emit(Op::And, TypeId::Bool, 0, t, t);
	b.emit(Op::FNeg, TypeId::F64, 0, f);
	b.emit(Op::FNeg, TypeId::F32, 0, nan);
	b.emit(Op::FNeg, TypeId::F64, 0, inf);
	b.emit(Op::Ret, TypeId::Void, 0, NONE);
	b.endFunction();
	auto text = print(m);
	INFO(text);
	REQUIRE(text.find("add -1:i8, -1:i8") != std::string::npos);
	REQUIRE(text.find("and true:bool, true:bool") != std::string::npos);
	REQUIRE(text.find("fneg 1.5:f64[audited]") != std::string::npos);
	REQUIRE(text.find("fneg -nan(0x400001):f32") != std::string::npos);
	REQUIRE(text.find("fneg -inf:f64[raw]") != std::string::npos);
}

TEST_CASE("names and strings are escaped", "[nir][printer]") {
	Module m;
	Builder b(m);
	auto sig = m.internSignature(test::tw(TypeId::Void), {});
	b.beginFunction(m.internName("lib.math::hash"), sig, Visibility::Internal, Level::SSA);
	b.beginBlock();
	b.emit(Op::Ret, TypeId::Void, 0, NONE);
	b.endFunction();
	b.beginFunction(m.internName("plain_name.x$"), sig, Visibility::Internal, Level::SSA);
	b.beginBlock();
	b.emit(Op::Ret, TypeId::Void, 0, NONE);
	b.endFunction();
	m.module.nameId = m.internName("a\"b\\c\n");
	auto text = print(m);
	REQUIRE(text.find("module \"a\\\"b\\\\c\\x0a\"") != std::string::npos);
	REQUIRE(text.find("@\"lib.math::hash\"") != std::string::npos);
	REQUIRE(text.find("@plain_name.x$") != std::string::npos);
}
