#include "TestModules.hpp"
#include "nautilus/nir/Verifier.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus::nir;
using test::inst;
using test::negative;

namespace {
/// Offset of the call's extra list: [argc, args..., unwind, thunk]
uint32_t callList(Module& m) {
	return inst(m, Op::Call).c;
}
} // namespace

TEST_CASE("NIR-V001 a function has a block", "[nir][verifier]") {
	Module m;
	Builder b(m);
	auto sig = m.internSignature(makeTypeWord(TypeId::Void), {});
	b.beginFunction(m.internName("f"), sig, Visibility::Internal, Level::SSA);
	b.endFunction();
	REQUIRE(test::has(verify(m), DiagCode::V001));
}

TEST_CASE("NIR-V002 terminators", "[nir][verifier]") {
	negative(DiagCode::V002, [](Module& m) { inst(m, Op::Ret).op = static_cast<uint16_t>(Op::Not); });
	negative(DiagCode::V002, [](Module& m) {
		auto& i = inst(m, Op::Sub);
		i = Inst {static_cast<uint16_t>(Op::Resume), 0, 0, 0, 0, 0};
	});
	Module m;
	Builder b(m);
	b.beginFunction(m.internName("f"), m.internSignature(makeTypeWord(TypeId::Void), {}), Visibility::Internal,
	                Level::SSA);
	b.beginBlock();
	b.endFunction();
	REQUIRE(test::has(verify(m), DiagCode::V002)); // an empty block
}

TEST_CASE("NIR-V003 opcodes", "[nir][verifier]") {
	negative(DiagCode::V003, [](Module& m) { inst(m, Op::Not).op = 7; });
	negative(DiagCode::V003, [](Module& m) { inst(m, Op::Not).op = 300; });
	auto m = test::validModule();
	inst(m, Op::Not).op = 300;
	VerifyOptions o;
	o.isExtensionOp = [](uint16_t op) {
		return op == 300;
	};
	REQUIRE_FALSE(test::has(verify(m, o), DiagCode::V003));
}

TEST_CASE("NIR-V004 reserved bits", "[nir][verifier]") {
	negative(DiagCode::V004, [](Module& m) { inst(m, Op::Add).c = 5; });
	negative(DiagCode::V004, [](Module& m) { inst(m, Op::Add).flags = 1; });
	negative(DiagCode::V004, [](Module& m) { inst(m, Op::Shr).flags = 2; });
	negative(DiagCode::V004, [](Module& m) { m.blocks[0].flags = 4; });
	negative(DiagCode::V004, [](Module& m) { m.blocks[0].pad = 1; });
	negative(DiagCode::V004, [](Module& m) { inst(m, Op::Resume).a = 1; });
}

TEST_CASE("NIR-V005 value references", "[nir][verifier]") {
	negative(DiagCode::V005, [](Module& m) { inst(m, Op::Add).a = 9999; });
	negative(DiagCode::V005, [](Module& m) { inst(m, Op::Add).a = NONE; });
	negative(DiagCode::V005, [](Module& m) { inst(m, Op::Add).a = 0x7FFFFFFF; });
	negative(DiagCode::V005, [](Module& m) { inst(m, Op::Add).a = makeConstRef(999); });
	negative(DiagCode::V005, [](Module& m) { inst(m, Op::Add).a = 23; }); // the store has no result
	negative(DiagCode::V005, [](Module& m) { m.extra[callList(m) + 1] = 9999; });
}

TEST_CASE("NIR-V006 operand and result types", "[nir][verifier]") {
	negative(DiagCode::V006, [](Module& m) { inst(m, Op::Add).type = static_cast<uint8_t>(TypeId::F64); });
	negative(DiagCode::V006, [](Module& m) { inst(m, Op::Add).b = 1; }); // i64 + f64
	negative(DiagCode::V006, [](Module& m) { inst(m, Op::Not).a = 0; });
	negative(DiagCode::V006, [](Module& m) { inst(m, Op::FNeg).a = 0; });
	negative(DiagCode::V006, [](Module& m) { inst(m, Op::Select).a = 0; });
	negative(DiagCode::V006, [](Module& m) { inst(m, Op::Load).a = 0; });
	negative(DiagCode::V006, [](Module& m) { inst(m, Op::Cmp).type = static_cast<uint8_t>(TypeId::I32); });
	negative(DiagCode::V006, [](Module& m) { inst(m, Op::Store).type = static_cast<uint8_t>(TypeId::I64); });
	negative(DiagCode::V006, [](Module& m) { inst(m, Op::BlockArg).type = 0; });
	negative(DiagCode::V006, [](Module& m) { inst(m, Op::CondBr).a = 0; });
	negative(DiagCode::V006, [](Module& m) { inst(m, Op::Alloca).type = static_cast<uint8_t>(TypeId::I64); });
	negative(DiagCode::V006, [](Module& m) { inst(m, Op::Call).type = static_cast<uint8_t>(TypeId::F32); });
	negative(DiagCode::V006, [](Module& m) { inst(m, Op::Cmp).type = 12; });
}

TEST_CASE("NIR-V007 table indices", "[nir][verifier]") {
	negative(DiagCode::V007, [](Module& m) { inst(m, Op::Alloca).a = 5; });
	negative(DiagCode::V007, [](Module& m) { inst(m, Op::BindAddr).a = 9; });
	negative(DiagCode::V007, [](Module& m) { inst(m, Op::DataAddr).a = 9; });
	negative(DiagCode::V007, [](Module& m) { inst(m, Op::PtrConst).a = 99; });
	negative(DiagCode::V007, [](Module& m) { inst(m, Op::PtrConst).a = 0; }); // a callee entry
	negative(DiagCode::V007, [](Module& m) { inst(m, Op::Call).a = 99; });
	negative(DiagCode::V007, [](Module& m) { inst(m, Op::CallIndirect).b = 99; });
	negative(DiagCode::V007, [](Module& m) { inst(m, Op::FuncAddr).a = 99; });
	negative(DiagCode::V007, [](Module& m) { inst(m, Op::Br).a = 99; });
	auto m = test::validModule();
	inst(m, Op::PtrConst).a = NONE; // the null pointer
	REQUIRE(verify(m).empty());
}

TEST_CASE("NIR-V008 extra lists", "[nir][verifier]") {
	negative(DiagCode::V008, [](Module& m) { inst(m, Op::Call).c = 999999; });
	negative(DiagCode::V008, [](Module& m) { m.extra[callList(m)] = 999999; });
	negative(DiagCode::V008, [](Module& m) { inst(m, Op::Br).c = callList(m); }); // overlap with the call
	negative(DiagCode::V008, [](Module& m) { m.blocks[m.functions[1].firstBlock].first = 99999; });
}

TEST_CASE("NIR-V009 branch arguments", "[nir][verifier]") {
	negative(DiagCode::V009, [](Module& m) { m.extra[inst(m, Op::Br).c] = 0; });
	negative(DiagCode::V009, [](Module& m) { m.extra[inst(m, Op::Br).c + 1] = 1; }); // f64 for an i64 blockarg
	negative(DiagCode::V009, [](Module& m) { m.blocks[1].argCount = 0; });
	negative(DiagCode::V009, [](Module& m) { m.extra[inst(m, Op::CondBr).c + 1] = 0; });
}

TEST_CASE("NIR-V010 ret", "[nir][verifier]") {
	negative(DiagCode::V010, [](Module& m) { inst(m, Op::Ret).a = NONE; });
	negative(DiagCode::V010, [](Module& m) { inst(m, Op::Ret).a = 1; }); // f64
	negative(DiagCode::V010, [](Module& m) {
		TypeWord none[1];
		m.functions[0].signature = m.internSignature(makeTypeWord(TypeId::Void), std::span<const TypeWord>(none, 0));
	});
}

TEST_CASE("NIR-V011 call arguments", "[nir][verifier]") {
	negative(DiagCode::V011, [](Module& m) { m.extra[callList(m)] = 0; });
	negative(DiagCode::V011, [](Module& m) { m.extra[callList(m) + 1] = 1; }); // f64 argument
}

TEST_CASE("NIR-V012 unwind entry forms", "[nir][verifier]") {
	negative(DiagCode::V012, [](Module& m) { m.extra[callList(m) + 2] = 9999; });
	negative(DiagCode::V012, [](Module& m) { inst(m, Op::Div).c = 9999; });
	// NO_CLEANUP and a landing-pad block id are valid forms at the SSA level
	auto m = test::validModule();
	m.extra[callList(m) + 2] = NO_CLEANUP;
	REQUIRE_FALSE(test::has(verify(m), DiagCode::V012));
	// pre-SSA: NO_CLEANUP is not allowed
	negative(DiagCode::V012, [](Module& m2) { inst(m2, Op::Div, 1).c = NO_CLEANUP; });
}

TEST_CASE("NIR-V013 noUnwind calls", "[nir][verifier]") {
	negative(DiagCode::V013, [](Module& m) { m.callees[0].attrs = AttrNoUnwind; });
	negative(DiagCode::V013, [](Module& m) {
		auto list = inst(m, Op::CallIndirect).c;
		m.extra[list + 2] = NO_CLEANUP;
		m.extra[list + 4] = AttrNoUnwind; // call.indirect attrs
	});
}

TEST_CASE("NIR-V014 capture thunks", "[nir][verifier]") {
	negative(DiagCode::V014, [](Module& m) { m.extra[callList(m) + 3] = NONE; });      // may throw, no thunk
	negative(DiagCode::V014, [](Module& m) { m.extra[callList(m) + 2] = NO_UNWIND; }); // thunk on a NO_UNWIND call
	negative(DiagCode::V014, [](Module& m) { m.extra[callList(m) + 3] = 0; });         // pointers[0] is a callee
}

TEST_CASE("NIR-V015 funcaddr and intrinsics", "[nir][verifier]") {
	negative(DiagCode::V015, [](Module& m) {
		m.extensions.push_back(ExtensionRecord {m.internName("std.x@1"), 0});
		CalleeRecord c {};
		c.kind = static_cast<uint8_t>(CalleeKind::Intrinsic);
		c.signature = m.callees[0].signature;
		inst(m, Op::FuncAddr).a = m.internCallee(c);
	});
}

TEST_CASE("NIR-V016 div and rem flags", "[nir][verifier]") {
	negative(DiagCode::V016, [](Module& m) { inst(m, Op::Div).c = NO_UNWIND; });
	negative(DiagCode::V016, [](Module& m) { inst(m, Op::Rem).c = 3; });
	negative(DiagCode::V016, [](Module& m) {
		auto& r = inst(m, Op::Rem);
		r = Inst {static_cast<uint16_t>(Op::Rem), static_cast<uint8_t>(TypeId::F64), 0, 1, 1, 7};
	});
	negative(DiagCode::V016, [](Module& m) {
		auto& r = inst(m, Op::Rem);
		r = Inst {static_cast<uint16_t>(Op::Rem), static_cast<uint8_t>(TypeId::F64), 2, 1, 1, 0};
	});
}

TEST_CASE("NIR-V017 comparisons", "[nir][verifier]") {
	negative(DiagCode::V017, [](Module& m) { inst(m, Op::Cmp).flags = 6; });
	negative(DiagCode::V017, [](Module& m) { inst(m, Op::Cmp).flags = 8; }); // unsigned eq
	negative(DiagCode::V017, [](Module& m) {
		inst(m, Op::Cmp) = Inst {static_cast<uint16_t>(Op::Cmp), static_cast<uint8_t>(TypeId::Bool), 2 | 8, 1, 1, 0};
	});
	negative(DiagCode::V017, [](Module& m) {
		inst(m, Op::Cmp) = Inst {static_cast<uint16_t>(Op::Cmp), static_cast<uint8_t>(TypeId::Bool), 2, 3, 3, 0};
	}); // bool lt
}

TEST_CASE("NIR-V018 casts", "[nir][verifier]") {
	negative(DiagCode::V018, [](Module& m) { inst(m, Op::Cast).flags = 2; }); // trunc i64 -> f64
	negative(DiagCode::V018, [](Module& m) { inst(m, Op::Cast).flags = 12; });
	negative(DiagCode::V018, [](Module& m) {
		inst(m, Op::Cast) = Inst {static_cast<uint16_t>(Op::Cast), static_cast<uint8_t>(TypeId::F64), 9, 1, 0, 0};
	});
	auto m = test::validModule();
	inst(m, Op::Cast) = Inst {static_cast<uint16_t>(Op::Cast), static_cast<uint8_t>(TypeId::I64), 9, 1, 0, 0};
	REQUIRE(verify(m).empty()); // bitcast f64 -> i64 is valid
}

TEST_CASE("NIR-V019 alignment codes", "[nir][verifier]") {
	negative(DiagCode::V019, [](Module& m) { inst(m, Op::Load).flags = 14; });
	negative(DiagCode::V019, [](Module& m) { inst(m, Op::Load).flags = 4; }); // f64: 2^3 = natural
	auto m = test::validModule();
	inst(m, Op::Load).flags = 3; // 4 bytes
	REQUIRE(verify(m).empty());
}

TEST_CASE("NIR-V020 blockarg placement", "[nir][verifier]") {
	negative(DiagCode::V020, [](Module& m) { inst(m, Op::BlockArg).a = 3; });
	negative(DiagCode::V020,
	         [](Module& m) { inst(m, Op::Add) = Inst {static_cast<uint16_t>(Op::BlockArg), 5, 0, 7, 0, 0}; });
	negative(DiagCode::V020, [](Module& m) { m.blocks[0].argCount = 5; });
	negative(DiagCode::V020, [](Module& m) { inst(m, Op::BlockArg, 1).a = 3; }); // pre-SSA parameter numbering
}

TEST_CASE("NIR-V054 pre-SSA block membership", "[nir][verifier]") {
	negative(DiagCode::V054, [](Module& m) {
		auto b = m.functions[1].firstBlock;
		m.extra[m.blocks[b + 1].first] = m.extra[m.blocks[b].first]; // listed twice, another one in no block
	});
	negative(DiagCode::V054, [](Module& m) { m.extra[m.blocks[m.functions[1].firstBlock].first] = 999; });
}

TEST_CASE("NIR-V067 function ranges", "[nir][verifier]") {
	negative(DiagCode::V067, [](Module& m) { m.functions[1].firstInst += 1; });
	negative(DiagCode::V067, [](Module& m) { m.functions[0].instCount -= 1; });
	negative(DiagCode::V067, [](Module& m) { m.blocks[0].count -= 1; });
	negative(DiagCode::V067, [](Module& m) { m.functions[1].firstAlloca += 1; });
}

TEST_CASE("NIR-V050 assign", "[nir][verifier]") {
	negative(DiagCode::V050, [](Module& m) { inst(m, Op::Assign, 1).a = makeConstRef(0); });
	negative(DiagCode::V050, [](Module& m) {
		inst(m, Op::Assign, 1).b = makeConstRef(m.consts.intern(TypeId::F64, Provenance::Literal, 0));
	});
}

TEST_CASE("NIR-V052 and NIR-V053 pre-SSA blocks", "[nir][verifier]") {
	negative(DiagCode::V052, [](Module& m) { m.blocks[m.functions[1].firstBlock].argCount = 1; });
	negative(DiagCode::V052, [](Module& m) { m.extra[inst(m, Op::Br, 1).c] = 1; });
	negative(DiagCode::V053, [](Module& m) { m.blocks[m.functions[1].firstBlock + 1].flags = BLOCK_LANDING_PAD; });
}

TEST_CASE("NIR-V055 pre-SSA div and rem", "[nir][verifier]") {
	negative(DiagCode::V055, [](Module& m) {
		inst(m, Op::Div, 1).flags = 0;
		inst(m, Op::Div, 1).c = 0;
	});
	negative(DiagCode::V055, [](Module& m) { inst(m, Op::Div, 1).c = NO_UNWIND; });
}

TEST_CASE("NIR-V056 cleanup lists", "[nir][verifier]") {
	auto addDtor = [](Module& m) {
		TypeWord p[] = {makeTypeWord(TypeId::Ptr)};
		CalleeRecord c {};
		c.kind = static_cast<uint8_t>(CalleeKind::External);
		c.target =
		    m.internPointer(PointerRecord {static_cast<uint8_t>(PointerKind::Destructor), 0, 0, 0, 0, 0x9000, 0});
		c.signature = m.internSignature(makeTypeWord(TypeId::Void), p);
		return m.internCallee(c);
	};
	// a cleanup list [1, (destructor, object)] where the object is a ptr value: the call's list in function 0
	// supplies one (the load address argument); here the pre-SSA div gets a list with the entry
	auto build = [&](Module& m, uint32_t calleeIdx, uint32_t object) {
		uint32_t list[] = {1, calleeIdx, object};
		inst(m, Op::Div, 1).c = m.addExtra(list);
	};
	negative(DiagCode::V056, [&](Module& m) { build(m, 0, 2); });          // callee 0 is not a destructor
	negative(DiagCode::V056, [&](Module& m) { build(m, addDtor(m), 0); }); // object is i64, not ptr
}

TEST_CASE("NIR-V060 SSA has no assign or var", "[nir][verifier]") {
	negative(DiagCode::V060,
	         [](Module& m) { inst(m, Op::Not) = Inst {static_cast<uint16_t>(Op::Var), 1, 0, 0, 0, 0}; });
	negative(DiagCode::V060,
	         [](Module& m) { inst(m, Op::Not) = Inst {static_cast<uint16_t>(Op::Assign), 0, 0, 0, 0, 0}; });
}

TEST_CASE("NIR-V063 reachability", "[nir][verifier]") {
	negative(DiagCode::V063, [](Module& m) { m.extra[callList(m) + 2] = NO_CLEANUP; }); // pad ^b3 is not entered
	negative(DiagCode::V063, [](Module& m) {
		auto& c = inst(m, Op::CondBr);
		c.b = 2; // ^b1 is not entered
		m.extra[c.c + 3] = 2;
	});
}

TEST_CASE("NIR-V064 landing pads", "[nir][verifier]") {
	negative(DiagCode::V064, [](Module& m) { m.blocks[3].argCount = 1; });
	negative(DiagCode::V064, [](Module& m) { m.extra[callList(m) + 2] = 1; }); // unwinds to a normal block
	negative(DiagCode::V064, [](Module& m) { inst(m, Op::Br).a = 3; });        // a normal block branches to a pad
	negative(DiagCode::V064, [](Module& m) { m.blocks[1].flags = BLOCK_LANDING_PAD; }); // ^b1 as a pad with a br to ^b2
	negative(DiagCode::V064,
	         [](Module& m) { inst(m, Op::Resume) = Inst {static_cast<uint16_t>(Op::Br), 0, 0, 3, 0, 0}; });
}

TEST_CASE("NIR-V065 calls and resume in pads", "[nir][verifier]") {
	negative(DiagCode::V065,
	         [](Module& m) { inst(m, Op::Ret) = Inst {static_cast<uint16_t>(Op::Resume), 0, 0, 0, 0, 0}; });
}

TEST_CASE("NIR-V066 entry block", "[nir][verifier]") {
	negative(DiagCode::V066, [](Module& m) { m.blocks[0].flags = BLOCK_LANDING_PAD; });
	negative(DiagCode::V066, [](Module& m) { m.functions[0].signature = m.functions[1].signature; });
	negative(DiagCode::V066, [](Module& m) { inst(m, Op::BlockArg).type = static_cast<uint8_t>(TypeId::I32); });
}
