#pragma once

#include "nautilus/nir/Builder.hpp"
#include "nautilus/nir/Verifier.hpp"
#include <catch2/catch_all.hpp>
#include <set>

namespace nautilus::nir::test {

inline TypeWord tw(TypeId t) {
	switch (t) {
	case TypeId::Bool:
		return makeTypeWord(t, ExtKind::Zext);
	case TypeId::I8:
	case TypeId::I16:
		return makeTypeWord(t, ExtKind::Sext);
	default:
		return makeTypeWord(t);
	}
}

/// Records the opcodes used so a test can check that a module uses every opcode of Opcodes.def.
inline std::set<uint16_t> opcodesUsed(const Module& m) {
	std::set<uint16_t> s;
	for (auto& i : m.insts) {
		s.insert(i.op);
	}
	return s;
}

/// SSA function `all`: uses every SSA-level opcode. Returns the function index.
inline uint32_t buildAllSSA(Module& m) {
	Builder b(m);
	TypeWord params[] = {tw(TypeId::I64), tw(TypeId::F64), tw(TypeId::Ptr), tw(TypeId::Bool)};
	auto sig = m.internSignature(tw(TypeId::I64), params);
	TypeWord cparams[] = {tw(TypeId::I64)};
	auto csig = m.internSignature(tw(TypeId::I64), cparams);
	CalleeRecord ext {};
	ext.kind = static_cast<uint8_t>(CalleeKind::External);
	ext.target = m.internPointer(PointerRecord {static_cast<uint8_t>(PointerKind::Callee), 0, 0, 0, 0, 0x1000, 0});
	ext.signature = csig;
	auto callee = m.internCallee(ext);
	auto ptrIdx =
	    m.internPointer(PointerRecord {static_cast<uint8_t>(PointerKind::TracedAddress), 0, 2, 0, 0, 0x2000, 0});
	auto bind = m.addBinding(m.internName("state"), TypeId::Void, 16, 8);
	std::byte bytes[4] = {};
	auto dat = m.internData(bytes, 4);
	auto c1 = m.consts.intern(TypeId::I64, Provenance::Literal, 1);
	auto f1 = m.consts.intern(TypeId::F64, Provenance::Literal, 0x3FF0000000000000ull);
	auto t = m.consts.intern(TypeId::Bool, Provenance::Literal, 1);

	auto f = b.beginFunction(m.internName("all"), sig, Visibility::Exported, Level::SSA);
	b.addAlloca(16, 8);
	b.beginBlock(4);
	auto x = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	auto d = b.emit(Op::BlockArg, TypeId::F64, 0, 1);
	auto p = b.emit(Op::BlockArg, TypeId::Ptr, 0, 2);
	auto q = b.emit(Op::BlockArg, TypeId::Bool, 0, 3);
	auto s = b.emit(Op::Add, TypeId::I64, 0, x, c1);
	s = b.emit(Op::Sub, TypeId::I64, 0, s, c1);
	s = b.emit(Op::Mul, TypeId::I64, 0, s, x);
	auto dv = b.emit(Op::Div, TypeId::I64, 0b11, s, x, NO_CLEANUP);
	auto rm = b.emit(Op::Rem, TypeId::I64, 0, dv, c1);
	auto fn = b.emit(Op::FNeg, TypeId::F64, 0, d);
	(void) f1;
	auto a1 = b.emit(Op::And, TypeId::Bool, 0, q, t);
	auto a2 = b.emit(Op::Or, TypeId::Bool, 0, a1, q);
	auto a3 = b.emit(Op::Not, TypeId::Bool, 0, a2);
	auto b1 = b.emit(Op::BAnd, TypeId::I64, 0, rm, x);
	b1 = b.emit(Op::BOr, TypeId::I64, 0, b1, x);
	b1 = b.emit(Op::BXor, TypeId::I64, 0, b1, x);
	b1 = b.emit(Op::BNot, TypeId::I64, 0, b1);
	b1 = b.emit(Op::Shl, TypeId::I64, 0, b1, c1);
	b1 = b.emit(Op::Shr, TypeId::I64, 1, b1, c1);
	auto cm = b.emit(Op::Cmp, TypeId::Bool, 2 | 8, b1, x);
	auto sel = b.emit(Op::Select, TypeId::I64, 0, cm, b1, x);
	auto ca = b.emit(Op::Cast, TypeId::F64, 3, sel);
	auto al = b.emit(Op::Alloca, TypeId::Ptr, 0, 0);
	b.emit(Op::Store, TypeId::Void, 0, al, ca);
	auto ld = b.emit(Op::Load, TypeId::F64, 0, p);
	auto thunk =
	    m.internPointer(PointerRecord {static_cast<uint8_t>(PointerKind::CaptureThunk), 0, 0, 0, 0, 0x3000, 0});
	uint32_t cargs[] = {1, sel, 3, thunk}; // unwinds to the landing pad ^b3
	auto cl = b.emit(Op::Call, TypeId::I64, 0, callee, 0, m.addExtra(cargs));
	uint32_t ciargs[] = {1, cl, NO_UNWIND, NONE, 0};
	auto fa = b.emit(Op::FuncAddr, TypeId::Ptr, 0, callee);
	auto ci = b.emit(Op::CallIndirect, TypeId::I64, 0, fa, csig, m.addExtra(ciargs));
	auto ba = b.emit(Op::BindAddr, TypeId::Ptr, 0, bind);
	auto da = b.emit(Op::DataAddr, TypeId::Ptr, 0, dat);
	auto pc = b.emit(Op::PtrConst, TypeId::Ptr, 0, ptrIdx);
	(void) fn;
	(void) a3;
	(void) ld;
	(void) ba;
	(void) da;
	(void) pc;
	uint32_t condArgs[] = {NONE, 1, ci, 1, 1, ci};
	b.emit(Op::CondBr, TypeId::Void, 0, cm, 1, m.addExtra(condArgs));
	b.beginBlock(1);
	auto r = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	uint32_t brArgs[] = {1, r};
	b.emit(Op::Br, TypeId::Void, 0, 2, 0, m.addExtra(brArgs));
	b.beginBlock(1);
	auto r2 = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	b.emit(Op::Ret, TypeId::Void, 0, r2);
	b.beginBlock(0, BLOCK_LANDING_PAD);
	b.emit(Op::Resume, TypeId::Void, 0);
	b.endFunction();
	return f;
}

/// Pre-SSA function `pre`: var, assign and a cleanup list. Returns the function index.
inline uint32_t buildPreSSA(Module& m) {
	Builder b(m);
	TypeWord params[] = {tw(TypeId::I64)};
	auto sig = m.internSignature(tw(TypeId::I64), params);
	auto c1 = m.consts.intern(TypeId::I64, Provenance::Literal, 1);
	auto f = b.beginFunction(m.internName("pre"), sig, Visibility::Exported, Level::PreSSA);
	b.beginBlock();
	auto x = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	auto v = b.emit(Op::Var, TypeId::I64, 0, x);
	uint32_t cleanup[] = {0};
	auto dv = b.emit(Op::Div, TypeId::I64, 0b11, v, c1, m.addExtra(cleanup));
	b.emit(Op::Assign, TypeId::Void, 0, v, dv);
	uint32_t brArgs[] = {0};
	b.emit(Op::Br, TypeId::Void, 0, 1, 0, m.addExtra(brArgs));
	b.beginBlock();
	uint32_t brArgs2[] = {0};
	b.emit(Op::Br, TypeId::Void, 0, 2, 0, m.addExtra(brArgs2));
	b.beginBlock();
	b.emit(Op::Ret, TypeId::Void, 0, v);
	b.endFunction();
	return f;
}

inline Module validModule() {
	Module m;
	buildAllSSA(m);
	buildPreSSA(m);
	m.module.processToken = 42; // the test module has ProcessLocal pointers
	return m;
}

inline bool has(const std::vector<Diagnostic>& d, DiagCode c) {
	return std::any_of(d.begin(), d.end(), [&](auto& x) { return x.code == c; });
}

/// The first instruction of function `f` with opcode `op`.
inline Inst& inst(Module& m, Op op, uint32_t f = 0) {
	for (auto& i : m.functionInsts(f)) {
		if (i.op == static_cast<uint16_t>(op)) {
			return i;
		}
	}
	throw std::logic_error("no such instruction");
}

/// The module is valid, and after `corrupt` the verifier reports code `c`.
template <class F>
void negative(DiagCode c, F corrupt) {
	auto m = validModule();
	{
		auto d = verify(m);
		std::string all;
		for (auto& x : d) {
			all += x.str() + "\n";
		}
		INFO(all);
		REQUIRE(d.empty());
	}
	corrupt(m);
	auto d = verify(m);
	std::string all(codeName(c));
	all += " expected; got:\n";
	for (auto& x : d) {
		all += x.str() + "\n";
	}
	INFO(all);
	REQUIRE(has(d, c));
}

} // namespace nautilus::nir::test
