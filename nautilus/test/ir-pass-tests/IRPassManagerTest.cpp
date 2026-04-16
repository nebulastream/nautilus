#include "IRGraphFixtures.hpp"
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

	void apply(compiler::ir::IRGraph& /*ir*/) override {
		log_.push_back(name_);
	}

	std::string getName() const override {
		return name_;
	}

private:
	std::string name_;
	std::vector<std::string>& log_;
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

} // namespace nautilus::testing
