#include "nautilus/common/ExceptionCleanup.hpp"
#include "nautilus/compiler/backends/cpp/CPPLoweringProvider.hpp"
#include "nautilus/compiler/ir/ExceptionRegion.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/ExceptionCleanupPreparationPass.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <catch2/catch_test_macros.hpp>
#include <unordered_map>

namespace nautilus::compiler::ir {

namespace {
void destroyTestValue(void*) noexcept {
}

void mayThrow() {
}

struct LinearFunction {
	std::shared_ptr<IRGraph> ir;
	BasicBlock* entry;
	FunctionOperation* function;
};

LinearFunction createLinearFunction(std::vector<AllocaSpec> allocas, std::vector<CleanupState> cleanupStates = {}) {
	auto ir = std::make_shared<IRGraph>("exception-cleanup-pass");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* function = arena.create<FunctionOperation>(
	    "execute", std::vector<BasicBlock*> {entry}, std::vector<Type> {}, std::vector<std::string> {}, Type::v,
	    std::move(allocas), std::unordered_map<std::string, std::string> {}, std::move(cleanupStates));
	ir->addFunctionOperation(function);
	return {std::move(ir), entry, function};
}

ProxyCallOperation* addCall(BasicBlock& block, OperationIdentifier id, FunctionAttributes attributes = {},
                            std::optional<CleanupStateId> cleanupState = std::nullopt,
                            std::optional<CallKind> callKind = std::nullopt) {
	const auto kind = callKind.value_or(classifyCallKind(attributes, cleanupState.value_or(EMPTY_CLEANUP_STATE)));
	return block.addOperation<ProxyCallOperation>("mayThrow", "mayThrow", reinterpret_cast<void*>(&mayThrow), id,
	                                              std::span<Operation* const> {}, Type::v, attributes, kind,
	                                              std::nullopt, cleanupState);
}
} // namespace

TEST_CASE("Exception cleanup metadata is separate from the ordinary CFG") {
	auto ir = std::make_shared<IRGraph>("exception-cleanup-metadata");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});

	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	std::vector<AllocaSpec> allocas {{8, 8, destructor}};
	auto* function = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry}, std::vector<Type> {},
	                                                 std::vector<std::string> {}, Type::v, std::move(allocas),
	                                                 std::unordered_map<std::string, std::string> {},
	                                                 std::vector<CleanupState> {CleanupState {}, CleanupState {{0}}});
	ir->addFunctionOperation(function);

	REQUIRE_FALSE(function->hasExceptionRegion());
	REQUIRE(function->getBasicBlocks() == std::vector<BasicBlock*> {entry});
	REQUIRE(function->getAllocaSpecs().front().destructor.has_value());
	REQUIRE(function->getCleanupStates().at(1).active == std::vector<AllocaIndex> {0});

	FunctionExceptionRegion region;
	region.pads.push_back(CleanupPad {0, {0}});
	function->setExceptionRegion(std::move(region));

	REQUIRE(function->hasExceptionRegion());
	REQUIRE(function->getExceptionRegion().pads.size() == 1);
	REQUIRE(function->getExceptionRegion().pads.front().active == std::vector<AllocaIndex> {0});
	REQUIRE(function->getBasicBlocks() == std::vector<BasicBlock*> {entry});
}

TEST_CASE("Exception cleanup preparation shares exact cleanup pads") {
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	auto fixture =
	    createLinearFunction({AllocaSpec {8, 8, destructor}, AllocaSpec {16, 8, destructor}},
	                         {CleanupState {}, CleanupState {{0}}, CleanupState {{0, 1}}, CleanupState {{1}}});
	addCall(*fixture.entry, OperationIdentifier {1});
	addCall(*fixture.entry, OperationIdentifier {2}, {}, CleanupStateId {1});
	auto* firstThrow = addCall(*fixture.entry, OperationIdentifier {3}, {}, CleanupStateId {2});
	auto* secondThrow = addCall(*fixture.entry, OperationIdentifier {4}, {}, CleanupStateId {2});
	addCall(*fixture.entry, OperationIdentifier {5}, {.noUnwind = true});
	auto* thirdThrow = addCall(*fixture.entry, OperationIdentifier {6}, {}, CleanupStateId {3});
	fixture.entry->addOperation<ReturnOperation>();

	REQUIRE(ExceptionCleanupPreparationPass().apply(*fixture.ir));
	const auto& region = fixture.function->getExceptionRegion();
	REQUIRE(region.pads.size() == 3);
	REQUIRE(region.pads[0].active == std::vector<AllocaIndex> {0});
	REQUIRE(region.pads[1].active == std::vector<AllocaIndex> {0, 1});
	REQUIRE(region.pads[2].active == std::vector<AllocaIndex> {1});
	REQUIRE(region.callSites.size() == 5);
	REQUIRE(region.callSites[0].cleanup == std::nullopt);
	REQUIRE(region.callSites[1].cleanup == CleanupPadId {0});
	REQUIRE(region.callSites[2] == ExceptionalCallSite {firstThrow, CleanupPadId {1}});
	REQUIRE(region.callSites[3] == ExceptionalCallSite {secondThrow, CleanupPadId {1}});
	REQUIRE(region.callSites[4] == ExceptionalCallSite {thirdThrow, CleanupPadId {2}});
}

TEST_CASE("Exception cleanup preparation keeps may-throw calls without active destructors") {
	auto fixture = createLinearFunction({});
	auto* throwingCall = addCall(*fixture.entry, OperationIdentifier {1});
	fixture.entry->addOperation<ReturnOperation>();

	REQUIRE(ExceptionCleanupPreparationPass().apply(*fixture.ir));
	REQUIRE(fixture.function->hasExceptionRegion());
	REQUIRE(fixture.function->getExceptionRegion().pads.empty());
	REQUIRE(fixture.function->getExceptionRegion().callSites ==
	        std::vector<ExceptionalCallSite> {{throwingCall, std::nullopt}});

	auto code = compiler::cpp::CPPLoweringProvider::lower(fixture.ir);
	REQUIRE(code.find("catch (...)") == std::string::npos);
}

TEST_CASE("Exception cleanup preparation ignores noexcept calls") {
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}}, {CleanupState {}, CleanupState {{0}}});
	addCall(*fixture.entry, OperationIdentifier {1}, {.noUnwind = true});
	fixture.entry->addOperation<ReturnOperation>();

	REQUIRE_FALSE(ExceptionCleanupPreparationPass().apply(*fixture.ir));
	REQUIRE_FALSE(fixture.function->hasExceptionRegion());
}

TEST_CASE("Exception cleanup preparation rejects an unknown cleanup state") {
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}}, {CleanupState {}});
	addCall(*fixture.entry, OperationIdentifier {1}, {}, CleanupStateId {4});
	fixture.entry->addOperation<ReturnOperation>();

	REQUIRE_THROWS_AS(ExceptionCleanupPreparationPass().apply(*fixture.ir), RuntimeException);
}

TEST_CASE("Exception cleanup preparation rejects malformed cleanup states") {
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};

	SECTION("missing alloca") {
		auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}}, {CleanupState {}, CleanupState {{1}}});
		addCall(*fixture.entry, OperationIdentifier {1}, {}, CleanupStateId {1});
		REQUIRE_THROWS_AS(ExceptionCleanupPreparationPass().apply(*fixture.ir), RuntimeException);
	}

	SECTION("alloca without destructor") {
		auto fixture = createLinearFunction({AllocaSpec {8, 8}}, {CleanupState {}, CleanupState {{0}}});
		addCall(*fixture.entry, OperationIdentifier {1}, {}, CleanupStateId {1});
		REQUIRE_THROWS_AS(ExceptionCleanupPreparationPass().apply(*fixture.ir), RuntimeException);
	}

	SECTION("duplicate alloca") {
		auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}}, {CleanupState {}, CleanupState {{0, 0}}});
		addCall(*fixture.entry, OperationIdentifier {1}, {}, CleanupStateId {1});
		REQUIRE_THROWS_AS(ExceptionCleanupPreparationPass().apply(*fixture.ir), RuntimeException);
	}
}

TEST_CASE("Exception cleanup preparation rejects invalid call-kind combinations") {
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};

	SECTION("exception handling call marked noexcept") {
		auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}}, {CleanupState {}, CleanupState {{0}}});
		addCall(*fixture.entry, OperationIdentifier {1}, {.noUnwind = true}, CleanupStateId {1},
		        CallKind::WithExceptionHandling);
		REQUIRE_THROWS_AS(ExceptionCleanupPreparationPass().apply(*fixture.ir), RuntimeException);
	}

	SECTION("exception handling call without cleanup state") {
		auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}}, {CleanupState {}});
		addCall(*fixture.entry, OperationIdentifier {1}, {}, std::nullopt, CallKind::WithExceptionHandling);
		REQUIRE_THROWS_AS(ExceptionCleanupPreparationPass().apply(*fixture.ir), RuntimeException);
	}

	SECTION("exception handling call with empty cleanup state") {
		auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}}, {CleanupState {}});
		addCall(*fixture.entry, OperationIdentifier {1}, {}, EMPTY_CLEANUP_STATE, CallKind::WithExceptionHandling);
		REQUIRE_THROWS_AS(ExceptionCleanupPreparationPass().apply(*fixture.ir), RuntimeException);
	}

	SECTION("regular call with cleanup state") {
		auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}}, {CleanupState {}, CleanupState {{0}}});
		addCall(*fixture.entry, OperationIdentifier {1}, {}, CleanupStateId {1}, CallKind::Regular);
		REQUIRE_THROWS_AS(ExceptionCleanupPreparationPass().apply(*fixture.ir), RuntimeException);
	}
}

TEST_CASE("IR verifier rejects invalid call-kind combinations") {
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}}, {CleanupState {}, CleanupState {{0}}});
	addCall(*fixture.entry, OperationIdentifier {1}, {}, CleanupStateId {1}, CallKind::Regular);
	addCall(*fixture.entry, OperationIdentifier {2}, {}, std::nullopt, CallKind::WithExceptionHandling);
	addCall(*fixture.entry, OperationIdentifier {3}, {.noUnwind = true}, CleanupStateId {1},
	        CallKind::WithExceptionHandling);
	fixture.entry->addOperation<ReturnOperation>();

	const auto result = IRVerifier::verify(*fixture.ir);
	REQUIRE_FALSE(result.ok());
	REQUIRE(result.errors.size() >= 3);
}

TEST_CASE("IR verifier rejects exception-region pads that disagree with call kinds") {
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}}, {CleanupState {}, CleanupState {{0}}});
	auto* regular = addCall(*fixture.entry, OperationIdentifier {1});
	auto* withCleanup = addCall(*fixture.entry, OperationIdentifier {2}, {}, CleanupStateId {1});
	fixture.entry->addOperation<ReturnOperation>();
	fixture.function->setExceptionRegion(FunctionExceptionRegion {
	    {CleanupPad {0, {0}}},
	    {ExceptionalCallSite {regular, CleanupPadId {0}}, ExceptionalCallSite {withCleanup, std::nullopt}}});

	const auto result = IRVerifier::verify(*fixture.ir);
	REQUIRE_FALSE(result.ok());
	REQUIRE(result.errors.size() >= 2);
}

TEST_CASE("IR verifier rejects malformed exception cleanup regions") {
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}});
	auto* noThrowCall = addCall(*fixture.entry, OperationIdentifier {1}, {.noUnwind = true});
	fixture.entry->addOperation<ReturnOperation>();
	fixture.function->setExceptionRegion(
	    FunctionExceptionRegion {{CleanupPad {7, {4}}}, {ExceptionalCallSite {noThrowCall, CleanupPadId {3}}}});

	auto result = IRVerifier::verify(*fixture.ir);
	REQUIRE_FALSE(result.ok());
	REQUIRE(result.errors.size() >= 3);
}

TEST_CASE("C++ lowering materializes logical cleanup pads in reverse order") {
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}, AllocaSpec {16, 8, destructor}});
	auto* throwingCall = addCall(*fixture.entry, OperationIdentifier {1});
	fixture.entry->addOperation<ReturnOperation>();
	fixture.function->setExceptionRegion(
	    FunctionExceptionRegion {{CleanupPad {0, {0, 1}}}, {ExceptionalCallSite {throwingCall, CleanupPadId {0}}}});

	auto code = compiler::cpp::CPPLoweringProvider::lower(fixture.ir);
	REQUIRE(code.find("catch (...)") != std::string::npos);
	const auto destroySecond = code.find("cleanup_dtor_1(alloca_buf_1)");
	const auto destroyFirst = code.find("cleanup_dtor_0(alloca_buf_0)");
	REQUIRE(destroySecond != std::string::npos);
	REQUIRE(destroyFirst != std::string::npos);
	REQUIRE(destroySecond < destroyFirst);
}

} // namespace nautilus::compiler::ir
