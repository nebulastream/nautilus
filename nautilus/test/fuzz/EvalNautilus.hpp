#pragma once

#include "Ast.hpp"
#include <array>
#include <nautilus/Engine.hpp>
#include <nautilus/select.hpp>
#include <nautilus/val.hpp>

namespace nautilus::fuzz {

using TracedValue = val<Value>;
using TracedArgs = std::array<TracedValue, NUM_PARAMS>;

/**
 * @brief Realize an AST as traced Nautilus operations.
 *
 * Walks the exact same tree as evalNative, but emits val<uint64_t> operations
 * so the function can be traced, optimized and lowered through every backend.
 * Each construct is the well-defined counterpart of the native oracle:
 *   - comparisons become `select(cond, 1, 0)` (an IR compare + select),
 *   - `If` uses real traced control flow so branch lowering is exercised,
 *   - divisors are forced non-zero and shift amounts masked, exactly as native.
 */
inline TracedValue evalNautilus(const Ast& ast, int idx, const TracedArgs& args) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::Const:
		return TracedValue(static_cast<Value>(n.imm));
	case Kind::Param:
		return args[n.imm % NUM_PARAMS];
	case Kind::Select: {
		TracedValue c = evalNautilus(ast, n.kid[0], args);
		TracedValue t = evalNautilus(ast, n.kid[1], args);
		TracedValue f = evalNautilus(ast, n.kid[2], args);
		return select(c != TracedValue(0), t, f);
	}
	case Kind::If: {
		TracedValue c = evalNautilus(ast, n.kid[0], args);
		// Else-branch first (unconditional), then conditionally overwrite with
		// the then-branch inside a real traced branch -> produces a phi/merge.
		TracedValue result = evalNautilus(ast, n.kid[2], args);
		if (c != TracedValue(0)) {
			result = evalNautilus(ast, n.kid[1], args);
		}
		return result;
	}
	default:
		break;
	}

	TracedValue l = evalNautilus(ast, n.kid[0], args);
	TracedValue r = evalNautilus(ast, n.kid[1], args);
	switch (n.kind) {
	case Kind::Add:
		return l + r;
	case Kind::Sub:
		return l - r;
	case Kind::Mul:
		return l * r;
	case Kind::Div:
		return l / (r | TracedValue(1));
	case Kind::Mod:
		return l % (r | TracedValue(1));
	case Kind::And:
		return l & r;
	case Kind::Or:
		return l | r;
	case Kind::Xor:
		return l ^ r;
	case Kind::Shl:
		return l << (r & TracedValue(63));
	case Kind::Shr:
		return l >> (r & TracedValue(63));
	case Kind::Eq:
		return select(l == r, TracedValue(1), TracedValue(0));
	case Kind::Ne:
		return select(l != r, TracedValue(1), TracedValue(0));
	case Kind::Lt:
		return select(l < r, TracedValue(1), TracedValue(0));
	case Kind::Le:
		return select(l <= r, TracedValue(1), TracedValue(0));
	case Kind::Gt:
		return select(l > r, TracedValue(1), TracedValue(0));
	case Kind::Ge:
		return select(l >= r, TracedValue(1), TracedValue(0));
	default:
		return TracedValue(0); // unreachable
	}
}

inline TracedValue evalNautilus(const Ast& ast, const TracedArgs& args) {
	return evalNautilus(ast, ast.root, args);
}

} // namespace nautilus::fuzz
