#include "TestModules.hpp"
#include "nautilus/nir/Verifier.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus::nir;
using test::has;

namespace {

/// One block of a small control flow graph: calls that unwind to the listed pads, then a terminator.
struct Blk {
	enum Kind { Br, CondBr, Ret, Resume } kind;
	uint32_t t = 0, f = 0;
	std::vector<uint32_t> unwindsTo = {};
	bool pad = false;
};

/// A function f(bool c) with the given blocks; block 0 holds the parameter.
Module cfgModule(const std::vector<Blk>& blocks) {
	Module m;
	Builder b(m);
	TypeWord params[] = {test::tw(TypeId::Bool)};
	auto sig = m.internSignature(makeTypeWord(TypeId::Void), params);
	auto voidSig = m.internSignature(makeTypeWord(TypeId::Void), {});
	CalleeRecord g {};
	g.kind = static_cast<uint8_t>(CalleeKind::External);
	g.target = m.internPointer(PointerRecord {static_cast<uint8_t>(PointerKind::Callee), 0, 0, 0, 0, 0x1000, 0});
	g.signature = voidSig;
	auto callee = m.internCallee(g);
	auto thunk =
	    m.internPointer(PointerRecord {static_cast<uint8_t>(PointerKind::CaptureThunk), 0, 0, 0, 0, 0x2000, 0});
	b.beginFunction(m.internName("f"), sig, Visibility::Exported, Level::SSA);
	for (uint32_t i = 0; i < blocks.size(); ++i) {
		auto& k = blocks[i];
		b.beginBlock(i == 0 ? 1 : 0, k.pad ? BLOCK_LANDING_PAD : 0);
		if (i == 0) {
			b.emit(Op::BlockArg, TypeId::Bool, 0, 0);
		}
		for (auto u : k.unwindsTo) {
			uint32_t list[] = {0, u, thunk};
			b.emit(Op::Call, TypeId::Void, 0, callee, 0, m.addExtra(list));
		}
		uint32_t none[] = {0};
		switch (k.kind) {
		case Blk::Br:
			b.emit(Op::Br, TypeId::Void, 0, k.t, 0, m.addExtra(none));
			break;
		case Blk::CondBr: {
			uint32_t list[] = {NONE, 0, k.f, 0};
			b.emit(Op::CondBr, TypeId::Void, 0, 0, k.t, m.addExtra(list));
			break;
		}
		case Blk::Ret:
			b.emit(Op::Ret, TypeId::Void, 0, NONE);
			break;
		case Blk::Resume:
			b.emit(Op::Resume, TypeId::Void, 0);
			break;
		}
	}
	b.endFunction();
	m.module.processToken = 1;
	return m;
}

void requireValid(const Module& m) {
	auto d = verify(m);
	std::string all;
	for (auto& x : d) {
		all += x.str() + "\n";
	}
	INFO(all);
	REQUIRE(d.empty());
}

} // namespace

TEST_CASE("cfg: straight line, diamond and loop are valid", "[nir][verifier][flow]") {
	requireValid(cfgModule({{Blk::Ret}}));
	requireValid(cfgModule({{Blk::CondBr, 1, 2}, {Blk::Br, 3}, {Blk::Br, 3}, {Blk::Ret}}));
	requireValid(cfgModule({{Blk::Br, 1}, {Blk::CondBr, 2, 4}, {Blk::Br, 3}, {Blk::Br, 1}, {Blk::Ret}}));
}

TEST_CASE("NIR-V062 forward edges go forward", "[nir][verifier][flow]") {
	// b2 -> b1 goes backwards but b1 does not dominate b2
	auto m = cfgModule({{Blk::CondBr, 2, 1}, {Blk::Br, 3}, {Blk::Br, 1}, {Blk::Ret}});
	REQUIRE(has(verify(m), DiagCode::V062));
	// the same graph in reverse post-order is fine
	requireValid(cfgModule({{Blk::CondBr, 1, 2}, {Blk::Br, 3}, {Blk::Br, 3}, {Blk::Ret}}));
}

TEST_CASE("NIR-V062 irreducible regions are exempt", "[nir][verifier][flow]") {
	// {b1, b2} is entered from b0 twice and loops: irreducible
	requireValid(cfgModule({{Blk::CondBr, 1, 2}, {Blk::CondBr, 2, 3}, {Blk::CondBr, 1, 3}, {Blk::Ret}}));
}

TEST_CASE("NIR-V062 reducible loops are contiguous", "[nir][verifier][flow]") {
	// loop {b1, b3}, with b2 in between that is not part of it
	auto m = cfgModule({{Blk::Br, 1}, {Blk::CondBr, 3, 2}, {Blk::Ret}, {Blk::Br, 1}});
	REQUIRE(has(verify(m), DiagCode::V062));
	// b2 moved behind the loop
	requireValid(cfgModule({{Blk::Br, 1}, {Blk::CondBr, 2, 3}, {Blk::Br, 1}, {Blk::Ret}}));
}

TEST_CASE("NIR-V062 landing pads come last in order of first reference", "[nir][verifier][flow]") {
	requireValid(cfgModule({{Blk::Ret, 0, 0, {1, 2}}, {Blk::Resume, 0, 0, {}, true}, {Blk::Resume, 0, 0, {}, true}}));
	// referenced in the order b2, b1
	auto swapped = cfgModule({{Blk::Ret, 0, 0, {2, 1}}, {Blk::Resume, 0, 0, {}, true}, {Blk::Resume, 0, 0, {}, true}});
	REQUIRE(has(verify(swapped), DiagCode::V062));
	// a pad before a block that is not a pad
	auto early = cfgModule({{Blk::CondBr, 2, 2, {1}}, {Blk::Resume, 0, 0, {}, true}, {Blk::Ret}});
	REQUIRE(has(verify(early), DiagCode::V062));
}

TEST_CASE("NIR-V064 pad edges", "[nir][verifier][flow]") {
	// a pad may branch to another pad
	requireValid(cfgModule({{Blk::Ret, 0, 0, {1}}, {Blk::Br, 2, 0, {}, true}, {Blk::Resume, 0, 0, {}, true}}));
	// pad cycle
	auto cycle = cfgModule({{Blk::Ret, 0, 0, {1}}, {Blk::Br, 2, 0, {}, true}, {Blk::Br, 1, 0, {}, true}});
	REQUIRE(has(verify(cycle), DiagCode::V064));
}

TEST_CASE("NIR-V061 dominance of uses", "[nir][verifier][flow]") {
	test::negative(DiagCode::V061, [](Module& m) { test::inst(m, Op::Add).a = 6; }); // used before its definition
	test::negative(DiagCode::V061, [](Module& m) {
		// block 1's branch passes a value of block 2
		m.extra[test::inst(m, Op::Br).c + 1] = 34;
	});
}

namespace {
/// f(ptr p): a call that unwinds to a pad which calls a destructor on an object.
Module padUse(bool objectDefinedAfterCall) {
	Module m;
	Builder b(m);
	TypeWord params[] = {test::tw(TypeId::Ptr)};
	auto sig = m.internSignature(makeTypeWord(TypeId::Void), params);
	CalleeRecord g {};
	g.kind = static_cast<uint8_t>(CalleeKind::External);
	g.target = m.internPointer(PointerRecord {static_cast<uint8_t>(PointerKind::Callee), 0, 0, 0, 0, 0x1000, 0});
	g.signature = m.internSignature(makeTypeWord(TypeId::Void), {});
	auto callee = m.internCallee(g);
	CalleeRecord d {};
	d.kind = static_cast<uint8_t>(CalleeKind::External);
	d.target = m.internPointer(PointerRecord {static_cast<uint8_t>(PointerKind::Destructor), 0, 0, 0, 0, 0x4000, 0});
	d.signature = sig;
	d.attrs = AttrNoUnwind;
	auto dtor = m.internCallee(d);
	auto thunk =
	    m.internPointer(PointerRecord {static_cast<uint8_t>(PointerKind::CaptureThunk), 0, 0, 0, 0, 0x2000, 0});
	b.beginFunction(m.internName("f"), sig, Visibility::Exported, Level::SSA);
	b.beginBlock(1);
	auto p = b.emit(Op::BlockArg, TypeId::Ptr, 0, 0);
	uint32_t call[] = {0, 1, thunk};
	b.emit(Op::Call, TypeId::Void, 0, callee, 0, m.addExtra(call));
	Ref object = p;
	if (objectDefinedAfterCall) {
		object = b.emit(Op::PtrConst, TypeId::Ptr, 0, NONE);
	}
	b.emit(Op::Ret, TypeId::Void, 0, NONE);
	b.beginBlock(0, BLOCK_LANDING_PAD);
	uint32_t dcall[] = {1, object, NO_UNWIND, NONE};
	b.emit(Op::Call, TypeId::Void, 0, dtor, 0, m.addExtra(dcall));
	b.emit(Op::Resume, TypeId::Void, 0);
	b.endFunction();
	m.module.processToken = 1;
	return m;
}
} // namespace

TEST_CASE("NIR-V061 values used in a pad are defined before the throwing instruction", "[nir][verifier][flow]") {
	requireValid(padUse(false));
	auto m = padUse(true);
	REQUIRE(has(verify(m), DiagCode::V061));
}

TEST_CASE("NIR-V051 pre-SSA definitions reach every use", "[nir][verifier][flow]") {
	auto build = [](bool varInBranch) {
		Module m;
		Builder b(m);
		TypeWord params[] = {test::tw(TypeId::I64)};
		auto sig = m.internSignature(test::tw(TypeId::I64), params);
		auto zero = m.consts.intern(TypeId::I64, Provenance::Literal, 0);
		b.beginFunction(m.internName("f"), sig, Visibility::Exported, Level::PreSSA);
		b.beginBlock();
		auto x = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
		auto c = b.emit(Op::Cmp, TypeId::Bool, 0, x, zero);
		Ref v = NONE;
		if (!varInBranch) {
			v = b.emit(Op::Var, TypeId::I64, 0, x);
		}
		uint32_t cond[] = {NONE, 0, 2, 0};
		b.emit(Op::CondBr, TypeId::Void, 0, c, 1, m.addExtra(cond));
		uint32_t none[] = {0};
		b.beginBlock();
		if (varInBranch) {
			v = b.emit(Op::Var, TypeId::I64, 0, x);
		}
		b.emit(Op::Br, TypeId::Void, 0, 3, 0, m.addExtra(none));
		b.beginBlock();
		b.emit(Op::Br, TypeId::Void, 0, 3, 0, m.addExtra(none));
		b.beginBlock();
		b.emit(Op::Ret, TypeId::Void, 0, v);
		b.endFunction();
		return m;
	};
	requireValid(build(false));
	REQUIRE(has(verify(build(true)), DiagCode::V051));
}
