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

std::ostream& operator<<(std::ostream& os, const Op& operation);

} // namespace nautilus::tracing
