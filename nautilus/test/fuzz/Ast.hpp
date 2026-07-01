#pragma once

#include "ByteReader.hpp"
#include "Types.hpp"
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

namespace nautilus::fuzz {

/// Number of parameters every generated kernel takes.
inline constexpr uint32_t NUM_PARAMS = 3;

enum class Kind : uint8_t {
	// Leaves
	Const, // imm holds the literal value, packed via packImm<T>
	Param, // imm holds the parameter index in [0, NUM_PARAMS)
	// Binary arithmetic / bitwise (integer domain only, except Add/Sub/Mul/Div which also apply to floats)
	Add,
	Sub,
	Mul,
	Div, // integer domain: divisor forced non-zero
	Mod, // integer domain only
	And, // integer domain only
	Or,  // integer domain only
	Xor, // integer domain only
	Shl, // integer domain only; shift amount masked to [0, bit-width(T))
	Shr, // integer domain only; shift amount masked to [0, bit-width(T))
	// Comparisons, each yielding 0 or 1 (as T)
	Eq,
	Ne,
	Lt,
	Le,
	Gt,
	Ge,
	// Ternaries
	Select, // (cond, t, f): cond != 0 ? t : f, lowered to an IR select
	If,     // (cond, t, e): cond != 0 ? t : e, lowered to real control flow
	// Unary
	Neg, // integer and float domains: -x
	Not, // integer domain only: ~x
	// Cast: round-trips the single child through another type, i.e. (T)(To)x.
	// imm holds the target TypeId, drawn from any of the ten types -- this
	// can cross the int/float domain boundary (the output type is always T,
	// the surrounding tree's type, exactly like a same-domain cast; only the
	// intermediate type's domain changes). Whichever leg of the round trip
	// is float->int is range-clamped (NaN -> 0, out-of-range -> min/max) so
	// the cast is well-defined for every input; see castThrough in
	// EvalNative.hpp / EvalNautilus.hpp.
	Cast,
	// Bounded, data-dependent fold: kid[0]=count, kid[1]=init, kid[2]=body.
	// trips = clamp(eval(count)) in [0, LOOP_MAX_TRIPS]; acc = eval(init);
	// acc = eval(body) repeated `trips` times, with LoopIndex/LoopAcc bound
	// to the current iteration inside body. Lowered to a real traced `for`
	// loop on the Nautilus side, exercising loop-lowering (not unrolling).
	Loop,
	// Leaves, only legal while generating inside a Loop body: current
	// iteration index / accumulator of the innermost enclosing Loop. No imm
	// payload -- nesting uses simple shadowing (innermost wins).
	LoopIndex,
	LoopAcc,
	// --- Memory / pointer domain -------------------------------------------
	// A generated program owns one shared, fixed-size `T` buffer (BUFFER_ELEMS
	// elements). kid[0] of Load/Store/PtrToInt and both kids of the Ptr*
	// comparisons are *pointer-domain* nodes (Kind::PtrBase/PtrAdd/PtrSub, see
	// below) built by generatePtrNode, not by generateNode -- they evaluate to
	// a `T*` (val<T*> when traced), never to a `T`.
	//
	// Load: kid[0] = ptr-domain node. Value-domain result: *ptr.
	Load,
	// Store: kid[0] = ptr-domain node, kid[1] = value-domain node. Writes
	// through the pointer and yields the stored value, mirroring the value of
	// a C++ assignment expression (`*ptr = v` evaluates to `v`).
	Store,
	// PtrToInt: kid[0] = ptr-domain node. `(T)(uintptr_t)ptr`, exactly
	// `base_ptr_val`'s pointer -> arithmetic conversion.
	PtrToInt,
	// PtrEq/Ne/Lt/Le/Gt/Ge: both kids are ptr-domain nodes; yields 0/1 as T,
	// same convention as the value-domain comparisons above.
	PtrEq,
	PtrNe,
	PtrLt,
	PtrLe,
	PtrGt,
	PtrGe,
	// --- Pointer-domain nodes -----------------------------------------------
	// Never chosen directly by generateNode/INT_KINDS/FLOAT_KINDS, and never
	// the AST root -- only reachable as a kid of one of the memory-domain
	// nodes above, built by generatePtrNode. Always resolve to an in-bounds
	// index of the shared buffer (no nesting -- see generatePtrNode), so every
	// Load/Store/comparison is guaranteed well-defined by construction.
	//
	// PtrBase: the raw pointer parameter, i.e. `&memory[0]`.
	PtrBase,
	// PtrAdd: kid[0] = value-domain offset expression (type T). Result =
	// `base + clampBufferIndex<T>(offset)`, exercising val<T*>::operator+.
	PtrAdd,
	// PtrSub: kid[0] = value-domain offset expression (type T). Result =
	// `(base + (BUFFER_ELEMS - 1)) - clampBufferIndex<T>(offset)`, exercising
	// val<T*>::operator-.
	PtrSub
};

struct Node {
	Kind kind;
	uint64_t imm = 0;
	int kid[3] = {-1, -1, -1};
};

struct Ast {
	std::vector<Node> nodes;
	int root = -1;
};

namespace detail {

// Load/Store/PtrToInt/Ptr-comparisons (below) are appended identically to
// both INT_KINDS and FLOAT_KINDS: a buffer of T is equally meaningful (and
// equally safe, given generatePtrNode's bounded construction) regardless of
// T's domain.
inline constexpr Kind INT_KINDS[] = {
    Kind::Add,      Kind::Sub,   Kind::Mul,   Kind::Div,   Kind::Mod,   Kind::And,   Kind::Or,   Kind::Xor,
    Kind::Shl,      Kind::Shr,   Kind::Eq,    Kind::Ne,    Kind::Lt,    Kind::Le,    Kind::Gt,   Kind::Ge,
    Kind::Select,   Kind::If,    Kind::Neg,   Kind::Not,   Kind::Cast,  Kind::Loop,  Kind::Load, Kind::Store,
    Kind::PtrToInt, Kind::PtrEq, Kind::PtrNe, Kind::PtrLt, Kind::PtrLe, Kind::PtrGt, Kind::PtrGe};

inline constexpr Kind FLOAT_KINDS[] = {Kind::Add,   Kind::Sub,   Kind::Mul,   Kind::Div,   Kind::Eq,     Kind::Ne,
                                       Kind::Lt,    Kind::Le,    Kind::Gt,    Kind::Ge,    Kind::Select, Kind::If,
                                       Kind::Neg,   Kind::Cast,  Kind::Loop,  Kind::Load,  Kind::Store,  Kind::PtrToInt,
                                       Kind::PtrEq, Kind::PtrNe, Kind::PtrLt, Kind::PtrLe, Kind::PtrGt,  Kind::PtrGe};

inline int arity(Kind k) {
	switch (k) {
	case Kind::Const:
	case Kind::Param:
	case Kind::LoopIndex:
	case Kind::LoopAcc:
	case Kind::PtrBase:
		return 0;
	case Kind::Neg:
	case Kind::Not:
	case Kind::Cast:
	case Kind::Load:
	case Kind::PtrToInt:
	case Kind::PtrAdd:
	case Kind::PtrSub:
		return 1;
	case Kind::Select:
	case Kind::If:
	case Kind::Loop:
		return 3;
	default:
		return 2;
	}
}

/// True for the six pointer-domain comparison kinds (both kids are
/// pointer-domain nodes, unlike Eq..Ge whose kids are value-domain).
inline bool isPtrCompare(Kind k) {
	return k >= Kind::PtrEq && k <= Kind::PtrGe;
}

template <typename T>
int generateNode(Ast& ast, ByteReader& reader, int depth, int& budget, int loopDepth);

/// Build a bounded pointer-domain expression: either the raw base pointer, or
/// a single-hop offset from it (`Kind::PtrAdd`/`Kind::PtrSub`) computed from a
/// value-domain offset expression. Deliberately never nests (a PtrAdd's own
/// child is always a value-domain node, never another pointer-domain node),
/// so every pointer this can produce is directly `base + clampBufferIndex(x)`
/// or `end - clampBufferIndex(x)` -- always an in-bounds index into the
/// shared buffer, with no need to reason about compounding offsets across
/// hops. `depth`/`budget`/`loopDepth` are threaded through exactly like
/// generateNode's, so the shared node budget still bounds total tree size.
template <typename T>
int generatePtrNode(Ast& ast, ByteReader& reader, int depth, int& budget, int loopDepth) {
	const uint8_t sel = reader.byte();
	const bool leaf = depth <= 0 || budget <= 1 || reader.exhausted() || (sel & 0x1) == 0;

	Node node;
	if (leaf) {
		node.kind = Kind::PtrBase;
		const int idx = static_cast<int>(ast.nodes.size());
		ast.nodes.push_back(node);
		return idx;
	}

	node.kind = (sel & 0x2) ? Kind::PtrAdd : Kind::PtrSub;
	--budget;
	node.kid[0] = generateNode<T>(ast, reader, depth - 1, budget, loopDepth);
	const int idx = static_cast<int>(ast.nodes.size());
	ast.nodes.push_back(node);
	return idx;
}

/// `loopDepth` counts how many Loop bodies the recursion is currently
/// nested inside; LoopIndex/LoopAcc are only legal leaves while it's > 0.
/// Passed by value: it naturally "pops" back down via the call stack on
/// return, no explicit restore needed.
template <typename T>
int generateNode(Ast& ast, ByteReader& reader, int depth, int& budget, int loopDepth) {
	const uint8_t sel = reader.byte();
	bool leaf = depth <= 0 || budget <= 1 || reader.exhausted();
	if (!leaf) {
		// Bias toward smaller trees: ~1/4 of eligible nodes become leaves.
		leaf = (sel & 0x3) == 0;
	}

	Node node;
	if (leaf) {
		if (loopDepth > 0 && (sel & 0x6) == 0x6) {
			// ~1/4 of in-loop leaves reference the innermost loop's index/acc.
			node.kind = (sel & 0x1) ? Kind::LoopAcc : Kind::LoopIndex;
		} else if (sel & 0x4) {
			node.kind = Kind::Param;
			node.imm = reader.byte() % NUM_PARAMS;
		} else {
			node.kind = Kind::Const;
			node.imm = packImm<T>(reader.consume<T>());
		}
		const int idx = static_cast<int>(ast.nodes.size());
		ast.nodes.push_back(node);
		return idx;
	}

	if constexpr (std::is_floating_point_v<T>) {
		constexpr uint32_t kindCount = sizeof(FLOAT_KINDS) / sizeof(FLOAT_KINDS[0]);
		node.kind = FLOAT_KINDS[reader.byte() % kindCount];
	} else {
		constexpr uint32_t kindCount = sizeof(INT_KINDS) / sizeof(INT_KINDS[0]);
		node.kind = INT_KINDS[reader.byte() % kindCount];
	}
	--budget;

	if (node.kind == Kind::Cast) {
		// Target may be any of the ten types, crossing the int/float domain
		// boundary (see the Kind::Cast comment for why this stays sound).
		constexpr uint32_t typeCount = sizeof(ALL_TYPES) / sizeof(ALL_TYPES[0]);
		node.imm = static_cast<uint64_t>(ALL_TYPES[reader.byte() % typeCount]);
	}

	const int childCount = arity(node.kind);
	// Children must be generated before the parent is pushed so that parent
	// indices stay greater than their children (handy for evaluation/printing).
	int kids[3] = {-1, -1, -1};
	if (node.kind == Kind::Loop) {
		// count (kid[0]) and init (kid[1]) live in the *outer* scope -- they
		// must not see this loop's own LoopIndex/LoopAcc. Only the body
		// (kid[2]) is generated one loop deeper.
		kids[0] = generateNode<T>(ast, reader, depth - 1, budget, loopDepth);
		kids[1] = generateNode<T>(ast, reader, depth - 1, budget, loopDepth);
		kids[2] = generateNode<T>(ast, reader, depth - 1, budget, loopDepth + 1);
	} else if (node.kind == Kind::Load || node.kind == Kind::PtrToInt) {
		kids[0] = generatePtrNode<T>(ast, reader, depth - 1, budget, loopDepth);
	} else if (node.kind == Kind::Store) {
		kids[0] = generatePtrNode<T>(ast, reader, depth - 1, budget, loopDepth);
		kids[1] = generateNode<T>(ast, reader, depth - 1, budget, loopDepth);
	} else if (isPtrCompare(node.kind)) {
		kids[0] = generatePtrNode<T>(ast, reader, depth - 1, budget, loopDepth);
		kids[1] = generatePtrNode<T>(ast, reader, depth - 1, budget, loopDepth);
	} else {
		for (int i = 0; i < childCount; ++i) {
			kids[i] = generateNode<T>(ast, reader, depth - 1, budget, loopDepth);
		}
	}
	node.kid[0] = kids[0];
	node.kid[1] = kids[1];
	node.kid[2] = kids[2];
	const int idx = static_cast<int>(ast.nodes.size());
	ast.nodes.push_back(node);
	return idx;
}

} // namespace detail

/// Depth / node budget bounds. Kept small so each program compiles quickly
/// (this is a soundness fuzzer, not a throughput one) and generation always
/// terminates even on huge inputs.
inline constexpr int MAX_DEPTH = 6;
inline constexpr int MAX_NODES = 64;

/// Upper bound on a Loop node's trip count (inclusive), enforced identically
/// by EvalNative.hpp and EvalNautilus.hpp. Compile cost is governed by AST
/// node count, not trip count -- the body is traced once regardless of how
/// many times the compiled loop executes it -- so this only bounds oracle
/// interpretation cost (which multiplies with loop nesting).
inline constexpr int LOOP_MAX_TRIPS = 8;

/// Number of `T` elements in the shared buffer every generated program is
/// handed a pointer to (see Kind::PtrBase). Every pointer-domain node
/// (generatePtrNode) is constructed so it always lands in [0, BUFFER_ELEMS),
/// so Load/Store/PtrToInt/Ptr-comparisons never read or write out of bounds --
/// small enough to keep the oracle/traced buffers cheap, large enough to make
/// pointer differences/comparisons non-trivial.
inline constexpr int BUFFER_ELEMS = 8;

/// Build a random, always-valid AST of type T from the reader. Never returns
/// an empty tree: an empty buffer yields a single Const(0) leaf.
template <typename T>
Ast generate(ByteReader& reader) {
	Ast ast;
	ast.nodes.reserve(MAX_NODES + 1);
	int budget = MAX_NODES;
	ast.root = detail::generateNode<T>(ast, reader, MAX_DEPTH, budget, /*loopDepth=*/0);
	return ast;
}

namespace detail {

inline const char* opSymbol(Kind k) {
	switch (k) {
	case Kind::Add:
		return "+";
	case Kind::Sub:
		return "-";
	case Kind::Mul:
		return "*";
	case Kind::Div:
		return "/";
	case Kind::Mod:
		return "%";
	case Kind::And:
		return "&";
	case Kind::Or:
		return "|";
	case Kind::Xor:
		return "^";
	case Kind::Shl:
		return "<<";
	case Kind::Shr:
		return ">>";
	case Kind::Eq:
		return "==";
	case Kind::Ne:
		return "!=";
	case Kind::Lt:
		return "<";
	case Kind::Le:
		return "<=";
	case Kind::Gt:
		return ">";
	case Kind::Ge:
		return ">=";
	default:
		return "?";
	}
}

template <typename T>
void print(const Ast& ast, int idx, std::string& out) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::Const:
		out += formatValue<T>(unpackImm<T>(n.imm)) + typeSuffix<T>();
		return;
	case Kind::Param:
		out += "p" + std::to_string(n.imm);
		return;
	case Kind::LoopIndex:
		// Always refers to the innermost enclosing loop -- ambiguous to read
		// back out of a nested-loop printout, but this is a debug-only
		// printer; crash reports also include the full args + expected/got.
		out += "li";
		return;
	case Kind::LoopAcc:
		out += "la";
		return;
	case Kind::Neg:
		out += "(-";
		print<T>(ast, n.kid[0], out);
		out += ")";
		return;
	case Kind::Not:
		out += "(~";
		print<T>(ast, n.kid[0], out);
		out += ")";
		return;
	case Kind::Cast: {
		const TypeId target = static_cast<TypeId>(n.imm);
		out += "(";
		out += typeName(target);
		out += ")(";
		print<T>(ast, n.kid[0], out);
		out += ")";
		return;
	}
	case Kind::Select:
		out += "sel(";
		print<T>(ast, n.kid[0], out);
		out += " != 0, ";
		print<T>(ast, n.kid[1], out);
		out += ", ";
		print<T>(ast, n.kid[2], out);
		out += ")";
		return;
	case Kind::If:
		out += "if(";
		print<T>(ast, n.kid[0], out);
		out += " != 0, ";
		print<T>(ast, n.kid[1], out);
		out += ", ";
		print<T>(ast, n.kid[2], out);
		out += ")";
		return;
	case Kind::Loop:
		out += "loop(count=";
		print<T>(ast, n.kid[0], out);
		out += ", init=";
		print<T>(ast, n.kid[1], out);
		out += ", body=";
		print<T>(ast, n.kid[2], out);
		out += ")";
		return;
	case Kind::Load:
		out += "*(";
		print<T>(ast, n.kid[0], out);
		out += ")";
		return;
	case Kind::Store:
		out += "(*(";
		print<T>(ast, n.kid[0], out);
		out += ") = ";
		print<T>(ast, n.kid[1], out);
		out += ")";
		return;
	case Kind::PtrToInt:
		out += "(uintptr_t)(";
		print<T>(ast, n.kid[0], out);
		out += ")";
		return;
	case Kind::PtrEq:
	case Kind::PtrNe:
	case Kind::PtrLt:
	case Kind::PtrLe:
	case Kind::PtrGt:
	case Kind::PtrGe: {
		const char* sym = "?";
		switch (n.kind) {
		case Kind::PtrEq:
			sym = "==";
			break;
		case Kind::PtrNe:
			sym = "!=";
			break;
		case Kind::PtrLt:
			sym = "<";
			break;
		case Kind::PtrLe:
			sym = "<=";
			break;
		case Kind::PtrGt:
			sym = ">";
			break;
		default:
			sym = ">=";
			break;
		}
		out += "((";
		print<T>(ast, n.kid[0], out);
		out += " ";
		out += sym;
		out += " ";
		print<T>(ast, n.kid[1], out);
		out += ") ? 1 : 0)";
		return;
	}
	case Kind::PtrBase:
		out += "mem";
		return;
	case Kind::PtrAdd:
		out += "(mem + idx(";
		print<T>(ast, n.kid[0], out);
		out += "))";
		return;
	case Kind::PtrSub:
		out += "(mem_end - idx(";
		print<T>(ast, n.kid[0], out);
		out += "))";
		return;
	default:
		break;
	}
	// Binary node: comparisons are wrapped so the 0/1 result is explicit.
	const bool isCmp = n.kind >= Kind::Eq && n.kind <= Kind::Ge;
	if (isCmp) {
		out += "(";
	}
	out += "(";
	print<T>(ast, n.kid[0], out);
	out += " ";
	out += opSymbol(n.kind);
	out += " ";
	print<T>(ast, n.kid[1], out);
	out += ")";
	if (isCmp) {
		out += " ? 1 : 0)";
	}
}

} // namespace detail

/// Render the AST as a human-readable expression for failure reports and for
/// turning a saved crash input into a fixed regression test.
template <typename T>
std::string toString(const Ast& ast) {
	std::string out;
	if (ast.root >= 0) {
		detail::print<T>(ast, ast.root, out);
	}
	return out;
}

} // namespace nautilus::fuzz
