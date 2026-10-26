#pragma once

#include <cstdint>

namespace nautilus::nir {

/// `NONE` is 0xFFFFFFFF wherever a u32 field allows "no entry" (13-nir-spec.md, conventions).
constexpr uint32_t NONE = 0xFFFFFFFFu;

/// A 32-bit value reference (§3.2): a value id, a constant (bit 31 set), or NONE.
using Ref = uint32_t;

constexpr uint32_t CONST_BIT = 0x80000000u;
constexpr uint32_t MAX_VALUE_ID = 0x7FFFFFFEu - 0; // ids 0 .. 0x7FFFFFFE
constexpr uint32_t MAX_CONSTANTS = 0x7FFFFFFEu;

constexpr Ref makeConstRef(uint32_t constIndex) {
	return CONST_BIT | constIndex;
}
constexpr bool isNone(Ref r) {
	return r == NONE;
}
constexpr bool isConstRef(Ref r) {
	return r != NONE && r != 0xFFFFFFFEu && (r & CONST_BIT) != 0;
}
constexpr bool isValueRef(Ref r) {
	return r < 0x7FFFFFFFu;
}
/// The two reserved patterns (0x7FFFFFFF, 0xFFFFFFFE) must not occur.
constexpr bool isReservedRef(Ref r) {
	return r == 0x7FFFFFFFu || r == 0xFFFFFFFEu;
}
constexpr uint32_t constIndexOf(Ref r) {
	return r & ~CONST_BIT;
}

/// Unwind entries (§3.4).
constexpr uint32_t NO_UNWIND = 0xFFFFFFFFu;
constexpr uint32_t NO_CLEANUP = 0xFFFFFFFEu;

/// Opcodes below 256 are core, from 256 up extension operations (§3.1).
enum class Op : uint16_t {
#define NIR_OP(name, code, text, a, b, c, flags) name = code,
#include "nautilus/nir/Opcodes.def"
#undef NIR_OP
};
constexpr uint16_t FIRST_EXTENSION_OP = 256;

/// What the field a, b or c of an instruction holds (13-nir-spec.md §5).
enum class OperandKind : uint8_t {
	Unused,     ///< must be 0
	Value,      ///< value reference
	ValueOpt,   ///< value reference, or NONE
	Block,      ///< block id
	Callee,     ///< callee index
	Signature,  ///< signature index
	Alloca,     ///< function-local alloca slot
	Binding,    ///< binding index
	Data,       ///< data object index
	PointerOpt, ///< pointers index, or NONE
	Imm,        ///< immediate
	Extra,      ///< offset in the extra array
	Unwind,     ///< unwind entry (c of div, rem)
};

enum OpFlags : uint8_t {
	OpNone = 0,
	OpTerminator = 1 << 0,
	OpPreSSAOnly = 1 << 1,
	OpMayThrow = 1 << 2,
	OpEffect = 1 << 3, ///< ordered with other effects (§5.0)
	OpPure = 1 << 4,   ///< no effects, may be merged
};

/// 13-nir-spec.md §3.1: the 16-byte instruction record.
struct Inst {
	uint16_t op;
	uint8_t type;
	uint8_t flags;
	uint32_t a, b, c;

	bool operator==(const Inst&) const = default;
};
static_assert(sizeof(Inst) == 16 && alignof(Inst) == 4, "Inst is 16 bytes, 4-byte aligned");

struct OpInfo {
	const char* name;
	OperandKind a, b, c;
	uint8_t flags;
};

/// Static information of a core opcode; nullptr if the code is not a core opcode.
const OpInfo* coreOpInfo(uint16_t op);

constexpr bool isCoreOp(uint16_t op) {
	return op < FIRST_EXTENSION_OP;
}

} // namespace nautilus::nir
