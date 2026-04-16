#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/ConstantFoldingAndCopyPropagationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/options.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::testing {

namespace {

using compiler::ir::BasicBlock;
using compiler::ir::BasicBlockArgument;
using compiler::ir::BlockIdentifier;
using compiler::ir::FunctionOperation;
using compiler::ir::IRGraph;
using compiler::ir::Operation;
using compiler::ir::OperationIdentifier;

/// Constructs a single-block graph containing an `execute` function that
/// owns @p entry. The entry block is wrapped in the graph's arena so the
/// returned `shared_ptr` keeps everything alive for the duration of the
/// test.
std::shared_ptr<IRGraph> wrapInGraph(std::shared_ptr<IRGraph> ir, BasicBlock* entry, Type returnStamp = Type::i32) {
	auto& arena = ir->getArena();
	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry}, std::vector<Type> {},
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

BasicBlock* entryBlock(IRGraph& ir) {
	return ir.getFunctionOperations().front()->getEntryBlock();
}

void runPass(IRGraph& ir) {
	engine::Options opts;
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::ConstantFoldingAndCopyPropagationPass>());
	mgr.run(ir);
}

} // namespace

TEST_CASE("ConstantFolding: single constant passthrough is unchanged") {
	auto ir = std::make_shared<IRGraph>("cf-single-const");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* c = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {42}, Type::i32);
	entry->addOperation<compiler::ir::ReturnOperation>(c);
	wrapInGraph(ir, entry);

	runPass(*ir);

	const auto& ops = entryBlock(*ir)->getOperations();
	REQUIRE(ops.size() == 2);
	REQUIRE(ops[0]->getOperationType() == Operation::OperationType::ConstIntOp);
	REQUIRE(ops[1]->getOperationType() == Operation::OperationType::ReturnOp);
	REQUIRE(compiler::ir::as<compiler::ir::ConstIntOperation>(ops[0])->getValue() == 42);
}

TEST_CASE("ConstantFolding: 5 + 3 folds to 8") {
	auto ir = std::make_shared<IRGraph>("cf-add");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* c5 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {5}, Type::i32);
	auto* c3 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {3}, Type::i32);
	auto* add = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {3}, c5, c3);
	entry->addOperation<compiler::ir::ReturnOperation>(add);
	wrapInGraph(ir, entry);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 0);
	// The add's slot is replaced in place, so the third op is now a ConstInt.
	const auto& ops = entryBlock(*ir)->getOperations();
	REQUIRE(ops.size() == 4);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(ops[2]);
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == 8);
	REQUIRE(folded->getStamp() == Type::i32);

	// Copy propagation: return's input is now the folded constant, not the
	// stale AddOp pointer.
	auto* ret = compiler::ir::as<compiler::ir::ReturnOperation>(ops[3]);
	REQUIRE(ret->getReturnValue() == folded);
}

TEST_CASE("ConstantFolding: chained (5 + 3) * 2 collapses to 16") {
	auto ir = std::make_shared<IRGraph>("cf-chain");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* c5 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {5}, Type::i32);
	auto* c3 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {3}, Type::i32);
	auto* add = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {3}, c5, c3);
	auto* c2 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {4}, int64_t {2}, Type::i32);
	auto* mul = entry->addOperation<compiler::ir::MulOperation>(OperationIdentifier {5}, add, c2);
	entry->addOperation<compiler::ir::ReturnOperation>(mul);
	wrapInGraph(ir, entry);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 0);
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::MulOp) == 0);

	const auto& ops = entryBlock(*ir)->getOperations();
	auto* ret = compiler::ir::as<compiler::ir::ReturnOperation>(ops.back());
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(ret->getReturnValue());
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == 16);
}

TEST_CASE("ConstantFolding: non-foldable arg + 7 stays in place") {
	auto ir = std::make_shared<IRGraph>("cf-non-foldable");
	auto& arena = ir->getArena();
	auto* blockArg = arena.create<BasicBlockArgument>(OperationIdentifier {10}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {blockArg});
	auto* c7 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {7}, Type::i32);
	auto* add = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {2}, blockArg, c7);
	entry->addOperation<compiler::ir::ReturnOperation>(add);
	wrapInGraph(ir, entry);

	runPass(*ir);

	// The add must still be present, with its original operands intact.
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 1);
	const auto& ops = entryBlock(*ir)->getOperations();
	auto* stillAdd = compiler::ir::dyn_cast<compiler::ir::AddOperation>(ops[1]);
	REQUIRE(stillAdd != nullptr);
	REQUIRE(stillAdd->getLeftInput() == blockArg);
	REQUIRE(stillAdd->getRightInput() == c7);
}

TEST_CASE("ConstantFolding: divide-by-zero is skipped and does not crash") {
	auto ir = std::make_shared<IRGraph>("cf-div-zero");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* c10 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {10}, Type::i32);
	auto* c0 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {0}, Type::i32);
	auto* div = entry->addOperation<compiler::ir::DivOperation>(OperationIdentifier {3}, c10, c0);
	entry->addOperation<compiler::ir::ReturnOperation>(div);
	wrapInGraph(ir, entry);

	REQUIRE_NOTHROW(runPass(*ir));

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::DivOp) == 1);
	auto* stillDiv = compiler::ir::dyn_cast<compiler::ir::DivOperation>(entryBlock(*ir)->getOperations()[2]);
	REQUIRE(stillDiv != nullptr);
	REQUIRE(stillDiv->getLeftInput() == c10);
	REQUIRE(stillDiv->getRightInput() == c0);
}

TEST_CASE("ConstantFolding: mod-by-zero is skipped") {
	auto ir = std::make_shared<IRGraph>("cf-mod-zero");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* cl = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {9}, Type::i32);
	auto* cr = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {0}, Type::i32);
	auto* mod = entry->addOperation<compiler::ir::ModOperation>(OperationIdentifier {3}, cl, cr);
	entry->addOperation<compiler::ir::ReturnOperation>(mod);
	wrapInGraph(ir, entry);

	REQUIRE_NOTHROW(runPass(*ir));

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::ModOp) == 1);
}

TEST_CASE("ConstantFolding: compare folds to ConstBoolean") {
	auto ir = std::make_shared<IRGraph>("cf-compare");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* c7 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {7}, Type::i32);
	auto* c9 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {9}, Type::i32);
	auto* cmp = entry->addOperation<compiler::ir::CompareOperation>(OperationIdentifier {3}, c7, c9,
	                                                                compiler::ir::CompareOperation::LT);
	entry->addOperation<compiler::ir::ReturnOperation>(cmp);
	wrapInGraph(ir, entry, Type::b);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::CompareOp) == 0);
	const auto& ops = entryBlock(*ir)->getOperations();
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstBooleanOperation>(ops[2]);
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == true);
	REQUIRE(folded->getStamp() == Type::b);
	auto* ret = compiler::ir::as<compiler::ir::ReturnOperation>(ops.back());
	REQUIRE(ret->getReturnValue() == folded);
}

TEST_CASE("ConstantFolding: copy-propagation through a branch invocation argument") {
	auto ir = std::make_shared<IRGraph>("cf-branch-arg");
	auto& arena = ir->getArena();

	// Return block: takes one block argument, returns it.
	auto* retArg = arena.create<BasicBlockArgument>(OperationIdentifier {20}, Type::i32);
	auto* retBlock = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {retArg});
	retBlock->addOperation<compiler::ir::ReturnOperation>(retArg);

	// Entry block: computes 2 + 4, branches to ret block passing the add.
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* c2 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {2}, Type::i32);
	auto* c4 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {4}, Type::i32);
	auto* add = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {3}, c2, c4);
	Operation* branchArgs[] = {add};
	entry->addNextBlock(retBlock, std::span<Operation* const> {branchArgs, 1});

	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, retBlock},
	                                           std::vector<Type> {}, std::vector<std::string> {}, Type::i32);
	ir->addFunctionOperation(fn);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 0);

	// The branch invocation in the entry block must now carry the folded
	// ConstInt(6) in place of the stale add pointer.
	auto* terminator = entry->getTerminatorOp();
	auto* branch = compiler::ir::as<compiler::ir::BranchOperation>(terminator);
	const auto args = branch->getNextBlockInvocation().getArguments();
	REQUIRE(args.size() == 1);
	auto* forwarded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(args[0]);
	REQUIRE(forwarded != nullptr);
	REQUIRE(forwarded->getValue() == 6);
}

TEST_CASE("ConstantFolding: shift by negative amount is skipped") {
	auto ir = std::make_shared<IRGraph>("cf-neg-shift");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* c1 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {1}, Type::i32);
	auto* cNeg = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {-3}, Type::i32);
	auto* shift = entry->addOperation<compiler::ir::ShiftOperation>(OperationIdentifier {3}, c1, cNeg,
	                                                                compiler::ir::ShiftOperation::LS);
	entry->addOperation<compiler::ir::ReturnOperation>(shift);
	wrapInGraph(ir, entry);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::ShiftOp) == 1);
	auto* stillShift = compiler::ir::dyn_cast<compiler::ir::ShiftOperation>(entryBlock(*ir)->getOperations()[2]);
	REQUIRE(stillShift != nullptr);
	REQUIRE(stillShift->getLeftInput() == c1);
	REQUIRE(stillShift->getRightInput() == cNeg);
}

TEST_CASE("ConstantFolding: boolean And folds") {
	auto ir = std::make_shared<IRGraph>("cf-bool-and");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* cT = entry->addOperation<compiler::ir::ConstBooleanOperation>(OperationIdentifier {1}, true);
	auto* cF = entry->addOperation<compiler::ir::ConstBooleanOperation>(OperationIdentifier {2}, false);
	auto* andOp = entry->addOperation<compiler::ir::AndOperation>(OperationIdentifier {3}, cT, cF);
	entry->addOperation<compiler::ir::ReturnOperation>(andOp);
	wrapInGraph(ir, entry, Type::b);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AndOp) == 0);
	const auto& ops = entryBlock(*ir)->getOperations();
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstBooleanOperation>(ops[2]);
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == false);
}

} // namespace nautilus::testing
