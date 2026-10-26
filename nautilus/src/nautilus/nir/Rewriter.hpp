#pragma once

#include "nautilus/nir/Module.hpp"
#include "nautilus/nir/Operands.hpp"
#include <functional>
#include <vector>

namespace nautilus::nir {

/// Copies a module function by function into a new module. Instructions are copied, replaced by another value, or
/// erased; surviving instructions are renumbered densely and every operand is mapped through `forward[]`, the
/// old-to-new value table (13-nir-spec.md §3.2; 02-nir.md §3.6). Block ids, callee indices and the other tables keep
/// their numbers, so a rewrite never invalidates them.
///
/// SSA functions are copied block by block in layout order, pre-SSA functions in id order with their block lists
/// rebuilt. A replacement target must be defined before the replaced instruction, or be a constant.
class Rewriter {
public:
	/// Called for every instruction of the old function in order. Returns true if it handled the instruction with
	/// `copy`, `emit`, `replace` or `erase`; if it returns false the instruction is copied unchanged.
	using Visitor = std::function<bool(Rewriter&, uint32_t oldId, const Inst&)>;

	/// `dst` must be empty. All tables except the function bodies are copied.
	Rewriter(const Module& src, Module& dst);

	void rewriteFunction(uint32_t f, const Visitor& visitor = {});
	/// Copies every function; stops at nothing, so the result has all of `src`'s functions in order.
	void rewriteAll(const Visitor& visitor = {});

	// operations for visitors
	/// Copies `inst` with its operands mapped; returns the new value reference.
	Ref copy(uint32_t oldId, const Inst& inst);
	/// Appends an instruction whose operands are already new references and maps `oldId` to it.
	Ref emit(uint32_t oldId, const Inst& inst);
	/// `oldId` becomes `newRef`: a new value or a constant. Nothing is emitted.
	void replace(uint32_t oldId, Ref newRef);
	/// `oldId` has no users; using it afterwards is an error.
	void erase(uint32_t oldId);
	/// Maps an old operand to its new reference; constants are unchanged.
	Ref map(Ref old) const;

	/// Value of old instruction id in the new function, or NONE when erased. Valid after rewriteFunction.
	std::span<const Ref> forward() const {
		return forward_;
	}
	const Module& source() const {
		return src_;
	}
	Module& target() {
		return dst_;
	}

private:
	Ref append(const Inst& inst);
	void rewriteList(Inst& inst, const Inst& old);

	const Module& src_;
	Module& dst_;
	std::vector<Ref> forward_;
	uint32_t srcFn_ = NONE;
	Level level_ = Level::SSA;
	std::vector<uint32_t> blockOfInst_; // pre-SSA: block of each old instruction
	std::vector<std::vector<uint32_t>> blockLists_;
	std::vector<uint32_t> newBlockOf_;
};

} // namespace nautilus::nir
