#pragma once

#include "nautilus/tracing/Types.hpp"
#include "nautilus/val_concepts.hpp"
#include <cstdint>
#include <iosfwd>

namespace nautilus::tracing {

enum Op : uint8_t {
	JMP,
	CMP,
	RETURN,
	ASSIGN,
	CONST,
	CAST,
	FREE,
	CALL,
	INDIRECT_CALL,
	// Memory
	LOAD,
	STORE,
	// Logical
	EQ,
	NEQ,
	LT,
	LTE,
	GT,
	GTE,
	NOT,
	AND,
	OR,
	// Arithmetic
	ADD,
	MUL,
	DIV,
	SUB,
	MOD,
	// Binary
	LSH,
	RSH,
	BOR,
	BXOR,
	BAND,
	NEGATE,
	// Ternary
	SELECT,
	// Stack allocation. The single input is an AllocaIndex referencing an entry
	// in ExecutionTrace::allocaSpecs (which records (size, align) for every
	// alloca encountered during tracing). Backends emit one real alloca per
	// table entry in the function prologue and treat the index as a table
	// lookup at every use site, so allocas no longer need to be hoisted.
	ALLOCA,
	FUNC_ADDR,
};

constexpr const char* toString(Op type) {
	switch (type) {
	case FREE:
		return "FREE";
	case CAST:
		return "CAST";
	case CALL:
		return "CALL";
	case INDIRECT_CALL:
		return "INDIRECT_CALL";
	case LOAD:
		return "LOAD";
	case STORE:
		return "STORE";
	case JMP:
		return "JMP";
	case CMP:
		return "CMP";
	case RETURN:
		return "RETURN";
	case ASSIGN:
		return "ASSIGN";
	case CONST:
		return "CONST";
	case EQ:
		return "EQ";
	case NEQ:
		return "NEQ";
	case LT:
		return "LT";
	case LTE:
		return "LTE";
	case GT:
		return "GT";
	case GTE:
		return "GTE";
	case NOT:
		return "NOT";
	case AND:
		return "AND";
	case OR:
		return "OR";
	case ADD:
		return "ADD";
	case MUL:
		return "MUL";
	case DIV:
		return "DIV";
	case SUB:
		return "SUB";
	case MOD:
		return "MOD";
	case LSH:
		return "LSH";
	case RSH:
		return "RSH";
	case BOR:
		return "BOR";
	case BAND:
		return "BAND";
	case BXOR:
		return "BXOR";
	case NEGATE:
		return "NEGATE";
	case SELECT:
		return "SELECT";
	case ALLOCA:
		return "ALLOCA";
	case FUNC_ADDR:
		return "FUNC_ADDR";
	default:
		__builtin_unreachable();
	}
}

/**
 * @brief Returns the number of inputs a TraceOperation of the given op/resultType has.
 *
 * For every op except RETURN the count is statically fixed by the op code.  RETURN
 * carries the returned value as a single input when the traced function is non-void
 * and no inputs for a void return; this mirrors the invariant enforced by
 * ExecutionTrace::addReturn, which is the only site that creates RETURN ops.
 */
constexpr uint8_t inputCountFor(Op op, Type resultType) noexcept {
	switch (op) {
	case RETURN:
		return resultType == Type::v ? 0 : 1;
	case CMP:
		return 4;
	case SELECT:
		return 3;
	case STORE:
	case EQ:
	case NEQ:
	case LT:
	case LTE:
	case GT:
	case GTE:
	case AND:
	case OR:
	case ADD:
	case MUL:
	case DIV:
	case SUB:
	case MOD:
	case LSH:
	case RSH:
	case BOR:
	case BXOR:
	case BAND:
		return 2;
	// Single-input ops: JMP, CALL, INDIRECT_CALL, ASSIGN, CONST, CAST, FREE,
	// LOAD, NOT, NEGATE, ALLOCA, FUNC_ADDR.
	case JMP:
	case CALL:
	case INDIRECT_CALL:
	case ASSIGN:
	case CONST:
	case CAST:
	case FREE:
	case LOAD:
	case NOT:
	case NEGATE:
	case ALLOCA:
	case FUNC_ADDR:
		return 1;
	}
	__builtin_unreachable();
}

} // namespace nautilus::tracing
