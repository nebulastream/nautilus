#pragma once

#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"

namespace nautilus::compiler::tbc {

/**
 * @brief Configuration for the TBC lowering.
 *
 * Unlike the bc backend, every optimization defaults to ON — the flags exist
 * for A/B benchmarking and debugging, not for backwards compatibility.
 *
 * enableRegisterAllocator: linear register allocation with a free list; when
 * disabled every SSA value keeps its own register.
 *
 * enableRegisterCoalescing: sequence block-invocation arguments targeting
 * already-bound registers as a parallel copy (minimum number of MOVs, one temp
 * only for a genuine permutation cycle) instead of staging each argument
 * through a temp.
 *
 * enableSuperinstructions: fuse a single-use comparison feeding a conditional
 * branch into one 2-word compare-and-branch instruction.
 *
 * enableImmediates: fold small constant right operands of i32/i64 add/sub/mul
 * into the instruction's 16-bit immediate field.
 */
struct TBCLoweringOptions {
	bool enableRegisterAllocator = true;
	bool enableRegisterCoalescing = true;
	bool enableSuperinstructions = true;
	bool enableImmediates = true;
};

/**
 * @brief Lowers one IR function to the TBC flat bytecode format.
 *
 * `program` must already contain the module-wide function index (so calls to
 * other functions in the same module become interpreter-native CALLs) and the
 * pre-sized internal-handle arena; the call-site table is appended to.
 */
class TBCLoweringProvider {
public:
	static void lower(const ir::FunctionOperation* func, TBCProgram& program, TBCFunction& out,
	                  const TBCLoweringOptions& options);
};

} // namespace nautilus::compiler::tbc
