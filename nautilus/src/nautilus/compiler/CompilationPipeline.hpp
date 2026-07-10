#pragma once

#include "nautilus/common/Arena.hpp"
#include "nautilus/options.hpp"
#include <list>
#include <memory>
#include <string>

namespace nautilus::compiler {

class CompilationStatistics;
class Executable;
class CompilationBackendRegistry;
class CompilableFunction;

namespace ir {
class IRGraph;
}

/// Creates a fresh, unique compilation-unit id (timestamp + random suffix).
/// Only available in builds with ENABLE_COMPILER and ENABLE_TRACING.
std::string createCompilationUnitID();

/**
 * @brief Frontend + backend compilation pipeline shared by all tiers.
 *
 * Provides the tracing → SSA → IR-generation → backend machinery that the
 * TieredJITCompiler runs for every tier compile. It is an internal helper,
 * not a JITCompiler strategy of its own.
 *
 * The pipeline never owns an Arena.  It draws a fresh trace arena from the
 * supplied trace ArenaPool for the duration of each compileToIR() and
 * returns it (softReset and recycled) when done.  Because the pool is
 * internally synchronized and hands every concurrent compile a distinct
 * arena, the pipeline is safe to use from multiple threads.  Both the
 * trace pool and the IR-arena pool must outlive the pipeline.
 */
class CompilationPipeline {
public:
	/// Construct a pipeline that draws trace arenas from @p traceArenaPool and
	/// IR-graph arenas from @p irArenaPool.  Both must outlive the pipeline.
	CompilationPipeline(engine::Options options, common::ArenaPool& traceArenaPool, common::ArenaPool& irArenaPool);
	~CompilationPipeline();

	const engine::Options& getOptions() const {
		return options;
	}

	/**
	 * @brief Trace and convert functions to IR without backend compilation.
	 *
	 * When @p statistics is non-null, tracing/SSA/IR-generation/IR-pass
	 * timings and entity counts are recorded into it. Logging of the
	 * final report is the caller's responsibility.
	 *
	 * @return Shared IR graph that can be compiled by any backend
	 */
	[[nodiscard]] std::shared_ptr<ir::IRGraph> compileToIR(std::list<CompilableFunction>& functions,
	                                                       const engine::ModuleOptions& moduleOptions,
	                                                       CompilationStatistics* statistics = nullptr) const;

	/**
	 * @brief Compile a pre-built IR graph with a specific backend.
	 *
	 * When @p statistics is non-null the selected backend records its
	 * per-phase timings and code-size metrics into it.
	 *
	 * @param ir The IR graph
	 * @param backendName Which backend to use
	 * @param statistics Optional statistics sink populated by the backend.
	 * @return Compiled executable
	 */
	[[nodiscard]] std::unique_ptr<Executable> compileIR(const std::shared_ptr<ir::IRGraph>& ir,
	                                                    const std::string& backendName,
	                                                    const engine::ModuleOptions& moduleOptions,
	                                                    CompilationStatistics* statistics = nullptr) const;

private:
	const engine::Options options;
	const CompilationBackendRegistry* backends;

	/// Non-owning pointer to the externally supplied trace-arena pool.  Each
	/// compileToIR() acquires a fresh arena from it for the duration of
	/// tracing and IR generation, then returns it.  Marked mutable because
	/// compileToIR() is const but acquire() mutates the pool.
	mutable common::ArenaPool* traceArenaPool_;
	/// Non-owning pointer to the externally supplied IR-arena pool.  Each
	/// IRGraph created during compileToIR acquires its arena from here, so
	/// successive compiles reuse heap chunks across IR graphs.
	mutable common::ArenaPool* irArenaPool_;
};
} // namespace nautilus::compiler
