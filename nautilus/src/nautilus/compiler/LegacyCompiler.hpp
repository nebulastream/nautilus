#pragma once

#include "nautilus/JITCompiler.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/ir/passes/IRPass.hpp"
#include <functional>
#include <list>
#include <memory>
#include <vector>

namespace nautilus::compiler {

class CompilationStatistics;

/**
 * @brief Standard single-tier JIT compiler implementation.
 *
 * Compiles functions using a single backend (selected via options).
 * This is the original Nautilus compilation strategy.
 *
 * The compiler never owns an Arena; callers must supply one that
 * outlives the compiler.  The same Arena is reused across every
 * compile() invocation: softReset() is called at the start of each
 * compilation so chunk memory from the previous compile is recycled
 * instead of being freed and reallocated.  This amortises allocation
 * cost over many compilations and is the reason compile() is not
 * thread-safe (callers that need concurrent compilation should hold
 * one LegacyCompiler per thread, each with its own Arena).
 */
class LegacyCompiler : public JITCompiler {
public:
	/// Construct a compiler that uses the supplied Arena for every trace
	/// and the supplied ArenaPool for IR-graph arenas.  Both must outlive
	/// the compiler.
	LegacyCompiler(engine::Options options, common::Arena& arena, common::ArenaPool& irArenaPool);
	~LegacyCompiler() override;

	[[nodiscard]] std::unique_ptr<Executable> compile(wrapper_function function) const override;
	[[nodiscard]] std::unique_ptr<Executable> compile(std::list<CompilableFunction>& functions) const override;

	std::string getName() const override;
	const engine::Options& getOptions() const override {
		return options;
	}

	[[nodiscard]] std::shared_ptr<ir::IRGraph> compileToIR(std::list<CompilableFunction>& functions) const override {
		return compileToIR(functions, nullptr);
	}

	[[nodiscard]] std::unique_ptr<Executable> compileIR(const std::shared_ptr<ir::IRGraph>& ir,
	                                                    const std::string& backendName) const override {
		return compileIR(ir, backendName, nullptr);
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
	                                                       CompilationStatistics* statistics) const;

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
	                                                    CompilationStatistics* statistics) const;

	void addIRPass(std::unique_ptr<ir::IRPass> pass) override;

private:
	const engine::Options options;
	const CompilationBackendRegistry* backends;

	/// Non-owning pointer to the externally supplied Arena.  softReset()'d
	/// at the start of each compile() invocation.  Marked mutable because
	/// compile() is const but needs to recycle the arena between calls.
	mutable common::Arena* arena_;
	/// Non-owning pointer to the externally supplied IR-arena pool.  Each
	/// IRGraph created during compileToIR acquires its arena from here, so
	/// successive compiles reuse heap chunks across IR graphs.
	mutable common::ArenaPool* irArenaPool_;

	/// Plugin IR passes registered via @ref addIRPass. Held by `shared_ptr`
	/// because the same pass instance is wrapped in a `unique_ptr` adapter
	/// for each compileToIR invocation (the `IRPassManager` consumes
	/// `unique_ptr`, but the pass must outlive any single compile so it can
	/// be reused across compilations and across tier-0/tier-1 promotions).
	std::vector<std::shared_ptr<ir::IRPass>> pluginPasses_;
};
} // namespace nautilus::compiler
