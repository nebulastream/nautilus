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
};

struct ConversionOptions {};

/// Converts every function of a pre-SSA module to SSA. The result shares the module-level tables (constants, callees,
/// pointers, bindings, names, data) with the input and has its own `insts`, `blocks`, `extra`, `allocas` and
/// `functions`. Throws std::runtime_error for a function that is not a valid pre-SSA function (verify first).
nir::Module convert(const nir::Module& preSSA, const ConversionOptions& options = {},
                    ConversionStatistics* statistics = nullptr);

} // namespace nautilus::conversion
