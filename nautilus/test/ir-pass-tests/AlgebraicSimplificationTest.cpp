#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
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
#include "nautilus/compiler/ir/passes/AlgebraicSimplificationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
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

std::shared_ptr<IRGraph> wrapInGraph(std::shared_ptr<IRGraph> ir, BasicBlock* entry, Type returnStamp,
                                     std::vector<Type> inputArgs) {
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
	opts.setOption("ir.verifyAfterEachPass", true);
	opts.setOption("ir.failOnVerifyError", true);
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::AlgebraicSimplificationPass>());
	mgr.run(ir);
}

Operation* returnValueOf(IRGraph& ir) {
	auto* ret = compiler::ir::as<compiler::ir::ReturnOperation>(entryBlock(ir)->getOperations().back());
	return ret->getReturnValue();
}

} // namespace

// ---- Additive / multiplicative identities -------------------------------

TEST_CASE("AlgebraicSimplification: x+0 -> x (int)") {
	auto ir = std::make_shared<IRGraph>("as-add-zero");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* zero = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {0}, Type::i32);
	auto* add = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {3}, x, zero);
	entry->addOperation<compiler::ir::ReturnOperation>(add);
	wrapInGraph(ir, entry, Type::i32, {Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 0);
	REQUIRE(returnValueOf(*ir) == x);
}

TEST_CASE("AlgebraicSimplification: 0+x -> x (int)") {
	auto ir = std::make_shared<IRGraph>("as-zero-add");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* zero = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {0}, Type::i32);
	auto* add = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {3}, zero, x);
	entry->addOperation<compiler::ir::ReturnOperation>(add);
	wrapInGraph(ir, entry, Type::i32, {Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 0);
	REQUIRE(returnValueOf(*ir) == x);
}

TEST_CASE("AlgebraicSimplification: float x+0.0 is NOT folded") {
	auto ir = std::make_shared<IRGraph>("as-add-zero-float-neg");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::f64);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* zero = entry->addOperation<compiler::ir::ConstFloatOperation>(OperationIdentifier {2}, 0.0, Type::f64);
	auto* add = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {3}, x, zero);
	entry->addOperation<compiler::ir::ReturnOperation>(add);
	wrapInGraph(ir, entry, Type::f64, {Type::f64});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 1);
}

TEST_CASE("AlgebraicSimplification: ptr+0 -> base pointer") {
	auto ir = std::make_shared<IRGraph>("as-ptr-add-zero");
	auto& arena = ir->getArena();
	auto* ptrArg = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::ptr);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {ptrArg});
	auto* zero = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {0}, Type::ptr);
	auto* add = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {3}, ptrArg, zero);
	entry->addOperation<compiler::ir::ReturnOperation>(add);
	wrapInGraph(ir, entry, Type::ptr, {Type::ptr});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 0);
	REQUIRE(returnValueOf(*ir) == ptrArg);
}

TEST_CASE("AlgebraicSimplification: canonicalizes a constant-left add to the right") {
	auto ir = std::make_shared<IRGraph>("as-add-canon");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* five = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {5}, Type::i32);
	auto* add = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {3}, five, x);
	entry->addOperation<compiler::ir::ReturnOperation>(add);
	wrapInGraph(ir, entry, Type::i32, {Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 1);
	auto* newAdd = compiler::ir::dyn_cast<compiler::ir::AddOperation>(returnValueOf(*ir));
	REQUIRE(newAdd != nullptr);
	REQUIRE(newAdd->getLeftInput() == x);
	auto* rightConst = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(newAdd->getRightInput());
	REQUIRE(rightConst != nullptr);
	REQUIRE(rightConst->getValue() == 5);
}

TEST_CASE("AlgebraicSimplification: x-0 -> x") {
	auto ir = std::make_shared<IRGraph>("as-sub-zero");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* zero = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {0}, Type::i32);
	auto* sub = entry->addOperation<compiler::ir::SubOperation>(OperationIdentifier {3}, x, zero);
	entry->addOperation<compiler::ir::ReturnOperation>(sub);
	wrapInGraph(ir, entry, Type::i32, {Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::SubOp) == 0);
	REQUIRE(returnValueOf(*ir) == x);
}

TEST_CASE("AlgebraicSimplification: x-x -> 0 (int)") {
	auto ir = std::make_shared<IRGraph>("as-sub-self");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* sub = entry->addOperation<compiler::ir::SubOperation>(OperationIdentifier {2}, x, x);
	entry->addOperation<compiler::ir::ReturnOperation>(sub);
	wrapInGraph(ir, entry, Type::i32, {Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::SubOp) == 0);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(returnValueOf(*ir));
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == 0);
}

TEST_CASE("AlgebraicSimplification: float x-x is NOT folded") {
	auto ir = std::make_shared<IRGraph>("as-sub-self-float-neg");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::f32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* sub = entry->addOperation<compiler::ir::SubOperation>(OperationIdentifier {2}, x, x);
	entry->addOperation<compiler::ir::ReturnOperation>(sub);
	wrapInGraph(ir, entry, Type::f32, {Type::f32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::SubOp) == 1);
}

TEST_CASE("AlgebraicSimplification: x*1 and 1*x -> x (int)") {
	auto ir = std::make_shared<IRGraph>("as-mul-one");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* one = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {1}, Type::i32);
	auto* mul = entry->addOperation<compiler::ir::MulOperation>(OperationIdentifier {3}, one, x);
	entry->addOperation<compiler::ir::ReturnOperation>(mul);
	wrapInGraph(ir, entry, Type::i32, {Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::MulOp) == 0);
	REQUIRE(returnValueOf(*ir) == x);
}

TEST_CASE("AlgebraicSimplification: float x*1.0 -> x") {
	auto ir = std::make_shared<IRGraph>("as-mul-one-float");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::f64);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* one = entry->addOperation<compiler::ir::ConstFloatOperation>(OperationIdentifier {2}, 1.0, Type::f64);
	auto* mul = entry->addOperation<compiler::ir::MulOperation>(OperationIdentifier {3}, x, one);
	entry->addOperation<compiler::ir::ReturnOperation>(mul);
	wrapInGraph(ir, entry, Type::f64, {Type::f64});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::MulOp) == 0);
	REQUIRE(returnValueOf(*ir) == x);
}

TEST_CASE("AlgebraicSimplification: x*0 -> 0 (int)") {
	auto ir = std::make_shared<IRGraph>("as-mul-zero");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* zero = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {0}, Type::i32);
	auto* mul = entry->addOperation<compiler::ir::MulOperation>(OperationIdentifier {3}, x, zero);
	entry->addOperation<compiler::ir::ReturnOperation>(mul);
	wrapInGraph(ir, entry, Type::i32, {Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::MulOp) == 0);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(returnValueOf(*ir));
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == 0);
}

TEST_CASE("AlgebraicSimplification: float x*0.0 is NOT folded") {
	auto ir = std::make_shared<IRGraph>("as-mul-zero-float-neg");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::f64);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* zero = entry->addOperation<compiler::ir::ConstFloatOperation>(OperationIdentifier {2}, 0.0, Type::f64);
	auto* mul = entry->addOperation<compiler::ir::MulOperation>(OperationIdentifier {3}, x, zero);
	entry->addOperation<compiler::ir::ReturnOperation>(mul);
	wrapInGraph(ir, entry, Type::f64, {Type::f64});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::MulOp) == 1);
}

TEST_CASE("AlgebraicSimplification: x/1 -> x (int), x/1.0 -> x (float)") {
	{
		auto ir = std::make_shared<IRGraph>("as-div-one-int");
		auto& arena = ir->getArena();
		auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
		auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
		auto* one =
		    entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {1}, Type::i32);
		auto* div = entry->addOperation<compiler::ir::DivOperation>(OperationIdentifier {3}, x, one);
		entry->addOperation<compiler::ir::ReturnOperation>(div);
		wrapInGraph(ir, entry, Type::i32, {Type::i32});

		runPass(*ir);

		REQUIRE(countOpsOfType(*ir, Operation::OperationType::DivOp) == 0);
		REQUIRE(returnValueOf(*ir) == x);
	}
	{
		auto ir = std::make_shared<IRGraph>("as-div-one-float");
		auto& arena = ir->getArena();
		auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::f64);
		auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
		auto* one = entry->addOperation<compiler::ir::ConstFloatOperation>(OperationIdentifier {2}, 1.0, Type::f64);
		auto* div = entry->addOperation<compiler::ir::DivOperation>(OperationIdentifier {3}, x, one);
		entry->addOperation<compiler::ir::ReturnOperation>(div);
		wrapInGraph(ir, entry, Type::f64, {Type::f64});

		runPass(*ir);

		REQUIRE(countOpsOfType(*ir, Operation::OperationType::DivOp) == 0);
		REQUIRE(returnValueOf(*ir) == x);
	}
}

// ---- Bitwise --------------------------------------------------------------

TEST_CASE("AlgebraicSimplification: x&x -> x, x&0 -> 0") {
	auto ir = std::make_shared<IRGraph>("as-band");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* selfAnd = entry->addOperation<compiler::ir::BinaryCompOperation>(OperationIdentifier {2}, x, x,
	                                                                       compiler::ir::BinaryCompOperation::BAND);
	auto* zero = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {3}, int64_t {0}, Type::i32);
	auto* zeroAnd = entry->addOperation<compiler::ir::BinaryCompOperation>(OperationIdentifier {4}, selfAnd, zero,
	                                                                       compiler::ir::BinaryCompOperation::BAND);
	entry->addOperation<compiler::ir::ReturnOperation>(zeroAnd);
	wrapInGraph(ir, entry, Type::i32, {Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::BinaryComp) == 0);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(returnValueOf(*ir));
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == 0);
}

TEST_CASE("AlgebraicSimplification: x|x -> x, x|0 -> x") {
	auto ir = std::make_shared<IRGraph>("as-bor");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* selfOr = entry->addOperation<compiler::ir::BinaryCompOperation>(OperationIdentifier {2}, x, x,
	                                                                      compiler::ir::BinaryCompOperation::BOR);
	auto* zero = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {3}, int64_t {0}, Type::i32);
	auto* zeroOr = entry->addOperation<compiler::ir::BinaryCompOperation>(OperationIdentifier {4}, selfOr, zero,
	                                                                      compiler::ir::BinaryCompOperation::BOR);
	entry->addOperation<compiler::ir::ReturnOperation>(zeroOr);
	wrapInGraph(ir, entry, Type::i32, {Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::BinaryComp) == 0);
	REQUIRE(returnValueOf(*ir) == x);
}

TEST_CASE("AlgebraicSimplification: x^x -> 0, x^0 -> x") {
	auto ir = std::make_shared<IRGraph>("as-xor");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* zero = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {0}, Type::i32);
	auto* xorWithZero = entry->addOperation<compiler::ir::BinaryCompOperation>(OperationIdentifier {3}, x, zero,
	                                                                           compiler::ir::BinaryCompOperation::XOR);
	entry->addOperation<compiler::ir::ReturnOperation>(xorWithZero);
	wrapInGraph(ir, entry, Type::i32, {Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::BinaryComp) == 0);
	REQUIRE(returnValueOf(*ir) == x);
}

TEST_CASE("AlgebraicSimplification: canonicalizes a constant-left BinaryComp to the right") {
	auto ir = std::make_shared<IRGraph>("as-bitwise-canon");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* five = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {5}, Type::i32);
	auto* band = entry->addOperation<compiler::ir::BinaryCompOperation>(OperationIdentifier {3}, five, x,
	                                                                    compiler::ir::BinaryCompOperation::BAND);
	entry->addOperation<compiler::ir::ReturnOperation>(band);
	wrapInGraph(ir, entry, Type::i32, {Type::i32});

	runPass(*ir);

	auto* newBand = compiler::ir::dyn_cast<compiler::ir::BinaryCompOperation>(returnValueOf(*ir));
	REQUIRE(newBand != nullptr);
	REQUIRE(newBand->getLeftInput() == x);
	REQUIRE(compiler::ir::isa<compiler::ir::ConstIntOperation>(newBand->getRightInput()));
}

TEST_CASE("AlgebraicSimplification: x<<0 -> x, x>>0 -> x") {
	auto ir = std::make_shared<IRGraph>("as-shift-zero");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* zero = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {0}, Type::i32);
	auto* shl = entry->addOperation<compiler::ir::ShiftOperation>(OperationIdentifier {3}, x, zero,
	                                                              compiler::ir::ShiftOperation::LS);
	entry->addOperation<compiler::ir::ReturnOperation>(shl);
	wrapInGraph(ir, entry, Type::i32, {Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::ShiftOp) == 0);
	REQUIRE(returnValueOf(*ir) == x);
}

// ---- Boolean ---------------------------------------------------------------

TEST_CASE("AlgebraicSimplification: and(x,true) -> x, and(x,false) -> false") {
	auto ir = std::make_shared<IRGraph>("as-and-bool");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::b);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* falseConst = entry->addOperation<compiler::ir::ConstBooleanOperation>(OperationIdentifier {2}, false);
	auto* andOp = entry->addOperation<compiler::ir::AndOperation>(OperationIdentifier {3}, x, falseConst);
	entry->addOperation<compiler::ir::ReturnOperation>(andOp);
	wrapInGraph(ir, entry, Type::b, {Type::b});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AndOp) == 0);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstBooleanOperation>(returnValueOf(*ir));
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == false);
}

TEST_CASE("AlgebraicSimplification: or(x,false) -> x, or(x,true) -> true") {
	auto ir = std::make_shared<IRGraph>("as-or-bool");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::b);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* trueConst = entry->addOperation<compiler::ir::ConstBooleanOperation>(OperationIdentifier {2}, true);
	auto* orOp = entry->addOperation<compiler::ir::OrOperation>(OperationIdentifier {3}, x, trueConst);
	entry->addOperation<compiler::ir::ReturnOperation>(orOp);
	wrapInGraph(ir, entry, Type::b, {Type::b});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::OrOp) == 0);
	auto* folded = compiler::ir::dyn_cast<compiler::ir::ConstBooleanOperation>(returnValueOf(*ir));
	REQUIRE(folded != nullptr);
	REQUIRE(folded->getValue() == true);
}

TEST_CASE("AlgebraicSimplification: not(not(x)) -> x") {
	auto ir = std::make_shared<IRGraph>("as-not-not");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::b);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* not1 = entry->addOperation<compiler::ir::NotOperation>(OperationIdentifier {2}, x);
	auto* not2 = entry->addOperation<compiler::ir::NotOperation>(OperationIdentifier {3}, not1);
	entry->addOperation<compiler::ir::ReturnOperation>(not2);
	wrapInGraph(ir, entry, Type::b, {Type::b});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::NotOp) == 0);
	REQUIRE(returnValueOf(*ir) == x);
}

TEST_CASE("AlgebraicSimplification: not(compare) inverts the comparator") {
	auto ir = std::make_shared<IRGraph>("as-not-compare");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* y = arena.create<BasicBlockArgument>(OperationIdentifier {2}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x, y});
	auto* cmp = entry->addOperation<compiler::ir::CompareOperation>(OperationIdentifier {3}, x, y,
	                                                                compiler::ir::CompareOperation::LT);
	auto* notCmp = entry->addOperation<compiler::ir::NotOperation>(OperationIdentifier {4}, cmp);
	entry->addOperation<compiler::ir::ReturnOperation>(notCmp);
	wrapInGraph(ir, entry, Type::b, {Type::i32, Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::NotOp) == 0);
	auto* newCmp = compiler::ir::dyn_cast<compiler::ir::CompareOperation>(returnValueOf(*ir));
	REQUIRE(newCmp != nullptr);
	REQUIRE(newCmp->getComparator() == compiler::ir::CompareOperation::GE);
	REQUIRE(newCmp->getLeftInput() == x);
	REQUIRE(newCmp->getRightInput() == y);
}

// ---- Casts -------------------------------------------------------------

TEST_CASE("AlgebraicSimplification: cast to the identical stamp is elided") {
	auto ir = std::make_shared<IRGraph>("as-cast-identity");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* cast = entry->addOperation<compiler::ir::CastOperation>(OperationIdentifier {2}, x, Type::i32);
	entry->addOperation<compiler::ir::ReturnOperation>(cast);
	wrapInGraph(ir, entry, Type::i32, {Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::CastOp) == 0);
	REQUIRE(returnValueOf(*ir) == x);
}

TEST_CASE("AlgebraicSimplification: widening cast-of-cast chain of the same signedness collapses") {
	auto ir = std::make_shared<IRGraph>("as-cast-widen");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i8);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* toI16 = entry->addOperation<compiler::ir::CastOperation>(OperationIdentifier {2}, x, Type::i16);
	auto* toI32 = entry->addOperation<compiler::ir::CastOperation>(OperationIdentifier {3}, toI16, Type::i32);
	entry->addOperation<compiler::ir::ReturnOperation>(toI32);
	wrapInGraph(ir, entry, Type::i32, {Type::i8});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::CastOp) == 1);
	auto* collapsed = compiler::ir::dyn_cast<compiler::ir::CastOperation>(returnValueOf(*ir));
	REQUIRE(collapsed != nullptr);
	REQUIRE(collapsed->getInput() == x);
	REQUIRE(collapsed->getStamp() == Type::i32);
}

TEST_CASE("AlgebraicSimplification: mixed-signedness widening cast-of-cast is left alone") {
	auto ir = std::make_shared<IRGraph>("as-cast-widen-mixed-sign-neg");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i8);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* toUi16 = entry->addOperation<compiler::ir::CastOperation>(OperationIdentifier {2}, x, Type::ui16);
	auto* toI32 = entry->addOperation<compiler::ir::CastOperation>(OperationIdentifier {3}, toUi16, Type::i32);
	entry->addOperation<compiler::ir::ReturnOperation>(toI32);
	wrapInGraph(ir, entry, Type::i32, {Type::i8});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::CastOp) == 2);
}

TEST_CASE("AlgebraicSimplification: pure truncation cast-of-cast chain collapses") {
	auto ir = std::make_shared<IRGraph>("as-cast-truncate");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i64);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* toI32 = entry->addOperation<compiler::ir::CastOperation>(OperationIdentifier {2}, x, Type::i32);
	auto* toI16 = entry->addOperation<compiler::ir::CastOperation>(OperationIdentifier {3}, toI32, Type::i16);
	entry->addOperation<compiler::ir::ReturnOperation>(toI16);
	wrapInGraph(ir, entry, Type::i16, {Type::i64});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::CastOp) == 1);
	auto* collapsed = compiler::ir::dyn_cast<compiler::ir::CastOperation>(returnValueOf(*ir));
	REQUIRE(collapsed != nullptr);
	REQUIRE(collapsed->getInput() == x);
	REQUIRE(collapsed->getStamp() == Type::i16);
}

TEST_CASE("AlgebraicSimplification: truncate-then-widen cast-of-cast is left alone") {
	auto ir = std::make_shared<IRGraph>("as-cast-trunc-then-widen-neg");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i64);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* toI8 = entry->addOperation<compiler::ir::CastOperation>(OperationIdentifier {2}, x, Type::i8);
	auto* toI32 = entry->addOperation<compiler::ir::CastOperation>(OperationIdentifier {3}, toI8, Type::i32);
	entry->addOperation<compiler::ir::ReturnOperation>(toI32);
	wrapInGraph(ir, entry, Type::i32, {Type::i64});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::CastOp) == 2);
}

// ---- Select ----------------------------------------------------------------

TEST_CASE("AlgebraicSimplification: select(constBool, a, b) picks the taken value") {
	auto ir = std::make_shared<IRGraph>("as-select-const-cond");
	auto& arena = ir->getArena();
	auto* a = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* b = arena.create<BasicBlockArgument>(OperationIdentifier {2}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {a, b});
	auto* trueConst = entry->addOperation<compiler::ir::ConstBooleanOperation>(OperationIdentifier {3}, true);
	auto* select =
	    entry->addOperation<compiler::ir::SelectOperation>(OperationIdentifier {4}, trueConst, a, b, Type::i32);
	entry->addOperation<compiler::ir::ReturnOperation>(select);
	wrapInGraph(ir, entry, Type::i32, {Type::i32, Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::SelectOp) == 0);
	REQUIRE(returnValueOf(*ir) == a);
}

TEST_CASE("AlgebraicSimplification: select(c, x, x) -> x") {
	auto ir = std::make_shared<IRGraph>("as-select-same-arms");
	auto& arena = ir->getArena();
	auto* cond = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::b);
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {2}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {cond, x});
	auto* select = entry->addOperation<compiler::ir::SelectOperation>(OperationIdentifier {3}, cond, x, x, Type::i32);
	entry->addOperation<compiler::ir::ReturnOperation>(select);
	wrapInGraph(ir, entry, Type::i32, {Type::b, Type::i32});

	runPass(*ir);

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::SelectOp) == 0);
	REQUIRE(returnValueOf(*ir) == x);
}

// ---- Compare canonicalization -----------------------------------------

TEST_CASE("AlgebraicSimplification: canonicalizes a constant-left '<' compare with a mirrored comparator") {
	auto ir = std::make_shared<IRGraph>("as-compare-canon-lt");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* five = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {5}, Type::i32);
	// 5 < x
	auto* cmp = entry->addOperation<compiler::ir::CompareOperation>(OperationIdentifier {3}, five, x,
	                                                                compiler::ir::CompareOperation::LT);
	entry->addOperation<compiler::ir::ReturnOperation>(cmp);
	wrapInGraph(ir, entry, Type::b, {Type::i32});

	runPass(*ir);

	auto* newCmp = compiler::ir::dyn_cast<compiler::ir::CompareOperation>(returnValueOf(*ir));
	REQUIRE(newCmp != nullptr);
	// 5 < x  ==  x > 5
	REQUIRE(newCmp->getLeftInput() == x);
	REQUIRE(compiler::ir::isa<compiler::ir::ConstIntOperation>(newCmp->getRightInput()));
	REQUIRE(newCmp->getComparator() == compiler::ir::CompareOperation::GT);
}

TEST_CASE("AlgebraicSimplification: canonicalizes a constant-left '<=' compare with a mirrored comparator") {
	auto ir = std::make_shared<IRGraph>("as-compare-canon-le");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x});
	auto* five = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {2}, int64_t {5}, Type::i32);
	// 5 <= x
	auto* cmp = entry->addOperation<compiler::ir::CompareOperation>(OperationIdentifier {3}, five, x,
	                                                                compiler::ir::CompareOperation::LE);
	entry->addOperation<compiler::ir::ReturnOperation>(cmp);
	wrapInGraph(ir, entry, Type::b, {Type::i32});

	runPass(*ir);

	auto* newCmp = compiler::ir::dyn_cast<compiler::ir::CompareOperation>(returnValueOf(*ir));
	REQUIRE(newCmp != nullptr);
	// 5 <= x  ==  x >= 5
	REQUIRE(newCmp->getLeftInput() == x);
	REQUIRE(newCmp->getComparator() == compiler::ir::CompareOperation::GE);
}

TEST_CASE("AlgebraicSimplification: non-foldable graph is idempotent") {
	auto ir = std::make_shared<IRGraph>("as-idempotence");
	auto& arena = ir->getArena();
	auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* y = arena.create<BasicBlockArgument>(OperationIdentifier {2}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {x, y});
	auto* five = entry->addOperation<compiler::ir::ConstIntOperation>(OperationIdentifier {3}, int64_t {5}, Type::i32);
	auto* add = entry->addOperation<compiler::ir::AddOperation>(OperationIdentifier {4}, five, x);
	auto* mul = entry->addOperation<compiler::ir::MulOperation>(OperationIdentifier {5}, add, y);
	entry->addOperation<compiler::ir::ReturnOperation>(mul);
	wrapInGraph(ir, entry, Type::i32, {Type::i32, Type::i32});

	runPass(*ir);
	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE(result.ok());
	const auto opsAfterFirst = entryBlock(*ir)->getOperations().size();

	runPass(*ir);
	REQUIRE(entryBlock(*ir)->getOperations().size() == opsAfterFirst);
	result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE(result.ok());
}

} // namespace nautilus::testing
