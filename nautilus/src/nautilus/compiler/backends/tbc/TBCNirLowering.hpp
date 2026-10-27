#pragma once

#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include "nautilus/nir/Module.hpp"

namespace nautilus::compiler::tbc {

/// Lowers one function of an SSA-level NIR module to the TBC flat bytecode (docs/plans/nautilus-0.4/05-backends.md §3,
/// M4.1). Near 1:1: one register per value, one instruction per operation, except where the defined arithmetic of D10
/// and D11 needs more:
///   - a checked integer `div`/`rem` compares the divisor (and, signed, the dividend against the minimum) and branches
///     to a cold block that calls `nautilus_rt_throw_arith` through its capture thunk;
///   - a shift amount is masked to the bit width;
///   - `fptosi` and `fptoui` call a saturating helper.
/// `program` already holds every function's signature and native closure, so calls and `funcaddr` of internal
/// functions resolve to the function index and the closure.
class TBCNirLowering {
public:
	static void lower(const nir::Module& module, uint32_t function, TBCProgram& program, TBCFunction& out,
	                  const compiler::nirbackend::HostEnvironment& host);
};

/// The `tbc` type of a type word of a signature: narrow integers become the signedness of their extension kind.
Type tbcTypeOfWord(nir::TypeWord word);

} // namespace nautilus::compiler::tbc
