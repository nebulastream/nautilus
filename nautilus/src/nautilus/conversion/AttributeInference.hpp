#pragma once

#include "nautilus/nir/Module.hpp"

namespace nautilus::conversion {

/// What the pre-pass found, per module.
struct InferenceStatistics {
	/// Internal functions whose attributes were derived.
	uint32_t functions = 0;
	/// Of those: functions in a cycle of the call graph (all pessimistic) and functions that kept `noUnwind` /
	/// `willReturn`.
	uint32_t cyclic = 0;
	uint32_t noUnwind = 0;
	uint32_t willReturn = 0;
};

/// The attribute pre-pass of docs/plans/nautilus-0.4/03-conversion-and-passes.md §3.5, run over the pre-SSA bodies of
/// all functions of a module before any of them is converted. For every internal function it derives the guarantee bits
/// of 13 §4.6.2 (no read, no write of memory, `noUnwind`, `willReturn`) from the function's own instructions and from
/// the final bits of the functions it calls, in the order of the call graph's strongly connected components, and stores
/// them in the function record and in every callee record that names the function (13 §6.3: they must agree).
///
/// - A function in a cycle of the call graph (direct or mutual recursion) gets no guarantee: an optimistic fixed point
///   would be more precise, but the bits must never be more optimistic than 0.3's, which treats every cycle this way.
/// - A loop makes a function not `willReturn`; an indirect call makes it fully pessimistic.
/// - A checked `div`/`rem` makes a function may-throw unless its divisor is a constant that makes every division safe
///   (non-zero, and not -1 when signed) or an unassigned variable that holds one.
/// - Exported functions keep what the trace recorded: callers outside the module may rely on it.
InferenceStatistics inferAttributes(nir::Module& preSSA);

} // namespace nautilus::conversion
