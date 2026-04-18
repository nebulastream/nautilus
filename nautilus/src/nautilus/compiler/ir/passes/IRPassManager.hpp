
#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/passes/IRPass.hpp"
#include "nautilus/options.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace nautilus::compiler {
class DumpHandler;
class CompilationStatistics;
} // namespace nautilus::compiler

namespace nautilus::compiler::ir {

/**
 * @brief Orchestrates a sequence of `IRPass`es over an `IRGraph`.
 *
 * Behaviour is driven entirely by `engine::Options`:
 *
 *  - `ir.verifyBeforePipeline` (bool, default false): verify once before
 *    any pass runs.
 *  - `ir.verifyAfterEachPass`  (bool, default false): verify after every
 *    pass. Useful with `ir.failOnVerifyError` to pinpoint a broken pass.
 *  - `ir.failOnVerifyError`    (bool, default false): throw a
 *    `RuntimeException` instead of logging verifier errors.
 *  - `ir.dumpAfterEachPass`    (bool, default false): hand the IR string
 *    to the dump handler under `after_<pass-name>` after each pass.
 *
 * When a non-null `CompilationStatistics*` is supplied, the manager
 * records baseline IR counts under @c irPasses.baseline.*, per-pass
 * durations under @c irPasses.<name>.ms, per-pass entity deltas under
 * @c irPasses.<name>.{blocks,operations}Delta, and the pipeline total
 * under @c irPasses.totalMs. Logging of the report happens at the end
 * of compilation; the pass manager itself is silent about statistics.
 *
 * Before running any pass, the manager calls `rebuildPredecessorLists` on
 * the graph so passes can rely on `BasicBlock::getPredecessors()`.
 */
class IRPassManager {
public:
	explicit IRPassManager(const engine::Options& options, compiler::DumpHandler* dumpHandler = nullptr,
	                       compiler::CompilationStatistics* statistics = nullptr);

	/// Append a pass to the pipeline. Built-in callers typically pass a
	/// freshly created `std::make_shared<...>()`; plugin callers (via
	/// `LegacyCompiler::addIRPass`) hand over an already-shared instance so
	/// the same pass survives across compilations.
	void addPass(std::shared_ptr<IRPass> pass);

	/// Executes every registered pass, in registration order, on @p ir.
	void run(IRGraph& ir);

	[[nodiscard]] size_t size() const noexcept {
		return passes.size();
	}

private:
	std::vector<std::shared_ptr<IRPass>> passes;
	compiler::DumpHandler* dumpHandler;
	compiler::CompilationStatistics* statistics;
	bool verifyBeforePipeline;
	bool verifyAfterEachPass;
	bool failOnVerifyError;
	bool dumpAfterEachPass;

	void verifyAndReport(const IRGraph& ir, const char* stage);
};

} // namespace nautilus::compiler::ir
