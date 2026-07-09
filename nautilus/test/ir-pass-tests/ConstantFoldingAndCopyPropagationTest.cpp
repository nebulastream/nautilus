#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/compiler/ir/passes/ConstantFoldingAndCopyPropagationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
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

// Regression tests for issue #327: propagateReplacements must rewire EVERY
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
	auto* call = entry->addOperation<compiler::ir::ProxyCallOperation>(
	    OperationIdentifier {4}, std::span<Operation* const> {callArgs, 1}, Type::i32);
	entry->addOperation<compiler::ir::ReturnOperation>(call);
	wrapInGraph(ir, entry);

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 0);
	REQUIRE(call->getInputArguments().size() == 1);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(call->getInputArguments()[0]);
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == 8);
}

} // namespace nautilus::testing
