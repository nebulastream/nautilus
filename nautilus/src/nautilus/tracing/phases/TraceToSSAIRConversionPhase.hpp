#pragma once

#include "nautilus/JITCompiler.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include <memory>

namespace nautilus::common {
class ArenaPool;
}

namespace nautilus::tracing {

class TraceModule;

/**
 * @brief Translates a trace straight into IR in SSA form, without first
 * rewriting the trace with `SSACreationPhase` (`ir.directSSAConstruction`).
 *
 * A traced value is either defined once -- every operation result -- or is a
 * variable, a value some `ASSIGN` redefines. A value defined once needs no
 * block argument anywhere: the trace only uses it where its definition
 * dominates, so the IR refers to the defining operation directly. Variables
 * get SSA form on the fly, following Braun et al., "Simple and Efficient
 * Construction of Static Single Assignment Form" (CC 2013): a read looks for
 * the variable's definition in the block, then walks to the predecessors, and
 * only a block with several predecessors gets a block argument. An argument
 * every edge passes the same value for is folded away as soon as its edges are
 * known. The trace's CFG is complete before conversion starts, so a block is
 * sealed exactly when all of its predecessors have been converted.
 *
 * Produces the same program as `SSACreationPhase` followed by
 * `TraceToIRConversionPhase`, with block arguments only where control flow
 * merges different values of a variable.
 */
class TraceToSSAIRConversionPhase {
public:
	std::shared_ptr<compiler::ir::IRGraph> apply(std::shared_ptr<TraceModule> traceModule,
	                                             const compiler::CompilationUnitID& id = "");

	/// Pool-backed variant of @ref apply (see `TraceToIRConversionPhase`).
	std::shared_ptr<compiler::ir::IRGraph> apply(std::shared_ptr<TraceModule> traceModule, common::ArenaPool& pool,
	                                             const compiler::CompilationUnitID& id = "");
};

} // namespace nautilus::tracing
