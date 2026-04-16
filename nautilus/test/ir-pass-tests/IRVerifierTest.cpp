#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::testing {

TEST_CASE("IRVerifier: well-formed single-block graph is ok") {
	auto ir = IRGraphFixtures::makeSingleBlockGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE(result.ok());
}

TEST_CASE("IRVerifier: block without terminator is flagged") {
	auto ir = IRGraphFixtures::makeMalformedGraph_NoTerminator();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE_FALSE(result.ok());
	// One of the errors should mention that the block's last op is not a terminator.
	bool found = false;
	for (const auto& err : result.errors) {
		if (err.message.find("terminator") != std::string::npos) {
			found = true;
			break;
		}
	}
	REQUIRE(found);
}

TEST_CASE("IRVerifier: duplicate block identifiers are flagged") {
	auto ir = IRGraphFixtures::makeMalformedGraph_DuplicateBlockId();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE_FALSE(result.ok());
	bool found = false;
	for (const auto& err : result.errors) {
		if (err.message.find("duplicate") != std::string::npos) {
			found = true;
			break;
		}
	}
	REQUIRE(found);
}

TEST_CASE("IRVerifier: chain-of-empty fixture is well-formed") {
	auto ir = IRGraphFixtures::makeChainOfEmptyBlocks(4);
	compiler::ir::rebuildPredecessorLists(*ir);
	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE(result.ok());
}

} // namespace nautilus::testing
