
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

// Computed goto (labels-as-values) is a GCC/Clang extension. Where it is not
// available we fall back to the switch path, so "threaded" stays selectable
// everywhere and simply degrades to "switch".
#if defined(__GNUC__) || defined(__clang__)
#define NAUTILUS_BC_HAS_COMPUTED_GOTO 1
#endif

DispatchMode parseDispatchMode(const std::string& value) {
	if (value == "switch") {
		return DispatchMode::Switch;
	}
	if (value == "threaded") {
		return DispatchMode::Threaded;
	}
	return DispatchMode::Call;
}

BCInterpreter::BCInterpreter(Code code, RegisterFile registerFile, BCInterpreterOptions options)
    : code(std::move(code)), registerFile(std::move(registerFile)), options(options) {
	// The register file was built against a temporary Code instance whose allocaBuffers
	// may have been copied (and thus relocated) before reaching this constructor.
	// Re-point each alloca register at the actual buffer in *this* code object.
	for (const auto& [reg, bufIdx] : this->code.allocaRegisterMap) {
		this->registerFile[reg] = reinterpret_cast<int64_t>(this->code.allocaBuffers[bufIdx].data());
	}
#ifdef NAUTILUS_BC_HAS_COMPUTED_GOTO
	if (this->options.dispatch == DispatchMode::Threaded) {
		buildFlatCode();
	}
#endif
}

namespace {
// Map a comparison opcode to its fused compare+branch opcode, if one exists.
// Returns true and sets `out` on success. Drives the Step 5 fusion.
bool fusedBranchFor(ByteCode cmp, ByteCode& out) {
	switch (cmp) {
#define NAUTILUS_BC_FUSED_MAP(fused, src, ctype, cmpOp)                                                                \
	case ByteCode::src:                                                                                                \
		out = ByteCode::fused;                                                                                         \
		return true;
		NAUTILUS_BC_FUSED_BRANCH_LIST(NAUTILUS_BC_FUSED_MAP)
#undef NAUTILUS_BC_FUSED_MAP
	default:
		return false;
	}
}

// Map an arithmetic opcode to its immediate-folded opcode, if one exists.
// Returns true and sets `out` on success. Drives the Step 6 folding.
bool immOpcodeFor(ByteCode op, ByteCode& out) {
	switch (op) {
#define NAUTILUS_BC_IMM_MAP(immOp, src, ctype, oper)                                                                   \
	case ByteCode::src:                                                                                                \
		out = ByteCode::immOp;                                                                                         \
		return true;
		NAUTILUS_BC_IMM_LIST(NAUTILUS_BC_IMM_MAP)
#undef NAUTILUS_BC_IMM_MAP
	default:
		return false;
	}
}
} // namespace

void BCInterpreter::buildFlatCode() {
	// Concatenate every block's operations in their existing order (block order
	// matters: visitIf emits landing-pad blocks inline, so offsets are correct by
	// construction). After each block's ops, append its terminator as a JMP/CJMP/
	// RET pseudo-op. Terminator targets stay block indices here; they are mapped to
	// flat offsets through blockStart_ at run time.
	blockStart_.resize(code.blocks.size());
	for (size_t i = 0; i < code.blocks.size(); i++) {
		const auto& block = code.blocks[i];
		blockStart_[i] = static_cast<int32_t>(flatCode_.size());

		// Step 6: emit op `k` of this block, folding a recorded small constant right
		// operand into an *_imm opcode (operand in reg1, immediate in reg2) when
		// enabled. foldableImmediates is sorted by index, so a single cursor suffices.
		size_t foldCursor = 0;
		auto emitOp = [&](size_t k) {
			const OpCode& op = block.code[k];
			if (options.immediates) {
				while (foldCursor < block.foldableImmediates.size() && block.foldableImmediates[foldCursor].first < k) {
					foldCursor++;
				}
				if (foldCursor < block.foldableImmediates.size() && block.foldableImmediates[foldCursor].first == k) {
					ByteCode immOp {};
					if (immOpcodeFor(op.op, immOp)) {
						const int16_t imm = block.foldableImmediates[foldCursor].second;
						// reg1 = operand, reg2 = immediate, output = result.
						flatCode_.emplace_back(immOp, op.reg1, imm, op.output);
						return;
					}
				}
			}
			flatCode_.push_back(op);
		};

		// Step 5: fuse a single-use trailing compare into the conditional jump when
		// enabled. Sound only if (a) the lowering marked the compare single-use and
		// (b) the compare is the block's last op (so its operands are not clobbered
		// before the branch) writing the condition register, and (c) the comparison
		// has a fused form. Otherwise fall through to the plain emission below.
		if (options.superinstructions && block.fuseCompareIntoBranch && !block.code.empty()) {
			if (const auto* res = std::get_if<ConditionalJumpOp>(&block.terminatorOp)) {
				const OpCode& last = block.code.back();
				ByteCode fused {};
				if (last.output == res->conditionalReg && fusedBranchFor(last.op, fused)) {
					for (size_t k = 0; k + 1 < block.code.size(); k++) {
						emitOp(k);
					}
					// reg1 = left, reg2 = right, output = true block, reg3 = false block.
					flatCode_.emplace_back(fused, last.reg1, last.reg2, res->trueBlock, res->falseBlock);
					continue;
				}
			}
		}

		for (size_t k = 0; k < block.code.size(); k++) {
			emitOp(k);
		}
		if (const auto* res = std::get_if<BranchOp>(&block.terminatorOp)) {
			flatCode_.emplace_back(ByteCode::JMP, res->nextBlock, -1, -1);
		} else if (const auto* res = std::get_if<ConditionalJumpOp>(&block.terminatorOp)) {
			// OpCode ctor order is (op, reg1, reg2, output, reg3).
			flatCode_.emplace_back(ByteCode::CJMP, res->conditionalReg, res->trueBlock, -1, res->falseBlock);
		} else {
			const auto& ret = std::get<ReturnOp>(block.terminatorOp);
			flatCode_.emplace_back(ByteCode::RET, ret.resultReg, -1, -1);
		}
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

namespace {
// Thread-local pool that recycles per-invocation register files so repeated calls
// avoid a heap allocation (the dominant cost for tiny functions). Each acquire()
// hands back a distinct buffer, so nested bc->bc calls on one thread never alias;
// the pool is thread_local, so concurrent threads stay independent. Used only when
// bc.regfileReuse is enabled. The copy-assign in acquire reuses the buffer's heap
// capacity, so after warmup no allocation happens.
thread_local std::vector<RegisterFile> tlRegisterFilePool;

RegisterFile acquireRegisterFile(const RegisterFile& init) {
	if (tlRegisterFilePool.empty()) {
		return init;
	}
	RegisterFile buf = std::move(tlRegisterFilePool.back());
	tlRegisterFilePool.pop_back();
	buf = init;
	return buf;
}

void releaseRegisterFile(RegisterFile&& buf) {
	tlRegisterFilePool.push_back(std::move(buf));
}

// Same pooling pattern as tlRegisterFilePool, applied to the per-invocation alloca
// buffers: each entry is one invocation's full set of alloca slots
// (std::vector<std::vector<uint8_t>>, matching invoke()'s localAllocaBuffers). No
// keying by Code identity — like the register-file pool, this is a free list of
// interchangeable buffers; acquire() always resizes and re-zeroes from the calling
// Code's allocaBuffers sizes, so a buffer shaped for one function is transparently
// reshaped for another on its next acquire. Unlike the register file (whose `init`
// carries real default register values to copy), alloca buffers must always come
// back zeroed, so acquire() re-zeroes every byte via assign() rather than copying
// `sizes`' contents — assign() still reuses each inner buffer's existing heap
// capacity, so after warmup no allocation happens. Used only when bc.regfileReuse
// is enabled.
thread_local std::vector<std::vector<std::vector<uint8_t>>> tlAllocaBufferPool;

std::vector<std::vector<uint8_t>> acquireAllocaBuffers(const std::vector<std::vector<uint8_t>>& sizes) {
	if (tlAllocaBufferPool.empty()) {
		std::vector<std::vector<uint8_t>> bufs(sizes.size());
		for (size_t i = 0; i < sizes.size(); i++) {
			bufs[i].resize(sizes[i].size(), 0);
		}
		return bufs;
	}
	std::vector<std::vector<uint8_t>> bufs = std::move(tlAllocaBufferPool.back());
	tlAllocaBufferPool.pop_back();
	bufs.resize(sizes.size());
	for (size_t i = 0; i < sizes.size(); i++) {
		bufs[i].assign(sizes[i].size(), 0);
	}
	return bufs;
}

void releaseAllocaBuffers(std::vector<std::vector<uint8_t>>&& bufs) {
	tlAllocaBufferPool.push_back(std::move(bufs));
}
} // namespace

int64_t BCInterpreter::invoke(DCArgs* args, const std::vector<Type>& argTypes) {
	// Per-invocation register file: a fresh copy by default, or one recycled from a
	// thread-local pool when bc.regfileReuse is set. Both give each invocation its
	// own buffer, so nested and concurrent calls remain correct.
	const bool reuse = options.reuseRegisterFile;
	RegisterFile regs = reuse ? acquireRegisterFile(registerFile) : registerFile;
	// Return the recycled buffer to the pool on every exit path, including if
	// execute() throws.
	struct PoolGuard {
		RegisterFile* regs;
		bool active;
		~PoolGuard() {
			if (active) {
				releaseRegisterFile(std::move(*regs));
			}
		}
	} guard {&regs, reuse};

	// Per-invocation alloca buffers, zeroed for a clean stack frame. Recycled from a
	// thread-local pool under the same bc.regfileReuse flag as the register file —
	// both are per-invocation scratch state, so there is no reason to want one
	// pooled without the other.
	std::vector<std::vector<uint8_t>> localAllocaBuffers =
	    reuse ? acquireAllocaBuffers(code.allocaBuffers) : std::vector<std::vector<uint8_t>>(code.allocaBuffers.size());
	if (!reuse) {
		for (const auto& [reg, bufIdx] : code.allocaRegisterMap) {
			localAllocaBuffers[bufIdx].resize(code.allocaBuffers[bufIdx].size(), 0);
		}
	}
	// Return the recycled buffers to the pool on every exit path, including if
	// execute() throws.
	struct AllocaPoolGuard {
		std::vector<std::vector<uint8_t>>* bufs;
		bool active;
		~AllocaPoolGuard() {
			if (active) {
				releaseAllocaBuffers(std::move(*bufs));
			}
		}
	} allocaGuard {&localAllocaBuffers, reuse};
	for (const auto& [reg, bufIdx] : code.allocaRegisterMap) {
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
#ifdef NAUTILUS_BC_HAS_COMPUTED_GOTO
	if (options.dispatch == DispatchMode::Threaded) {
		return executeThreaded(regs);
	}
#endif
	// first block is always the entrypoint
	auto* currentBlock = &code.blocks[0];
	// Threaded falls here when computed goto is unavailable, behaving as Switch.
	const bool useSwitch = options.dispatch != DispatchMode::Call;
	while (true) {
		// execute operations in block. The dispatch mode is constant for the whole
		// run, so this branch is perfectly predicted; it picks between the legacy
		// indirect-call table and the inlined switch (which avoids a non-inlined
		// call per instruction and lets the compiler keep the register base hot).
		if (useSwitch) {
			for (const auto& c : currentBlock->code) {
				switch (c.op) {
#define NAUTILUS_BC_SWITCH_CASE(name, ...)                                                                             \
	case ByteCode::name:                                                                                               \
		__VA_ARGS__(c, regs);                                                                                          \
		break;
					NAUTILUS_BC_OPCODE_LIST(NAUTILUS_BC_SWITCH_CASE)
#undef NAUTILUS_BC_SWITCH_CASE
				// Terminator pseudo-opcodes never appear in a block's operation stream
				// in the call/switch paths (they keep the structured terminators); these
				// cases exist only so the switch covers the whole ByteCode enum.
				case ByteCode::JMP:
				case ByteCode::CJMP:
				case ByteCode::RET:
#define NAUTILUS_BC_FUSED_SWITCH(fused, src, ctype, cmp) case ByteCode::fused:
					NAUTILUS_BC_FUSED_BRANCH_LIST(NAUTILUS_BC_FUSED_SWITCH)
#undef NAUTILUS_BC_FUSED_SWITCH
#define NAUTILUS_BC_IMM_SWITCH(immOp, src, ctype, oper) case ByteCode::immOp:
					NAUTILUS_BC_IMM_LIST(NAUTILUS_BC_IMM_SWITCH)
#undef NAUTILUS_BC_IMM_SWITCH
					break;
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

#ifdef NAUTILUS_BC_HAS_COMPUTED_GOTO
// Token-threaded execution: each handler computes its result and jumps straight
// to the next handler via a computed goto, instead of returning to a central
// loop. This gives the CPU a distinct indirect-branch site per opcode (better
// prediction) and removes the per-op loop bookkeeping. The label table is plain
// data (addresses of labels in this function), so this stays a pure interpreter
// with no runtime code generation. labels-as-values is a GCC/Clang extension, so
// the pedantic diagnostic is locally suppressed.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wgnu-label-as-value"
#endif
int64_t BCInterpreter::executeThreaded(RegisterFile& regs) const {
	// Address table indexed by opcode, in the same order as the ByteCode enum.
	// Built once at first entry (label addresses are not constant expressions).
	// The value ops come from NAUTILUS_BC_OPCODE_LIST; the three terminator
	// pseudo-opcodes (JMP/CJMP/RET) are appended in the same order they were added
	// to the enum, so labels[(int) op] stays correct for every opcode.
	static void* const labels[] = {
#define NAUTILUS_BC_LABEL(name, ...) &&L_##name,
	    NAUTILUS_BC_OPCODE_LIST(NAUTILUS_BC_LABEL)
#undef NAUTILUS_BC_LABEL
	        && L_JMP,
	    &&L_CJMP, &&L_RET,
	// Fused compare+branch labels, in the same order they were appended to the enum.
#define NAUTILUS_BC_FUSED_LABEL(fused, src, ctype, cmp) &&L_##fused,
	    NAUTILUS_BC_FUSED_BRANCH_LIST(NAUTILUS_BC_FUSED_LABEL)
#undef NAUTILUS_BC_FUSED_LABEL
	// Immediate-folded labels, in the same order they were appended to the enum.
#define NAUTILUS_BC_IMM_LABEL(immOp, src, ctype, oper) &&L_##immOp,
	        NAUTILUS_BC_IMM_LIST(NAUTILUS_BC_IMM_LABEL)
#undef NAUTILUS_BC_IMM_LABEL
	};

	// Flattened stream: blocks concatenated in order, each ended by a JMP/CJMP/RET.
	// There is no end-of-block check and no variant dispatch — every block ends in
	// a terminator op, so control always leaves via L_JMP/L_CJMP/L_RET.
	const OpCode* const base = flatCode_.data();
	const OpCode* ip = base;

#define NAUTILUS_BC_NEXT() goto* labels[(int16_t) ip->op]

	NAUTILUS_BC_NEXT();

#define NAUTILUS_BC_HANDLER(name, ...)                                                                                 \
	L_##name : {                                                                                                       \
		__VA_ARGS__(*ip, regs);                                                                                        \
		++ip;                                                                                                          \
		NAUTILUS_BC_NEXT();                                                                                            \
	}
	NAUTILUS_BC_OPCODE_LIST(NAUTILUS_BC_HANDLER)
#undef NAUTILUS_BC_HANDLER

L_JMP:
	ip = base + blockStart_[ip->reg1];
	NAUTILUS_BC_NEXT();
L_CJMP:
	ip = base + blockStart_[readReg<bool>(regs, ip->reg1) ? ip->reg2 : ip->reg3];
	NAUTILUS_BC_NEXT();
L_RET:
	if (ip->reg1 < 0) {
		return 0;
	}
	return regs[ip->reg1];

	// Fused compare+branch handlers (Step 5): read both operands, compare, and jump
	// to the true/false block in one step. reg1 = left, reg2 = right, output = true
	// block, reg3 = false block. Only reached via the label table.
#define NAUTILUS_BC_FUSED_HANDLER(fused, src, ctype, cmp)                                                              \
	L_##fused : {                                                                                                      \
		const bool taken = readReg<ctype>(regs, ip->reg1) cmp readReg<ctype>(regs, ip->reg2);                          \
		ip = base + blockStart_[taken ? ip->output : ip->reg3];                                                        \
		NAUTILUS_BC_NEXT();                                                                                            \
	}
	NAUTILUS_BC_FUSED_BRANCH_LIST(NAUTILUS_BC_FUSED_HANDLER)
#undef NAUTILUS_BC_FUSED_HANDLER

	// Immediate-folded arithmetic handlers (Step 6): operand in reg1, signed 16-bit
	// immediate in reg2 (widened to the op type), result in output.
#define NAUTILUS_BC_IMM_HANDLER(immOp, src, ctype, oper)                                                               \
	L_##immOp : {                                                                                                      \
		writeReg<ctype>(regs, ip->output,                                                                              \
		                static_cast<ctype>(readReg<ctype>(regs, ip->reg1) oper static_cast<ctype>(ip->reg2)));         \
		++ip;                                                                                                          \
		NAUTILUS_BC_NEXT();                                                                                            \
	}
	NAUTILUS_BC_IMM_LIST(NAUTILUS_BC_IMM_HANDLER)
#undef NAUTILUS_BC_IMM_HANDLER
#undef NAUTILUS_BC_NEXT
}
#pragma GCC diagnostic pop
#endif // NAUTILUS_BC_HAS_COMPUTED_GOTO

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
