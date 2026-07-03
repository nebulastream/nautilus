#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/passes/EmptyBlockEliminationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/options.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::testing {

namespace {

size_t blockCount(const compiler::ir::IRGraph& ir) {
	size_t n = 0;
	for (const auto* fn : ir.getFunctionOperations()) {
		n += fn->getBasicBlocks().size();
	}
	return n;
}

} // namespace

TEST_CASE("EmptyBlockElimination: single block is untouched") {
	auto ir = IRGraphFixtures::makeSingleBlockGraph();
	REQUIRE(blockCount(*ir) == 1);

	engine::Options opts;
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::EmptyBlockEliminationPass>());
	mgr.run(*ir);

	REQUIRE(blockCount(*ir) == 1);
}

TEST_CASE("EmptyBlockElimination: entry block with trivial branch is preserved") {
	// entry -> empty -> ret. Entry is a candidate for substitution but
	// must never be the *victim*. The intermediate empty block is the
	// only removable one.
	auto ir = IRGraphFixtures::makeGraphWithEmptyBlock();
	REQUIRE(blockCount(*ir) == 3);

	auto* fn = ir->getFunctionOperations().front();
	auto* entryBefore = fn->getEntryBlock();

	engine::Options opts;
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::EmptyBlockEliminationPass>());
	mgr.run(*ir);

	// Exactly one block removed.
	REQUIRE(blockCount(*ir) == 2);
	// Entry block survives.
	REQUIRE(fn->getEntryBlock() == entryBefore);
}

TEST_CASE("EmptyBlockElimination: chain of 3 empty blocks collapses") {
	auto ir = IRGraphFixtures::makeChainOfEmptyBlocks(3);
	REQUIRE(blockCount(*ir) == 5); // entry + 3 empty + return

	engine::Options opts;
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::EmptyBlockEliminationPass>());
	mgr.run(*ir);

	// All 3 empty blocks gone; entry -> return directly.
	REQUIRE(blockCount(*ir) == 2);
}

TEST_CASE("EmptyBlockElimination: self-loop empty block is not removed") {
	// Build: entry -> self-looping empty block. Self-loop guard should
	// leave it alone.
	auto ir = std::make_shared<compiler::ir::IRGraph>("self-loop-test");
	auto& arena = ir->getArena();

	auto* selfLoop = arena.create<compiler::ir::BasicBlock>(arena, compiler::ir::BlockIdentifier {1},
	                                                        std::vector<compiler::ir::BasicBlockArgument*> {});
	selfLoop->addNextBlock(selfLoop, std::span<compiler::ir::Operation* const> {});

	auto* entry = arena.create<compiler::ir::BasicBlock>(arena, compiler::ir::BlockIdentifier {0},
	                                                     std::vector<compiler::ir::BasicBlockArgument*> {});
	entry->addNextBlock(selfLoop, std::span<compiler::ir::Operation* const> {});

	auto* fn = arena.create<compiler::ir::FunctionOperation>(
	    "execute", std::vector<compiler::ir::BasicBlock*> {entry, selfLoop}, std::vector<Type> {},
	    std::vector<std::string> {}, Type::v);
	ir->addFunctionOperation(fn);

	engine::Options opts;
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::EmptyBlockEliminationPass>());
	mgr.run(*ir);

	// Self-loop stays. The empty block is its own successor, which is
	// explicitly blocked by `isRemovableEmptyBlock`.
	REQUIRE(blockCount(*ir) == 2);
}

TEST_CASE("EmptyBlockElimination: eliminated block leaves no stale predecessor entry on its successor") {
	// A chain of empty blocks collapses one victim at a time; each victim's
	// own outgoing edge into its successor must be un-registered from that
	// successor's predecessor list, not just the *incoming* edges rewired
	// from the victim's real predecessors. Verified via V4 (predecessor
	// multiset) rather than a manual predecessor-list inspection, so this
	// doubles as the regression test discovered by the V7 corpus gate.
	auto ir = IRGraphFixtures::makeChainOfEmptyBlocks(3);

	engine::Options opts;
	opts.setOption("ir.verifyAfterEachPass", true);
	opts.setOption("ir.failOnVerifyError", true);
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::EmptyBlockEliminationPass>());
	REQUIRE_NOTHROW(mgr.run(*ir));

	REQUIRE(blockCount(*ir) == 2);
	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE(result.ok());
}

} // namespace nautilus::testing
