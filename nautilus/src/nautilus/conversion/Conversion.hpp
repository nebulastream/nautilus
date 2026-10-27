#pragma once

#include "nautilus/nir/Module.hpp"
#include <cstdint>

/// The conversion from pre-SSA to SSA-level NIR (docs/plans/nautilus-0.4/03-conversion-and-passes.md §3).
namespace nautilus::conversion {

/// What a conversion produced, per module.
struct ConversionStatistics {
	uint32_t functions = 0;
	uint32_t blocks = 0;
	uint32_t landingPads = 0;
	/// Block parameters (phis) that survived trivial-phi removal.
	uint32_t phis = 0;
	/// Trivial phis folded away.
	uint32_t removedPhis = 0;
	uint32_t instructions = 0;
	/// Reducible loops and irreducible regions found.
	uint32_t loops = 0;
	uint32_t irreducibleRegions = 0;
	/// Instructions replaced by a constant or one of their operands on emission, and `div`/`rem` whose check was
	/// dropped.
	uint32_t folded = 0;
	uint32_t uncheckedDivisions = 0;
	/// Blocks never emitted because no live edge entered them, and conditional branches that became jumps.
	uint32_t deadBlocks = 0;
	uint32_t foldedBranches = 0;
	/// Instructions replaced by an equal value computed earlier in the same extended basic block.
	uint32_t cse = 0;
	/// Instructions and block parameters the dead-value sweep removed.
	uint32_t deadValues = 0;
	/// The attribute pre-pass: internal functions it derived attributes for, those in a cycle of the call graph, and
	/// the number that kept `noUnwind` and `willReturn`.
	uint32_t inferredFunctions = 0;
	uint32_t cyclicFunctions = 0;
	uint32_t inferredNoUnwind = 0;
	uint32_t inferredWillReturn = 0;
	/// Calls that lost their unwind entry because the callee cannot throw.
	uint32_t droppedUnwinds = 0;
};

struct ConversionOptions {
	/// Fold and simplify on emission, fold constant branches, drop dead blocks and values. Off, the conversion only
	/// builds SSA (the tests compare the two).
	bool optimize = true;
	/// Derive the attributes of the internal functions before converting (the attribute pre-pass); off, the callee
	/// records keep what the trace recorded.
	bool inferAttributes = true;
};

/// Converts every function of a pre-SSA module to SSA. The result shares the module-level tables (constants, callees,
/// pointers, bindings, names, data) with the input and has its own `insts`, `blocks`, `extra`, `allocas` and
/// `functions`. Throws std::runtime_error for a function that is not a valid pre-SSA function (verify first).
nir::Module convert(const nir::Module& preSSA, const ConversionOptions& options = {},
                    ConversionStatistics* statistics = nullptr);

} // namespace nautilus::conversion
