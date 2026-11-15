#pragma once

#include "nautilus/nir/Module.hpp"
#include "nautilus/nir/Operands.hpp"
#include <cstdint>
#include <vector>

/// The optional passes of NIR (docs/plans/nautilus-0.4/03-conversion-and-passes.md §5).
namespace nautilus::passes {

/// One instruction of a function that a pass edits. Value operands are *uids* (stable names: the id the instruction had
/// when the function was decoded, or a fresh one above all of them), block operands are *block uids*, and the `extra`
/// list of the instruction (call and branch arguments, unwind entry, extension payload) is a copy of its own, so
/// that a pass inserts, moves and removes instructions and blocks without renumbering anything. `inst.c` of an
/// instruction that has a list is 0; `encode` writes the list and the offset.
struct MInst {
	nir::Inst inst {};
	std::vector<uint32_t> list;
	uint32_t uid = nir::NONE;
	/// The debug location and region of the instruction (indices in the module's debug sections), if it has any.
	uint32_t location = nir::NONE;
	uint32_t region = nir::NONE;
};

struct MBlock {
	uint32_t uid = nir::NONE;
	uint16_t flags = 0;
	/// The block arguments first (`blockarg`), the terminator last.
	std::vector<MInst> insts;

	[[nodiscard]] bool isPad() const {
		return (flags & nir::BLOCK_LANDING_PAD) != 0;
	}
	[[nodiscard]] uint16_t argCount() const;
	[[nodiscard]] MInst& terminator() {
		return insts.back();
	}
	[[nodiscard]] const MInst& terminator() const {
		return insts.back();
	}
};

/// An SSA-level function as a vector of blocks in layout order. `encode` writes it back in the form of 13 §4.3 and §6.3
/// (dense ids, landing pads last in the order of their first reference, unreachable blocks dropped).
struct MutableFunction {
	nir::FunctionRecord record {};
	std::vector<nir::AllocaRecord> allocas;
	std::vector<MBlock> blocks;
	/// The next unused value uid and block uid.
	uint32_t nextUid = 0;
	uint32_t nextBlockUid = 0;
	/// Whether the module had per-instruction debug locations and regions (the output then has them again).
	bool hasLocations = false;
	bool hasRegions = false;

	uint32_t freshUid() {
		return nextUid++;
	}
	uint32_t freshBlockUid() {
		return nextBlockUid++;
	}
	[[nodiscard]] size_t instCount() const;
	/// The index in `blocks` of the block with this uid, or `NONE`.
	[[nodiscard]] std::vector<uint32_t> blockIndex() const;
};

/// Number of words of the list of `inst` (0 if it has none). `extra` is the array its `c` refers to.
size_t listLength(const nir::Inst& inst, const uint32_t* extra);

/// Calls `f(Ref&)` for every value reference of the instruction (constants included, as `nir::forEachRef`).
template <class F>
void forEachValue(MInst& mi, F&& f) {
	nir::forEachRef(mi.inst, mi.list.data(), nir::Level::SSA, f);
}
template <class F>
void forEachValue(const MInst& mi, F&& f) {
	nir::forEachRef(mi.inst, mi.list.data(), nir::Level::SSA, f);
}

/// Calls `f(uint32_t&)` for every block uid of the instruction: the targets of `br` and `condbr` and the landing pad of
/// an unwind entry (`NO_UNWIND` and `NO_CLEANUP` are not blocks and are skipped). With a const instruction `f` gets
/// const references.
template <class Mi, class F>
void forEachBlock(Mi& mi, F&& f) {
	auto& inst = mi.inst;
	auto unwind = [&](auto& entry) {
		if (entry != nir::NO_UNWIND && entry != nir::NO_CLEANUP) {
			f(entry);
		}
	};
	switch (static_cast<nir::Op>(inst.op)) {
	case nir::Op::Br:
		f(inst.a);
		break;
	case nir::Op::CondBr:
		f(inst.b);
		f(mi.list[2 + mi.list[1]]);
		break;
	case nir::Op::Call:
	case nir::Op::CallIndirect:
		unwind(mi.list[1 + mi.list[0]]);
		break;
	case nir::Op::Div:
	case nir::Op::Rem:
		if (inst.flags & 2) {
			unwind(inst.c);
		}
		break;
	default:
		break;
	}
}

/// A module with the tables of `in` and no function bodies, ready for `encode`: the signatures have their parameter
/// types in the new `extra`, the interning tables are built, and the per-instruction debug arrays are empty (`encode`
/// writes them again).
nir::Module copyForRewrite(const nir::Module& in);

/// Decodes function `f` of an SSA-level module.
MutableFunction decode(const nir::Module& m, uint32_t f);

/// Appends `fn` to `out`, which has the tables of the module (and no function body for `fn`). Throws
/// `std::logic_error` if a reachable instruction uses a value that has no definition.
void encode(const MutableFunction& fn, nir::Module& out);

/// The block successors of the terminator of `block` (block uids): none for `ret` and `resume`.
std::vector<uint32_t> successors(const MBlock& block);

/// Replaces every use of the values in `subst` (uid to reference) by their replacement, following chains. Entries
/// that are `NONE` are not substituted.
void substitute(MutableFunction& fn, const std::vector<nir::Ref>& subst);

} // namespace nautilus::passes
