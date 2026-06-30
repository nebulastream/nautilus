#pragma once

#include "ByteReader.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace nautilus::fuzz {

/// Number of integer parameters every generated kernel takes.
inline constexpr uint32_t NUM_PARAMS = 3;

/// All values flow as uint64_t. Unsigned 64-bit avoids C++ integer-promotion
/// surprises (uint64_t never promotes) and gives fully-defined wraparound
/// arithmetic, so the native oracle and the traced kernel agree by
/// construction. See EvalNative.hpp for the matching semantics.
using Value = uint64_t;

enum class Kind : uint8_t {
	// Leaves
	Const, // imm holds the literal value
	Param, // imm holds the parameter index in [0, NUM_PARAMS)
	// Binary arithmetic / bitwise (well-defined for unsigned operands)
	Add,
	Sub,
	Mul,
	Div, // divisor forced non-zero
	Mod, // divisor forced non-zero
	And,
	Or,
	Xor,
	Shl, // shift amount masked to [0, 64)
	Shr, // shift amount masked to [0, 64)
	// Comparisons, each yielding 0 or 1
	Eq,
	Ne,
	Lt,
	Le,
	Gt,
	Ge,
	// Ternaries
	Select, // (cond, t, f): cond != 0 ? t : f, lowered to an IR select
	If      // (cond, t, e): cond != 0 ? t : e, lowered to real control flow
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

inline constexpr Kind INTERNAL_KINDS[] = {Kind::Add, Kind::Sub, Kind::Mul, Kind::Div, Kind::Mod,    Kind::And,
                                          Kind::Or,  Kind::Xor, Kind::Shl, Kind::Shr, Kind::Eq,     Kind::Ne,
                                          Kind::Lt,  Kind::Le,  Kind::Gt,  Kind::Ge,  Kind::Select, Kind::If};

inline int arity(Kind k) {
	switch (k) {
	case Kind::Const:
	case Kind::Param:
		return 0;
	case Kind::Select:
	case Kind::If:
		return 3;
	default:
		return 2;
	}
}

inline int generateNode(Ast& ast, ByteReader& reader, int depth, int& budget) {
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
			node.imm = reader.consume<uint64_t>();
		}
		const int idx = static_cast<int>(ast.nodes.size());
		ast.nodes.push_back(node);
		return idx;
	}

	constexpr uint32_t kindCount = sizeof(INTERNAL_KINDS) / sizeof(INTERNAL_KINDS[0]);
	node.kind = INTERNAL_KINDS[reader.byte() % kindCount];
	--budget;
	const int childCount = arity(node.kind);
	// Children must be generated before the parent is pushed so that parent
	// indices stay greater than their children (handy for evaluation/printing).
	int kids[3] = {-1, -1, -1};
	for (int i = 0; i < childCount; ++i) {
		kids[i] = generateNode(ast, reader, depth - 1, budget);
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

/// Build a random, always-valid AST from the reader. Never returns an empty
/// tree: an empty buffer yields a single Const(0) leaf.
inline Ast generate(ByteReader& reader) {
	Ast ast;
	ast.nodes.reserve(MAX_NODES + 1);
	int budget = MAX_NODES;
	ast.root = detail::generateNode(ast, reader, MAX_DEPTH, budget);
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

inline void print(const Ast& ast, int idx, std::string& out) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::Const:
		out += std::to_string(n.imm) + "u";
		return;
	case Kind::Param:
		out += "p" + std::to_string(n.imm);
		return;
	case Kind::Select:
		out += "sel(";
		print(ast, n.kid[0], out);
		out += " != 0, ";
		print(ast, n.kid[1], out);
		out += ", ";
		print(ast, n.kid[2], out);
		out += ")";
		return;
	case Kind::If:
		out += "if(";
		print(ast, n.kid[0], out);
		out += " != 0, ";
		print(ast, n.kid[1], out);
		out += ", ";
		print(ast, n.kid[2], out);
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
	print(ast, n.kid[0], out);
	out += " ";
	out += opSymbol(n.kind);
	out += " ";
	print(ast, n.kid[1], out);
	out += ")";
	if (isCmp) {
		out += " ? 1u : 0u)";
	}
}

} // namespace detail

/// Render the AST as a human-readable expression for failure reports and for
/// turning a saved crash input into a fixed regression test.
inline std::string toString(const Ast& ast) {
	std::string out;
	if (ast.root >= 0) {
		detail::print(ast, ast.root, out);
	}
	return out;
}

} // namespace nautilus::fuzz
