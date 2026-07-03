#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/compiler/ir/passes/DeadCodeEliminationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/options.hpp"
#include <catch2/catch_all.hpp>
#include <span>

namespace nautilus::testing {

namespace {

using compiler::ir::BasicBlock;
using compiler::ir::BasicBlockArgument;
using compiler::ir::BlockIdentifier;
using compiler::ir::FunctionOperation;
using compiler::ir::IRGraph;
using compiler::ir::Operation;
using compiler::ir::OperationIdentifier;

std::shared_ptr<IRGraph> wrapInGraph(std::shared_ptr<IRGraph> ir, BasicBlock* entry, Type returnStamp = Type::i32,
                                     std::vector<Type> inputArgs = {}) {
	auto& arena = ir->getArena();
	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry}, std::move(inputArgs),
	                                           std::vector<std::string> {}, returnStamp);
	ir->addFunctionOperation(fn);
	return ir;
}

size_t countOpsOfType(const IRGraph& ir, Operation::OperationType type) {
	size_t n = 0;
	for (const auto* fn : ir.getFunctionOperations()) {
		for (const auto* block : fn->getBasicBlocks()) {
			for (const auto* op : block->getOperations()) {
				if (op->getOperationType() == type) {
					++n;
				}
			}
		}
	}
	return n;
}

size_t totalOpCount(const IRGraph& ir) {
	size_t n = 0;
	for (const auto* fn : ir.getFunctionOperations()) {
		for (const auto* block : fn->getBasicBlocks()) {
			n += block->getOperations().size();
		}
	}
	return n;
}

BasicBlock* entryBlock(IRGraph& ir) {
	return ir.getFunctionOperations().front()->getEntryBlock();
}

void runPass(IRGraph& ir) {
	engine::Options opts;
	opts.setOption("ir.verifyAfterEachPass", true);
	opts.setOption("ir.failOnVerifyError", true);
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::DeadCodeEliminationPass>());
	mgr.run(ir);
}

} // namespace

TEST_CASE("DeadCodeElimination: dead constant chain and dead cast are removed, live value survives") {
	auto ir = IRGraphFixtures::makeDeadChainGraph();
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::ConstIntOp) == 2);
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::CastOp) == 1);
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 1);

	runPass(*ir);

	// The dead chain (const -> cast -> add, all unused) is fully swept.
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 0);
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::CastOp) == 0);
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::ConstIntOp) == 1);
	// The live constant, the alloca, the store, and the return remain.
	REQUIRE(totalOpCount(*ir) == 4);

	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE(result.ok());
}

TEST_CASE("DeadCodeElimination: op used only by a dead op is removed transitively") {
	auto ir = std::make_shared<IRGraph>("dce-transitive");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});

	// c1 -> cast -> add(cast, cast) -- the whole chain is dead; only `ret`
	// keeps the live constant alive.
	auto* c1 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {1}, Type::i32);
	auto* cast = entry->addOperation<compiler::ir::CastOperation>(OperationIdentifier {2}, c1, Type::i64);
	entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {3}, cast, cast);
	auto* live = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {4}, int64_t {9}, Type::i32);
	entry->addOperation<compiler::ir::ReturnOperation>(live);
	wrapInGraph(ir, entry);

	runPass(*ir);

	REQUIRE(totalOpCount(*ir) == 2); // live const + return
	const auto& ops = entryBlock(*ir)->getOperations();
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(ops[0]);
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == 9);
}

TEST_CASE("DeadCodeElimination: unused Load is not removed") {
	auto ir = std::make_shared<IRGraph>("dce-load");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* alloca = entry->addOperation<compiler::ir::AllocaOperation>(OperationIdentifier {1}, 0u);
	entry->addOperation<compiler::ir::LoadOperation>(OperationIdentifier {2}, alloca, Type::i32);
	entry->addOperation<compiler::ir::ReturnOperation>();
	wrapInGraph(ir, entry, Type::v);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::LoadOp) == 1);
}

TEST_CASE("DeadCodeElimination: unused Store is not removed") {
	auto ir = std::make_shared<IRGraph>("dce-store");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* alloca = entry->addOperation<compiler::ir::AllocaOperation>(OperationIdentifier {1}, 0u);
	auto* value = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {5}, Type::i32);
	entry->addOperation<compiler::ir::StoreOperation>(value, alloca);
	entry->addOperation<compiler::ir::ReturnOperation>();
	wrapInGraph(ir, entry, Type::v);

	runPass(*ir);

	// The store keeps its value operand alive too (store is not pure, so it
	// is never a DCE candidate, and its operand is therefore not use-free).
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::StoreOp) == 1);
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::ConstIntOp) == 1);
}

TEST_CASE("DeadCodeElimination: unused ProxyCall is not removed") {
	auto ir = std::make_shared<IRGraph>("dce-call");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	entry->addOperation<compiler::ir::ProxyCallOperation>(OperationIdentifier {1},
	                                                      std::span<compiler::ir::Operation* const> {}, Type::v);
	entry->addOperation<compiler::ir::ReturnOperation>();
	wrapInGraph(ir, entry, Type::v);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::ProxyCallOp) == 1);
}

TEST_CASE("DeadCodeElimination: a pure op used only as an invocation argument is not removed") {
	auto ir = std::make_shared<IRGraph>("dce-invocation-arg");
	auto& arena = ir->getArena();

	auto* succArg = arena.create<BasicBlockArgument>(OperationIdentifier {10}, Type::i32);
	auto* succ = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {succArg});
	succ->addOperation<compiler::ir::ReturnOperation>(succArg);

	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* passed =
	    entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {3}, Type::i32);
	entry->addNextBlock(succ, std::vector<compiler::ir::Operation*> {passed});

	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, succ}, std::vector<Type> {},
	                                           std::vector<std::string> {}, Type::i32);
	ir->addFunctionOperation(fn);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::ConstIntOp) == 1);
}

TEST_CASE("DeadCodeElimination: single block is untouched when nothing is dead") {
	auto ir = IRGraphFixtures::makeSingleBlockGraph();
	runPass(*ir);
	REQUIRE(totalOpCount(*ir) == 1); // just the return
}

TEST_CASE("DeadCodeElimination: idempotent on an already-clean graph") {
	auto ir = IRGraphFixtures::makeDeadChainGraph();
	runPass(*ir);
	const auto countAfterFirst = totalOpCount(*ir);

	runPass(*ir);
	REQUIRE(totalOpCount(*ir) == countAfterFirst);

	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE(result.ok());
}

} // namespace nautilus::testing
