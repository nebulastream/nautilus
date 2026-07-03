#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/ir/passes/EmptyBlockEliminationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRStatistics.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/options.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::testing {

namespace {

/// Test-only pass that records the order in which passes are invoked.
class RecordingPass : public compiler::ir::IRPass {
public:
	RecordingPass(std::string name, std::vector<std::string>& log) : name_(std::move(name)), log_(log) {
	}

	bool apply(compiler::ir::IRGraph& /*ir*/) override {
		log_.push_back(name_);
		return changesOnApply_;
	}

	void setChangesOnApply(bool value) {
		changesOnApply_ = value;
	}

	std::string getName() const override {
		return name_;
	}

private:
	std::string name_;
	std::vector<std::string>& log_;
	bool changesOnApply_ = true;
};

/// Test-only pass that reports a change for exactly its first N applications,
/// then reports no further change -- used to exercise fixed-point
/// convergence without depending on a real pass's semantics.
class ChangesForNApplications : public compiler::ir::IRPass {
public:
	ChangesForNApplications(std::string name, size_t n, size_t& callCount)
	    : name_(std::move(name)), remaining_(n), callCount_(callCount) {
	}

	bool apply(compiler::ir::IRGraph& /*ir*/) override {
		++callCount_;
		if (remaining_ == 0) {
			return false;
		}
		--remaining_;
		return true;
	}

	std::string getName() const override {
		return name_;
	}

private:
	std::string name_;
	size_t remaining_;
	size_t& callCount_;
};

} // namespace

TEST_CASE("IRPassManager: runs passes in registration order") {
	std::vector<std::string> log;
	auto ir = IRGraphFixtures::makeSingleBlockGraph();

	engine::Options opts;
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<RecordingPass>("first", log));
	mgr.addPass(std::make_unique<RecordingPass>("second", log));
	mgr.addPass(std::make_unique<RecordingPass>("third", log));
	REQUIRE(mgr.size() == 3);

	mgr.run(*ir);

	REQUIRE(log == std::vector<std::string> {"first", "second", "third"});
}

TEST_CASE("IRPassManager: statistics reflect block counts") {
	auto ir = IRGraphFixtures::makeChainOfEmptyBlocks(3);
	auto before = compiler::ir::computeStatistics(*ir);
	REQUIRE(before.numFunctions == 1);
	REQUIRE(before.numBlocks == 5);

	engine::Options opts;
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::EmptyBlockEliminationPass>());
	mgr.run(*ir);

	auto after = compiler::ir::computeStatistics(*ir);
	REQUIRE(after.numBlocks == 2);
	auto delta = before - after;
	// numBlocks decreased by 3; `before - after` yields 3 in the delta.
	REQUIRE(delta.numBlocks == 3);
}

TEST_CASE("IRPassManager: failOnVerifyError throws on invalid IR") {
	auto ir = IRGraphFixtures::makeMalformedGraph_NoTerminator();

	engine::Options opts;
	opts.setOption("ir.verifyBeforePipeline", true);
	opts.setOption("ir.failOnVerifyError", true);
	compiler::ir::IRPassManager mgr(opts);
	REQUIRE_THROWS_AS(mgr.run(*ir), nautilus::RuntimeException);
}

TEST_CASE("IRPassManager: verifyBeforePipeline logs (no throw) by default") {
	auto ir = IRGraphFixtures::makeMalformedGraph_NoTerminator();

	engine::Options opts;
	opts.setOption("ir.verifyBeforePipeline", true);
	// failOnVerifyError defaults to false; we should get a logged error
	// but no exception.
	compiler::ir::IRPassManager mgr(opts);
	REQUIRE_NOTHROW(mgr.run(*ir));
}

TEST_CASE("IRPassManager: fixed-point group converges before maxIterations") {
	auto ir = IRGraphFixtures::makeSingleBlockGraph();
	size_t callCount = 0;

	engine::Options opts;
	compiler::ir::IRPassManager mgr(opts);
	std::vector<std::unique_ptr<compiler::ir::IRPass>> group;
	group.push_back(std::make_unique<ChangesForNApplications>("convergent", 2, callCount));
	mgr.addFixedPointGroup(std::move(group), 10);
	REQUIRE(mgr.size() == 1);

	mgr.run(*ir);

	// Two rounds report a change, a third round reports none and stops the
	// group early -- three calls total, far short of maxIterations (10).
	REQUIRE(callCount == 3);
}

TEST_CASE("IRPassManager: fixed-point group respects the maxIterations bound") {
	auto ir = IRGraphFixtures::makeSingleBlockGraph();
	size_t callCount = 0;

	engine::Options opts;
	compiler::ir::IRPassManager mgr(opts);
	std::vector<std::unique_ptr<compiler::ir::IRPass>> group;
	// Never converges (always reports a change): maxIterations must be the
	// only thing that stops it.
	group.push_back(std::make_unique<ChangesForNApplications>("never-converges", 1000, callCount));
	mgr.addFixedPointGroup(std::move(group), 4);

	mgr.run(*ir);

	REQUIRE(callCount == 4);
}

TEST_CASE("IRPassManager: a pass reporting no change is not dumped") {
	auto ir = IRGraphFixtures::makeSingleBlockGraph();
	std::vector<std::string> log;

	engine::Options opts;
	opts.setOption("ir.dumpAfterEachPass", true);
	opts.setOption("dump.all", true);
	opts.setOption("dump.console", false);
	compiler::DumpHandler dumpHandler(opts, "irpassmanager-dump-test");

	auto changedPass = std::make_unique<RecordingPass>("changed-pass", log);
	auto unchangedPass = std::make_unique<RecordingPass>("unchanged-pass", log);
	unchangedPass->setChangesOnApply(false);

	compiler::ir::IRPassManager mgr(opts, &dumpHandler);
	mgr.addPass(std::move(changedPass));
	mgr.addPass(std::move(unchangedPass));
	mgr.run(*ir);

	const auto& generated = dumpHandler.getGeneratedFiles();
	REQUIRE(generated.contains("after_changed-pass"));
	REQUIRE_FALSE(generated.contains("after_unchanged-pass"));
}

} // namespace nautilus::testing
