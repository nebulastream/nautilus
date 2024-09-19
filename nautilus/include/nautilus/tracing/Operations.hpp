#pragma once

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
};

constexpr const char* toString(Op type) {
	switch (type) {
	case FREE:
		return "FREE";
	case CAST:
		return "CAST";
	case CALL:
		return "CALL";
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
	default:
		__builtin_unreachable();
	}
}

} // namespace nautilus::tracing
