#pragma once

#include "nautilus/compiler/backends/tbc/TBCOpcodes.hpp"
#include "nautilus/tracing/Types.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace nautilus::compiler::tbc {

/// Register index sentinel: "no register" (void results, unused fields).
constexpr uint16_t kNoReg = 0xFFFF;

/// One fixed-width instruction word. Fields are opcode-specific; the common
/// convention for value ops is a=dst, b=src1, c=src2 (or c=imm16). Branch
/// offsets are signed 32-bit relative instruction counts packed as b|c<<16
/// (or word1.a|word1.b<<16 for 2-word fused branches).
struct Instr {
	uint16_t op;
	uint16_t a;
	uint16_t b;
	uint16_t c;
};

static_assert(sizeof(Instr) == 8);

constexpr int32_t packedOffset(const Instr& i) {
	return static_cast<int32_t>(static_cast<uint32_t>(i.b) | (static_cast<uint32_t>(i.c) << 16));
}

constexpr int32_t packedOffsetLoHi(const Instr& i) {
	return static_cast<int32_t>(static_cast<uint32_t>(i.a) | (static_cast<uint32_t>(i.b) << 16));
}

/// Per-call-site record. The instruction stream only carries the record index;
/// argument registers, types, and the target live here. Used by CALL (internal,
/// interpreter-native), CALL_EXT (external via dyncall), and CALL_IND (register
/// target, internal or external).
struct CallSite {
	void* target = nullptr;         // external function pointer (CALL_EXT)
	uint32_t internalFnIdx = ~0u;   // callee index (CALL)
	Type returnType = Type::v;
	std::vector<Type> argTypes;     // callee signature, for dyncall marshaling
	std::vector<uint16_t> argRegs;  // caller registers holding the arguments
};

/// One lowered function: a flat instruction stream plus its frame metadata.
/// Frame layout (in 8-byte slots, pushed on the contiguous VM stack):
///   base[0..2]                     header: caller fp, return ip, caller dst reg
///   fp[0 .. regSlots)              value + constant registers (fp = base + 3)
///   fp[regSlots .. frameSlots-3)   alloca area (16-aligned at runtime)
struct TBCFunction {
	std::string name;
	std::vector<Instr> code;
	/// Initial register contents, memcpy'd into the frame on every call.
	/// Constant slots hold their values; everything else is zero.
	std::vector<uint64_t> initImage;
	uint32_t regSlots = 0;
	uint32_t frameSlots = 0; // 3 header + regSlots + alloca slots (incl. alignment slack)
	uint32_t allocaBytes = 0;
	std::vector<std::pair<uint16_t, uint32_t>> allocaRegs; // register -> byte offset in the alloca area
	std::vector<uint16_t> argRegs;
	std::vector<Type> argTypes;
	Type returnType = Type::v;

	std::string toString() const;
};

enum class DispatchMode : uint8_t { Tailcall, Goto, Switch };

/// A whole compiled module: all functions plus the program-wide call-site
/// table. Heap-allocated once and never moved afterwards: escaping internal
/// function pointers (see TBCTrampoline.hpp) bind trampoline slots to this
/// object's address, released again by the destructor.
struct TBCProgram {
	TBCProgram() = default;
	TBCProgram(const TBCProgram&) = delete;
	TBCProgram& operator=(const TBCProgram&) = delete;
	~TBCProgram();

	std::vector<TBCFunction> functions;
	std::unordered_map<std::string, uint32_t> functionIndex;
	std::vector<CallSite> callsites;
	uint64_t minStackSlots = 0;
	DispatchMode dispatch = DispatchMode::Switch;
};

} // namespace nautilus::compiler::tbc
