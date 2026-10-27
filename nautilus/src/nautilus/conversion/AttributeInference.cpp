#include "nautilus/conversion/AttributeInference.hpp"
#include "nautilus/nir/Operands.hpp"
#include "nautilus/nir/Semantics.hpp"
#include <algorithm>
#include <unordered_map>
#include <vector>

namespace nautilus::conversion {

using namespace nautilus::nir;

namespace {

constexpr uint32_t NO_READ = AttrNoArgRead | AttrNoOtherRead;
constexpr uint32_t NO_WRITE = AttrNoArgWrite | AttrNoOtherWrite;

/// What one function does on its own, before its callees are taken into account.
struct Local {
	bool reads = false;
	bool writes = false;
	bool mayThrow = false;
	bool loops = false;
	/// An indirect call or a call that does not resolve: nothing can be said.
	bool opaque = false;
	std::vector<uint32_t> callees; // functions called through internal callees
};

bool divisorIsSafeConstant(const Module& m, std::span<const Inst> insts, const std::vector<uint8_t>& assigned,
                           const Inst& inst, bool isUnsigned, TypeId type) {
	Ref divisor = inst.b;
	if (!isConstRef(divisor)) {
		// an unassigned variable that holds a constant: `%1 = var 4:i64`
		if (divisor >= insts.size() || assigned[divisor] || insts[divisor].op != static_cast<uint16_t>(Op::Var)) {
			return false;
		}
		divisor = insts[divisor].a;
		if (!isConstRef(divisor)) {
			return false;
		}
	}
	return semantics::divisorIsAlwaysSafe(isUnsigned, type, m.consts.record(divisor).bits);
}

Local analyze(const Module& m, uint32_t f) {
	Local local;
	const auto insts = m.functionInsts(f);
	const auto blocks = m.functionBlocks(f);
	const auto& rec = m.functions[f];

	// variables that an `assign` writes: a `var` of a constant that nothing assigns is a constant
	std::vector<uint8_t> assigned(insts.size(), 0);
	for (const auto& inst : insts) {
		if (inst.op == static_cast<uint16_t>(Op::Assign) && inst.a < insts.size()) {
			assigned[inst.a] = 1;
		}
	}

	for (uint32_t i = 0; i < insts.size(); ++i) {
		const auto& inst = insts[i];
		switch (static_cast<Op>(inst.op)) {
		case Op::Load:
			local.reads = true;
			break;
		case Op::Store:
			local.writes = true;
			break;
		case Op::Div:
		case Op::Rem: {
			const auto type = static_cast<TypeId>(inst.type);
			if ((inst.flags & 2) && !semantics::isFloatType(type) &&
			    !divisorIsSafeConstant(m, insts, assigned, inst, inst.flags & 1, type)) {
				local.mayThrow = true;
			}
			break;
		}
		case Op::CallIndirect:
			local.opaque = true;
			break;
		case Op::Call: {
			if (inst.a >= m.callees.size()) {
				local.opaque = true;
				break;
			}
			const auto& callee = m.callees[inst.a];
			if (callee.kind == static_cast<uint8_t>(CalleeKind::Internal)) {
				local.callees.push_back(callee.target);
			} else {
				// external, imported and intrinsic callees are as declared
				local.reads |= (callee.attrs & NO_READ) != NO_READ;
				local.writes |= (callee.attrs & NO_WRITE) != NO_WRITE;
				local.mayThrow |= (callee.attrs & AttrNoUnwind) == 0;
				if ((callee.attrs & AttrWillReturn) == 0) {
					local.loops = true; // may not return: same consequence for the caller
				}
			}
			break;
		}
		default:
			break;
		}
	}

	// a loop is a back edge of a depth-first search of the blocks
	const uint32_t n = rec.blockCount;
	std::vector<std::vector<uint32_t>> succ(n);
	for (uint32_t b = 0; b < n; ++b) {
		if (blocks[b].count == 0) {
			continue;
		}
		const auto ids = std::span<const uint32_t>(m.extra.data() + blocks[b].first, blocks[b].count);
		const auto& term = insts[ids.back()];
		if (term.op == static_cast<uint16_t>(Op::Br)) {
			succ[b].push_back(term.a);
		} else if (term.op == static_cast<uint16_t>(Op::CondBr)) {
			const uint32_t* list = m.extra.data() + term.c;
			succ[b].push_back(term.b);
			succ[b].push_back(list[1 + list[1] + 1]);
		}
	}
	enum : uint8_t { White, Grey, Black };
	std::vector<uint8_t> color(n, White);
	std::vector<std::pair<uint32_t, uint32_t>> stack;
	for (uint32_t start = 0; start < n && !local.loops; ++start) {
		if (color[start] != White) {
			continue;
		}
		stack.push_back({start, 0});
		color[start] = Grey;
		while (!stack.empty() && !local.loops) {
			auto& [b, next] = stack.back();
			if (next < succ[b].size()) {
				const auto to = succ[b][next++];
				if (to >= n) {
					continue;
				}
				if (color[to] == Grey) {
					local.loops = true;
				} else if (color[to] == White) {
					color[to] = Grey;
					stack.push_back({to, 0});
				}
			} else {
				color[b] = Black;
				stack.pop_back();
			}
		}
		stack.clear();
	}
	return local;
}

} // namespace

InferenceStatistics inferAttributes(Module& m) {
	InferenceStatistics stats;
	const uint32_t count = static_cast<uint32_t>(m.functions.size());
	std::vector<Local> locals(count);
	for (uint32_t f = 0; f < count; ++f) {
		locals[f] = analyze(m, f);
	}

	// strongly connected components of the call graph (Tarjan, iterative); a component is complete when all of its
	// callees are, so the order of completion is the order of the analysis
	std::vector<uint32_t> index(count, NONE), low(count, 0), component(count, NONE);
	std::vector<uint8_t> onStack(count, 0);
	std::vector<uint32_t> sccStack;
	std::vector<std::vector<uint32_t>> components;
	uint32_t counter = 0;
	for (uint32_t root = 0; root < count; ++root) {
		if (index[root] != NONE) {
			continue;
		}
		std::vector<std::pair<uint32_t, size_t>> work {{root, 0}};
		index[root] = low[root] = counter++;
		sccStack.push_back(root);
		onStack[root] = 1;
		while (!work.empty()) {
			auto& [f, next] = work.back();
			if (next < locals[f].callees.size()) {
				const auto to = locals[f].callees[next++];
				if (to >= count) {
					continue;
				}
				if (index[to] == NONE) {
					index[to] = low[to] = counter++;
					sccStack.push_back(to);
					onStack[to] = 1;
					work.push_back({to, 0});
				} else if (onStack[to]) {
					low[f] = std::min(low[f], index[to]);
				}
			} else {
				const auto done = f;
				work.pop_back();
				if (!work.empty()) {
					low[work.back().first] = std::min(low[work.back().first], low[done]);
				}
				if (low[done] == index[done]) {
					components.emplace_back();
					uint32_t member;
					do {
						member = sccStack.back();
						sccStack.pop_back();
						onStack[member] = 0;
						component[member] = static_cast<uint32_t>(components.size() - 1);
						components.back().push_back(member);
					} while (member != done);
				}
			}
		}
	}

	std::vector<uint32_t> bits(count, 0);
	for (const auto& members : components) {
		bool cyclic = members.size() > 1;
		if (!cyclic) {
			for (auto callee : locals[members[0]].callees) {
				cyclic |= callee == members[0];
			}
		}
		for (auto f : members) {
			if (cyclic || locals[f].opaque) {
				bits[f] = 0;
				continue;
			}
			bool reads = locals[f].reads, writes = locals[f].writes, mayThrow = locals[f].mayThrow,
			     mayNotReturn = locals[f].loops;
			for (auto callee : locals[f].callees) {
				if (callee >= count) {
					reads = writes = mayThrow = mayNotReturn = true;
					continue;
				}
				reads |= (bits[callee] & NO_READ) != NO_READ;
				writes |= (bits[callee] & NO_WRITE) != NO_WRITE;
				mayThrow |= (bits[callee] & AttrNoUnwind) == 0;
				mayNotReturn |= (bits[callee] & AttrWillReturn) == 0;
			}
			bits[f] = (reads ? 0 : NO_READ) | (writes ? 0 : NO_WRITE) | (mayThrow ? 0 : AttrNoUnwind) |
			          (mayNotReturn ? 0 : AttrWillReturn);
		}
		if (cyclic) {
			stats.cyclic += static_cast<uint32_t>(members.size());
		}
	}

	// store the bits on the internal functions and on every callee record that names one
	for (uint32_t f = 0; f < count; ++f) {
		auto& rec = m.functions[f];
		if (rec.visibility != static_cast<uint8_t>(Visibility::Internal)) {
			continue;
		}
		rec.attrs = (rec.attrs & ~GUARANTEE_MASK) | bits[f];
		++stats.functions;
		stats.noUnwind += (bits[f] & AttrNoUnwind) != 0;
		stats.willReturn += (bits[f] & AttrWillReturn) != 0;
	}
	for (auto& callee : m.callees) {
		if (callee.kind == static_cast<uint8_t>(CalleeKind::Internal) && callee.target < count &&
		    m.functions[callee.target].visibility == static_cast<uint8_t>(Visibility::Internal)) {
			callee.attrs = (callee.attrs & ~GUARANTEE_MASK) | bits[callee.target];
		}
	}
	return stats;
}

} // namespace nautilus::conversion
