#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/NoThrowInferencePass.hpp"
#include "nautilus/options.hpp"
#include <catch2/catch_all.hpp>
#include <span>
#include <vector>

namespace nautilus::testing {

namespace {

using compiler::ir::BasicBlock;
using compiler::ir::BasicBlockArgument;
using compiler::ir::BlockIdentifier;
using compiler::ir::CallOperation;
using compiler::ir::FunctionOperation;
using compiler::ir::IRGraph;
using compiler::ir::Operation;
using compiler::ir::OperationIdentifier;

struct Fixture {
	std::shared_ptr<IRGraph> ir;
};

Fixture makeFixture(const std::string& name = "no-throw-inference-test") {
	return {std::make_shared<IRGraph>(name)};
}

/// Adds a single-block function named `name` to the fixture's module and
/// returns its entry block, so tests can populate the body before the
/// function's own ReturnOperation is appended. `blockId` must be unique
/// across every function added to the same fixture.
BasicBlock* addFunction(Fixture& f, const std::string& name, uint32_t blockId) {
	auto& arena = f.ir->getArena();
	auto* entry =
	    arena.create<BasicBlock>(arena, compiler::ir::BlockIdentifier {blockId}, std::vector<BasicBlockArgument*> {});
	auto* fn = arena.create<FunctionOperation>(name, std::vector<BasicBlock*> {entry}, std::vector<Type> {},
	                                           std::vector<std::string> {}, Type::v);
	f.ir->addFunctionOperation(fn);
	return entry;
}

void runPass(IRGraph& ir) {
	engine::Options opts;
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::NoThrowInferencePass>());
	mgr.run(ir);
}

/// A Nautilus-to-Nautilus call (`isNautilusCall = true`) into `calleeName`,
/// traced pessimistically as exception-handling -- exactly what the C++
/// trace-time heuristic produces for any lvalue-argument NautilusFunction
/// call, regardless of whether the callee can actually throw.
CallOperation* addNautilusCall(BasicBlock* block, OperationIdentifier id, const std::string& calleeName) {
	return block->addOperation<CallOperation>(calleeName, calleeName, nullptr, id, std::span<Operation* const> {},
	                                          Type::v, FunctionAttributes {}, compiler::ir::INVALID_FUNCTION_ID,
	                                          std::vector<CallOperation::Destructor> {},
	                                          /*exceptionHandling=*/true, /*captureFunc=*/nullptr,
	                                          /*isNautilusCall=*/true);
}

/// A raw external call (`isNautilusCall = false`) still marked
/// exception-handling -- the accurate case: the callee is a real,
/// non-noexcept C++ function, established correctly at trace time.
CallOperation* addThrowingExternalCall(BasicBlock* block, OperationIdentifier id) {
	return block->addOperation<CallOperation>("ext", "ext", nullptr, id, std::span<Operation* const> {}, Type::v,
	                                          FunctionAttributes {}, compiler::ir::INVALID_FUNCTION_ID,
	                                          std::vector<CallOperation::Destructor> {},
	                                          /*exceptionHandling=*/true);
}

} // namespace

TEST_CASE("NoThrowInference: callee with no throw source downgrades the caller's call") {
	Fixture f = makeFixture();
	auto* calleeEntry = addFunction(f, "callee", 0);
	calleeEntry->addOperation<compiler::ir::ReturnOperation>();

	auto* callerEntry = addFunction(f, "caller", 1);
	auto* call = addNautilusCall(callerEntry, OperationIdentifier {2}, "callee");
	callerEntry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	REQUIRE_FALSE(call->requiresExceptionHandling());
	REQUIRE(call->getFunctionAttributes().noUnwind);
	REQUIRE(call->getDestructors().empty());
}

TEST_CASE("NoThrowInference: callee with a raw external throwing call keeps the caller's call exception-handling") {
	Fixture f = makeFixture();
	auto* calleeEntry = addFunction(f, "callee", 0);
	auto* externalCall = addThrowingExternalCall(calleeEntry, OperationIdentifier {1});
	calleeEntry->addOperation<compiler::ir::ReturnOperation>();

	auto* callerEntry = addFunction(f, "caller", 2);
	auto* call = addNautilusCall(callerEntry, OperationIdentifier {3}, "callee");
	callerEntry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	REQUIRE(call->requiresExceptionHandling());
	// The pass never touches a raw external call's own flags.
	REQUIRE(externalCall->requiresExceptionHandling());
}

TEST_CASE("NoThrowInference: callee with an indirect call keeps the caller's call exception-handling") {
	Fixture f = makeFixture();
	auto* calleeEntry = addFunction(f, "callee", 0);
	auto* fnPtr = calleeEntry->addOperation<compiler::ir::ConstPtrOperation>(OperationIdentifier {1}, nullptr);
	calleeEntry->addOperation<compiler::ir::IndirectCallOperation>(
	    OperationIdentifier {2}, fnPtr, std::span<Operation* const> {}, Type::v, FunctionAttributes {},
	    std::vector<compiler::ir::IndirectCallOperation::Destructor> {}, /*exceptionHandling=*/true);
	calleeEntry->addOperation<compiler::ir::ReturnOperation>();

	auto* callerEntry = addFunction(f, "caller", 3);
	auto* call = addNautilusCall(callerEntry, OperationIdentifier {4}, "callee");
	callerEntry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	REQUIRE(call->requiresExceptionHandling());
}

TEST_CASE("NoThrowInference: direct recursion with no external throw is proven noThrow") {
	Fixture f = makeFixture();
	auto* entry = addFunction(f, "recursive", 0);
	auto* selfCall = addNautilusCall(entry, OperationIdentifier {1}, "recursive");
	entry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	REQUIRE_FALSE(selfCall->requiresExceptionHandling());
}

TEST_CASE("NoThrowInference: mutual recursion with no external throw is proven noThrow for both") {
	Fixture f = makeFixture();
	auto* aEntry = addFunction(f, "a", 0);
	auto* callToB = addNautilusCall(aEntry, OperationIdentifier {1}, "b");
	aEntry->addOperation<compiler::ir::ReturnOperation>();

	auto* bEntry = addFunction(f, "b", 2);
	auto* callToA = addNautilusCall(bEntry, OperationIdentifier {3}, "a");
	bEntry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	REQUIRE_FALSE(callToB->requiresExceptionHandling());
	REQUIRE_FALSE(callToA->requiresExceptionHandling());
}

TEST_CASE("NoThrowInference: a noThrow proof propagates transitively through a call chain") {
	Fixture f = makeFixture();
	auto* leafEntry = addFunction(f, "leaf", 0);
	leafEntry->addOperation<compiler::ir::ReturnOperation>();

	auto* midEntry = addFunction(f, "mid", 1);
	auto* midCall = addNautilusCall(midEntry, OperationIdentifier {2}, "leaf");
	midEntry->addOperation<compiler::ir::ReturnOperation>();

	auto* callerEntry = addFunction(f, "caller", 3);
	auto* callerCall = addNautilusCall(callerEntry, OperationIdentifier {4}, "mid");
	callerEntry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	REQUIRE_FALSE(midCall->requiresExceptionHandling());
	REQUIRE_FALSE(callerCall->requiresExceptionHandling());
}

TEST_CASE("NoThrowInference: a call to a name absent from the module stays exception-handling") {
	Fixture f = makeFixture();
	auto* callerEntry = addFunction(f, "caller", 0);
	auto* call = addNautilusCall(callerEntry, OperationIdentifier {1}, "missing");
	callerEntry->addOperation<compiler::ir::ReturnOperation>();

	runPass(*f.ir);

	REQUIRE(call->requiresExceptionHandling());
}

} // namespace nautilus::testing
