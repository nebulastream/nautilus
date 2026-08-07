#include "nautilus/common/ExceptionCleanup.hpp"
#include "nautilus/compiler/backends/cpp/CPPLoweringProvider.hpp"
#include "nautilus/compiler/ir/ExceptionRegion.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/ExceptionCleanupPreparationPass.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <catch2/catch_test_macros.hpp>

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

LinearFunction createLinearFunction(std::vector<AllocaSpec> allocas) {
	auto ir = std::make_shared<IRGraph>("exception-cleanup-pass");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* function = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry}, std::vector<Type> {},
	                                                 std::vector<std::string> {}, Type::v, std::move(allocas));
	ir->addFunctionOperation(function);
	return {std::move(ir), entry, function};
}

ProxyCallOperation* addCall(BasicBlock& block, OperationIdentifier id, FunctionAttributes attributes = {},
                            std::optional<CleanupEffect> effect = std::nullopt) {
	return block.addOperation<ProxyCallOperation>("mayThrow", "mayThrow", reinterpret_cast<void*>(&mayThrow), id,
	                                              std::span<Operation* const> {}, Type::v, attributes, effect);
}
} // namespace

TEST_CASE("Exception cleanup metadata is separate from the ordinary CFG") {
	auto ir = std::make_shared<IRGraph>("exception-cleanup-metadata");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});

	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	std::vector<AllocaSpec> allocas {{8, 8, destructor}};
	auto* function = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry}, std::vector<Type> {},
	                                                 std::vector<std::string> {}, Type::v, std::move(allocas));
	ir->addFunctionOperation(function);

	REQUIRE_FALSE(function->hasExceptionRegion());
	REQUIRE(function->getBasicBlocks() == std::vector<BasicBlock*> {entry});
	REQUIRE(function->getAllocaSpecs().front().destructor.has_value());

	FunctionExceptionRegion region;
	region.pads.push_back(CleanupPad {0, {0}});
	function->setExceptionRegion(std::move(region));

	REQUIRE(function->hasExceptionRegion());
	REQUIRE(function->getExceptionRegion().pads.size() == 1);
	REQUIRE(function->getExceptionRegion().pads.front().active == std::vector<AllocaIndex> {0});
	REQUIRE(function->getBasicBlocks() == std::vector<BasicBlock*> {entry});

	constexpr CleanupEffect activation {CleanupEffectKind::ActivateAfterSuccess, 0};
	STATIC_REQUIRE(activation.kind == CleanupEffectKind::ActivateAfterSuccess);
	STATIC_REQUIRE(activation.alloca == 0);
}

TEST_CASE("Lifetime-changing operations retain their cleanup effect") {
	auto ir = std::make_shared<IRGraph>("exception-cleanup-effects");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	constexpr CleanupEffect activation {CleanupEffectKind::ActivateAfterSuccess, 0};
	constexpr CleanupEffect deactivation {CleanupEffectKind::DeactivateBeforeCall, 0};

	auto* alloca = entry->addOperation<AllocaOperation>(OperationIdentifier {1}, 0, activation);
	auto* destructor = entry->addOperation<ProxyCallOperation>(
	    "destroyTestValue", "destroyTestValue", reinterpret_cast<void*>(&destroyTestValue), OperationIdentifier {2},
	    std::span<Operation* const> {}, Type::v, FunctionAttributes {.noUnwind = true}, deactivation);

	REQUIRE(alloca->getCleanupEffect() == activation);
	REQUIRE(destructor->getCleanupEffect() == deactivation);
}

TEST_CASE("Exception cleanup preparation shares reverse-order cleanup pads") {
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}, AllocaSpec {16, 8, destructor}});
	addCall(*fixture.entry, OperationIdentifier {1}, {}, CleanupEffect {CleanupEffectKind::ActivateAfterSuccess, 0});
	addCall(*fixture.entry, OperationIdentifier {2}, {}, CleanupEffect {CleanupEffectKind::ActivateAfterSuccess, 1});
	auto* firstThrow = addCall(*fixture.entry, OperationIdentifier {3});
	auto* secondThrow = addCall(*fixture.entry, OperationIdentifier {4});
	addCall(*fixture.entry, OperationIdentifier {5}, {.noUnwind = true});
	addCall(*fixture.entry, OperationIdentifier {6}, {.noUnwind = true},
	        CleanupEffect {CleanupEffectKind::DeactivateBeforeCall, 0});
	auto* thirdThrow = addCall(*fixture.entry, OperationIdentifier {7});
	addCall(*fixture.entry, OperationIdentifier {8}, {.noUnwind = true},
	        CleanupEffect {CleanupEffectKind::DeactivateBeforeCall, 1});
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
	REQUIRE(region.callSites[2].cleanup == CleanupPadId {1});
	REQUIRE(region.callSites[2].call == firstThrow);
	REQUIRE(region.callSites[3].cleanup == CleanupPadId {1});
	REQUIRE(region.callSites[3].call == secondThrow);
	REQUIRE(region.callSites[4].cleanup == CleanupPadId {2});
	REQUIRE(region.callSites[4].call == thirdThrow);
}

TEST_CASE("Exception cleanup preparation excludes constructors and destructors from their own pads") {
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}});
	auto* constructor = addCall(*fixture.entry, OperationIdentifier {1}, {},
	                            CleanupEffect {CleanupEffectKind::ActivateAfterSuccess, 0});
	auto* destructorCall = addCall(*fixture.entry, OperationIdentifier {2}, {},
	                               CleanupEffect {CleanupEffectKind::DeactivateBeforeCall, 0});
	fixture.entry->addOperation<ReturnOperation>();

	ExceptionCleanupPreparationPass().apply(*fixture.ir);
	const auto& sites = fixture.function->getExceptionRegion().callSites;
	REQUIRE(sites.size() == 2);
	REQUIRE(sites[0].call == constructor);
	REQUIRE(sites[0].cleanup == std::nullopt);
	REQUIRE(sites[1].call == destructorCall);
	REQUIRE(sites[1].cleanup == std::nullopt);
}

TEST_CASE("Exception cleanup preparation does not create pads without active destructors") {
	auto fixture = createLinearFunction({});
	addCall(*fixture.entry, OperationIdentifier {1});
	fixture.entry->addOperation<ReturnOperation>();

	REQUIRE(ExceptionCleanupPreparationPass().apply(*fixture.ir));
	REQUIRE(fixture.function->hasExceptionRegion());
	REQUIRE(fixture.function->getExceptionRegion().pads.empty());
	REQUIRE(fixture.function->getExceptionRegion().callSites.size() == 1);

	auto code = compiler::cpp::CPPLoweringProvider::lower(fixture.ir);
	REQUIRE(code.find("catch (...)") == std::string::npos);
}

TEST_CASE("Exception cleanup preparation ignores noexcept calls") {
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	auto fixture = createLinearFunction({AllocaSpec {8, 8, destructor}});
	addCall(*fixture.entry, OperationIdentifier {1}, {.noUnwind = true},
	        CleanupEffect {CleanupEffectKind::ActivateAfterSuccess, 0});
	addCall(*fixture.entry, OperationIdentifier {2}, {.noUnwind = true},
	        CleanupEffect {CleanupEffectKind::DeactivateBeforeCall, 0});
	fixture.entry->addOperation<ReturnOperation>();

	REQUIRE_FALSE(ExceptionCleanupPreparationPass().apply(*fixture.ir));
	REQUIRE_FALSE(fixture.function->hasExceptionRegion());
}

TEST_CASE("Exception cleanup preparation rejects unequal merge states") {
	auto ir = std::make_shared<IRGraph>("exception-cleanup-merge-mismatch");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* left = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {});
	auto* right = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {});
	auto* merge = arena.create<BasicBlock>(arena, BlockIdentifier {3}, std::vector<BasicBlockArgument*> {});
	auto* condition = entry->addOperation<ConstBooleanOperation>(OperationIdentifier {1}, true);
	auto* branch = entry->addOperation<IfOperation>(condition, 0.5);
	branch->setTrueBlockInvocation(left);
	branch->setFalseBlockInvocation(right);
	left->addOperation<AllocaOperation>(OperationIdentifier {2}, 0,
	                                    CleanupEffect {CleanupEffectKind::ActivateAfterSuccess, 0});
	left->addNextBlock(merge, std::span<Operation* const> {});
	right->addNextBlock(merge, std::span<Operation* const> {});
	merge->addOperation<ReturnOperation>();
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	auto* function = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, left, right, merge},
	                                                 std::vector<Type> {}, std::vector<std::string> {}, Type::v,
	                                                 std::vector<AllocaSpec> {{8, 8, destructor}});
	ir->addFunctionOperation(function);

	REQUIRE_THROWS_AS(ExceptionCleanupPreparationPass().apply(*ir), RuntimeException);
}

TEST_CASE("Exception cleanup preparation rejects loop lifetime drift") {
	auto ir = std::make_shared<IRGraph>("exception-cleanup-loop-mismatch");
	auto& arena = ir->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* loop = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {});
	entry->addNextBlock(loop, std::span<Operation* const> {});
	loop->addOperation<AllocaOperation>(OperationIdentifier {1}, 0,
	                                    CleanupEffect {CleanupEffectKind::ActivateAfterSuccess, 0});
	loop->addNextBlock(loop, std::span<Operation* const> {});
	DestructorSpec destructor {reinterpret_cast<void*>(&destroyTestValue), "destroyTestValue", {.noUnwind = true}};
	auto* function = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, loop},
	                                                 std::vector<Type> {}, std::vector<std::string> {}, Type::v,
	                                                 std::vector<AllocaSpec> {{8, 8, destructor}});
	ir->addFunctionOperation(function);

	REQUIRE_THROWS_AS(ExceptionCleanupPreparationPass().apply(*ir), RuntimeException);
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
