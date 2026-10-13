#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/compiler/ir/passes/ConstantFoldingAndCopyPropagationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/options.hpp"
#include <catch2/catch_all.hpp>
#include <cmath>
#include <limits>
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

/// Constructs a single-block graph containing an `execute` function that
/// owns @p entry. The entry block is wrapped in the graph's arena so the
/// returned `shared_ptr` keeps everything alive for the duration of the
/// test.
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

BasicBlock* entryBlock(IRGraph& ir) {
	return ir.getFunctionOperations().front()->getEntryBlock();
}

void runPass(IRGraph& ir) {
	engine::Options opts;
	// Enforce pointer-consistent IR after the pass (issue #327): the verifier
	// fails any input edge still pointing at a replaced, removed operation.
	opts.setOption("ir.verifyAfterEachPass", true);
	opts.setOption("ir.failOnVerifyError", true);
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
	// The add's slot is replaced in place, and `eraseIfDead` cascades away
	// the 5 and 3 constants once they become unused -- only the folded
	// constant and the return remain.
	const auto& ops = entryBlock(*ir)->getOperations();
	REQUIRE(ops.size() == 2);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(ops[0]);
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == 8);
	REQUIRE(folded->getStamp() == Type::i32);

	// Copy propagation: return's input is now the folded constant, not the
	// stale AddOp pointer.
	auto* ret = compiler::ir::as<compiler::ir::ReturnOperation>(ops[1]);
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
	wrapInGraph(ir, entry, Type::i32, std::vector<Type> {Type::i32});

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
	// 7 and 9 are cascaded away once the compare that consumed them is
	// folded and erased.
	const auto& ops = entryBlock(*ir)->getOperations();
	REQUIRE(ops.size() == 2);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstBooleanOperation>(ops[0]);
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

// Regression tests for issue #312: folding unsigned integer operations must use
// unsigned semantics. Each uses an operand whose value exceeds INT64_MAX, so a
// signed fold would give a different (wrong) result. Found by the differential
// AST fuzzer (test/fuzz).

TEST_CASE("ConstantFolding: ui64 less-than folds with unsigned semantics (#312)") {
	auto ir = std::make_shared<IRGraph>("cf-ui64-lt");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	// 0x8000000000000000 (2^63) is INT64_MIN when read as signed, so a signed
	// comparison would say (big < 5) == true. Unsigned, big is huge: false.
	auto* big = entry->addOperation<compiler::ir::ConstIntOperation>(
	    OperationIdentifier {1}, static_cast<int64_t>(0x8000000000000000ULL), Type::ui64);
	auto* small =
	    entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {5}, Type::ui64);
	auto* cmp = entry->addOperation<compiler::ir::CompareOperation>(OperationIdentifier {3}, big, small,
	                                                                compiler::ir::CompareOperation::LT);
	entry->addOperation<compiler::ir::ReturnOperation>(cmp);
	wrapInGraph(ir, entry, Type::b);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::CompareOp) == 0);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstBooleanOperation>(entryBlock(*ir)->getOperations()[0]);
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == false);
}

TEST_CASE("ConstantFolding: ui64 division folds with unsigned semantics (#312)") {
	auto ir = std::make_shared<IRGraph>("cf-ui64-div");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	// 0xC000000000000000 = 13835058055282163712; / 3 == 4611686018427387904.
	auto* big = entry->addOperation<compiler::ir::ConstIntOperation>(
	    OperationIdentifier {1}, static_cast<int64_t>(0xC000000000000000ULL), Type::ui64);
	auto* three =
	    entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {3}, Type::ui64);
	auto* div = entry->addOperation<compiler::ir::DivOperation>(OperationIdentifier {3}, big, three);
	entry->addOperation<compiler::ir::ReturnOperation>(div);
	wrapInGraph(ir, entry, Type::ui64);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::DivOp) == 0);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(entryBlock(*ir)->getOperations()[0]);
	REQUIRE(folded != nullptr);
	REQUIRE(static_cast<uint64_t>(folded->getValue()) == 4611686018427387904ULL);
	REQUIRE(folded->getStamp() == Type::ui64);
}

TEST_CASE("ConstantFolding: ui64 modulo folds with unsigned semantics (#312)") {
	auto ir = std::make_shared<IRGraph>("cf-ui64-mod");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	// 13835058055282163712 % 7 == 5 (unsigned).
	auto* big = entry->addOperation<compiler::ir::ConstIntOperation>(
	    OperationIdentifier {1}, static_cast<int64_t>(0xC000000000000000ULL), Type::ui64);
	auto* seven =
	    entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {7}, Type::ui64);
	auto* mod = entry->addOperation<compiler::ir::ModOperation>(OperationIdentifier {3}, big, seven);
	entry->addOperation<compiler::ir::ReturnOperation>(mod);
	wrapInGraph(ir, entry, Type::ui64);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::ModOp) == 0);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(entryBlock(*ir)->getOperations()[0]);
	REQUIRE(folded != nullptr);
	REQUIRE(static_cast<uint64_t>(folded->getValue()) == 5ULL);
}

TEST_CASE("ConstantFolding: ui64 right shift is logical, not arithmetic (#312)") {
	auto ir = std::make_shared<IRGraph>("cf-ui64-shr");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	// 0xC000000000000000 >> 1: logical == 0x6000000000000000 (6917529027641081856);
	// arithmetic would sign-fill to 0xE000000000000000.
	auto* big = entry->addOperation<compiler::ir::ConstIntOperation>(
	    OperationIdentifier {1}, static_cast<int64_t>(0xC000000000000000ULL), Type::ui64);
	auto* one = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {1}, Type::ui64);
	auto* shift = entry->addOperation<compiler::ir::ShiftOperation>(OperationIdentifier {3}, big, one,
	                                                                compiler::ir::ShiftOperation::RS);
	entry->addOperation<compiler::ir::ReturnOperation>(shift);
	wrapInGraph(ir, entry, Type::ui64);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::ShiftOp) == 0);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(entryBlock(*ir)->getOperations()[0]);
	REQUIRE(folded != nullptr);
	REQUIRE(static_cast<uint64_t>(folded->getValue()) == 0x6000000000000000ULL);
}

TEST_CASE("ConstantFolding: signed i64 less-than still folds with signed semantics") {
	auto ir = std::make_shared<IRGraph>("cf-i64-lt-signed");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	// Same bit patterns as the ui64 case, but a signed stamp: -2^63 < 5 == true.
	auto* neg = entry->addOperation<compiler::ir::ConstIntOperation>(
	    OperationIdentifier {1}, static_cast<int64_t>(0x8000000000000000ULL), Type::i64);
	auto* five = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {5}, Type::i64);
	auto* cmp = entry->addOperation<compiler::ir::CompareOperation>(OperationIdentifier {3}, neg, five,
	                                                                compiler::ir::CompareOperation::LT);
	entry->addOperation<compiler::ir::ReturnOperation>(cmp);
	wrapInGraph(ir, entry, Type::b);

	runPass(*ir);

	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstBooleanOperation>(entryBlock(*ir)->getOperations()[0]);
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == true);
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
	// true and false are cascaded away once the And that consumed them is
	// folded and erased.
	const auto& ops = entryBlock(*ir)->getOperations();
	REQUIRE(ops.size() == 2);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstBooleanOperation>(ops[0]);
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == false);
}

// Regression tests for issue #327: folding must rewire EVERY
// consumer kind's input pointer, not just binary/if/return/invocation inputs.
// Each test folds an operation and asserts that the consumer's input edge is
// the replacement constant itself (a stale pointer to the removed operation
// would fail the dyn_cast and, since #327, IR verification). Found while
// developing the AsmJit constant-deferral optimization (PR #326), whose
// pointer-based operand recovery crashed on the stale edges.

TEST_CASE("ConstantFolding: cast consumer is rewired to the folded constant (#327)") {
	auto ir = std::make_shared<IRGraph>("cf-cast-consumer");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	// The issue's repro shape: `202u8 & 7u8` folds to 2, and the cast that
	// widens the shift amount to i32 must consume the folded constant.
	auto* c202 =
	    entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {202}, Type::ui8);
	auto* c7 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {7}, Type::ui8);
	auto* band = entry->addOperation<compiler::ir::BinaryCompOperation>(OperationIdentifier {3}, c202, c7,
	                                                                    compiler::ir::BinaryCompOperation::BAND);
	auto* castOp = entry->addOperation<compiler::ir::CastOperation>(OperationIdentifier {4}, band, Type::i32);
	entry->addOperation<compiler::ir::ReturnOperation>(castOp);
	wrapInGraph(ir, entry);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::BinaryComp) == 0);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(castOp->getInput());
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == 2);
	REQUIRE(folded->getStamp() == Type::ui8);
}

TEST_CASE("ConstantFolding: select consumer is rewired on all three slots (#327)") {
	auto ir = std::make_shared<IRGraph>("cf-select-consumer");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* cT = entry->addOperation<compiler::ir::ConstBooleanOperation>(OperationIdentifier {1}, true);
	auto* cF = entry->addOperation<compiler::ir::ConstBooleanOperation>(OperationIdentifier {2}, false);
	auto* cond = entry->addOperation<compiler::ir::AndOperation>(OperationIdentifier {3}, cT, cF);
	auto* c5 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {4}, int64_t {5}, Type::i32);
	auto* c3 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {5}, int64_t {3}, Type::i32);
	auto* tv = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {6}, c5, c3);
	auto* fv = entry->addOperation<compiler::ir::MulOperation>(OperationIdentifier {7}, c5, c3);
	auto* sel = entry->addOperation<compiler::ir::SelectOperation>(OperationIdentifier {8}, cond, tv, fv, Type::i32);
	entry->addOperation<compiler::ir::ReturnOperation>(sel);
	wrapInGraph(ir, entry);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AndOp) == 0);
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 0);
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::MulOp) == 0);
	auto* foldedCond = compiler::ir::dyn_cast<compiler::ir::ConstBooleanOperation>(sel->getCondition());
	REQUIRE(foldedCond != nullptr);
	REQUIRE(foldedCond->getValue() == false);
	auto* foldedTrue = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(sel->getTrueValue());
	REQUIRE(foldedTrue != nullptr);
	REQUIRE(foldedTrue->getValue() == 8);
	auto* foldedFalse = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(sel->getFalseValue());
	REQUIRE(foldedFalse != nullptr);
	REQUIRE(foldedFalse->getValue() == 15);
}

TEST_CASE("ConstantFolding: unary not consumer is rewired (#327)") {
	auto ir = std::make_shared<IRGraph>("cf-not-consumer");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* cT = entry->addOperation<compiler::ir::ConstBooleanOperation>(OperationIdentifier {1}, true);
	auto* cF = entry->addOperation<compiler::ir::ConstBooleanOperation>(OperationIdentifier {2}, false);
	auto* orOp = entry->addOperation<compiler::ir::OrOperation>(OperationIdentifier {3}, cT, cF);
	auto* notOp = entry->addOperation<compiler::ir::NotOperation>(OperationIdentifier {4}, orOp);
	entry->addOperation<compiler::ir::ReturnOperation>(notOp);
	wrapInGraph(ir, entry, Type::b);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::OrOp) == 0);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstBooleanOperation>(notOp->getInput());
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == true);
}

TEST_CASE("ConstantFolding: store value consumer is rewired (#327)") {
	auto ir = std::make_shared<IRGraph>("cf-store-consumer");
	auto& arena = ir->getArena();
	auto* ptrArg = arena.create<BasicBlockArgument>(OperationIdentifier {10}, Type::ptr);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {ptrArg});
	auto* c5 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {5}, Type::i32);
	auto* c3 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {3}, Type::i32);
	auto* add = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {3}, c5, c3);
	auto* store = entry->addOperation<compiler::ir::StoreOperation>(add, ptrArg);
	entry->addOperation<compiler::ir::ReturnOperation>();
	wrapInGraph(ir, entry, Type::v, std::vector<Type> {Type::ptr});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 0);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(store->getValue());
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == 8);
	REQUIRE(store->getAddress() == ptrArg);
}

TEST_CASE("ConstantFolding: proxy call argument is rewired (#327)") {
	auto ir = std::make_shared<IRGraph>("cf-call-consumer");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* c5 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {5}, Type::i32);
	auto* c3 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {3}, Type::i32);
	auto* add = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {3}, c5, c3);
	Operation* callArgs[] = {add};
	compiler::ir::CalleeDescriptor callee;
	callee.key = reinterpret_cast<void*>(0x1000);
	callee.demangledName = "callConsumer";
	callee.resultType = Type::i32;
	callee.paramTypes = {Type::i32};
	auto* call = entry->addOperation<compiler::ir::CallOperation>(
	    OperationIdentifier {4}, std::span<Operation* const> {callArgs, 1}, Type::i32, ir->internCallee(callee));
	entry->addOperation<compiler::ir::ReturnOperation>(call);
	wrapInGraph(ir, entry);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 0);
	REQUIRE(call->getInputArguments().size() == 1);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(call->getInputArguments()[0]);
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == 8);
}

namespace {

using compiler::ir::BinaryCompOperation;
using compiler::ir::CompareOperation;
using compiler::ir::ShiftOperation;
using OpType = Operation::OperationType;

/// A one-block `execute` function built by a test, before and after the pass.
struct SingleBlock {
	std::shared_ptr<IRGraph> ir;
	BasicBlock* entry;
	uint32_t nextId = 1;

	explicit SingleBlock(const std::string& name) : ir(std::make_shared<IRGraph>(name)) {
		auto& arena = ir->getArena();
		entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	}

	OperationIdentifier id() {
		return OperationIdentifier {nextId++};
	}
	Operation* i(int64_t value, Type stamp) {
		return entry->addOperation<compiler::ir::ConstIntOperation>(id(), value, stamp);
	}
	Operation* f(double value, Type stamp) {
		return entry->addOperation<compiler::ir::ConstFloatOperation>(id(), value, stamp);
	}
	Operation* b(bool value) {
		return entry->addOperation<compiler::ir::ConstBooleanOperation>(id(), value);
	}
	template <typename T, typename... Args>
	Operation* op(Args&&... args) {
		return entry->addOperation<T>(id(), std::forward<Args>(args)...);
	}

	/// Returns @p value, runs the pass with verification on, and returns
	/// what the function returns afterwards.
	Operation* foldReturn(Operation* value) {
		entry->addOperation<compiler::ir::ReturnOperation>(value);
		wrapInGraph(ir, entry, value->getStamp());
		runPass(*ir);
		return compiler::ir::as<compiler::ir::ReturnOperation>(entry->getOperations().back())->getReturnValue();
	}
};

/// What a fold helper returns: the folded value together with the graph that
/// owns it. The helpers build their graph locally, so returning the bare
/// `Operation*` would leave it pointing into a freed arena.
struct Folded {
	std::shared_ptr<IRGraph> ir;
	Operation* op;
	Operation* operator->() const {
		return op;
	}
};

int64_t intValue(Operation* op) {
	auto* c = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(op);
	REQUIRE(c != nullptr);
	return c->getValue();
}

double floatValue(Operation* op) {
	auto* c = compiler::ir::dyn_cast<compiler::ir::ConstFloatOperation>(op);
	REQUIRE(c != nullptr);
	return c->getValue();
}

bool boolValue(Operation* op) {
	auto* c = compiler::ir::dyn_cast<compiler::ir::ConstBooleanOperation>(op);
	REQUIRE(c != nullptr);
	return c->getValue();
}

int64_t intValue(const Folded& f) {
	return intValue(f.op);
}

double floatValue(const Folded& f) {
	return floatValue(f.op);
}

bool boolValue(const Folded& f) {
	return boolValue(f.op);
}

template <typename T>
Folded foldIntBinary(Type stamp, int64_t l, int64_t r) {
	SingleBlock g("cf-int-binary");
	auto* result = g.foldReturn(g.op<T>(g.i(l, stamp), g.i(r, stamp)));
	return {g.ir, result};
}

Folded foldShift(Type stamp, int64_t l, int64_t r, ShiftOperation::ShiftType type) {
	SingleBlock g("cf-shift");
	auto* result = g.foldReturn(g.op<ShiftOperation>(g.i(l, stamp), g.i(r, stamp), type));
	return {g.ir, result};
}

Folded foldIntCompare(Type stamp, int64_t l, int64_t r, CompareOperation::Comparator cmp) {
	SingleBlock g("cf-int-compare");
	auto* result = g.foldReturn(g.op<CompareOperation>(g.i(l, stamp), g.i(r, stamp), cmp));
	return {g.ir, result};
}

Folded foldFloatCompare(double l, double r, CompareOperation::Comparator cmp) {
	SingleBlock g("cf-float-compare");
	auto* result = g.foldReturn(g.op<CompareOperation>(g.f(l, Type::f64), g.f(r, Type::f64), cmp));
	return {g.ir, result};
}

template <typename T>
Folded foldFloatBinary(Type stamp, double l, double r) {
	SingleBlock g("cf-float-binary");
	auto* result = g.foldReturn(g.op<T>(g.f(l, stamp), g.f(r, stamp)));
	return {g.ir, result};
}

Folded foldIntCast(int64_t value, Type from, Type to) {
	SingleBlock g("cf-int-cast");
	auto* result = g.foldReturn(g.op<compiler::ir::CastOperation>(g.i(value, from), to));
	return {g.ir, result};
}

Folded foldFloatCast(double value, Type from, Type to) {
	SingleBlock g("cf-float-cast");
	auto* result = g.foldReturn(g.op<compiler::ir::CastOperation>(g.f(value, from), to));
	return {g.ir, result};
}

constexpr int64_t I64_MIN = std::numeric_limits<int64_t>::min();
constexpr int64_t I64_MAX = std::numeric_limits<int64_t>::max();
constexpr int64_t I32_MIN = std::numeric_limits<int32_t>::min();
constexpr int64_t I32_MAX = std::numeric_limits<int32_t>::max();
constexpr int64_t UI32_MAX = std::numeric_limits<uint32_t>::max();

} // namespace

TEST_CASE("ConstantFolding: integer arithmetic folds with two's-complement semantics") {
	CHECK(intValue(foldIntBinary<compiler::ir::SubOperation>(Type::i32, 3, 10)) == -7);
	CHECK(intValue(foldIntBinary<compiler::ir::SubOperation>(Type::i64, I64_MIN, 1)) == I64_MAX);
	CHECK(intValue(foldIntBinary<compiler::ir::MulOperation>(Type::i64, I64_MAX, 2)) == -2);
	// Signed division truncates toward zero; the remainder takes the dividend's sign.
	CHECK(intValue(foldIntBinary<compiler::ir::DivOperation>(Type::i32, -7, 2)) == -3);
	CHECK(intValue(foldIntBinary<compiler::ir::ModOperation>(Type::i32, -7, 2)) == -1);
	CHECK(intValue(foldIntBinary<compiler::ir::ModOperation>(Type::i32, 7, -2)) == 1);
	CHECK(intValue(foldIntBinary<compiler::ir::DivOperation>(Type::ui32, UI32_MAX, 2)) == 2147483647);
}

TEST_CASE("ConstantFolding: results are wrapped to every narrow stamp") {
	CHECK(intValue(foldIntBinary<compiler::ir::AddOperation>(Type::i8, 127, 1)) == -128);
	CHECK(intValue(foldIntBinary<compiler::ir::AddOperation>(Type::i16, 32767, 1)) == -32768);
	CHECK(intValue(foldIntBinary<compiler::ir::AddOperation>(Type::i32, I32_MAX, 1)) == I32_MIN);
	CHECK(intValue(foldIntBinary<compiler::ir::AddOperation>(Type::ui8, 255, 1)) == 0);
	CHECK(intValue(foldIntBinary<compiler::ir::AddOperation>(Type::ui16, 65535, 1)) == 0);
	CHECK(intValue(foldIntBinary<compiler::ir::AddOperation>(Type::ui32, UI32_MAX, 1)) == 0);
	// Unsigned stamps stay zero-extended: 0 - 1 is 2^32 - 1, not -1.
	CHECK(intValue(foldIntBinary<compiler::ir::SubOperation>(Type::ui32, 0, 1)) == UI32_MAX);
	CHECK(intValue(foldIntBinary<compiler::ir::MulOperation>(Type::ui8, 16, 16)) == 0);
}

TEST_CASE("ConstantFolding: signed MIN / -1 and MIN % -1 are not folded") {
	// i64 used to crash the compiler itself: the fold executed the
	// overflowing division (SIGFPE on x86).
	for (auto [stamp, min] : {std::pair {Type::i64, I64_MIN}, std::pair {Type::i32, I32_MIN},
	                          std::pair {Type::i16, int64_t {-32768}}, std::pair {Type::i8, int64_t {-128}}}) {
		INFO("stamp " << toString(stamp));
		auto div = foldIntBinary<compiler::ir::DivOperation>(stamp, min, -1);
		CHECK(div->getOperationType() == OpType::DivOp);
		auto mod = foldIntBinary<compiler::ir::ModOperation>(stamp, min, -1);
		CHECK(mod->getOperationType() == OpType::ModOp);
	}
	// Neighbours of the overflow still fold.
	CHECK(intValue(foldIntBinary<compiler::ir::DivOperation>(Type::i64, I64_MIN + 1, -1)) == I64_MAX);
	CHECK(intValue(foldIntBinary<compiler::ir::DivOperation>(Type::i64, I64_MIN, 1)) == I64_MIN);
	// Unsigned has no overflow: UI32_MAX is the "-1" bit pattern but folds normally.
	CHECK(intValue(foldIntBinary<compiler::ir::DivOperation>(Type::ui32, 0x80000000, UI32_MAX)) == 0);
}

TEST_CASE("ConstantFolding: shifts fold per stamp width and signedness") {
	CHECK(intValue(foldShift(Type::i32, 1, 31, ShiftOperation::LS)) == I32_MIN);
	CHECK(intValue(foldShift(Type::i64, 1, 63, ShiftOperation::LS)) == I64_MIN);
	CHECK(intValue(foldShift(Type::ui32, 0xFFFF, 16, ShiftOperation::LS)) == 0xFFFF0000);
	CHECK(intValue(foldShift(Type::i32, -8, 1, ShiftOperation::RS)) == -4);
	CHECK(intValue(foldShift(Type::ui32, 0xFFFFFFF8, 1, ShiftOperation::RS)) == 0x7FFFFFFC);
	CHECK(intValue(foldShift(Type::i64, 5, 0, ShiftOperation::RS)) == 5);
}

TEST_CASE("ConstantFolding: shift amounts of at least the bit width are not folded") {
	// Undefined at runtime, and the backends disagree on the result.
	CHECK(foldShift(Type::i32, 1, 32, ShiftOperation::LS)->getOperationType() == OpType::ShiftOp);
	CHECK(foldShift(Type::i32, 1, 40, ShiftOperation::RS)->getOperationType() == OpType::ShiftOp);
	CHECK(foldShift(Type::i64, 1, 64, ShiftOperation::LS)->getOperationType() == OpType::ShiftOp);
	CHECK(foldShift(Type::ui64, 1, 100, ShiftOperation::RS)->getOperationType() == OpType::ShiftOp);
}

TEST_CASE("ConstantFolding: every integer comparator folds, signed and unsigned") {
	using C = CompareOperation;
	// -1 < 1 signed, but as ui32 the left operand is 2^32 - 1.
	const std::tuple<C::Comparator, bool, bool> cases[] = {
	    {C::EQ, false, false}, {C::NE, true, true},  {C::LT, true, false},
	    {C::LE, true, false},  {C::GT, false, true}, {C::GE, false, true},
	};
	for (auto [cmp, signedResult, unsignedResult] : cases) {
		INFO("comparator " << static_cast<int>(cmp));
		CHECK(boolValue(foldIntCompare(Type::i32, -1, 1, cmp)) == signedResult);
		CHECK(boolValue(foldIntCompare(Type::ui32, UI32_MAX, 1, cmp)) == unsignedResult);
	}
	CHECK(boolValue(foldIntCompare(Type::i64, 4, 4, C::EQ)));
	CHECK(boolValue(foldIntCompare(Type::i64, 4, 4, C::LE)));
	CHECK(boolValue(foldIntCompare(Type::i64, 4, 4, C::GE)));
}

TEST_CASE("ConstantFolding: bitwise ops fold on integers and booleans") {
	SECTION("integers") {
		SingleBlock g("cf-bitwise-int");
		auto* bor = g.op<BinaryCompOperation>(g.i(0b1010, Type::i32), g.i(0b0110, Type::i32), BinaryCompOperation::BOR);
		auto* bxor = g.op<BinaryCompOperation>(bor, g.i(0b0011, Type::i32), BinaryCompOperation::XOR);
		CHECK(intValue(g.foldReturn(bxor)) == 0b1101);
	}
	SECTION("booleans") {
		const std::tuple<BinaryCompOperation::Type, bool, bool, bool> cases[] = {
		    {BinaryCompOperation::BAND, true, false, false}, {BinaryCompOperation::BAND, true, true, true},
		    {BinaryCompOperation::BOR, false, false, false}, {BinaryCompOperation::BOR, true, false, true},
		    {BinaryCompOperation::XOR, true, true, false},   {BinaryCompOperation::XOR, true, false, true},
		};
		for (auto [type, l, r, expected] : cases) {
			SingleBlock g("cf-bitwise-bool");
			CHECK(boolValue(g.foldReturn(g.op<BinaryCompOperation>(g.b(l), g.b(r), type))) == expected);
		}
	}
	SECTION("logical or") {
		SingleBlock g("cf-or");
		CHECK(boolValue(g.foldReturn(g.op<compiler::ir::OrOperation>(g.b(false), g.b(true)))));
	}
}

TEST_CASE("ConstantFolding: boolean compares fold only for equality") {
	SingleBlock eq("cf-bool-eq");
	CHECK(boolValue(eq.foldReturn(eq.op<CompareOperation>(eq.b(true), eq.b(true), CompareOperation::EQ))));
	SingleBlock ne("cf-bool-ne");
	CHECK(boolValue(ne.foldReturn(ne.op<CompareOperation>(ne.b(true), ne.b(false), CompareOperation::NE))));
	SingleBlock lt("cf-bool-lt");
	CHECK(lt.foldReturn(lt.op<CompareOperation>(lt.b(false), lt.b(true), CompareOperation::LT))->getOperationType() ==
	      OpType::CompareOp);
}

TEST_CASE("ConstantFolding: f64 arithmetic folds with IEEE semantics") {
	CHECK(floatValue(foldFloatBinary<compiler::ir::AddOperation>(Type::f64, 0.1, 0.2)) == 0.1 + 0.2);
	CHECK(floatValue(foldFloatBinary<compiler::ir::SubOperation>(Type::f64, 1.0, 3.5)) == -2.5);
	CHECK(floatValue(foldFloatBinary<compiler::ir::MulOperation>(Type::f64, 1e300, 1e10)) ==
	      std::numeric_limits<double>::infinity());
	CHECK(floatValue(foldFloatBinary<compiler::ir::DivOperation>(Type::f64, 1.0, 3.0)) == 1.0 / 3.0);
	// Division by (signed) zero and float modulo stay in place.
	CHECK(foldFloatBinary<compiler::ir::DivOperation>(Type::f64, 1.0, 0.0)->getOperationType() == OpType::DivOp);
	CHECK(foldFloatBinary<compiler::ir::DivOperation>(Type::f64, 1.0, -0.0)->getOperationType() == OpType::DivOp);
	CHECK(foldFloatBinary<compiler::ir::ModOperation>(Type::f64, 7.0, 2.0)->getOperationType() == OpType::ModOp);
}

TEST_CASE("ConstantFolding: f32 arithmetic rounds to float (#542)") {
	const double a = static_cast<double>(0.1f);
	const double b = static_cast<double>(0.2f);
	CHECK(floatValue(foldFloatBinary<compiler::ir::AddOperation>(Type::f32, a, b)) == static_cast<double>(0.1f + 0.2f));
	// Underflows to 0 in f32, about 2.7e-84 in f64.
	CHECK(
	    floatValue(foldFloatBinary<compiler::ir::MulOperation>(Type::f32, static_cast<double>(9.1834094859526887e-41f),
	                                                           static_cast<double>(2.9427267750821158e-44f))) == 0.0);
	// Overflows to -inf in f32, finite in f64.
	CHECK(floatValue(foldFloatBinary<compiler::ir::MulOperation>(
	          Type::f32, static_cast<double>(-2.5353030138451882e+30f),
	          static_cast<double>(7.5557863725914323e+22f))) == -std::numeric_limits<double>::infinity());
	auto quotient = foldFloatBinary<compiler::ir::DivOperation>(Type::f32, 1.0, 3.0);
	CHECK(floatValue(quotient) == static_cast<double>(1.0f / 3.0f));
	CHECK(quotient->getStamp() == Type::f32);
}

TEST_CASE("ConstantFolding: float compares follow IEEE ordering, including NaN and -0") {
	using C = CompareOperation;
	const double nan = std::numeric_limits<double>::quiet_NaN();
	CHECK_FALSE(boolValue(foldFloatCompare(nan, nan, C::EQ)));
	CHECK(boolValue(foldFloatCompare(nan, 1.0, C::NE)));
	CHECK_FALSE(boolValue(foldFloatCompare(nan, 1.0, C::LT)));
	CHECK_FALSE(boolValue(foldFloatCompare(nan, 1.0, C::GE)));
	CHECK(boolValue(foldFloatCompare(-0.0, 0.0, C::EQ)));
	CHECK(boolValue(foldFloatCompare(-1.5, 2.0, C::LE)));
	CHECK(boolValue(foldFloatCompare(2.0, -1.5, C::GT)));
}

TEST_CASE("ConstantFolding: mixed operand kinds are not folded") {
	SingleBlock g("cf-mixed");
	auto* add = g.op<compiler::ir::AddOperation>(g.i(1, Type::i32), g.f(1.0, Type::f64));
	CHECK(g.foldReturn(add)->getOperationType() == OpType::AddOp);
	SingleBlock h("cf-mixed-bool");
	auto* cmp = h.op<CompareOperation>(h.b(true), h.i(1, Type::i32), CompareOperation::EQ);
	CHECK(h.foldReturn(cmp)->getOperationType() == OpType::CompareOp);
	// Each op only folds the operand kinds it is defined on.
	SingleBlock k("cf-and-ints");
	CHECK(k.foldReturn(k.op<compiler::ir::AndOperation>(k.i(1, Type::i32), k.i(1, Type::i32)))->getOperationType() ==
	      OpType::AndOp);
	SingleBlock l("cf-or-ints");
	CHECK(l.foldReturn(l.op<compiler::ir::OrOperation>(l.i(1, Type::i32), l.i(0, Type::i32)))->getOperationType() ==
	      OpType::OrOp);
	SingleBlock m("cf-shift-floats");
	CHECK(m.foldReturn(m.op<ShiftOperation>(m.f(1.0, Type::f64), m.f(1.0, Type::f64), ShiftOperation::LS))
	          ->getOperationType() == OpType::ShiftOp);
	SingleBlock n("cf-bitwise-floats");
	CHECK(n.foldReturn(n.op<BinaryCompOperation>(n.f(1.0, Type::f64), n.f(1.0, Type::f64), BinaryCompOperation::BOR))
	          ->getOperationType() == OpType::BinaryComp);
}

TEST_CASE("ConstantFolding: logical not of a constant folds") {
	SingleBlock g("cf-not");
	CHECK(boolValue(g.foldReturn(g.op<compiler::ir::NotOperation>(g.b(true)))) == false);
	// not(not(c)) collapses through the worklist.
	SingleBlock h("cf-not-not");
	auto* inner = h.op<compiler::ir::NotOperation>(h.b(false));
	CHECK(boolValue(h.foldReturn(h.op<compiler::ir::NotOperation>(inner))) == false);
}

TEST_CASE("ConstantFolding: negate is bitwise not for integers and a sign flip for floats") {
	auto negate = [](SingleBlock& g, Operation* in) {
		return g.foldReturn(g.op<compiler::ir::NegateOperation>(in));
	};
	SingleBlock a("cf-neg-i32");
	CHECK(intValue(negate(a, a.i(5, Type::i32))) == -6);
	SingleBlock b("cf-neg-ui8");
	CHECK(intValue(negate(b, b.i(0, Type::ui8))) == 255);
	SingleBlock c("cf-neg-ui64");
	CHECK(intValue(negate(c, c.i(0, Type::ui64))) == -1); // all ones
	SingleBlock d("cf-neg-f64");
	const double negZero = floatValue(negate(d, d.f(0.0, Type::f64)));
	CHECK(negZero == 0.0);
	CHECK(std::signbit(negZero));
	SingleBlock e("cf-neg-f32");
	auto* f32 = negate(e, e.f(1.5, Type::f32));
	CHECK(floatValue(f32) == -1.5);
	CHECK(f32->getStamp() == Type::f32);
}

TEST_CASE("ConstantFolding: integer casts extend by source signedness and wrap to the target") {
	CHECK(intValue(foldIntCast(-1, Type::i8, Type::ui32)) == UI32_MAX);  // sign-extended, then unsigned
	CHECK(intValue(foldIntCast(255, Type::ui8, Type::i32)) == 255);      // zero-extended
	CHECK(intValue(foldIntCast(-1, Type::i32, Type::ui32)) == UI32_MAX); // same width: reinterpret
	CHECK(intValue(foldIntCast(UI32_MAX, Type::ui32, Type::i32)) == -1);
	CHECK(intValue(foldIntCast(0x100000001, Type::i64, Type::i32)) == 1); // truncate
	CHECK(intValue(foldIntCast(-129, Type::i32, Type::i8)) == 127);
	CHECK(intValue(foldIntCast(-1, Type::i64, Type::ui64)) == -1); // bit pattern kept
	auto stamped = foldIntCast(7, Type::i32, Type::i16);
	CHECK(stamped->getStamp() == Type::i16);
}

TEST_CASE("ConstantFolding: integer to float casts round once") {
	CHECK(floatValue(foldIntCast(-3, Type::i32, Type::f64)) == -3.0);
	CHECK(floatValue(foldIntCast(-1, Type::ui64, Type::f64)) == 18446744073709551615.0);
	// 2^60 + 2^36 + 1 rounds up to 2^60 + 2^37 in f32 when converted
	// directly; going through f64 first loses the +1 and then ties to even,
	// giving 2^60.
	const int64_t x = (int64_t {1} << 60) + (int64_t {1} << 36) + 1;
	auto f32 = foldIntCast(x, Type::i64, Type::f32);
	CHECK(floatValue(f32) == static_cast<double>(static_cast<float>(x)));
	CHECK(floatValue(f32) == std::ldexp(1.0, 60) + std::ldexp(1.0, 37));
	CHECK(f32->getStamp() == Type::f32);
}

TEST_CASE("ConstantFolding: float to integer casts fold only in range") {
	CHECK(intValue(foldFloatCast(3.9, Type::f64, Type::i32)) == 3);
	CHECK(intValue(foldFloatCast(-3.9, Type::f64, Type::i32)) == -3);
	CHECK(intValue(foldFloatCast(-0.9, Type::f64, Type::ui32)) == 0); // truncates to 0: in range
	CHECK(intValue(foldFloatCast(4294967295.0, Type::f64, Type::ui32)) == UI32_MAX);
	CHECK(intValue(foldFloatCast(-2147483648.0, Type::f64, Type::i32)) == I32_MIN);
	CHECK(intValue(foldFloatCast(-9223372036854775808.0, Type::f64, Type::i64)) == I64_MIN);
	// Undefined conversions (LLVM poison) stay in place.
	const double nan = std::numeric_limits<double>::quiet_NaN();
	const double inf = std::numeric_limits<double>::infinity();
	for (auto [value, to] : {std::pair {nan, Type::i32}, std::pair {inf, Type::i64}, std::pair {-inf, Type::ui8},
	                         std::pair {2147483648.0, Type::i32}, std::pair {-2147483649.0, Type::i32},
	                         std::pair {-1.0, Type::ui32}, std::pair {4294967296.0, Type::ui32},
	                         std::pair {9223372036854775808.0, Type::i64}, std::pair {256.0, Type::ui8}}) {
		INFO(value << " -> " << toString(to));
		CHECK(foldFloatCast(value, Type::f64, to)->getOperationType() == OpType::CastOp);
	}
}

TEST_CASE("ConstantFolding: float to float casts round to f32 and keep special values") {
	CHECK(floatValue(foldFloatCast(0.1, Type::f64, Type::f32)) == static_cast<double>(0.1f));
	CHECK(floatValue(foldFloatCast(static_cast<double>(0.1f), Type::f32, Type::f64)) == static_cast<double>(0.1f));
	CHECK(std::isinf(floatValue(foldFloatCast(std::numeric_limits<double>::infinity(), Type::f64, Type::f32))));
	CHECK(std::isnan(floatValue(foldFloatCast(std::numeric_limits<double>::quiet_NaN(), Type::f64, Type::f32))));
	// A finite f64 beyond f32's range would overflow: left alone.
	CHECK(foldFloatCast(1e300, Type::f64, Type::f32)->getOperationType() == OpType::CastOp);
}

TEST_CASE("ConstantFolding: casts to and from pointers are not folded") {
	CHECK(foldIntCast(4096, Type::i64, Type::ptr)->getOperationType() == OpType::CastOp);
	CHECK(foldFloatCast(4096.0, Type::f64, Type::ptr)->getOperationType() == OpType::CastOp);
}

TEST_CASE("ConstantFolding: casts to bool test against zero") {
	CHECK_FALSE(boolValue(foldIntCast(0, Type::i32, Type::b)));
	CHECK(boolValue(foldIntCast(-5, Type::i64, Type::b)));
	CHECK_FALSE(boolValue(foldFloatCast(-0.0, Type::f64, Type::b)));
	CHECK(boolValue(foldFloatCast(0.5, Type::f32, Type::b)));
	CHECK(boolValue(foldFloatCast(std::numeric_limits<double>::quiet_NaN(), Type::f64, Type::b)));
}

TEST_CASE("ConstantFolding: unary folds chain with binary folds") {
	// cast<f64>(-(3 + 4)) == -8.0: Add, then Negate, then Cast fold in turn.
	SingleBlock g("cf-unary-chain");
	auto* add = g.op<compiler::ir::AddOperation>(g.i(3, Type::i32), g.i(4, Type::i32));
	auto* neg = g.op<compiler::ir::NegateOperation>(add);
	auto* cast = g.op<compiler::ir::CastOperation>(neg, Type::f64);
	auto* result = g.foldReturn(cast);
	CHECK(floatValue(result) == -8.0);
	// Only the folded constant and the return are left.
	CHECK(g.entry->getOperations().size() == 2);
}

TEST_CASE("ConstantFolding: apply reports whether it changed anything and is idempotent") {
	SingleBlock g("cf-changed");
	auto* add = g.op<compiler::ir::AddOperation>(g.i(1, Type::i32), g.i(2, Type::i32));
	g.entry->addOperation<compiler::ir::ReturnOperation>(add);
	wrapInGraph(g.ir, g.entry);

	compiler::ir::ConstantFoldingAndCopyPropagationPass pass;
	CHECK(pass.apply(*g.ir));
	const auto opsAfterFirst = g.entry->getOperations();
	CHECK_FALSE(pass.apply(*g.ir));
	CHECK(g.entry->getOperations() == opsAfterFirst);

	SingleBlock nothing("cf-unchanged");
	auto* arg = g.ir->getArena().create<BasicBlockArgument>(OperationIdentifier {99}, Type::i32);
	auto* entry = nothing.ir->getArena().create<BasicBlock>(nothing.ir->getArena(), BlockIdentifier {0},
	                                                        std::vector<BasicBlockArgument*> {arg});
	auto* addArg = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {1}, arg, arg);
	entry->addOperation<compiler::ir::ReturnOperation>(addArg);
	wrapInGraph(nothing.ir, entry, Type::i32, {Type::i32});
	CHECK_FALSE(pass.apply(*nothing.ir));
}

TEST_CASE("ConstantFolding: every function of a module is folded") {
	auto ir = std::make_shared<IRGraph>("cf-two-functions");
	auto& arena = ir->getArena();
	for (const char* name : {"first", "second"}) {
		auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
		auto* l = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {6}, Type::i64);
		auto* r = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {7}, Type::i64);
		auto* mul = entry->addOperation<compiler::ir::MulOperation>(OperationIdentifier {3}, l, r);
		entry->addOperation<compiler::ir::ReturnOperation>(mul);
		ir->addFunctionOperation(arena.create<FunctionOperation>(
		    name, std::vector<BasicBlock*> {entry}, std::vector<Type> {}, std::vector<std::string> {}, Type::i64));
	}
	runPass(*ir);
	for (auto* fn : ir->getFunctionOperations()) {
		auto* ret = compiler::ir::as<compiler::ir::ReturnOperation>(fn->getEntryBlock()->getTerminatorOp());
		CHECK(intValue(ret->getReturnValue()) == 42);
	}
}

TEST_CASE("ConstantFolding: constants defined in one block fold uses in another") {
	auto ir = std::make_shared<IRGraph>("cf-cross-block");
	auto& arena = ir->getArena();
	auto* exit = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {});
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* c2 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {1}, int64_t {2}, Type::i32);
	auto* c5 = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {5}, Type::i32);
	entry->addNextBlock(exit, {});
	auto* mul = exit->addOperation<compiler::ir::MulOperation>(OperationIdentifier {3}, c2, c5);
	exit->addOperation<compiler::ir::ReturnOperation>(mul);
	ir->addFunctionOperation(arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, exit},
	                                                         std::vector<Type> {}, std::vector<std::string> {},
	                                                         Type::i32));

	runPass(*ir);

	auto* ret = compiler::ir::as<compiler::ir::ReturnOperation>(exit->getTerminatorOp());
	CHECK(intValue(ret->getReturnValue()) == 10);
	// The operands, orphaned in the other block, are swept there too.
	CHECK(entry->getOperations().size() == 1);
	CHECK(exit->getOperations().size() == 2);
}

TEST_CASE("ConstantFolding: unfoldable ops stay put while their neighbours fold") {
	// 1/0 must survive (it would trap) while the surrounding adds fold, and
	// the block keeps its order.
	SingleBlock g("cf-mixed-foldability");
	auto* sum = g.op<compiler::ir::AddOperation>(g.i(1, Type::i32), g.i(2, Type::i32));
	auto* div = g.op<compiler::ir::DivOperation>(sum, g.i(0, Type::i32));
	auto* after = g.op<compiler::ir::AddOperation>(div, g.i(4, Type::i32));
	auto* result = g.foldReturn(after);
	CHECK(result == after);
	auto* keptDiv = compiler::ir::as<compiler::ir::DivOperation>(after->getInputs()[0]);
	CHECK(intValue(keptDiv->getInputs()[0]) == 3);
	const auto& ops = g.entry->getOperations();
	REQUIRE(ops.size() == 6); // 3, 0, div, 4, add, return
	CHECK(std::find(ops.begin(), ops.end(), keptDiv) < std::find(ops.begin(), ops.end(), after));
}

TEST_CASE("ConstantFolding: a long chain folds completely and keeps a valid block") {
	// Exercises the in-place replacement and deferred sweep at scale: each
	// fold orphans two constants, all removed in one sweep at the end.
	SingleBlock g("cf-long-chain");
	constexpr int64_t n = 20000;
	Operation* acc = g.i(0, Type::i64);
	for (int64_t k = 1; k <= n; ++k) {
		acc = g.op<compiler::ir::AddOperation>(acc, g.i(k, Type::i64));
	}
	CHECK(intValue(g.foldReturn(acc)) == n * (n + 1) / 2);
	CHECK(g.entry->getOperations().size() == 2);
}

TEST_CASE("ConstantFolding: independent folds with interleaved survivors") {
	// Many independent compares in one block, each feeding an op that cannot
	// fold: the folded constants must land exactly in the compares' slots.
	auto ir = std::make_shared<IRGraph>("cf-many-compares");
	auto& arena = ir->getArena();
	auto* arg = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::b);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {arg});
	uint32_t id = 2;
	Operation* acc = arg;
	std::vector<Operation*> ands;
	for (int k = 0; k < 2000; ++k) {
		auto* l =
		    entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {id++}, int64_t {k}, Type::i32);
		auto* r =
		    entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {id++}, int64_t {1000}, Type::i32);
		auto* cmp = entry->addOperation<CompareOperation>(OperationIdentifier {id++}, l, r, CompareOperation::LT);
		acc = entry->addOperation<compiler::ir::AndOperation>(OperationIdentifier {id++}, acc, cmp);
		ands.push_back(acc);
	}
	entry->addOperation<compiler::ir::ReturnOperation>(acc);
	wrapInGraph(ir, entry, Type::b, {Type::b});

	runPass(*ir);

	const auto& ops = entry->getOperations();
	REQUIRE(ops.size() == 2 * ands.size() + 1);
	for (size_t k = 0; k < ands.size(); ++k) {
		CHECK(boolValue(ops[2 * k]) == (k < 1000));
		CHECK(ops[2 * k + 1] == ands[k]);
		CHECK(ands[k]->getInputs()[1] == ops[2 * k]);
	}
}

} // namespace nautilus::testing
