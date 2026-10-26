#include "nautilus/nir/Builder.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus::nir;

namespace {
uint32_t i64Sig(Module& m, int params) {
	std::vector<TypeWord> p(params, makeTypeWord(TypeId::I64));
	return m.internSignature(makeTypeWord(TypeId::I64), p);
}
} // namespace

TEST_CASE("names, signatures, callees and pointers are interned", "[nir][module]") {
	Module m;
	REQUIRE(m.internName("f") == m.internName("f"));
	REQUIRE(m.internName("f") != m.internName("g"));
	REQUIRE(m.name(m.internName("hello")) == "hello");
	REQUIRE(m.internName("") == m.internName(""));

	auto s1 = i64Sig(m, 2);
	REQUIRE(i64Sig(m, 2) == s1);
	REQUIRE(i64Sig(m, 1) != s1);
	REQUIRE(m.signatures[s1].paramCount == 2);
	REQUIRE(m.paramType(m.signatures[s1], 1) == makeTypeWord(TypeId::I64));
	auto s0 = i64Sig(m, 0);
	REQUIRE(m.signatures[s0].paramTypes == 0);

	CalleeRecord c {};
	c.kind = static_cast<uint8_t>(CalleeKind::Internal);
	c.signature = s1;
	REQUIRE(m.internCallee(c) == m.internCallee(c));
	c.attrs = AttrNoUnwind;
	REQUIRE(m.internCallee(c) == 1);

	PointerRecord p {};
	p.kind = static_cast<uint8_t>(PointerKind::TracedAddress);
	p.address = 0x1000;
	REQUIRE(m.internPointer(p) == m.internPointer(p));
	p.address = 0x2000;
	REQUIRE(m.internPointer(p) == 1);
}

TEST_CASE("data objects are aligned and deduplicated", "[nir][module]") {
	Module m;
	std::byte a[3] = {std::byte {1}, std::byte {2}, std::byte {3}};
	std::byte b[8] = {};
	auto d0 = m.internData(a, 1);
	auto d1 = m.internData(b, 8);
	REQUIRE(m.internData(a, 1) == d0);
	REQUIRE(m.data[d1].byteOffset % 8 == 0);
	REQUIRE(m.data[d1].byteOffset == 8);
	REQUIRE(m.data[d1].size == 8);
}

TEST_CASE("builder: SSA function with blocks and use counts", "[nir][builder]") {
	Module m;
	Builder b(m);
	auto sig = i64Sig(m, 2);
	auto f = b.beginFunction(m.internName("add"), sig, Visibility::Exported, Level::SSA);
	b.beginBlock(2);
	auto x = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	auto y = b.emit(Op::BlockArg, TypeId::I64, 0, 1);
	auto s = b.emit(Op::Add, TypeId::I64, 0, x, y);
	auto one = m.consts.intern(TypeId::I64, Provenance::Literal, 1);
	auto t = b.emit(Op::Add, TypeId::I64, 0, s, one);
	b.emit(Op::Ret, TypeId::Void, 0, t);
	REQUIRE(b.useCount(x) == 1);
	REQUIRE(b.useCount(s) == 1);
	REQUIRE(b.useCount(t) == 1);
	b.endFunction();

	REQUIRE(m.functions[f].instCount == 5);
	REQUIRE(m.functions[f].blockCount == 1);
	REQUIRE(m.functionBlocks(f)[0].first == 0);
	REQUIRE(m.functionBlocks(f)[0].count == 5);
	REQUIRE(m.functionBlocks(f)[0].argCount == 2);
	REQUIRE(m.functionInsts(f)[3].op == static_cast<uint16_t>(Op::Add));
}

TEST_CASE("builder: two functions are contiguous", "[nir][builder]") {
	Module m;
	Builder b(m);
	auto sig = i64Sig(m, 0);
	for (int i = 0; i < 2; ++i) {
		b.beginFunction(m.internName(i ? "g" : "f"), sig, Visibility::Internal, Level::SSA);
		b.beginBlock();
		b.emit(Op::Ret, TypeId::Void, 0, m.consts.intern(TypeId::I64, Provenance::Literal, i));
		b.endFunction();
	}
	REQUIRE(m.functions[1].firstInst == 1);
	REQUIRE(m.functions[1].firstBlock == 1);
	REQUIRE(m.functions[0].instCount == 1);
}

TEST_CASE("builder: pre-SSA blocks list their instruction ids in extra", "[nir][builder]") {
	Module m;
	Builder b(m);
	auto sig = i64Sig(m, 1);
	auto f = b.beginFunction(m.internName("f"), sig, Visibility::Exported, Level::PreSSA);
	b.beginBlock();
	auto x = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	b.beginBlock();
	auto v = b.emit(Op::Var, TypeId::I64, 0, x);
	b.beginBlock(); // block 2 is created before 1 is finished in recording order
	b.emit(Op::Ret, TypeId::Void, 0, v);
	b.endFunction();
	REQUIRE(m.functions[f].blockCount == 3);
	auto blocks = m.functionBlocks(f);
	REQUIRE(blocks[0].count == 1);
	REQUIRE(m.extra[blocks[0].first] == x);
	REQUIRE(m.extra[blocks[1].first] == v);
	REQUIRE(blocks[2].count == 1);
	REQUIRE(blocks[0].argCount == 0);
}

TEST_CASE("builder counts uses in extra lists", "[nir][builder]") {
	Module m;
	Builder b(m);
	auto sig = i64Sig(m, 1);
	CalleeRecord c {};
	c.kind = static_cast<uint8_t>(CalleeKind::External);
	c.signature = sig;
	auto callee = m.internCallee(c);
	b.beginFunction(m.internName("f"), sig, Visibility::Exported, Level::SSA);
	b.beginBlock(1);
	auto x = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	uint32_t args[] = {1, x, NO_UNWIND, NONE};
	auto call = b.emit(Op::Call, TypeId::I64, 0, callee, 0, m.addExtra(args));
	REQUIRE(b.useCount(x) == 1);
	uint32_t brArgs[] = {2, x, call};
	b.beginBlock(2);
	b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	b.emit(Op::BlockArg, TypeId::I64, 0, 1);
	b.emit(Op::Ret, TypeId::Void, 0, NONE);
	b.beginBlock(0);
	b.emit(Op::Br, TypeId::Void, 0, 1, 0, m.addExtra(brArgs));
	REQUIRE(b.useCount(x) == 2);
	REQUIRE(b.useCount(call) == 1);
	b.endFunction();
}
