#pragma once

#include "nautilus/JITCompiler.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include <memory>

namespace nautilus::common {
class ArenaPool;
}

namespace nautilus::tracing {

class ExecutionTrace;
class TraceModule;

/**
 * @brief Translates a trace into IR in SSA form.
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
 * sealed exactly when all of its predecessors have been converted. Trace
 * blocks that hold nothing but a jump are threaded rather than converted, so
 * a loop the tracer split across two headers comes out with one.
 */
class TraceToIRConversionPhase {
public:
	std::shared_ptr<compiler::ir::IRGraph> apply(std::shared_ptr<TraceModule> traceModule,
	                                             const compiler::CompilationUnitID& id = "");

	/// Pool-backed variant of @ref apply. The IR graph's arena is acquired from
	/// @p pool and recycled into it when the graph dies, amortising the
	/// per-graph heap allocation across many compiles.
	std::shared_ptr<compiler::ir::IRGraph> apply(std::shared_ptr<TraceModule> traceModule, common::ArenaPool& pool,
	                                             const compiler::CompilationUnitID& id = "");

	/// Converts a single trace into a module holding it as the entry function "execute".
	std::shared_ptr<compiler::ir::IRGraph> apply(std::shared_ptr<ExecutionTrace> trace,
	                                             const compiler::CompilationUnitID& id = "");

	/// Pool-backed variant of the single-trace @ref apply.
	std::shared_ptr<compiler::ir::IRGraph> apply(std::shared_ptr<ExecutionTrace> trace, common::ArenaPool& pool,
	                                             const compiler::CompilationUnitID& id = "");
};

} // namespace nautilus::tracing
