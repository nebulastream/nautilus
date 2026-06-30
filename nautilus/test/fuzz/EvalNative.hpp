#pragma once

#include "Ast.hpp"
#include <array>
#include <cstdint>

namespace nautilus::fuzz {

/**
 * @brief Independent ground-truth interpreter for an AST.
 *
 * Pure C++ scalar arithmetic with the exact, fully-defined semantics the
 * generator guarantees:
 *   - all arithmetic is unsigned uint64_t (defined wraparound),
 *   - division/modulo divisors are forced non-zero (`| 1`),
 *   - shift amounts are masked to [0, 64).
 * Because every operation is well-defined, a disagreement between this oracle
 * and any Nautilus backend is an unambiguous miscompile (or tracing bug).
 */
inline Value evalNative(const Ast& ast, int idx, const std::array<Value, NUM_PARAMS>& args) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::Const:
		return n.imm;
	case Kind::Param:
		return args[n.imm % NUM_PARAMS];
	case Kind::Select: {
		const Value c = evalNative(ast, n.kid[0], args);
		return c != 0 ? evalNative(ast, n.kid[1], args) : evalNative(ast, n.kid[2], args);
	}
	case Kind::If: {
		const Value c = evalNative(ast, n.kid[0], args);
		// Mirror the traced kernel: the else-branch is evaluated unconditionally,
		// the then-branch only when the condition holds.
		const Value e = evalNative(ast, n.kid[2], args);
		return c != 0 ? evalNative(ast, n.kid[1], args) : e;
	}
	default:
		break;
	}

	const Value l = evalNative(ast, n.kid[0], args);
	const Value r = evalNative(ast, n.kid[1], args);
	switch (n.kind) {
	case Kind::Add:
		return l + r;
	case Kind::Sub:
		return l - r;
	case Kind::Mul:
		return l * r;
	case Kind::Div:
		return l / (r | 1u);
	case Kind::Mod:
		return l % (r | 1u);
	case Kind::And:
		return l & r;
	case Kind::Or:
		return l | r;
	case Kind::Xor:
		return l ^ r;
	case Kind::Shl:
		return l << (r & 63u);
	case Kind::Shr:
		return l >> (r & 63u);
	case Kind::Eq:
		return l == r ? 1u : 0u;
	case Kind::Ne:
		return l != r ? 1u : 0u;
	case Kind::Lt:
		return l < r ? 1u : 0u;
	case Kind::Le:
		return l <= r ? 1u : 0u;
	case Kind::Gt:
		return l > r ? 1u : 0u;
	case Kind::Ge:
		return l >= r ? 1u : 0u;
	default:
		return 0; // unreachable
	}
}

inline Value evalNative(const Ast& ast, const std::array<Value, NUM_PARAMS>& args) {
	return evalNative(ast, ast.root, args);
}

} // namespace nautilus::fuzz
