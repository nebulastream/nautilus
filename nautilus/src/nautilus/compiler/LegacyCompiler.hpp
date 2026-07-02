#pragma once

#include "nautilus/JITCompiler.hpp"
#include "nautilus/common/Arena.hpp"
#include <functional>
#include <list>
#include <memory>

namespace nautilus::compiler {

class CompilationStatistics;

/**
 * @brief Standard single-tier JIT compiler implementation.
 *
 * Compiles functions using a single backend (selected via options).
 * This is the original Nautilus compilation strategy.
 *
 * The compiler never owns an Arena.  It draws a fresh trace arena from the
 * supplied trace ArenaPool for the duration of each compile() and returns it
 * (softReset and recycled) when done.  Because the pool is internally
 * synchronized and hands every concurrent compile() a distinct arena,
 * compile() is safe to call from multiple threads on a single shared
 * compiler.  Both the trace pool and the IR-arena pool must outlive the
 * compiler.
 */
class LegacyCompiler : public JITCompiler {
public:
	/// Construct a compiler that draws trace arenas from @p traceArenaPool and
	/// IR-graph arenas from @p irArenaPool.  Both must outlive the compiler.
	LegacyCompiler(engine::Options options, common::ArenaPool& traceArenaPool, common::ArenaPool& irArenaPool);
	~LegacyCompiler() override;

	[[nodiscard]] std::unique_ptr<Executable> compile(wrapper_function function,
	                                                  const engine::ModuleOptions& moduleOptions = {}) const override;
	[[nodiscard]] std::unique_ptr<Executable> compile(std::list<CompilableFunction>& functions,
	                                                  const engine::ModuleOptions& moduleOptions = {}) const override;

	void compileModule(std::list<CompilableFunction>& functions, const engine::ModuleOptions& moduleOptions,
	                   std::shared_ptr<engine::details::ModuleState> state) const override;

	std::string getName() const override;
	const engine::Options& getOptions() const override {
		return options;
	}

	/**
	 * @brief Trace and convert functions to IR without backend compilation.
	 *
	 * When @p statistics is non-null, tracing/SSA/IR-generation/IR-pass
	 * timings and entity counts are recorded into it. Logging of the
	 * final report is the caller's responsibility; @ref compile wires it
	 * up for the common case.
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
	/// compile() acquires a fresh arena from it for the duration of tracing
	/// and IR generation, then returns it.  Marked mutable because compile()
	/// is const but acquire() mutates the pool.
	mutable common::ArenaPool* traceArenaPool_;
	/// Non-owning pointer to the externally supplied IR-arena pool.  Each
	/// IRGraph created during compileToIR acquires its arena from here, so
	/// successive compiles reuse heap chunks across IR graphs.
	mutable common::ArenaPool* irArenaPool_;
};
} // namespace nautilus::compiler
