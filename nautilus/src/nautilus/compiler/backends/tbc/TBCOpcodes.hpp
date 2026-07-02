#pragma once

#include <cstdint>

namespace nautilus::compiler::tbc {

// The TBC opcode set is generated from the X-macro lists below. Every consumer
// (the Op enum, the three dispatch skins, the name table for the disassembler)
// expands the same lists, so they cannot drift.
//
// Numeric families expand over the canonical 10-type order
//   i8, i16, i32, i64, ui8, ui16, ui32, ui64, f32, f64
// and the lowering computes `family base + type index` instead of a per-type
// switch, so this order is load-bearing (see the static_asserts at the bottom).
// Type::b and Type::ptr are mapped onto this order at lowering time (b -> ui8
// for compares, ptr -> i64/ui64), mirroring the bc backend's choices.
//
// Each entry is X(NAME, (handlerExpression)). The handler expression is a
// value-op body `void(const Instr&, uint64_t*)` and is parenthesized so
// template arguments with commas survive macro expansion; consumers that only
// need the name (enum, name table) simply ignore it.

#define TBC_NUM10(X, NAME, FN)                                                                                         \
	X(NAME##_i8, (FN<int8_t>))                                                                                         \
	X(NAME##_i16, (FN<int16_t>))                                                                                       \
	X(NAME##_i32, (FN<int32_t>))                                                                                       \
	X(NAME##_i64, (FN<int64_t>))                                                                                       \
	X(NAME##_ui8, (FN<uint8_t>))                                                                                       \
	X(NAME##_ui16, (FN<uint16_t>))                                                                                     \
	X(NAME##_ui32, (FN<uint32_t>))                                                                                     \
	X(NAME##_ui64, (FN<uint64_t>))                                                                                     \
	X(NAME##_f32, (FN<float>))                                                                                         \
	X(NAME##_f64, (FN<double>))

#define TBC_INT8(X, NAME, FN)                                                                                          \
	X(NAME##_i8, (FN<int8_t>))                                                                                         \
	X(NAME##_i16, (FN<int16_t>))                                                                                       \
	X(NAME##_i32, (FN<int32_t>))                                                                                       \
	X(NAME##_i64, (FN<int64_t>))                                                                                       \
	X(NAME##_ui8, (FN<uint8_t>))                                                                                       \
	X(NAME##_ui16, (FN<uint16_t>))                                                                                     \
	X(NAME##_ui32, (FN<uint32_t>))                                                                                     \
	X(NAME##_ui64, (FN<uint64_t>))

// One row of the cast grid: SRC -> each of the 10 numeric types. The full grid
// is 10x10 in row-major (source-major) order so the lowering can compute
// `CAST_BASE + srcIdx * 10 + dstIdx`. Diagonal (identity) entries exist to
// keep the index math trivial; the lowering emits MOV for identity casts, so
// they are never dispatched.
#define TBC_CAST_ROW(X, S, SN)                                                                                         \
	X(CAST_##SN##_i8, (opCast<S, int8_t>))                                                                             \
	X(CAST_##SN##_i16, (opCast<S, int16_t>))                                                                           \
	X(CAST_##SN##_i32, (opCast<S, int32_t>))                                                                           \
	X(CAST_##SN##_i64, (opCast<S, int64_t>))                                                                           \
	X(CAST_##SN##_ui8, (opCast<S, uint8_t>))                                                                           \
	X(CAST_##SN##_ui16, (opCast<S, uint16_t>))                                                                         \
	X(CAST_##SN##_ui32, (opCast<S, uint32_t>))                                                                         \
	X(CAST_##SN##_ui64, (opCast<S, uint64_t>))                                                                         \
	X(CAST_##SN##_f32, (opCast<S, float>))                                                                             \
	X(CAST_##SN##_f64, (opCast<S, double>))

#define TBC_CAST_GRID(X)                                                                                               \
	TBC_CAST_ROW(X, int8_t, i8)                                                                                        \
	TBC_CAST_ROW(X, int16_t, i16)                                                                                      \
	TBC_CAST_ROW(X, int32_t, i32)                                                                                      \
	TBC_CAST_ROW(X, int64_t, i64)                                                                                      \
	TBC_CAST_ROW(X, uint8_t, ui8)                                                                                      \
	TBC_CAST_ROW(X, uint16_t, ui16)                                                                                    \
	TBC_CAST_ROW(X, uint32_t, ui32)                                                                                    \
	TBC_CAST_ROW(X, uint64_t, ui64)                                                                                    \
	TBC_CAST_ROW(X, float, f32)                                                                                        \
	TBC_CAST_ROW(X, double, f64)

// Small-constant materialization: writes the sign-extended 16-bit immediate in
// field `c` through the type's normal write semantics. Bool constants use the
// ui8 form; float/ptr constants always go through the constant image.
#define TBC_MOVIMM_LIST(X)                                                                                             \
	X(MOV_imm_i8, (opMovImm<int8_t>))                                                                                  \
	X(MOV_imm_i16, (opMovImm<int16_t>))                                                                                \
	X(MOV_imm_i32, (opMovImm<int32_t>))                                                                                \
	X(MOV_imm_i64, (opMovImm<int64_t>))                                                                                \
	X(MOV_imm_ui8, (opMovImm<uint8_t>))                                                                                \
	X(MOV_imm_ui16, (opMovImm<uint16_t>))                                                                              \
	X(MOV_imm_ui32, (opMovImm<uint32_t>))                                                                              \
	X(MOV_imm_ui64, (opMovImm<uint64_t>))

// Immediate-folded arithmetic (default on): right operand is a sign-extended
// 16-bit immediate in field `c`, replacing one register read + the separate
// constant materialization on the hot path (dominant loop-increment idiom).
#define TBC_IMM_ARITH_LIST(X)                                                                                          \
	X(ADD_imm_i32, (opAddImm<int32_t>))                                                                                \
	X(ADD_imm_i64, (opAddImm<int64_t>))                                                                                \
	X(SUB_imm_i32, (opSubImm<int32_t>))                                                                                \
	X(SUB_imm_i64, (opSubImm<int64_t>))                                                                                \
	X(MUL_imm_i32, (opMulImm<int32_t>))                                                                                \
	X(MUL_imm_i64, (opMulImm<int64_t>))

// Every 1-word value opcode: executes its body, then falls through to the next
// instruction word. Comparison families are contiguous in EQ,NE,LT,LE,GT,GE
// order (matching ir::CompareOperation::Comparator) so the lowering computes
// `EQ_i8 + comparator * 10 + typeIdx`.
#define TBC_VALUE_OPCODE_LIST(X)                                                                                       \
	X(NOP, (opNop))                                                                                                    \
	X(MOV, (opMov))                                                                                                    \
	TBC_MOVIMM_LIST(X)                                                                                                 \
	TBC_NUM10(X, ADD, opAdd)                                                                                           \
	TBC_NUM10(X, SUB, opSub)                                                                                           \
	TBC_NUM10(X, MUL, opMul)                                                                                           \
	TBC_NUM10(X, DIV, opDiv)                                                                                           \
	TBC_INT8(X, MOD, opMod)                                                                                            \
	TBC_NUM10(X, EQ, opEq)                                                                                             \
	TBC_NUM10(X, NE, opNe)                                                                                             \
	TBC_NUM10(X, LT, opLt)                                                                                             \
	TBC_NUM10(X, LE, opLe)                                                                                             \
	TBC_NUM10(X, GT, opGt)                                                                                             \
	TBC_NUM10(X, GE, opGe)                                                                                             \
	X(AND_b, (opAnd))                                                                                                  \
	X(OR_b, (opOr))                                                                                                    \
	X(NOT_b, (opNot))                                                                                                  \
	TBC_NUM10(X, LOAD, opLoad)                                                                                         \
	X(LOAD_b, (opLoad<bool>))                                                                                          \
	TBC_NUM10(X, STORE, opStore)                                                                                       \
	X(STORE_b, (opStore<bool>))                                                                                        \
	TBC_INT8(X, BAND, opBand)                                                                                          \
	TBC_INT8(X, BOR, opBor)                                                                                            \
	TBC_INT8(X, BXOR, opBxor)                                                                                          \
	TBC_INT8(X, SHL, opShl)                                                                                            \
	TBC_INT8(X, SHR, opShr)                                                                                            \
	X(BNOT_i64, (opBnot))                                                                                              \
	TBC_CAST_GRID(X)                                                                                                   \
	TBC_IMM_ARITH_LIST(X)

// Control opcodes with hand-written handlers in every dispatch skin (they
// modify ip / fp / the VM stack, or consume a second instruction word).
//   SELECT   a=dst, b=cond, c=trueReg;  word1.a=falseReg          (2 words)
//   JMP      signed 32-bit relative offset in b|c<<16             (1 word)
//   CJMP     a=cond, taken offset in b|c<<16, else fall through   (1 word)
//   RET      a=result register (kNoReg for void)                  (1 word)
//   CALL     a=dst (kNoReg for void), b=function idx, c=callsite  (1 word)
//   CALL_EXT a=dst, b=callsite idx                                (1 word)
//   CALL_IND a=dst, b=function-pointer register, c=callsite idx   (1 word)
//   HALT     terminates the dispatch loop of an entry frame       (1 word)
//   TRAP     never emitted; throws on dispatch (encoding bugs)    (1 word)
#define TBC_CONTROL_SIMPLE_LIST(X)                                                                                     \
	X(SELECT)                                                                                                          \
	X(JMP)                                                                                                             \
	X(CJMP)                                                                                                            \
	X(RET)                                                                                                             \
	X(CALL)                                                                                                            \
	X(CALL_EXT)                                                                                                        \
	X(CALL_IND)                                                                                                        \
	X(HALT)                                                                                                            \
	X(TRAP)

// Fused compare-and-branch superinstructions (default on): a=lhs, b=rhs,
// taken offset as signed 32-bit in word1.a|word1.b<<16, else fall through
// (2 words). Ordered pred-major over {i32,i64} so the flattener computes
// `CJMP_EQ_i32 + pred * 2 + isI64`.
#define TBC_CJMP_FUSED_LIST(X)                                                                                         \
	X(CJMP_EQ_i32, int32_t, ==)                                                                                        \
	X(CJMP_EQ_i64, int64_t, ==)                                                                                        \
	X(CJMP_NE_i32, int32_t, !=)                                                                                        \
	X(CJMP_NE_i64, int64_t, !=)                                                                                        \
	X(CJMP_LT_i32, int32_t, <)                                                                                         \
	X(CJMP_LT_i64, int64_t, <)                                                                                         \
	X(CJMP_LE_i32, int32_t, <=)                                                                                        \
	X(CJMP_LE_i64, int64_t, <=)                                                                                        \
	X(CJMP_GT_i32, int32_t, >)                                                                                         \
	X(CJMP_GT_i64, int64_t, >)                                                                                         \
	X(CJMP_GE_i32, int32_t, >=)                                                                                        \
	X(CJMP_GE_i64, int64_t, >=)

enum class Op : uint16_t {
#define TBC_ENUM_V(name, fn) name,
	TBC_VALUE_OPCODE_LIST(TBC_ENUM_V)
#undef TBC_ENUM_V
#define TBC_ENUM_C(name) name,
	TBC_CONTROL_SIMPLE_LIST(TBC_ENUM_C)
#undef TBC_ENUM_C
#define TBC_ENUM_F(name, ctype, cmp) name,
	    TBC_CJMP_FUSED_LIST(TBC_ENUM_F)
#undef TBC_ENUM_F
	        COUNT,
};

constexpr uint16_t opIndex(Op op) {
	return static_cast<uint16_t>(op);
}

constexpr uint16_t kOpCount = opIndex(Op::COUNT);

// The lowering's base + index arithmetic relies on the family layout produced
// by the lists above; pin the corners so a reordering cannot silently corrupt
// the opcode math.
static_assert(opIndex(Op::ADD_f64) == opIndex(Op::ADD_i8) + 9);
static_assert(opIndex(Op::MOD_ui64) == opIndex(Op::MOD_i8) + 7);
static_assert(opIndex(Op::NE_i8) == opIndex(Op::EQ_i8) + 10);
static_assert(opIndex(Op::GE_f64) == opIndex(Op::EQ_i8) + 59);
static_assert(opIndex(Op::LOAD_b) == opIndex(Op::LOAD_i8) + 10);
static_assert(opIndex(Op::STORE_b) == opIndex(Op::STORE_i8) + 10);
static_assert(opIndex(Op::CAST_f64_f64) == opIndex(Op::CAST_i8_i8) + 99);
static_assert(opIndex(Op::MOV_imm_ui64) == opIndex(Op::MOV_imm_i8) + 7);
static_assert(opIndex(Op::CJMP_GE_i64) == opIndex(Op::CJMP_EQ_i32) + 11);
static_assert(kOpCount < 65535);

const char* opName(Op op);

} // namespace nautilus::compiler::tbc
