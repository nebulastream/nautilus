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
	// Cast: round-trips the single child through another same-domain type,
	// i.e. (T)(To)x. imm holds the target TypeId (an integer TypeId for the
	// integer domain, a float TypeId for the float domain). int<->float
	// casts are intentionally unsupported (would need UB-safe range
	// clamping before casting an out-of-range float to an integer type).
	Cast
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

inline constexpr Kind INT_KINDS[] = {Kind::Add, Kind::Sub, Kind::Mul,    Kind::Div, Kind::Mod, Kind::And, Kind::Or,
                                     Kind::Xor, Kind::Shl, Kind::Shr,    Kind::Eq,  Kind::Ne,  Kind::Lt,  Kind::Le,
                                     Kind::Gt,  Kind::Ge,  Kind::Select, Kind::If,  Kind::Neg, Kind::Not, Kind::Cast};

inline constexpr Kind FLOAT_KINDS[] = {Kind::Add, Kind::Sub, Kind::Mul, Kind::Div,    Kind::Eq, Kind::Ne,  Kind::Lt,
                                       Kind::Le,  Kind::Gt,  Kind::Ge,  Kind::Select, Kind::If, Kind::Neg, Kind::Cast};

inline int arity(Kind k) {
	switch (k) {
	case Kind::Const:
	case Kind::Param:
		return 0;
	case Kind::Neg:
	case Kind::Not:
	case Kind::Cast:
		return 1;
	case Kind::Select:
	case Kind::If:
		return 3;
	default:
		return 2;
	}
}

template <typename T>
int generateNode(Ast& ast, ByteReader& reader, int depth, int& budget) {
	const uint8_t sel = reader.byte();
	bool leaf = depth <= 0 || budget <= 1 || reader.exhausted();
	if (!leaf) {
		// Bias toward smaller trees: ~1/4 of eligible nodes become leaves.
		leaf = (sel & 0x3) == 0;
	}

	Node node;
	if (leaf) {
		if (sel & 0x4) {
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
		if constexpr (std::is_floating_point_v<T>) {
			node.imm = static_cast<uint64_t>(FLOAT_TYPE_IDS[reader.byte() % 2]);
		} else {
			node.imm = static_cast<uint64_t>(INT_TYPE_IDS[reader.byte() % 8]);
		}
	}

	const int childCount = arity(node.kind);
	// Children must be generated before the parent is pushed so that parent
	// indices stay greater than their children (handy for evaluation/printing).
	int kids[3] = {-1, -1, -1};
	for (int i = 0; i < childCount; ++i) {
		kids[i] = generateNode<T>(ast, reader, depth - 1, budget);
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

/// Build a random, always-valid AST of type T from the reader. Never returns
/// an empty tree: an empty buffer yields a single Const(0) leaf.
template <typename T>
Ast generate(ByteReader& reader) {
	Ast ast;
	ast.nodes.reserve(MAX_NODES + 1);
	int budget = MAX_NODES;
	ast.root = detail::generateNode<T>(ast, reader, MAX_DEPTH, budget);
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
