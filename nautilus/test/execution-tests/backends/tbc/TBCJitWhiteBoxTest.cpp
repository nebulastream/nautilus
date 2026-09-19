#include <catch2/catch_all.hpp>
#include <nautilus/config.hpp>

// White-box tests for the tbc copy-and-patch JIT: hand-built TBCPrograms
// stitched and executed directly, covering machinery that user-level kernels
// cannot reach — the helper exception bridge (a VM stack overflow during an
// internal CALL must surface as the same RuntimeException the interpreter
// throws, carried across stitched frames via VMContext::pendingException and
// rethrown by the entry shim) and basic stream-level opcode sanity.
#ifdef ENABLE_TBC_JIT

#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include "nautilus/compiler/backends/tbc/TBCInterpreter.hpp"
#include "nautilus/compiler/backends/tbc/TBCOpcodes.hpp"
#include "nautilus/compiler/backends/tbc/jit/TBCJit.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus::compiler::tbc {

namespace {

Instr instr(Op op, uint16_t a = 0, uint16_t b = 0, uint16_t c = 0) {
	return Instr {opIndex(op), a, b, c};
}

/// A program whose entry function computes ((7 + 7) * 3) via MOV_imm /
/// ADD_i64 / MUL_imm and returns it: exercises immediate patching, register
/// operands, and RET_VALUE on a hand-authored stream.
std::shared_ptr<TBCProgram> makeArithmeticProgram() {
	auto program = std::make_shared<TBCProgram>();
	auto& fn = program->functions.emplace_back();
	fn.name = "main";
	fn.returnType = Type::i64;
	fn.regSlots = 2;
	fn.frameSlots = 3 + fn.regSlots;
	fn.initImage.assign(fn.regSlots, 0);
	fn.code = {
	    instr(Op::MOV_imm_i64, 0, 0, 7), // r0 = 7
	    instr(Op::ADD_i64, 1, 0, 0),     // r1 = r0 + r0
	    instr(Op::MUL_imm_i64, 1, 1, 3), // r1 = r1 * 3
	    instr(Op::RET, 1),
	};
	program->functionIndex["main"] = 0;
	return program;
}

/// Entry function CALLs a callee whose frame can never fit on the VM stack,
/// so tbcJitPushFrame fails inside stitched code.
std::shared_ptr<TBCProgram> makeOverflowProgram() {
	auto program = std::make_shared<TBCProgram>();
	program->functions.resize(2);

	auto& main = program->functions[0];
	main.name = "main";
	main.returnType = Type::i64;
	main.regSlots = 1;
	main.frameSlots = 3 + main.regSlots;
	main.initImage.assign(main.regSlots, 0);
	main.code = {
	    instr(Op::CALL, 0, 1, 0), // r0 = call functions[1] via callsites[0]
	    instr(Op::RET, 0),
	};

	auto& huge = program->functions[1];
	huge.name = "huge";
	huge.returnType = Type::i64;
	huge.regSlots = 1;
	// Larger than any realistic VM stack: pushFrame's bounds check must
	// throw before touching the frame.
	huge.frameSlots = 1u << 30;
	huge.initImage.assign(huge.regSlots, 0);
	huge.code = {instr(Op::RET, kNoReg)};

	auto& site = program->callsites.emplace_back();
	site.internalFnIdx = 1;
	site.returnType = Type::i64;

	program->functionIndex["main"] = 0;
	program->functionIndex["huge"] = 1;
	return program;
}

} // namespace

TEST_CASE("TBC JIT white-box: hand-authored stream executes stitched") {
	if (!jit::jitRuntimeAvailable()) {
		SKIP("tbc-jit runtime unavailable on this build");
	}
	auto program = makeArithmeticProgram();

	// Interpreter reference on the identical program.
	const uint64_t interp = invoke(*program, 0, nullptr, 0);
	REQUIRE(static_cast<int64_t>(interp) == 42);

	std::string whyNot;
	program->jit = jit::stitchProgram(*program, &whyNot);
	INFO(whyNot);
	REQUIRE(program->jit != nullptr);
	REQUIRE(program->jit->codeBytes > 0);
	REQUIRE(program->jit->entries[0] != nullptr);

	// invoke() dispatches to the stitched code once program->jit is set.
	REQUIRE(invoke(*program, 0, nullptr, 0) == interp);
}

TEST_CASE("TBC JIT white-box: helper exceptions bridge across stitched frames") {
	if (!jit::jitRuntimeAvailable()) {
		SKIP("tbc-jit runtime unavailable on this build");
	}
	auto program = makeOverflowProgram();

	// Interpreter reference behavior: pushFrame overflow throws.
	REQUIRE_THROWS_AS(invoke(*program, 0, nullptr, 0), RuntimeException);

	std::string whyNot;
	program->jit = jit::stitchProgram(*program, &whyNot);
	INFO(whyNot);
	REQUIRE(program->jit != nullptr);

	// Stitched: the CALL stencil's tbcJitPushFrame catches the overflow into
	// VMContext::pendingException, exits through @UNWIND, and the entry shim
	// rethrows — no unwinding ever crosses a stitched frame.
	REQUIRE_THROWS_AS(invoke(*program, 0, nullptr, 0), RuntimeException);

	// The VM context must be fully restored: the arithmetic program still
	// runs on the same thread afterwards.
	auto sane = makeArithmeticProgram();
	sane->jit = jit::stitchProgram(*sane, &whyNot);
	REQUIRE(sane->jit != nullptr);
	REQUIRE(static_cast<int64_t>(invoke(*sane, 0, nullptr, 0)) == 42);
}

TEST_CASE("TBC JIT white-box: stitching reports missing stencils instead of failing") {
	if (!jit::jitRuntimeAvailable()) {
		SKIP("tbc-jit runtime unavailable on this build");
	}
	// HALT never occurs in lowered streams and deliberately has no stencil:
	// stitching must bail out with a reason, leaving the program on the
	// interpreter — the stale-generated-table degradation path.
	auto program = makeArithmeticProgram();
	program->functions[0].code.insert(program->functions[0].code.begin(), instr(Op::HALT));
	std::string whyNot;
	REQUIRE(jit::stitchProgram(*program, &whyNot) == nullptr);
	REQUIRE_FALSE(whyNot.empty());
}

} // namespace nautilus::compiler::tbc

#endif // ENABLE_TBC_JIT
