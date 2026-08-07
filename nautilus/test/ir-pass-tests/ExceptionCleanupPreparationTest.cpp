#include "nautilus/common/ExceptionCleanup.hpp"
#include "nautilus/compiler/ir/ExceptionRegion.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include <catch2/catch_test_macros.hpp>

namespace nautilus::compiler::ir {

namespace {
void destroyTestValue(void*) noexcept {
}
}

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

} // namespace nautilus::compiler::ir
