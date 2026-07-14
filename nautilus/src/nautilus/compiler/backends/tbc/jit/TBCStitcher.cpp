
#include "nautilus/compiler/backends/tbc/jit/TBCJit.hpp"
#include "nautilus/compiler/backends/tbc/jit/TBCStencils.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <asmjit/core.h>
#include <cstring>
#include <vector>

// Two-pass copy-and-patch stitcher.
//
// Pass 1 (layout): walk every function's instruction stream, pick each
// instruction's stencil, decide fall-through elision (a stencil's trailing
// "jump to _JIT_CONTINUE" is dropped when the next instruction is stitched
// immediately after it — always true in the linear walk), and assign span
// offsets. The @EPILOGUE/@UNWIND stencils, aarch64 helper-call thunks, and
// the read-only data image are appended once per program.
//
// Pass 2 (patch): copy stencil bytes into a scratch buffer and patch every
// hole with its resolved value (operand fields pre-scaled as frame byte
// offsets, branch targets as absolute stitched addresses, call-site/function
// record addresses, helper addresses, data-image addresses). On x86-64 a
// peephole rewrites patched `movabs $target, %reg; jmp *%reg` pairs into
// direct `jmp rel32` when the target is in the span (it always is), restoring
// direct-branch prediction on the hot paths.
//
// The buffer is then published through asmjit's JitAllocator, which owns the
// W^X policy (dual mapping where required, MAP_JIT + thread JIT-write toggles
// on macOS) and the instruction-cache flush.

namespace nautilus::compiler::tbc::jit {

namespace {

asmjit::JitAllocator& jitAllocator() {
	static asmjit::JitAllocator allocator;
	return allocator;
}

/// Concrete stitched-code handle: releases the executable span on program
/// destruction.
struct StitchedCode final : TBCJitCode {
	void* rx = nullptr;
	~StitchedCode() override {
		if (rx != nullptr) {
			jitAllocator().release(rx);
		}
	}
};

constexpr uint16_t kOpSelect = opIndex(Op::SELECT);
constexpr uint16_t kOpJmp = opIndex(Op::JMP);
constexpr uint16_t kOpCjmp = opIndex(Op::CJMP);
constexpr uint16_t kOpRet = opIndex(Op::RET);
constexpr uint16_t kOpCall = opIndex(Op::CALL);
constexpr uint16_t kOpCallExt = opIndex(Op::CALL_EXT);
constexpr uint16_t kOpCallInd = opIndex(Op::CALL_IND);
constexpr uint16_t kFusedFirst = opIndex(Op::CJMP_EQ_i32);
constexpr uint16_t kFusedLast = opIndex(Op::CJMP_GE_i64);

bool isFusedCjmp(uint16_t op) {
	return op >= kFusedFirst && op <= kFusedLast;
}

uint32_t instrWords(uint16_t op) {
	return (op == kOpSelect || isFusedCjmp(op)) ? 2 : 1;
}

/// Opcode families whose `c` field is a sign-extended 16-bit immediate
/// rather than a register (the stencil bodies consume the value directly).
bool cFieldIsImmediate(uint16_t op) {
	return (op >= opIndex(Op::MOV_imm_i8) && op <= opIndex(Op::MOV_imm_ui64)) ||
	       (op >= opIndex(Op::ADD_imm_i32) && op <= opIndex(Op::MUL_imm_i64)) ||
	       (op >= opIndex(Op::LOAD_off_i8) && op <= opIndex(Op::LOAD_off_b)) ||
	       (op >= opIndex(Op::STORE_off_i8) && op <= opIndex(Op::STORE_off_b));
}

/// Per-instruction layout decision.
struct InstrPlan {
	const Stencil* stencil = nullptr;
	uint32_t offset = 0;   // span offset of this instruction's code
	uint32_t emitSize = 0; // stencil size minus an elided trailing jump
	uint32_t gotOff = 0;   // span offset of this instruction's emulated-GOT slots (Mach-O)
};

/// Number of emulated-GOT slots an instruction needs: one per distinct hole
/// symbol referenced through a GOT-load pair (Mach-O arm64, where Apple
/// codegen routes every external symbol reference through the GOT — the
/// stitcher materializes per-instruction slots holding the patched values).
uint32_t gotSlotCount(const Stencil& stencil, uint32_t emitSize) {
	uint32_t seen = 0;
	for (uint16_t h = 0; h < stencil.holeCount; ++h) {
		const StencilHole& hole = stencil.holes[h];
		if (hole.offset >= emitSize) {
			continue;
		}
		if (hole.kind == HoleKind::A64GotLoadPage21 || hole.kind == HoleKind::A64GotLoadPageOff12) {
			seen |= 1u << static_cast<uint8_t>(hole.sym);
		}
	}
	return static_cast<uint32_t>(__builtin_popcount(seen));
}

struct Layout {
	std::vector<std::vector<InstrPlan>> plans;   // [fn][wordIdx], instruction starts only
	std::vector<std::vector<uint32_t>> wordOffs; // [fn][wordIdx] -> span offset (branch targets)
	uint32_t epilogueOff = 0;
	uint32_t unwindOff = 0;
	uint32_t thunkOff[3] = {0, 0, 0}; // aarch64 helper-call range thunks
	uint32_t dataOff = 0;
	uint32_t gotOff = 0; // emulated-GOT region base (Mach-O; 0 slots elsewhere)
	uint32_t totalSize = 0;
};

#if defined(__aarch64__)
constexpr bool kIsAArch64 = true;
#else
constexpr bool kIsAArch64 = false;
#endif
constexpr uint32_t kThunkSize = 20; // movz + 3*movk + br x16

void writeU32(uint8_t* at, uint32_t value) {
	std::memcpy(at, &value, sizeof(value));
}

uint32_t readU32(const uint8_t* at) {
	uint32_t value;
	std::memcpy(&value, at, sizeof(value));
	return value;
}

/// aarch64 helper-call thunk: materialize the 64-bit helper address in x16
/// (the linker scratch register, dead at any call boundary) and branch.
void emitThunk(uint8_t* at, uint64_t target) {
	writeU32(at + 0, 0xD2800010 | (static_cast<uint32_t>(target & 0xFFFF) << 5));          // movz x16, g0
	writeU32(at + 4, 0xF2A00010 | (static_cast<uint32_t>((target >> 16) & 0xFFFF) << 5));  // movk x16, g1
	writeU32(at + 8, 0xF2C00010 | (static_cast<uint32_t>((target >> 32) & 0xFFFF) << 5));  // movk x16, g2
	writeU32(at + 12, 0xF2E00010 | (static_cast<uint32_t>((target >> 48) & 0xFFFF) << 5)); // movk x16, g3
	writeU32(at + 16, 0xD61F0200);                                                         // br x16
}

void patchA64Movw(uint8_t* at, uint64_t value, unsigned group) {
	uint32_t insn = readU32(at);
	const auto imm16 = static_cast<uint32_t>((value >> (16 * group)) & 0xFFFF);
	insn = (insn & ~(0xFFFFu << 5)) | (imm16 << 5);
	writeU32(at, insn);
}

void patchA64Branch26(uint8_t* at, uint64_t patchSiteAddr, uint64_t value) {
	const auto delta = static_cast<int64_t>(value) - static_cast<int64_t>(patchSiteAddr);
	if (delta < -(int64_t {1} << 27) || delta >= (int64_t {1} << 27) || (delta & 3) != 0) {
		throw RuntimeException("tbc-jit: aarch64 branch target out of imm26 range");
	}
	uint32_t insn = readU32(at);
	insn = (insn & 0xFC000000u) | (static_cast<uint32_t>(delta >> 2) & 0x03FFFFFFu);
	writeU32(at, insn);
}

void patchA64AdrpPage21(uint8_t* at, uint64_t patchSiteAddr, uint64_t value) {
	const int64_t pageDelta =
	    static_cast<int64_t>(value & ~uint64_t {0xFFF}) - static_cast<int64_t>(patchSiteAddr & ~uint64_t {0xFFF});
	if (pageDelta < -(int64_t {1} << 32) || pageDelta >= (int64_t {1} << 32)) {
		throw RuntimeException("tbc-jit: aarch64 adrp target out of range");
	}
	const auto page = static_cast<uint64_t>(pageDelta) >> 12;
	uint32_t insn = readU32(at);
	insn = (insn & ~((0x3u << 29) | (0x7FFFFu << 5))) | (static_cast<uint32_t>(page & 3) << 29) |
	       (static_cast<uint32_t>((page >> 2) & 0x7FFFF) << 5);
	writeU32(at, insn);
}

void patchA64Imm12(uint8_t* at, uint64_t value, unsigned scaleShift) {
	const uint64_t lo12 = value & 0xFFF;
	if ((lo12 & ((uint64_t {1} << scaleShift) - 1)) != 0) {
		throw RuntimeException("tbc-jit: aarch64 unaligned lo12 load/store offset");
	}
	uint32_t insn = readU32(at);
	insn = (insn & ~(0xFFFu << 10)) | (static_cast<uint32_t>(lo12 >> scaleShift) << 10);
	writeU32(at, insn);
}

/// x86-64 peephole: a patched `movabs $imm64, %reg; jmp *%reg` whose target
/// lies within ±2GB of the site becomes `jmp rel32` (+ never-executed NOP
/// padding). Intra-span code targets always qualify.
#if defined(__x86_64__)
void relaxX64JumpAt(uint8_t* buffer, uint32_t holeOff, uint32_t emitEnd, uint64_t bufferBaseAddr, uint64_t target) {
	if (holeOff < 2) {
		return;
	}
	const uint8_t rex = buffer[holeOff - 2];
	const uint8_t opc = buffer[holeOff - 1];
	if ((rex != 0x48 && rex != 0x49) || (opc & 0xF8) != 0xB8) {
		return;
	}
	const unsigned movReg = ((rex & 1u) << 3) | (opc & 7u);
	const uint32_t jmpOff = holeOff + 8;
	uint32_t jmpLen = 0;
	if (jmpOff + 2 <= emitEnd && buffer[jmpOff] == 0xFF && (buffer[jmpOff + 1] & 0xF8) == 0xE0 &&
	    (buffer[jmpOff + 1] & 7u) == (movReg & 7u) && movReg < 8) {
		jmpLen = 2;
	} else if (jmpOff + 3 <= emitEnd && buffer[jmpOff] == 0x41 && buffer[jmpOff + 1] == 0xFF &&
	           (buffer[jmpOff + 2] & 0xF8) == 0xE0 && (8u | (buffer[jmpOff + 2] & 7u)) == movReg) {
		jmpLen = 3;
	} else {
		return;
	}
	const uint32_t start = holeOff - 2;
	const int64_t rel = static_cast<int64_t>(target) - static_cast<int64_t>(bufferBaseAddr + start + 5);
	if (rel < INT32_MIN || rel > INT32_MAX) {
		return;
	}
	buffer[start] = 0xE9;
	const auto rel32 = static_cast<int32_t>(rel);
	std::memcpy(buffer + start + 1, &rel32, sizeof(rel32));
	std::memset(buffer + start + 5, 0x90, (holeOff + 8 + jmpLen) - (start + 5));
}
#endif // __x86_64__

class Stitcher {
public:
	Stitcher(const TBCProgram& program, const ResolvedStencils& stencils) : program(program), stencils(stencils) {
	}

	std::unique_ptr<TBCJitCode> run(std::string* whyNot) {
		if (!layout(whyNot)) {
			return nullptr;
		}

		auto code = std::make_unique<StitchedCode>();
		asmjit::JitAllocator::Span span;
		if (jitAllocator().alloc(span, lay.totalSize) != asmjit::kErrorOk) {
			throw RuntimeException("tbc-jit: executable memory allocation failed");
		}
		code->rx = span.rx();
		code->codeBytes = lay.totalSize;
		const auto base = reinterpret_cast<uint64_t>(span.rx());

		std::vector<uint8_t> buffer(lay.totalSize, 0);
		patchAll(buffer.data(), base);

		if (jitAllocator().write(span, [&](asmjit::JitAllocator::Span& s) noexcept -> asmjit::Error {
			    std::memcpy(s.rw(), buffer.data(), buffer.size());
			    return asmjit::kErrorOk;
		    }) != asmjit::kErrorOk) {
			throw RuntimeException("tbc-jit: writing stitched code failed");
		}

		code->entries.resize(program.functions.size());
		for (size_t fn = 0; fn < program.functions.size(); ++fn) {
			code->entries[fn] =
			    program.functions[fn].code.empty() ? nullptr : reinterpret_cast<void*>(base + lay.wordOffs[fn][0]);
		}
		code->epilogue = reinterpret_cast<void*>(base + lay.epilogueOff);
		return code;
	}

private:
	const TBCProgram& program;
	const ResolvedStencils& stencils;
	Layout lay;

	const Stencil* stencilFor(const Instr& inst, std::string* whyNot) const {
		const uint16_t op = inst.op;
		const Stencil* stencil = nullptr;
		if (op == kOpRet) {
			stencil = inst.a == kNoReg ? &stencils.retVoid : &stencils.retValue;
		} else if (op < kOpCount) {
			stencil = &stencils.byOp[op];
		}
		if (stencil == nullptr || stencil->code == nullptr) {
			if (whyNot != nullptr) {
				*whyNot = std::string("no stencil for opcode ") + (op < kOpCount ? opName(static_cast<Op>(op)) : "?");
			}
			return nullptr;
		}
		return stencil;
	}

	bool layout(std::string* whyNot) {
		if (!stencils.valid) {
			if (whyNot != nullptr) {
				*whyNot = "no stencil table for this target";
			}
			return false;
		}
		uint32_t cursor = 0;
		uint32_t gotSlots = 0;
		lay.plans.resize(program.functions.size());
		lay.wordOffs.resize(program.functions.size());
		for (size_t fn = 0; fn < program.functions.size(); ++fn) {
			const auto& codeStream = program.functions[fn].code;
			lay.plans[fn].resize(codeStream.size());
			lay.wordOffs[fn].assign(codeStream.size(), 0);
			for (uint32_t word = 0; word < codeStream.size();) {
				const Instr& inst = codeStream[word];
				const Stencil* stencil = stencilFor(inst, whyNot);
				if (stencil == nullptr) {
					return false;
				}
				const uint32_t words = instrWords(inst.op);
				// Fall-through elision: the continuation is by construction
				// the next stitched instruction. Keep the jump on the last
				// instruction of a function as a hard safety net (streams
				// always end in RET/JMP, which have no continuation).
				const bool elide = stencil->continueJmpOffset != kNoContinueJmp && word + words < codeStream.size();
				InstrPlan& plan = lay.plans[fn][word];
				plan.stencil = stencil;
				plan.offset = cursor;
				plan.emitSize = elide ? stencil->continueJmpOffset : stencil->size;
				plan.gotOff = gotSlots * 8;
				gotSlots += gotSlotCount(*stencil, plan.emitSize);
				lay.wordOffs[fn][word] = cursor;
				cursor += plan.emitSize;
				word += words;
			}
		}
		lay.epilogueOff = cursor;
		cursor += stencils.epilogue.size;
		lay.unwindOff = cursor;
		cursor += stencils.unwind.size;
		if (kIsAArch64) {
			for (auto& off : lay.thunkOff) {
				off = cursor;
				cursor += kThunkSize;
			}
		}
		cursor = (cursor + 15u) & ~15u;
		lay.dataOff = cursor;
		cursor += stencils.dataSize;
		cursor = (cursor + 7u) & ~7u;
		lay.gotOff = cursor;
		cursor += gotSlots * 8;
		lay.totalSize = cursor;
		return true;
	}

	uint64_t helperAddress(HoleSym sym) const {
		switch (sym) {
		case HoleSym::HelperPushFrame:
			return reinterpret_cast<uint64_t>(&tbcJitPushFrame);
		case HoleSym::HelperExtCall:
			return reinterpret_cast<uint64_t>(&tbcJitExtCall);
		case HoleSym::HelperIndCall:
			return reinterpret_cast<uint64_t>(&tbcJitIndCall);
		default:
			throw RuntimeException("tbc-jit: not a helper hole");
		}
	}

	uint32_t helperThunkOff(HoleSym sym) const {
		switch (sym) {
		case HoleSym::HelperPushFrame:
			return lay.thunkOff[0];
		case HoleSym::HelperExtCall:
			return lay.thunkOff[1];
		case HoleSym::HelperIndCall:
			return lay.thunkOff[2];
		default:
			throw RuntimeException("tbc-jit: not a helper hole");
		}
	}

	/// Span offset of the instruction a branch in (fn, word) targets.
	uint32_t branchTargetOff(size_t fn, uint32_t word) const {
		const auto& codeStream = program.functions[fn].code;
		const Instr& inst = codeStream[word];
		int64_t target = word;
		if (inst.op == kOpJmp || inst.op == kOpCjmp) {
			target += packedOffset(inst);
		} else if (isFusedCjmp(inst.op)) {
			target += packedOffsetLoHi(codeStream[word + 1]);
		} else {
			throw RuntimeException("tbc-jit: TARGET hole in a non-branch stencil");
		}
		if (target < 0 || static_cast<size_t>(target) >= codeStream.size()) {
			throw RuntimeException("tbc-jit: branch target outside function");
		}
		return lay.wordOffs[fn][static_cast<size_t>(target)];
	}

	/// The 64-bit value this hole is patched with.
	uint64_t holeValue(const StencilHole& hole, size_t fn, uint32_t word, uint64_t base) const {
		const auto& codeStream = program.functions[fn].code;
		const Instr& inst = codeStream[word];
		const uint16_t op = inst.op;
		switch (hole.sym) {
		case HoleSym::A:
			// Raw destination-register field for the call family (consumed by
			// the helpers); frame byte offset everywhere else.
			if (op == kOpCall || op == kOpCallExt || op == kOpCallInd) {
				return inst.a;
			}
			return uint64_t {inst.a} * 8;
		case HoleSym::B:
			return uint64_t {inst.b} * 8;
		case HoleSym::C:
			if (cFieldIsImmediate(op)) {
				return static_cast<uint64_t>(static_cast<int64_t>(static_cast<int16_t>(inst.c)));
			}
			return uint64_t {inst.c} * 8;
		case HoleSym::D:
			return uint64_t {codeStream[word + 1].a} * 8;
		case HoleSym::Target:
			return base + branchTargetOff(fn, word);
		case HoleSym::Continue:
			return base + continueOff(fn, word);
		case HoleSym::CallTarget:
			if (program.functions[inst.b].code.empty()) {
				throw RuntimeException("tbc-jit: call to function without code");
			}
			return base + lay.wordOffs[inst.b][0];
		case HoleSym::Site:
			return reinterpret_cast<uint64_t>(&program.callsites[op == kOpCallExt ? inst.b : inst.c]);
		case HoleSym::Func:
			return reinterpret_cast<uint64_t>(&program.functions[inst.b]);
		case HoleSym::Unwind:
			return base + lay.unwindOff;
		case HoleSym::HelperPushFrame:
		case HoleSym::HelperExtCall:
		case HoleSym::HelperIndCall:
			return helperAddress(hole.sym);
		case HoleSym::Data:
			return base + lay.dataOff + static_cast<uint64_t>(hole.addend);
		}
		throw RuntimeException("tbc-jit: unknown hole symbol");
	}

	uint32_t continueOff(size_t fn, uint32_t word) const {
		const uint32_t next = word + instrWords(program.functions[fn].code[word].op);
		if (next >= program.functions[fn].code.size()) {
			throw RuntimeException("tbc-jit: continuation past end of function");
		}
		return lay.wordOffs[fn][next];
	}

	static bool isCodeAddressSym(HoleSym sym) {
		return sym == HoleSym::Target || sym == HoleSym::Continue || sym == HoleSym::CallTarget ||
		       sym == HoleSym::Unwind;
	}

	void applyHole(uint8_t* buffer, uint32_t instrOff, const StencilHole& hole, uint64_t value, uint64_t base) const {
		uint8_t* at = buffer + instrOff + hole.offset;
		const uint64_t patchSiteAddr = base + instrOff + hole.offset;
		switch (hole.kind) {
		case HoleKind::Abs64: {
			const uint64_t patched = value + static_cast<uint64_t>(hole.sym == HoleSym::Data ? 0 : hole.addend);
			std::memcpy(at, &patched, sizeof(patched));
			break;
		}
		case HoleKind::A64MovwG0:
			patchA64Movw(at, value, 0);
			break;
		case HoleKind::A64MovwG1:
			patchA64Movw(at, value, 1);
			break;
		case HoleKind::A64MovwG2:
			patchA64Movw(at, value, 2);
			break;
		case HoleKind::A64MovwG3:
			patchA64Movw(at, value, 3);
			break;
		case HoleKind::A64Jump26:
			patchA64Branch26(at, patchSiteAddr, value);
			break;
		case HoleKind::A64Call26: {
			// Helper calls: the helper lives at an arbitrary library address,
			// so route through the in-span thunk (always in imm26 range).
			const uint64_t thunk = base + helperThunkOff(hole.sym);
			patchA64Branch26(at, patchSiteAddr, thunk);
			break;
		}
		case HoleKind::A64AdrpPage21:
			patchA64AdrpPage21(at, patchSiteAddr, value);
			break;
		case HoleKind::A64AddLo12:
			patchA64Imm12(at, value, 0);
			break;
		case HoleKind::A64Ldst32Lo12:
			patchA64Imm12(at, value, 2);
			break;
		case HoleKind::A64Ldst64Lo12:
			patchA64Imm12(at, value, 3);
			break;
		case HoleKind::A64Ldst128Lo12:
			patchA64Imm12(at, value, 4);
			break;
		case HoleKind::A64GotLoadPage21:
		case HoleKind::A64GotLoadPageOff12:
			throw RuntimeException("tbc-jit: Mach-O GOT-load patching not implemented");
		}
	}

	void stitchOne(uint8_t* buffer, const InstrPlan& plan, size_t fn, uint32_t word, uint64_t base) const {
		const Stencil& stencil = *plan.stencil;
		const uint32_t offset = plan.offset;
		const uint32_t emitSize = plan.emitSize;
		std::memcpy(buffer + offset, stencil.code, emitSize);
		// Emulated-GOT slots (Mach-O): one per distinct symbol this
		// instruction references through a GOT-load pair, filled with the
		// patched value; the adrp/ldr pair is redirected to the slot.
		int8_t gotSlot[16];
		std::memset(gotSlot, -1, sizeof(gotSlot));
		uint32_t gotUsed = 0;
		for (uint16_t h = 0; h < stencil.holeCount; ++h) {
			const StencilHole& hole = stencil.holes[h];
			if (hole.offset >= emitSize) {
				continue; // hole inside the elided trailing jump
			}
			const uint64_t value = holeValue(hole, fn, word, base);
			if (hole.kind == HoleKind::A64GotLoadPage21 || hole.kind == HoleKind::A64GotLoadPageOff12) {
				int8_t& slot = gotSlot[static_cast<uint8_t>(hole.sym)];
				if (slot < 0) {
					slot = static_cast<int8_t>(gotUsed++);
					const uint32_t slotOff = lay.gotOff + plan.gotOff + static_cast<uint32_t>(slot) * 8;
					std::memcpy(buffer + slotOff, &value, sizeof(value));
				}
				const uint64_t slotAddr = base + lay.gotOff + plan.gotOff +
				                          static_cast<uint32_t>(gotSlot[static_cast<uint8_t>(hole.sym)]) * 8;
				uint8_t* at = buffer + offset + hole.offset;
				if (hole.kind == HoleKind::A64GotLoadPage21) {
					patchA64AdrpPage21(at, base + offset + hole.offset, slotAddr);
				} else {
					patchA64Imm12(at, slotAddr, 3);
				}
				continue;
			}
			applyHole(buffer, offset, hole, value, base);
#if defined(__x86_64__)
			if (hole.kind == HoleKind::Abs64 && isCodeAddressSym(hole.sym)) {
				relaxX64JumpAt(buffer, offset + hole.offset, offset + emitSize, base, value);
			}
#endif
		}
	}

	void patchAll(uint8_t* buffer, uint64_t base) const {
		for (size_t fn = 0; fn < program.functions.size(); ++fn) {
			const auto& codeStream = program.functions[fn].code;
			for (uint32_t word = 0; word < codeStream.size();) {
				stitchOne(buffer, lay.plans[fn][word], fn, word, base);
				word += instrWords(codeStream[word].op);
			}
		}
		// Synthetic stencils have no instruction context; they carry no holes.
		std::memcpy(buffer + lay.epilogueOff, stencils.epilogue.code, stencils.epilogue.size);
		std::memcpy(buffer + lay.unwindOff, stencils.unwind.code, stencils.unwind.size);
		if (kIsAArch64) {
			emitThunk(buffer + lay.thunkOff[0], reinterpret_cast<uint64_t>(&tbcJitPushFrame));
			emitThunk(buffer + lay.thunkOff[1], reinterpret_cast<uint64_t>(&tbcJitExtCall));
			emitThunk(buffer + lay.thunkOff[2], reinterpret_cast<uint64_t>(&tbcJitIndCall));
		}
		if (stencils.dataSize != 0) {
			std::memcpy(buffer + lay.dataOff, stencils.data, stencils.dataSize);
		}
	}
};

} // namespace

std::unique_ptr<TBCJitCode> stitchProgram(const TBCProgram& program, std::string* whyNot) {
	Stitcher stitcher(program, resolvedStencils());
	return stitcher.run(whyNot);
}

} // namespace nautilus::compiler::tbc::jit
