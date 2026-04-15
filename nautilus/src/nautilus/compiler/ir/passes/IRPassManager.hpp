
#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/passes/IRPass.hpp"
#include "nautilus/options.hpp"
#include <cstddef>
#include <memory>
#include <vector>

namespace nautilus::compiler {
class DumpHandler;
}

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
 *  - `ir.logPassStatistics`    (bool, default false): log function/block/op
 *    counts before the pipeline and a delta after each pass.
 *  - `ir.dumpAfterEachPass`    (bool, default false): hand the IR string
 *    to the dump handler under `after_<pass-name>` after each pass.
 *
 * Before running any pass, the manager calls `rebuildPredecessorLists` on
 * the graph so passes can rely on `BasicBlock::getPredecessors()`.
 */
class IRPassManager {
public:
	explicit IRPassManager(const engine::Options& options, compiler::DumpHandler* dumpHandler = nullptr);

	void addPass(std::unique_ptr<IRPass> pass);

	/// Executes every registered pass, in registration order, on @p ir.
	void run(IRGraph& ir);

	[[nodiscard]] size_t size() const noexcept {
		return passes.size();
	}

private:
	std::vector<std::unique_ptr<IRPass>> passes;
	const engine::Options& options;
	compiler::DumpHandler* dumpHandler;
	bool verifyBeforePipeline;
	bool verifyAfterEachPass;
	bool failOnVerifyError;
	bool logStatistics;
	bool dumpAfterEachPass;

	void verifyAndReport(const IRGraph& ir, const char* stage);
};

} // namespace nautilus::compiler::ir
