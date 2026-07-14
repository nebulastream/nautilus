#pragma once

#include <cstdint>

// Freestanding instruction-word definitions, split out of TBCCode.hpp so the
// copy-and-patch stencil translation unit (jit/TBCStencilSource.cpp) can
// include them without dragging in <string>/<vector>/<unordered_map>. Keep
// this header dependency-free beyond <cstdint>.

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

} // namespace nautilus::compiler::tbc
