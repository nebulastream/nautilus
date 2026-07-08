#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/passes/LocalCSEPass.hpp"
#include "nautilus/options.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::testing {

namespace {

using namespace compiler::ir;

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

void runCSE(IRGraph& ir) {
	engine::Options opts;
	opts.setOption("ir.verifyAfterEachPass", true);
	opts.setOption("ir.failOnVerifyError", true);
	IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<LocalCSEPass>());
	mgr.run(ir);
}

/// Single "execute" function around @p entry with a two-`i32`-argument ABI.
std::shared_ptr<IRGraph> wrap(std::shared_ptr<IRGraph> ir, BasicBlock* entry, std::vector<Type> args,
                              Type ret = Type::i32) {
	auto& arena = ir->getArena();
	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry}, std::move(args),
	                                           std::vector<std::string> {}, ret);
	ir->addFunctionOperation(fn);
	return ir;
}

} // namespace

TEST_CASE("LocalCSE: duplicate pure operation is deduped and uses are rewired") {
	auto ir = std::make_shared<IRGraph>("cse-dup");
	auto& arena = ir->getArena();
	auto* a = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* b = arena.create<BasicBlockArgument>(OperationIdentifier {2}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {a, b});
	auto* s1 = entry->addOperation<AddOperation>(OperationIdentifier {3}, a, b);
	auto* s2 = entry->addOperation<AddOperation>(OperationIdentifier {4}, a, b);
	auto* r = entry->addOperation<AddOperation>(OperationIdentifier {5}, s1, s2);
	entry->addOperation<ReturnOperation>(r);
	wrap(ir, entry, {Type::i32, Type::i32});

	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 3);
	runCSE(*ir);

	// s2 == s1 by value: it is removed, r reads s1 on both operands.
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 2);
	CHECK(r->getLeftInput() == s1);
	CHECK(r->getRightInput() == s1);
	REQUIRE(IRVerifier::verify(*ir).ok());
}

TEST_CASE("LocalCSE: commutative operands are normalized so a+b and b+a share a value") {
	auto ir = std::make_shared<IRGraph>("cse-commutative");
	auto& arena = ir->getArena();
	auto* a = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* b = arena.create<BasicBlockArgument>(OperationIdentifier {2}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {a, b});
	auto* s1 = entry->addOperation<AddOperation>(OperationIdentifier {3}, a, b);
	auto* s2 = entry->addOperation<AddOperation>(OperationIdentifier {4}, b, a); // swapped
	auto* r = entry->addOperation<AddOperation>(OperationIdentifier {5}, s1, s2);
	entry->addOperation<ReturnOperation>(r);
	wrap(ir, entry, {Type::i32, Type::i32});

	runCSE(*ir);
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == 2);
	CHECK(r->getLeftInput() == s1);
	CHECK(r->getRightInput() == s1);
}

TEST_CASE("LocalCSE: duplicate loads are NOT deduped (no alias analysis)") {
	auto ir = std::make_shared<IRGraph>("cse-loads");
	auto& arena = ir->getArena();
	auto* p = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::ptr);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {p});
	auto* l1 = entry->addOperation<LoadOperation>(OperationIdentifier {2}, p, Type::i32);
	auto* l2 = entry->addOperation<LoadOperation>(OperationIdentifier {3}, p, Type::i32);
	auto* r = entry->addOperation<AddOperation>(OperationIdentifier {4}, l1, l2);
	entry->addOperation<ReturnOperation>(r);
	wrap(ir, entry, {Type::ptr});

	runCSE(*ir);
	// Both loads survive; the add over them is not touched.
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::LoadOp) == 2);
	CHECK(r->getLeftInput() == l1);
	CHECK(r->getRightInput() == l2);
}

TEST_CASE("LocalCSE: compares differing only in comparator payload are NOT unified") {
	auto ir = std::make_shared<IRGraph>("cse-payload");
	auto& arena = ir->getArena();
	auto* a = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* b = arena.create<BasicBlockArgument>(OperationIdentifier {2}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {a, b});
	entry->addOperation<CompareOperation>(OperationIdentifier {3}, a, b, CompareOperation::LT);
	entry->addOperation<CompareOperation>(OperationIdentifier {4}, a, b, CompareOperation::LE);
	entry->addOperation<ReturnOperation>(a);
	wrap(ir, entry, {Type::i32, Type::i32});

	runCSE(*ir);
	// `<` and `<=` are distinct values -- neither is removed.
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::CompareOp) == 2);
}

TEST_CASE("LocalCSE: duplicate constants are NOT deduped (v1 excludes constants)") {
	auto ir = std::make_shared<IRGraph>("cse-const");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* c1 = entry->addOperation<ConstIntOperation>(OperationIdentifier {1}, int64_t {7}, Type::i32);
	auto* c2 = entry->addOperation<ConstIntOperation>(OperationIdentifier {2}, int64_t {7}, Type::i32);
	auto* r = entry->addOperation<AddOperation>(OperationIdentifier {3}, c1, c2);
	entry->addOperation<ReturnOperation>(r);
	wrap(ir, entry, {});

	runCSE(*ir);
	// Both constants survive; only computed pure ops are numbered in v1.
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::ConstIntOp) == 2);
}

TEST_CASE("LocalCSE: idempotent -- a second run reports and produces no further change") {
	auto ir = std::make_shared<IRGraph>("cse-idempotent");
	auto& arena = ir->getArena();
	auto* a = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* b = arena.create<BasicBlockArgument>(OperationIdentifier {2}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {a, b});
	auto* s1 = entry->addOperation<AddOperation>(OperationIdentifier {3}, a, b);
	auto* s2 = entry->addOperation<AddOperation>(OperationIdentifier {4}, a, b);
	auto* r = entry->addOperation<AddOperation>(OperationIdentifier {5}, s1, s2);
	entry->addOperation<ReturnOperation>(r);
	wrap(ir, entry, {Type::i32, Type::i32});

	runCSE(*ir);
	const auto afterFirst = countOpsOfType(*ir, Operation::OperationType::AddOp);

	LocalCSEPass pass;
	REQUIRE_FALSE(pass.apply(*ir)); // no change on the second run
	REQUIRE(countOpsOfType(*ir, Operation::OperationType::AddOp) == afterFirst);
	REQUIRE(IRVerifier::verify(*ir).ok());
}

} // namespace nautilus::testing
