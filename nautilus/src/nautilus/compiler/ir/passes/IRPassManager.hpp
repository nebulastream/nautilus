
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
 * @brief Orchestrates a sequence of `IRPass` groups over an `IRGraph`.
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
 *    to the dump handler under `after_<pass-name>` after each pass that
 *    reports a change (a pass reporting no change is not dumped, since its
 *    output is byte-identical to the previous stage's).
 *
 * When a non-null `CompilationStatistics*` is supplied, the manager
 * records baseline IR counts under @c irPasses.baseline.*, per-pass
 * durations under @c irPasses.<name>.ms, per-pass entity deltas under
 * @c irPasses.<name>.{blocks,operations}Delta, and the pipeline total
 * under @c irPasses.totalMs. When a pass runs more than once (inside a
 * fixed-point group), its `.ms`/`.blocksDelta`/`.operationsDelta` entries
 * accumulate across every iteration it ran rather than reflecting only the
 * last one. @c irPasses.pipelineIterations records the total number of
 * group iterations executed across the whole run. Logging of the report
 * happens at the end of compilation; the pass manager itself is silent
 * about statistics.
 *
 * Before running any pass, the manager calls `rebuildPredecessorLists` on
 * the graph so passes can rely on `BasicBlock::getPredecessors()`.
 *
 * Passes are organised into groups, run in registration order. A plain
 * `addPass` registers a group of one pass with `maxIterations == 1`.
 * `addFixedPointGroup` registers a group of one or more passes that is
 * re-run, as a whole, until either no pass in the group reports a change
 * during a full round or `maxIterations` rounds have run (whichever comes
 * first) -- this is what lets e.g. constant folding and empty-block
 * elimination feed each other's follow-up opportunities within one
 * `IRPassManager::run()` call.
 */
class IRPassManager {
public:
	/// @p printOptions is borrowed; caller keeps ownership and must outlive
	/// the pass-manager run.  Used for `dumpAfterEachPass` annotated IR dumps.
	explicit IRPassManager(const engine::Options& options, compiler::DumpHandler* dumpHandler = nullptr,
	                       compiler::CompilationStatistics* statistics = nullptr,
	                       const IRPrintOptions* printOptions = nullptr);

	/// Registers a group of one pass with `maxIterations == 1`.
	void addPass(std::unique_ptr<IRPass> pass);

	/// Registers @p passes as one fixed-point group: on `run()`, the whole
	/// sequence of passes is re-executed, in order, either until a full
	/// round changes nothing or until @p maxIterations rounds have run.
	/// @p maxIterations must be at least 1; a group of one pass with
	/// `maxIterations == 1` behaves exactly like `addPass`.
	void addFixedPointGroup(std::vector<std::unique_ptr<IRPass>> passes, size_t maxIterations);

	/// Executes every registered group, in registration order, on @p ir.
	void run(IRGraph& ir);

	/// Total number of individual passes registered across every group
	/// (not the number of groups).
	[[nodiscard]] size_t size() const noexcept {
		size_t total = 0;
		for (const auto& group : groups) {
			total += group.passes.size();
		}
		return total;
	}

private:
	struct PassGroup {
		std::vector<std::unique_ptr<IRPass>> passes;
		size_t maxIterations;
	};

	std::vector<PassGroup> groups;
	compiler::DumpHandler* dumpHandler;
	compiler::CompilationStatistics* statistics;
	bool verifyBeforePipeline;
	bool verifyAfterEachPass;
	bool failOnVerifyError;
	bool dumpAfterEachPass;
	const IRPrintOptions* printOptions;

	void verifyAndReport(const IRGraph& ir, const char* stage);
};

} // namespace nautilus::compiler::ir
