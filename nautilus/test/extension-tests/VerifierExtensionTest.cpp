// The verifier's checks of extension instructions, intrinsic calls and extension types in native signatures
// (13-nir-spec.md §5.11, §5.7, §4.5; plan M1.7). The registry is process-wide state, so these tests live in this binary.
#include "../nir-tests/TestModules.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::ext {
struct ExtensionRegistryTestAccess {
	static void reset();
};
} // namespace nautilus::ext

using namespace nautilus;
using namespace nautilus::ext;
using namespace nautilus::nir;

namespace {

void thunkFn() {
}
void fallbackFn() {
}

/// Registers: test.add (a, b values; result of operand 0), test.splat (a value, b imm), test.mask (a value, c const),
/// test.shuffle (a value, list: prefix [value, imm], group [type]), test.vec (a type without cabi), test.vec8 (with cabi).
struct Fixture {
	uint16_t add, splat, constOp, shuffle, hooked;
	uint8_t vec, vecC;
	Fixture() {
		ExtensionRegistryTestAccess::reset();
		auto& r = ExtensionRegistry::instance();
		TypeInfo t;
		t.name = "test.vec";
		t.size = 16;
		t.align = 16;
		vec = r.registerType(t);
		t.name = "test.vecc";
		t.cabi = "__m128";
		vecC = r.registerType(t);

		ExtOpInfo o;
		o.hasExpansion = true;
		o.name = "test.add";
		o.shape.a = o.shape.b = FieldKind::Value;
		o.sig.operands = 2;
		o.sig.resultFromOperand = 0;
		add = r.registerOp(o);

		o = ExtOpInfo {};
		o.hasExpansion = true;
		o.name = "test.splat";
		o.shape.a = FieldKind::Value;
		o.shape.b = FieldKind::Imm;
		o.sig.operands = 1;
		o.sig.result = TypeId::I64;
		splat = r.registerOp(o);

		o = ExtOpInfo {};
		o.hasExpansion = true;
		o.name = "test.constop";
		o.shape.a = FieldKind::Value;
		o.shape.c = FieldKind::Const;
		o.sig.operands = 2;
		o.sig.result = TypeId::I64;
		constOp = r.registerOp(o);

		o = ExtOpInfo {};
		o.hasExpansion = true;
		o.name = "test.shuffle";
		o.shape.c = FieldKind::List;
		o.shape.prefix = {WordKind::Value, WordKind::Imm};
		o.shape.group = {WordKind::Value, WordKind::Type};
		o.sig.operands = 1; // checked below against the actual count
		o.sig.result = TypeId::I64;
		shuffle = r.registerOp(o);

		o = ExtOpInfo {};
		o.hasExpansion = true;
		o.name = "test.hooked";
		o.shape.a = FieldKind::Value;
		o.sig.operands = 1;
		o.sig.result = TypeId::I64;
		o.verify = [](const Module&, const Inst& inst) -> std::string { return inst.flags == 7 ? "flags 7 are reserved" : ""; };
		hooked = r.registerOp(o);
	}
};

/// f(i64) -> i64 with one extension instruction emitted by `body`, which returns its value; then `ret`.
template <class F>
Module moduleWith(F&& body) {
	Module m;
	Builder b(m);
	TypeWord params[] = {test::tw(TypeId::I64)};
	auto sig = m.internSignature(test::tw(TypeId::I64), params);
	b.beginFunction(m.internName("f"), sig, Visibility::Exported, Level::SSA);
	b.beginBlock(1);
	auto x = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	auto v = body(m, b, x);
	b.emit(Op::Ret, TypeId::Void, 0, v);
	b.endFunction();
	return m;
}

bool has(const Module& m, DiagCode c) {
	return test::has(verify(m), c);
}

std::string all(const Module& m) {
	std::string s;
	for (auto& d : verify(m)) {
		s += d.str() + "\n";
	}
	return s;
}

} // namespace

TEST_CASE("a valid extension instruction verifies", "[verifier][extension]") {
	Fixture f;
	auto m = moduleWith([&](Module&, Builder& b, Ref x) { return b.emit(Inst {f.add, static_cast<uint8_t>(TypeId::I64), 0, x, x, 0}); });
	INFO(all(m));
	REQUIRE(verify(m).empty());
	// the result type follows operand 0
	auto bad = moduleWith([&](Module&, Builder& b, Ref x) { return b.emit(Inst {f.add, static_cast<uint8_t>(TypeId::F64), 0, x, x, 0}); });
	REQUIRE(has(bad, DiagCode::V071));
}

TEST_CASE("NIR-V003 an unregistered extension opcode", "[verifier][extension]") {
	Fixture f;
	auto m = moduleWith([&](Module&, Builder& b, Ref x) { return b.emit(Inst {400, static_cast<uint8_t>(TypeId::I64), 0, x, 0, 0}); });
	REQUIRE(has(m, DiagCode::V003));
}

TEST_CASE("NIR-V070 operand shape: fields", "[verifier][extension]") {
	Fixture f;
	auto i64 = static_cast<uint8_t>(TypeId::I64);
	// an unused field must be 0
	REQUIRE(has(moduleWith([&](Module&, Builder& b, Ref x) { return b.emit(Inst {f.splat, i64, 0, x, 0, 5}); }), DiagCode::V070));
	// an imm field may hold anything
	REQUIRE(verify(moduleWith([&](Module&, Builder& b, Ref x) { return b.emit(Inst {f.splat, i64, 0, x, 0xFFFFFFF0u, 0}); })).empty());
	// a value field needs a valid reference
	REQUIRE(has(moduleWith([&](Module&, Builder& b, Ref) { return b.emit(Inst {f.splat, i64, 0, 999, 0, 0}); }), DiagCode::V005));
	// a const field must be a constant
	REQUIRE(has(moduleWith([&](Module&, Builder& b, Ref x) { return b.emit(Inst {f.constOp, i64, 0, x, 0, x}); }), DiagCode::V070));
	REQUIRE(verify(moduleWith([&](Module& m, Builder& b, Ref x) {
		        return b.emit(Inst {f.constOp, i64, 0, x, 0, m.consts.intern(TypeId::I64, Provenance::Literal, 3)});
	        })).empty());
}

TEST_CASE("NIR-V070 operand shape: payload lists", "[verifier][extension]") {
	Fixture f;
	auto i64 = static_cast<uint8_t>(TypeId::I64);
	// prefix [value, imm] then groups of [value, type]; n = 2 + 2k
	auto build = [&](std::vector<uint32_t> words, auto&& patch) {
		return moduleWith([&](Module& m, Builder& b, Ref x) {
			for (auto& w : words) {
				if (w == 0xAAAA) {
					w = x;
				}
			}
			patch(m, words);
			std::vector<uint32_t> list {static_cast<uint32_t>(words.size())};
			list.insert(list.end(), words.begin(), words.end());
			auto inst = Inst {f.shuffle, i64, 0, 0, 0, m.addExtra(list)};
			return b.emit(inst);
		});
	};
	auto nothing = [](Module&, std::vector<uint32_t>&) {};
	auto types = static_cast<uint32_t>(TypeId::I64);
	// valid: n = 2, and n = 4 with one group
	auto two = build({0xAAAA, 7}, nothing);
	INFO(all(two));
	// the signature counts value operands, so two groups change the count: this module has operands = 1 + k
	REQUIRE_FALSE(has(two, DiagCode::V070));
	REQUIRE_FALSE(has(two, DiagCode::V008));
	// n = 3 does not fit prefix 2 + k * 2
	REQUIRE(has(build({0xAAAA, 7, 0xAAAA}, nothing), DiagCode::V070));
	// a value word that is not a value
	REQUIRE(has(build({999, 7}, nothing), DiagCode::V005));
	// a type word with high bits, and an unregistered type id
	REQUIRE(has(build({0xAAAA, 7, 0xAAAA, 0x100 | types}, nothing), DiagCode::V070));
	REQUIRE(has(build({0xAAAA, 7, 0xAAAA, 200}, nothing), DiagCode::V070));
	// a registered extension type and a core type are valid type words
	REQUIRE_FALSE(has(build({0xAAAA, 7, 0xAAAA, f.vec}, nothing), DiagCode::V070));
	REQUIRE_FALSE(has(build({0xAAAA, 7, 0xAAAA, types}, nothing), DiagCode::V070));
	// a list that runs past extra (patched in: the Builder reads lists while it counts uses)
	auto past = moduleWith([&](Module& m, Builder& b, Ref) {
		uint32_t list[] = {2, 0, 7};
		return b.emit(Inst {f.shuffle, i64, 0, 0, 0, m.addExtra(list)});
	});
	past.insts[1].c = 100000;
	REQUIRE(has(past, DiagCode::V008));
}

TEST_CASE("NIR-V070 payload pointer words", "[verifier][extension]") {
	Fixture f;
	ExtensionRegistryTestAccess::reset(); // a fresh registry with one op that has a pointer word
	auto& r = ExtensionRegistry::instance();
	ExtOpInfo o;
	o.hasExpansion = true;
	o.name = "test.ptrpayload";
	o.shape.c = FieldKind::List;
	o.shape.prefix = {WordKind::Pointer};
	o.sig.result = TypeId::I64;
	auto op = r.registerOp(o);
	auto build = [&](uint8_t pointerKind) {
		return moduleWith([&](Module& m, Builder& b, Ref) {
			auto p = m.internPointer(PointerRecord {pointerKind, 0, 0, 0, 0, 0x10, 0});
			uint32_t list[] = {1, p};
			return b.emit(Inst {op, static_cast<uint8_t>(TypeId::I64), 0, 0, 0, m.addExtra(list)});
		});
	};
	auto good = build(static_cast<uint8_t>(PointerKind::ExtensionPayload));
	good.module.processToken = 1;
	INFO(all(good));
	REQUIRE_FALSE(has(good, DiagCode::V070));
	auto bad = build(static_cast<uint8_t>(PointerKind::Callee)); // a valid entry, but not a payload address
	bad.module.processToken = 1;
	REQUIRE(has(bad, DiagCode::V070));
}

TEST_CASE("NIR-V071 signature and the plugin's verify hook", "[verifier][extension]") {
	Fixture f;
	auto i64 = static_cast<uint8_t>(TypeId::I64);
	// test.splat takes one operand; the result is i64
	REQUIRE(verify(moduleWith([&](Module&, Builder& b, Ref x) { return b.emit(Inst {f.splat, i64, 0, x, 0, 0}); })).empty());
	REQUIRE(has(moduleWith([&](Module&, Builder& b, Ref x) { return b.emit(Inst {f.splat, static_cast<uint8_t>(TypeId::F64), 0, x, 0, 0}); }),
	            DiagCode::V071));
	// the hook runs after the core checks and its message names the plugin's operation
	auto hooked = moduleWith([&](Module&, Builder& b, Ref x) { return b.emit(Inst {f.hooked, i64, 7, x, 0, 0}); });
	REQUIRE(has(hooked, DiagCode::V071));
	REQUIRE(all(hooked).find("test.hooked: flags 7 are reserved") != std::string::npos);
	REQUIRE(verify(moduleWith([&](Module&, Builder& b, Ref x) { return b.emit(Inst {f.hooked, i64, 1, x, 0, 0}); })).empty());
}

TEST_CASE("extension operands count as uses", "[verifier][extension]") {
	Fixture f;
	Module m;
	Builder b(m);
	TypeWord params[] = {test::tw(TypeId::I64)};
	auto sig = m.internSignature(test::tw(TypeId::I64), params);
	b.beginFunction(m.internName("f"), sig, Visibility::Exported, Level::SSA);
	b.beginBlock(1);
	auto x = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
	auto v = b.emit(Inst {f.add, static_cast<uint8_t>(TypeId::I64), 0, x, x, 0});
	auto w = b.emit(Inst {f.add, static_cast<uint8_t>(TypeId::I64), 0, v, v, 0});
	b.emit(Op::Ret, TypeId::Void, 0, w);
	REQUIRE(b.useCount(x) == 2);
	REQUIRE(b.useCount(v) == 2);
	b.endFunction();
	// and the dominance check sees them: a use before the definition is rejected
	m.insts[1].b = 2;
	REQUIRE(has(m, DiagCode::V061));
}

TEST_CASE("NIR-V014 calls of intrinsics", "[verifier][extension]") {
	Fixture f;
	auto& r = ExtensionRegistry::instance();
	IntrinsicInfo i;
	i.name = "test.mayThrow";
	i.signature = {test::tw(TypeId::I64), test::tw(TypeId::I64)};
	i.fallback = reinterpret_cast<void*>(fallbackFn);
	i.captureThunk = reinterpret_cast<void*>(thunkFn); // a may-throw intrinsic is registered with its thunk
	r.registerIntrinsic(i);
	auto build = [&](uint32_t unwind, uint32_t thunk) {
		Module m;
		TypeWord params[] = {test::tw(TypeId::I64)};
		auto sig = m.internSignature(test::tw(TypeId::I64), params);
		m.extensions.push_back(ExtensionRecord {m.internName("test.mayThrow@1"), static_cast<uint8_t>(ExtensionKind::Intrinsic), {}});
		CalleeRecord c {};
		c.kind = static_cast<uint8_t>(CalleeKind::Intrinsic);
		c.target = 0;
		c.signature = sig;
		auto callee = m.internCallee(c);
		auto pad = m.internPointer(PointerRecord {1, 0, 0, 0, 0, 0x5000, 0});
		(void)pad;
		Builder b(m);
		b.beginFunction(m.internName("f"), sig, Visibility::Exported, Level::SSA);
		b.beginBlock(1);
		auto x = b.emit(Op::BlockArg, TypeId::I64, 0, 0);
		uint32_t list[] = {1, x, unwind, thunk};
		auto v = b.emit(Op::Call, TypeId::I64, 0, callee, 0, m.addExtra(list));
		b.emit(Op::Ret, TypeId::Void, 0, v);
		if (unwind == 1) { // a landing pad for the unwind edge
			b.beginBlock(0, BLOCK_LANDING_PAD);
			b.emit(Op::Resume, TypeId::Void, 0);
		}
		b.endFunction();
		m.module.processToken = 1;
		return m;
	};
	// a may-throw intrinsic call needs no thunk of its own: the registry's thunk wraps the fallback
	auto good = build(1, NONE);
	INFO(all(good));
	REQUIRE_FALSE(has(good, DiagCode::V014));
	REQUIRE_FALSE(has(build(NO_UNWIND, NONE), DiagCode::V014));
	// naming a thunk on an intrinsic call is rejected, with or without an unwind edge
	REQUIRE(has(build(1, 0), DiagCode::V014));
	REQUIRE(has(build(NO_UNWIND, 0), DiagCode::V014));
}

TEST_CASE("NIR-V035 extension types need a C ABI type at native boundaries", "[verifier][extension]") {
	Fixture f;
	auto build = [&](uint8_t type) {
		Module m;
		Builder b(m);
		TypeWord params[] = {makeTypeWord(static_cast<TypeId>(type))};
		auto internalSig = m.internSignature(makeTypeWord(TypeId::Void), params);
		auto exportSig = m.internSignature(makeTypeWord(TypeId::Ptr), {});
		b.beginFunction(m.internName("helper"), internalSig, Visibility::Internal, Level::SSA);
		b.beginBlock(1);
		b.emit(Op::BlockArg, static_cast<TypeId>(type), 0, 0);
		b.emit(Op::Ret, TypeId::Void, 0, NONE);
		b.endFunction();
		CalleeRecord c {};
		c.kind = static_cast<uint8_t>(CalleeKind::Internal);
		c.target = 0;
		c.signature = internalSig;
		auto callee = m.internCallee(c);
		b.beginFunction(m.internName("main"), exportSig, Visibility::Exported, Level::SSA);
		b.beginBlock();
		auto addr = b.emit(Op::FuncAddr, TypeId::Ptr, 0, callee);
		b.emit(Op::Ret, TypeId::Void, 0, addr);
		b.endFunction();
		return m;
	};
	// the internal function may take the type (internal calls need no C ABI type) but its address may not escape
	auto without = build(f.vec);
	REQUIRE(has(without, DiagCode::V035));
	REQUIRE_FALSE(has(build(f.vecC), DiagCode::V035));
}
