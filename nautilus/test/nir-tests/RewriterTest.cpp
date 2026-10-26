#include "TestModules.hpp"
#include "nautilus/nir/Rewriter.hpp"
#include "nautilus/nir/Uses.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus::nir;

namespace {
void requireEqual(const Module& a, const Module& b) {
	REQUIRE(a.insts == b.insts);
	REQUIRE(a.blocks == b.blocks);
	REQUIRE(a.functions == b.functions);
	REQUIRE(a.allocas == b.allocas);
	REQUIRE(a.callees == b.callees);
	REQUIRE(a.pointers == b.pointers);
	REQUIRE(a.consts.records().size() == b.consts.records().size());
}
} // namespace

TEST_CASE("a module uses every opcode and survives an identity rewrite", "[nir][rewriter]") {
	Module m;
	test::buildAllSSA(m);
	test::buildPreSSA(m);
	auto used = test::opcodesUsed(m);
#define NIR_OP(OPNAME, CODE, TEXT, KA, KB, KC, FL) REQUIRE(used.count(CODE) == 1);
#include "nautilus/nir/Opcodes.def"
#undef NIR_OP

	Module out;
	Rewriter rw(m, out);
	rw.rewriteAll();
	REQUIRE(out.functions.size() == 2);
	REQUIRE(out.insts.size() == m.insts.size());
	// extra lists are rebuilt, so only the instruction fields that are not extra offsets must match
	for (size_t i = 0; i < m.insts.size(); ++i) {
		REQUIRE(out.insts[i].op == m.insts[i].op);
		REQUIRE(out.insts[i].a == m.insts[i].a);
		REQUIRE(out.insts[i].b == m.insts[i].b);
	}
	REQUIRE(out.blocks.size() == m.blocks.size());
	REQUIRE(out.callees == m.callees);
	REQUIRE(out.pointers == m.pointers);
	REQUIRE(out.signatures == m.signatures);
	REQUIRE(out.name(out.functions[0].nameId) == "all");

	// a second rewrite of the copy is a fixed point
	Module again;
	Rewriter rw2(out, again);
	rw2.rewriteAll();
	requireEqual(out, again);
	REQUIRE(again.extra == out.extra);
}

TEST_CASE("the rewriter compacts erased instructions and renumbers operands", "[nir][rewriter]") {
	Module m;
	Builder b(m);
	TypeWord params[] = {test::tw(TypeId::I64)};
	auto sig = m.internSignature(test::tw(TypeId::I64), params);
	auto c1 = m.consts.intern(TypeId::I64, Provenance::Literal, 1);
	b.beginFunction(m.internName("f"), sig, Visibility::Exported, Level::SSA);
	b.beginBlock(1);
	auto x = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	auto dead = b.emit(Op::Mul, TypeId::I64, 0, x, x);
	auto s = b.emit(Op::Add, TypeId::I64, 0, x, c1);
	auto t = b.emit(Op::Add, TypeId::I64, 0, s, s);
	b.emit(Op::Ret, TypeId::Void, 0, t);
	b.endFunction();

	Module out;
	Rewriter rw(m, out);
	rw.rewriteFunction(0, [&](Rewriter& r, uint32_t id, const Inst&) {
		if (id == dead) {
			r.erase(id);
			return true;
		}
		return false;
	});
	REQUIRE(out.functions[0].instCount == 4);
	REQUIRE(out.blocks[0].count == 4);
	REQUIRE(rw.forward()[dead] == NONE);
	REQUIRE(rw.forward()[s] == 1);
	REQUIRE(out.insts[2].a == 1);
	REQUIRE(out.insts[2].b == 1);
	REQUIRE(out.insts[3].a == 2);
}

TEST_CASE("replace forwards uses to a constant or an earlier value", "[nir][rewriter]") {
	Module m;
	Builder b(m);
	TypeWord params[] = {test::tw(TypeId::I64)};
	auto sig = m.internSignature(test::tw(TypeId::I64), params);
	auto zero = m.consts.intern(TypeId::I64, Provenance::Literal, 0);
	b.beginFunction(m.internName("f"), sig, Visibility::Exported, Level::SSA);
	b.beginBlock(1);
	auto x = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	auto add0 = b.emit(Op::Add, TypeId::I64, 0, x, zero);
	auto y = b.emit(Op::Mul, TypeId::I64, 0, add0, add0);
	b.emit(Op::Ret, TypeId::Void, 0, y);
	b.endFunction();

	Module out;
	Rewriter rw(m, out);
	rw.rewriteFunction(0, [&](Rewriter& r, uint32_t id, const Inst& inst) {
		if (id == add0) {
			r.replace(id, r.map(inst.a));
			return true;
		}
		return false;
	});
	REQUIRE(out.functions[0].instCount == 3);
	REQUIRE(out.insts[1].a == 0);
	REQUIRE(out.insts[1].b == 0);
}

TEST_CASE("using an erased value is an error", "[nir][rewriter]") {
	Module m;
	Builder b(m);
	TypeWord params[] = {test::tw(TypeId::I64)};
	auto sig = m.internSignature(test::tw(TypeId::I64), params);
	b.beginFunction(m.internName("f"), sig, Visibility::Exported, Level::SSA);
	b.beginBlock(1);
	auto x = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	b.emit(Op::Ret, TypeId::Void, 0, x);
	b.endFunction();
	Module out;
	Rewriter rw(m, out);
	REQUIRE_THROWS_AS(rw.rewriteFunction(0,
	                                     [&](Rewriter& r, uint32_t id, const Inst&) {
		                                     if (id == x) {
			                                     r.erase(id);
			                                     return true;
		                                     }
		                                     return false;
	                                     }),
	                  std::logic_error);
}

TEST_CASE("pre-SSA rewrite keeps block lists and cleanup lists", "[nir][rewriter]") {
	Module m;
	auto f = test::buildPreSSA(m);
	Module out;
	Rewriter rw(m, out);
	rw.rewriteFunction(f, [&](Rewriter& r, uint32_t id, const Inst&) {
		// erase nothing; check that ids are stable under identity
		(void) r;
		(void) id;
		return false;
	});
	REQUIRE(out.functions[0].blockCount == 3);
	auto blocks = out.functionBlocks(0);
	REQUIRE(blocks[0].count == 5);
	REQUIRE(blocks[1].count == 1);
	REQUIRE(out.extra[blocks[0].first + 3] == 3);
}

TEST_CASE("use table lists the users of each value", "[nir][uses]") {
	Module m;
	auto f = test::buildAllSSA(m);
	UseTable uses(m, f);
	Builder dummy(m);
	// block arg 0 (x) is used by add, mul, div, band... at least twice
	REQUIRE(uses.useCount(0) >= 5);
	auto users = uses.users(0);
	REQUIRE(std::is_sorted(users.begin(), users.end()));
	// the sum of all use counts equals the number of value operands
	uint32_t total = 0;
	for (uint32_t i = 0; i < m.functions[f].instCount; ++i) {
		total += uses.useCount(i);
	}
	uint32_t counted = 0;
	for (auto& inst : m.functionInsts(f)) {
		forEachRef(inst, m.extra.data(), Level::SSA, [&](Ref r) { counted += isValueRef(r); });
	}
	REQUIRE(total == counted);
}
