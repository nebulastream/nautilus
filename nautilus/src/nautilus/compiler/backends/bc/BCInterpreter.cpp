
#include <cassert>
#include <nautilus/compiler/backends/bc/BCInterpreter.hpp>
#include <nautilus/compiler/backends/bc/Dyncall.hpp>
#include <sstream>
#include <utility>

namespace nautilus::compiler::bc {
void regMov(const OpCode& c, RegisterFile& regs) {
	regs[c.output] = regs[c.reg1];
}

void dyncallReset(const OpCode&, RegisterFile&) {
	Dyncall::getVM().reset();
}

void dyncallArgB(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<bool>(regs, op.reg1);
	Dyncall::getVM().addArgB(value);
}

void dyncallArgI8(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<int8_t>(regs, op.reg1);
	Dyncall::getVM().addArgI8(value);
}

void dyncallArgI16(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<int16_t>(regs, op.reg1);
	Dyncall::getVM().addArgI16(value);
}

void dyncallArgI32(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<int32_t>(regs, op.reg1);
	Dyncall::getVM().addArgI32(value);
}

void dyncallArgI64(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<int64_t>(regs, op.reg1);
	Dyncall::getVM().addArgI64(value);
}

void dyncallArgF(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<float>(regs, op.reg1);
	Dyncall::getVM().addArgF(value);
}

void dyncallArgD(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<double>(regs, op.reg1);
	Dyncall::getVM().addArgD(value);
}

void dyncallArgPtr(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<void*>(regs, op.reg1);
	Dyncall::getVM().addArgPtr(value);
}

void dyncallCallV(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<void*>(regs, op.reg1);
	Dyncall::getVM().callVoid(value);
}

void dyncallCallB(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callB(address);
	writeReg<bool>(regs, op.output, returnValue);
}

void dyncallCallI8(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callI8(address);
	writeReg<int8_t>(regs, op.output, returnValue);
}

void dyncallCallI16(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callI16(address);
	writeReg<int16_t>(regs, op.output, returnValue);
}

void dyncallCallI32(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callI32(address);
	writeReg<int32_t>(regs, op.output, returnValue);
}

void dyncallCallI64(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callI64(address);
	writeReg<int64_t>(regs, op.output, returnValue);
}

void dyncallCallPtr(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callPtr(address);
	writeReg<void*>(regs, op.output, returnValue);
}

void dyncallCallf(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callF(address);
	writeReg<float>(regs, op.output, returnValue);
}

void dyncallCalld(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callD(address);
	writeReg<double>(regs, op.output, returnValue);
}

// Single source of truth mapping every ByteCode to its handler, listed in the
// exact order of the ByteCode enum. This list generates BOTH the function-pointer
// dispatch table (OpTable, used by the "call" dispatch mode) and the inlined
// switch in execute() (the "switch" dispatch mode). Generating both from one
// list guarantees they cannot diverge: a wrong or missing entry corrupts the
// default "call" path too, which every existing test exercises. The handler is
// taken via __VA_ARGS__ so commas inside template arguments (e.g.
// cast<float, int8_t>) are not split into separate macro arguments.
#define NAUTILUS_BC_OPCODE_LIST(X)                                                                                     \
	X(REG_MOV, regMov)                                                                                                 \
	/* add */                                                                                                          \
	X(ADD_i8, add<int8_t>)                                                                                             \
	X(ADD_i16, add<int16_t>)                                                                                           \
	X(ADD_i32, add<int32_t>)                                                                                           \
	X(ADD_i64, add<int64_t>)                                                                                           \
	X(ADD_ui8, add<uint8_t>)                                                                                           \
	X(ADD_ui16, add<uint16_t>)                                                                                         \
	X(ADD_ui32, add<uint32_t>)                                                                                         \
	X(ADD_ui64, add<uint64_t>)                                                                                         \
	X(ADD_f, add<float>)                                                                                               \
	X(ADD_d, add<double>)                                                                                              \
	/* sub */                                                                                                          \
	X(SUB_i8, sub<int8_t>)                                                                                             \
	X(SUB_i16, sub<int16_t>)                                                                                           \
	X(SUB_i32, sub<int32_t>)                                                                                           \
	X(SUB_i64, sub<int64_t>)                                                                                           \
	X(SUB_ui8, sub<uint8_t>)                                                                                           \
	X(SUB_ui16, sub<uint16_t>)                                                                                         \
	X(SUB_ui32, sub<uint32_t>)                                                                                         \
	X(SUB_ui64, sub<uint64_t>)                                                                                         \
	X(SUB_f, sub<float>)                                                                                               \
	X(SUB_d, sub<double>)                                                                                              \
	/* mul */                                                                                                          \
	X(MUL_i8, mul<int8_t>)                                                                                             \
	X(MUL_i16, mul<int16_t>)                                                                                           \
	X(MUL_i32, mul<int32_t>)                                                                                           \
	X(MUL_i64, mul<int64_t>)                                                                                           \
	X(MUL_ui8, mul<uint8_t>)                                                                                           \
	X(MUL_ui16, mul<uint16_t>)                                                                                         \
	X(MUL_ui32, mul<uint32_t>)                                                                                         \
	X(MUL_ui64, mul<uint64_t>)                                                                                         \
	X(MUL_f, mul<float>)                                                                                               \
	X(MUL_d, mul<double>)                                                                                              \
	/* div */                                                                                                          \
	X(DIV_i8, div<int8_t>)                                                                                             \
	X(DIV_i16, div<int16_t>)                                                                                           \
	X(DIV_i32, div<int32_t>)                                                                                           \
	X(DIV_i64, div<int64_t>)                                                                                           \
	X(DIV_ui8, div<uint8_t>)                                                                                           \
	X(DIV_ui16, div<uint16_t>)                                                                                         \
	X(DIV_ui32, div<uint32_t>)                                                                                         \
	X(DIV_ui64, div<uint64_t>)                                                                                         \
	X(DIV_f, div<float>)                                                                                               \
	X(DIV_d, div<double>)                                                                                              \
	/* mod */                                                                                                          \
	X(MOD_i8, mod<int8_t>)                                                                                             \
	X(MOD_i16, mod<int16_t>)                                                                                           \
	X(MOD_i32, mod<int32_t>)                                                                                           \
	X(MOD_i64, mod<int64_t>)                                                                                           \
	X(MOD_ui8, mod<uint8_t>)                                                                                           \
	X(MOD_ui16, mod<uint16_t>)                                                                                         \
	X(MOD_ui32, mod<uint32_t>)                                                                                         \
	X(MOD_ui64, mod<uint64_t>)                                                                                         \
	/* equals */                                                                                                       \
	X(EQ_i8, equals<int8_t>)                                                                                           \
	X(EQ_i16, equals<int16_t>)                                                                                         \
	X(EQ_i32, equals<int32_t>)                                                                                         \
	X(EQ_i64, equals<int64_t>)                                                                                         \
	X(EQ_ui8, equals<uint8_t>)                                                                                         \
	X(EQ_ui16, equals<uint16_t>)                                                                                       \
	X(EQ_ui32, equals<uint32_t>)                                                                                       \
	X(EQ_ui64, equals<uint64_t>)                                                                                       \
	X(EQ_f, equals<float>)                                                                                             \
	X(EQ_d, equals<double>)                                                                                            \
	X(EQ_b, equals<bool>)                                                                                              \
	/* less than */                                                                                                    \
	X(LESS_THAN_i8, lessThan<int8_t>)                                                                                  \
	X(LESS_THAN_i16, lessThan<int16_t>)                                                                                \
	X(LESS_THAN_i32, lessThan<int32_t>)                                                                                \
	X(LESS_THAN_i64, lessThan<int64_t>)                                                                                \
	X(LESS_THAN_ui8, lessThan<uint8_t>)                                                                                \
	X(LESS_THAN_ui16, lessThan<uint16_t>)                                                                              \
	X(LESS_THAN_ui32, lessThan<uint32_t>)                                                                              \
	X(LESS_THAN_ui64, lessThan<uint64_t>)                                                                              \
	X(LESS_THAN_f, lessThan<float>)                                                                                    \
	X(LESS_THAN_d, lessThan<double>)                                                                                   \
	/* less than equals */                                                                                             \
	X(LESS_THAN_EQUALS_i8, lessThanEquals<int8_t>)                                                                     \
	X(LESS_THAN_EQUALS_i16, lessThanEquals<int16_t>)                                                                   \
	X(LESS_THAN_EQUALS_i32, lessThanEquals<int32_t>)                                                                   \
	X(LESS_THAN_EQUALS_i64, lessThanEquals<int64_t>)                                                                   \
	X(LESS_THAN_EQUALS_ui8, lessThanEquals<uint8_t>)                                                                   \
	X(LESS_THAN_EQUALS_ui16, lessThanEquals<uint16_t>)                                                                 \
	X(LESS_THAN_EQUALS_ui32, lessThanEquals<uint32_t>)                                                                 \
	X(LESS_THAN_EQUALS_ui64, lessThanEquals<uint64_t>)                                                                 \
	X(LESS_THAN_EQUALS_f, lessThanEquals<float>)                                                                       \
	X(LESS_THAN_EQUALS_d, lessThanEquals<double>)                                                                      \
	/* greater than */                                                                                                 \
	X(GREATER_THAN_i8, greaterThan<int8_t>)                                                                            \
	X(GREATER_THAN_i16, greaterThan<int16_t>)                                                                          \
	X(GREATER_THAN_i32, greaterThan<int32_t>)                                                                          \
	X(GREATER_THAN_i64, greaterThan<int64_t>)                                                                          \
	X(GREATER_THAN_ui8, greaterThan<uint8_t>)                                                                          \
	X(GREATER_THAN_ui16, greaterThan<uint16_t>)                                                                        \
	X(GREATER_THAN_ui32, greaterThan<uint32_t>)                                                                        \
	X(GREATER_THAN_ui64, greaterThan<uint64_t>)                                                                        \
	X(GREATER_THAN_f, greaterThan<float>)                                                                              \
	X(GREATER_THAN_d, greaterThan<double>)                                                                             \
	/* greater than equals */                                                                                          \
	X(GREATER_THAN_EQUALS_i8, greaterThanEquals<int8_t>)                                                               \
	X(GREATER_THAN_EQUALS_i16, greaterThanEquals<int16_t>)                                                             \
	X(GREATER_THAN_EQUALS_i32, greaterThanEquals<int32_t>)                                                             \
	X(GREATER_THAN_EQUALS_i64, greaterThanEquals<int64_t>)                                                             \
	X(GREATER_THAN_EQUALS_ui8, greaterThanEquals<uint8_t>)                                                             \
	X(GREATER_THAN_EQUALS_ui16, greaterThanEquals<uint16_t>)                                                           \
	X(GREATER_THAN_EQUALS_ui32, greaterThanEquals<uint32_t>)                                                           \
	X(GREATER_THAN_EQUALS_ui64, greaterThanEquals<uint64_t>)                                                           \
	X(GREATER_THAN_EQUALS_f, greaterThanEquals<float>)                                                                 \
	X(GREATER_THAN_EQUALS_d, greaterThanEquals<double>)                                                                \
	/* not equals */                                                                                                   \
	X(NOT_EQUALS_b, notEquals<bool>)                                                                                   \
	X(NOT_EQUALS_i8, notEquals<int8_t>)                                                                                \
	X(NOT_EQUALS_i16, notEquals<int16_t>)                                                                              \
	X(NOT_EQUALS_i32, notEquals<int32_t>)                                                                              \
	X(NOT_EQUALS_i64, notEquals<int64_t>)                                                                              \
	X(NOT_EQUALS_ui8, notEquals<uint8_t>)                                                                              \
	X(NOT_EQUALS_ui16, notEquals<uint16_t>)                                                                            \
	X(NOT_EQUALS_ui32, notEquals<uint32_t>)                                                                            \
	X(NOT_EQUALS_ui64, notEquals<uint64_t>)                                                                            \
	X(NOT_EQUALS_f, notEquals<float>)                                                                                  \
	X(NOT_EQUALS_d, notEquals<double>)                                                                                 \
	/* load */                                                                                                         \
	X(LOAD_i8, load<int8_t>)                                                                                           \
	X(LOAD_i16, load<int16_t>)                                                                                         \
	X(LOAD_i32, load<int32_t>)                                                                                         \
	X(LOAD_i64, load<int64_t>)                                                                                         \
	X(LOAD_ui8, load<uint8_t>)                                                                                         \
	X(LOAD_ui16, load<uint16_t>)                                                                                       \
	X(LOAD_ui32, load<uint32_t>)                                                                                       \
	X(LOAD_ui64, load<uint64_t>)                                                                                       \
	X(LOAD_f, load<float>)                                                                                             \
	X(LOAD_d, load<double>)                                                                                            \
	X(LOAD_b, load<bool>)                                                                                              \
	/* store */                                                                                                        \
	X(STORE_i8, store<int8_t>)                                                                                         \
	X(STORE_i16, store<int16_t>)                                                                                       \
	X(STORE_i32, store<int32_t>)                                                                                       \
	X(STORE_i64, store<int64_t>)                                                                                       \
	X(STORE_ui8, store<uint8_t>)                                                                                       \
	X(STORE_ui16, store<uint16_t>)                                                                                     \
	X(STORE_ui32, store<uint32_t>)                                                                                     \
	X(STORE_ui64, store<uint64_t>)                                                                                     \
	X(STORE_f, store<float>)                                                                                           \
	X(STORE_d, store<double>)                                                                                          \
	X(STORE_b, store<bool>)                                                                                            \
	/* logical */                                                                                                      \
	X(AND_b, andOp<bool>)                                                                                              \
	X(OR_b, orOp<bool>)                                                                                                \
	X(NOT_b, notOp<bool>)                                                                                              \
	/* casts: int to int */                                                                                            \
	X(CAST_i8_i16, cast<int8_t, int16_t>)                                                                              \
	X(CAST_i8_i32, cast<int8_t, int32_t>)                                                                              \
	X(CAST_i8_i64, cast<int8_t, int64_t>)                                                                              \
	X(CAST_i16_i8, cast<int16_t, int8_t>)                                                                              \
	X(CAST_i16_i32, cast<int16_t, int32_t>)                                                                            \
	X(CAST_i16_i64, cast<int16_t, int64_t>)                                                                            \
	X(CAST_i32_i8, cast<int32_t, int8_t>)                                                                              \
	X(CAST_i32_i16, cast<int32_t, int16_t>)                                                                            \
	X(CAST_i32_i64, cast<int32_t, int64_t>)                                                                            \
	X(CAST_i64_i8, cast<int64_t, int8_t>)                                                                              \
	X(CAST_i64_i16, cast<int64_t, int16_t>)                                                                            \
	X(CAST_i64_i32, cast<int64_t, int32_t>)                                                                            \
	/* casts: uint to int */                                                                                           \
	X(CAST_ui8_i8, cast<uint8_t, int8_t>)                                                                              \
	X(CAST_ui8_i16, cast<uint8_t, int16_t>)                                                                            \
	X(CAST_ui8_i32, cast<uint8_t, int32_t>)                                                                            \
	X(CAST_ui8_i64, cast<uint8_t, int64_t>)                                                                            \
	X(CAST_ui16_i8, cast<uint16_t, int8_t>)                                                                            \
	X(CAST_ui16_i16, cast<uint16_t, int16_t>)                                                                          \
	X(CAST_ui16_i32, cast<uint16_t, int32_t>)                                                                          \
	X(CAST_ui16_i64, cast<uint16_t, int64_t>)                                                                          \
	X(CAST_ui32_i8, cast<uint32_t, int8_t>)                                                                            \
	X(CAST_ui32_i16, cast<uint32_t, int16_t>)                                                                          \
	X(CAST_ui32_i32, cast<uint32_t, int32_t>)                                                                          \
	X(CAST_ui32_i64, cast<uint32_t, int64_t>)                                                                          \
	X(CAST_ui64_i8, cast<uint64_t, int8_t>)                                                                            \
	X(CAST_ui64_i16, cast<uint64_t, int16_t>)                                                                          \
	X(CAST_ui64_i32, cast<uint64_t, int32_t>)                                                                          \
	X(CAST_ui64_i64, cast<uint64_t, int64_t>)                                                                          \
	/* casts: uint to uint */                                                                                          \
	X(CAST_ui8_ui16, cast<uint8_t, uint16_t>)                                                                          \
	X(CAST_ui8_ui32, cast<uint8_t, uint32_t>)                                                                          \
	X(CAST_ui8_ui64, cast<uint8_t, uint64_t>)                                                                          \
	X(CAST_ui16_ui8, cast<uint16_t, uint8_t>)                                                                          \
	X(CAST_ui16_ui32, cast<uint16_t, uint32_t>)                                                                        \
	X(CAST_ui16_ui64, cast<uint16_t, uint64_t>)                                                                        \
	X(CAST_ui32_ui8, cast<uint32_t, uint8_t>)                                                                          \
	X(CAST_ui32_ui16, cast<uint32_t, uint16_t>)                                                                        \
	X(CAST_ui32_ui64, cast<uint32_t, uint64_t>)                                                                        \
	X(CAST_ui64_ui8, cast<uint64_t, uint8_t>)                                                                          \
	X(CAST_ui64_ui16, cast<uint64_t, uint16_t>)                                                                        \
	X(CAST_ui64_ui32, cast<uint64_t, uint32_t>)                                                                        \
	/* casts: int to uint */                                                                                           \
	X(CAST_i8_ui8, cast<int8_t, uint8_t>)                                                                              \
	X(CAST_i8_ui16, cast<int8_t, uint16_t>)                                                                            \
	X(CAST_i8_ui32, cast<int8_t, uint32_t>)                                                                            \
	X(CAST_i8_ui64, cast<int8_t, uint64_t>)                                                                            \
	X(CAST_i16_ui8, cast<int16_t, uint8_t>)                                                                            \
	X(CAST_i16_ui16, cast<int16_t, uint16_t>)                                                                          \
	X(CAST_i16_ui32, cast<int16_t, uint32_t>)                                                                          \
	X(CAST_i16_ui64, cast<int16_t, uint64_t>)                                                                          \
	X(CAST_i32_ui8, cast<int32_t, uint8_t>)                                                                            \
	X(CAST_i32_ui16, cast<int32_t, uint16_t>)                                                                          \
	X(CAST_i32_ui32, cast<int32_t, uint32_t>)                                                                          \
	X(CAST_i32_ui64, cast<int32_t, uint64_t>)                                                                          \
	X(CAST_i64_ui8, cast<int64_t, uint8_t>)                                                                            \
	X(CAST_i64_ui16, cast<int64_t, uint16_t>)                                                                          \
	X(CAST_i64_ui32, cast<int64_t, uint32_t>)                                                                          \
	X(CAST_i64_ui64, cast<int64_t, uint64_t>)                                                                          \
	/* casts: float to int */                                                                                          \
	X(CAST_f_i8, cast<float, int8_t>)                                                                                  \
	X(CAST_f_i16, cast<float, int16_t>)                                                                                \
	X(CAST_f_i32, cast<float, int32_t>)                                                                                \
	X(CAST_f_i64, cast<float, int64_t>)                                                                                \
	X(CAST_f_ui8, cast<float, uint8_t>)                                                                                \
	X(CAST_f_ui16, cast<float, uint16_t>)                                                                              \
	X(CAST_f_ui32, cast<float, uint32_t>)                                                                              \
	X(CAST_f_ui64, cast<float, uint64_t>)                                                                              \
	X(CAST_f_d, cast<float, double>)                                                                                   \
	/* casts: double to int */                                                                                         \
	X(CAST_d_i8, cast<double, int8_t>)                                                                                 \
	X(CAST_d_i16, cast<double, int16_t>)                                                                               \
	X(CAST_d_i32, cast<double, int32_t>)                                                                               \
	X(CAST_d_i64, cast<double, int64_t>)                                                                               \
	X(CAST_d_ui8, cast<double, uint8_t>)                                                                               \
	X(CAST_d_ui16, cast<double, uint16_t>)                                                                             \
	X(CAST_d_ui32, cast<double, uint32_t>)                                                                             \
	X(CAST_d_ui64, cast<double, uint64_t>)                                                                             \
	X(CAST_d_f, cast<double, float>)                                                                                   \
	/* casts: int to float */                                                                                          \
	X(CAST_i8_f, cast<int8_t, float>)                                                                                  \
	X(CAST_i8_d, cast<int8_t, double>)                                                                                 \
	X(CAST_i16_f, cast<int16_t, float>)                                                                                \
	X(CAST_i16_d, cast<int16_t, double>)                                                                               \
	X(CAST_i32_f, cast<int32_t, float>)                                                                                \
	X(CAST_i32_d, cast<int32_t, double>)                                                                               \
	X(CAST_i64_f, cast<int64_t, float>)                                                                                \
	X(CAST_i64_d, cast<int64_t, double>)                                                                               \
	/* casts: uint to float */                                                                                         \
	X(CAST_ui8_f, cast<uint8_t, float>)                                                                                \
	X(CAST_ui8_d, cast<uint8_t, double>)                                                                               \
	X(CAST_ui16_f, cast<uint16_t, float>)                                                                              \
	X(CAST_ui16_d, cast<uint16_t, double>)                                                                             \
	X(CAST_ui32_f, cast<uint32_t, float>)                                                                              \
	X(CAST_ui32_d, cast<uint32_t, double>)                                                                             \
	X(CAST_ui64_f, cast<uint64_t, float>)                                                                              \
	X(CAST_ui64_d, cast<uint64_t, double>)                                                                             \
	/* dynamic function calls using dyncall.h */                                                                       \
	X(DYNCALL_reset, dyncallReset)                                                                                     \
	X(DYNCALL_arg_b, dyncallArgB)                                                                                      \
	X(DYNCALL_arg_i8, dyncallArgI8)                                                                                    \
	X(DYNCALL_arg_i16, dyncallArgI16)                                                                                  \
	X(DYNCALL_arg_i32, dyncallArgI32)                                                                                  \
	X(DYNCALL_arg_i64, dyncallArgI64)                                                                                  \
	X(DYNCALL_arg_f, dyncallArgF)                                                                                      \
	X(DYNCALL_arg_d, dyncallArgD)                                                                                      \
	X(DYNCALL_arg_ptr, dyncallArgPtr)                                                                                  \
	X(DYNCALL_call_v, dyncallCallV)                                                                                    \
	X(DYNCALL_call_b, dyncallCallB)                                                                                    \
	X(DYNCALL_call_i8, dyncallCallI8)                                                                                  \
	X(DYNCALL_call_i16, dyncallCallI16)                                                                                \
	X(DYNCALL_call_i32, dyncallCallI32)                                                                                \
	X(DYNCALL_call_i64, dyncallCallI64)                                                                                \
	X(DYNCALL_call_ptr, dyncallCallPtr)                                                                                \
	X(DYNCALL_call_f, dyncallCallf)                                                                                    \
	X(DYNCALL_call_d, dyncallCalld)                                                                                    \
	/* bitwise: band */                                                                                                \
	X(BAND_i8, bitwiseAnd<int8_t>)                                                                                     \
	X(BAND_i16, bitwiseAnd<int16_t>)                                                                                   \
	X(BAND_i32, bitwiseAnd<int32_t>)                                                                                   \
	X(BAND_i64, bitwiseAnd<int64_t>)                                                                                   \
	X(BAND_ui8, bitwiseAnd<uint8_t>)                                                                                   \
	X(BAND_ui16, bitwiseAnd<uint16_t>)                                                                                 \
	X(BAND_ui32, bitwiseAnd<uint32_t>)                                                                                 \
	X(BAND_ui64, bitwiseAnd<uint64_t>)                                                                                 \
	/* bitwise: bor */                                                                                                 \
	X(BOR_i8, bitwiseOr<int8_t>)                                                                                       \
	X(BOR_i16, bitwiseOr<int16_t>)                                                                                     \
	X(BOR_i32, bitwiseOr<int32_t>)                                                                                     \
	X(BOR_i64, bitwiseOr<int64_t>)                                                                                     \
	X(BOR_ui8, bitwiseOr<uint8_t>)                                                                                     \
	X(BOR_ui16, bitwiseOr<uint16_t>)                                                                                   \
	X(BOR_ui32, bitwiseOr<uint32_t>)                                                                                   \
	X(BOR_ui64, bitwiseOr<uint64_t>)                                                                                   \
	/* bitwise: bxor */                                                                                                \
	X(BXOR_i8, bitwiseXOr<int8_t>)                                                                                     \
	X(BXOR_i16, bitwiseXOr<int16_t>)                                                                                   \
	X(BXOR_i32, bitwiseXOr<int32_t>)                                                                                   \
	X(BXOR_i64, bitwiseXOr<int64_t>)                                                                                   \
	X(BXOR_ui8, bitwiseXOr<uint8_t>)                                                                                   \
	X(BXOR_ui16, bitwiseXOr<uint16_t>)                                                                                 \
	X(BXOR_ui32, bitwiseXOr<uint32_t>)                                                                                 \
	X(BXOR_ui64, bitwiseXOr<uint64_t>)                                                                                 \
	/* bitwise: blsh */                                                                                                \
	X(BLSH_i8, bitwiseLSH<int8_t>)                                                                                     \
	X(BLSH_i16, bitwiseLSH<int16_t>)                                                                                   \
	X(BLSH_i32, bitwiseLSH<int32_t>)                                                                                   \
	X(BLSH_i64, bitwiseLSH<int64_t>)                                                                                   \
	X(BLSH_ui8, bitwiseLSH<uint8_t>)                                                                                   \
	X(BLSH_ui16, bitwiseLSH<uint16_t>)                                                                                 \
	X(BLSH_ui32, bitwiseLSH<uint32_t>)                                                                                 \
	X(BLSH_ui64, bitwiseLSH<uint64_t>)                                                                                 \
	/* bitwise: brsh */                                                                                                \
	X(BRSH_i8, bitwiseRSH<int8_t>)                                                                                     \
	X(BRSH_i16, bitwiseRSH<int16_t>)                                                                                   \
	X(BRSH_i32, bitwiseRSH<int32_t>)                                                                                   \
	X(BRSH_i64, bitwiseRSH<int64_t>)                                                                                   \
	X(BRSH_ui8, bitwiseRSH<uint8_t>)                                                                                   \
	X(BRSH_ui16, bitwiseRSH<uint16_t>)                                                                                 \
	X(BRSH_ui32, bitwiseRSH<uint32_t>)                                                                                 \
	X(BRSH_ui64, bitwiseRSH<uint64_t>)                                                                                 \
	/* bitwise: bnot */                                                                                                \
	X(BNEGATE_I64, bitwiseNot<int64_t>)                                                                                \
	/* select */                                                                                                       \
	X(SELECT_i8, selectOp<int8_t>)                                                                                     \
	X(SELECT_i16, selectOp<int16_t>)                                                                                   \
	X(SELECT_i32, selectOp<int32_t>)                                                                                   \
	X(SELECT_i64, selectOp<int64_t>)                                                                                   \
	X(SELECT_ui8, selectOp<uint8_t>)                                                                                   \
	X(SELECT_ui16, selectOp<uint16_t>)                                                                                 \
	X(SELECT_ui32, selectOp<uint32_t>)                                                                                 \
	X(SELECT_ui64, selectOp<uint64_t>)                                                                                 \
	X(SELECT_f, selectOp<float>)                                                                                       \
	X(SELECT_d, selectOp<double>)                                                                                      \
	X(SELECT_b, selectOp<bool>)                                                                                        \
	X(SELECT_ptr, selectOp<void*>)

static Operation* OpTable[] = {
#define NAUTILUS_BC_TABLE_ENTRY(name, ...) (Operation*) __VA_ARGS__,
    NAUTILUS_BC_OPCODE_LIST(NAUTILUS_BC_TABLE_ENTRY)
#undef NAUTILUS_BC_TABLE_ENTRY
};

FunctionCallTarget::FunctionCallTarget(std::vector<std::pair<short, Type>> arguments, void* functionPtr)
    : arguments(std::move(arguments)), functionPtr(functionPtr) {
}

DispatchMode parseDispatchMode(const std::string& value) {
	if (value == "switch") {
		return DispatchMode::Switch;
	}
	return DispatchMode::Call;
}

BCInterpreter::BCInterpreter(Code code, RegisterFile registerFile, DispatchMode dispatchMode)
    : code(std::move(code)), registerFile(std::move(registerFile)), dispatchMode(dispatchMode) {
	// The register file was built against a temporary Code instance whose allocaBuffers
	// may have been copied (and thus relocated) before reaching this constructor.
	// Re-point each alloca register at the actual buffer in *this* code object.
	for (const auto& [reg, bufIdx] : this->code.allocaRegisterMap) {
		this->registerFile[reg] = reinterpret_cast<int64_t>(this->code.allocaBuffers[bufIdx].data());
	}
}

BCExecutable::BCExecutable(std::unordered_map<std::string, void*> functionPtrs,
                           std::vector<std::unique_ptr<BCCallbackData>> callbackData,
                           std::vector<DCCallback*> callbacks)
    : functionPtrs_(std::move(functionPtrs)), callbackData_(std::move(callbackData)), callbacks_(std::move(callbacks)) {
}

BCExecutable::~BCExecutable() {
	for (auto* cb : callbacks_) {
		dcbFreeCallback(cb);
	}
}

void* BCExecutable::getInvocableFunctionPtr(const std::string& member) {
	auto it = functionPtrs_.find(member);
	if (it != functionPtrs_.end()) {
		return it->second;
	}
	return nullptr;
}

bool BCExecutable::hasInvocableFunctionPtr() {
	return true;
}

int64_t BCInterpreter::invoke(DCArgs* args, const std::vector<Type>& argTypes) {
	// Per-invocation copy of the register file (thread-safe + reentrant).
	RegisterFile regs = registerFile;

	// Per-invocation alloca buffers, zeroed for a clean stack frame.
	std::vector<std::vector<uint8_t>> localAllocaBuffers(code.allocaBuffers.size());
	for (const auto& [reg, bufIdx] : code.allocaRegisterMap) {
		localAllocaBuffers[bufIdx].resize(code.allocaBuffers[bufIdx].size(), 0);
		regs[reg] = reinterpret_cast<int64_t>(localAllocaBuffers[bufIdx].data());
	}

	// Read arguments from DCArgs directly into the local register file.
	for (size_t i = 0; i < argTypes.size(); i++) {
		auto reg = code.arguments[i];
		switch (argTypes[i]) {
		case Type::b:
			// dyncall's dcbArgBool returns DCbool (int); mask to the low
			// byte before converting to bool — see Dyncall::callB.
			writeReg<bool>(regs, reg, (dcbArgBool(args) & 0xFF) != 0);
			break;
		case Type::i8:
			writeReg<int8_t>(regs, reg, static_cast<int8_t>(dcbArgChar(args)));
			break;
		case Type::i16:
			writeReg<int16_t>(regs, reg, static_cast<int16_t>(dcbArgShort(args)));
			break;
		case Type::i32:
			writeReg<int32_t>(regs, reg, static_cast<int32_t>(dcbArgInt(args)));
			break;
		case Type::i64:
			writeReg<int64_t>(regs, reg, static_cast<int64_t>(dcbArgLong(args)));
			break;
		case Type::ui8:
			writeReg<uint8_t>(regs, reg, static_cast<uint8_t>(dcbArgUChar(args)));
			break;
		case Type::ui16:
			writeReg<uint16_t>(regs, reg, static_cast<uint16_t>(dcbArgUShort(args)));
			break;
		case Type::ui32:
			writeReg<uint32_t>(regs, reg, static_cast<uint32_t>(dcbArgUInt(args)));
			break;
		case Type::ui64:
			writeReg<uint64_t>(regs, reg, static_cast<uint64_t>(dcbArgULong(args)));
			break;
		case Type::f32:
			writeReg<float>(regs, reg, static_cast<float>(dcbArgFloat(args)));
			break;
		case Type::f64:
			writeReg<double>(regs, reg, static_cast<double>(dcbArgDouble(args)));
			break;
		case Type::ptr:
			regs[reg] = reinterpret_cast<int64_t>(dcbArgPointer(args));
			break;
		default:
			break;
		}
	}

	return execute(regs);
}

int64_t BCInterpreter::execute(RegisterFile& regs) const {
	// first block is always the entrypoint
	auto* currentBlock = &code.blocks[0];
	while (true) {
		// execute operations in block. The dispatch mode is constant for the whole
		// run, so this branch is perfectly predicted; it picks between the legacy
		// indirect-call table and the inlined switch (which avoids a non-inlined
		// call per instruction and lets the compiler keep the register base hot).
		if (dispatchMode == DispatchMode::Switch) {
			for (const auto& c : currentBlock->code) {
				switch (c.op) {
#define NAUTILUS_BC_SWITCH_CASE(name, ...)                                                                             \
	case ByteCode::name:                                                                                               \
		__VA_ARGS__(c, regs);                                                                                          \
		break;
					NAUTILUS_BC_OPCODE_LIST(NAUTILUS_BC_SWITCH_CASE)
#undef NAUTILUS_BC_SWITCH_CASE
				}
			}
		} else {
			for (const auto& c : currentBlock->code) {
				OpTable[(int16_t) c.op](c, regs);
			}
		}
		// handle terminator
		if (const auto* res = std::get_if<BranchOp>(&currentBlock->terminatorOp)) {
			currentBlock = &code.blocks[res->nextBlock];
		} else if (const auto* res = std::get_if<ConditionalJumpOp>(&currentBlock->terminatorOp)) {
			if (readReg<bool>(regs, res->conditionalReg)) {
				currentBlock = &code.blocks[res->trueBlock];
			} else {
				currentBlock = &code.blocks[res->falseBlock];
			}
		} else if (const auto* res = std::get_if<ReturnOp>(&currentBlock->terminatorOp)) {
			if (res->resultReg < 0) {
				return 0;
			}
			return regs[res->resultReg];
		}
	}
}

std::ostream& operator<<(std::ostream& os, const Code& code) {
	for (size_t i = 0; i < code.blocks.size(); i++) {
		os << "Block " << i << "\n";
		os << code.blocks[i];
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const CodeBlock& block) {
	for (auto& code : block.code) {
		os << "\t" << code << "\n";
	}

	// handle terminator
	if (const auto* res = std::get_if<BranchOp>(&block.terminatorOp)) {
		os << "\t" << "BR " << res->nextBlock << "\n";
	} else if (const auto* res = std::get_if<ConditionalJumpOp>(&block.terminatorOp)) {
		os << "\t" << "CMP " << "r" << res->conditionalReg << " " << res->trueBlock << " " << res->falseBlock << "\n";
	} else if (const auto* res = std::get_if<ReturnOp>(&block.terminatorOp)) {
		os << "\t" << "Return " << "r" << res->resultReg << "\n";
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const OpCode& code) {
	// auto str = std::string(code.op);
	os << "str" << " r" << code.reg1;
	if (code.reg2 != -1) {
		os << " r" << code.reg2;
	}

	if (code.output != -1) {
		os << " r" << code.output;
	}

	return os;
}

std::string Code::toString() {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

} // namespace nautilus::compiler::bc
