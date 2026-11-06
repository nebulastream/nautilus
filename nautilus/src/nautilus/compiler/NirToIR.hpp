#pragma once

#include "nautilus/nir/Module.hpp"
#include "nautilus/options.hpp"

namespace nautilus::compiler::ir {
class IRGraph;
}

namespace nautilus::compiler::nirbackend {
struct HostEnvironment;
}

namespace nautilus::compiler {

/// The temporary bridge of the cut-over (docs/plans/nautilus-0.4/16-implementation-plan.md, PR 6): builds the 0.3 IR of
/// an SSA-level NIR module into `graph`, for the backends that have no NIR port yet (`asmjit`, `mlir`, `cpp`). It is
/// deleted with the 0.3 IR when the last of them is ported (PRs 7 and 8).
///
/// NIR is signless, so the bridge infers the signedness of every integer value from the operations that state it
/// (`div.u`, `cmp.lt.s`, `cast.zext`, the extension kinds of signatures) and inserts a same-width cast where a use
/// disagrees with the stamp of its operand. What it does not reproduce are the defined semantics of D10 and D11
/// (checked division, saturating float-to-int casts, masked shifts): the 0.3 backends behave as they did before.
///
/// Throws `RuntimeException` for what 0.3 cannot express (`data.addr`, `bind.addr`, imported callees, `bitcast`).
///
/// `host` resolves the pointers of the module that are not process-local (symbols); null for a traced module.
void buildIRFromNir(const nir::Module& module, ir::IRGraph& graph, const engine::Options& options,
                    const nirbackend::HostEnvironment* host = nullptr);

} // namespace nautilus::compiler
