#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/ExceptionCleanup.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/function.hpp"
#include "nautilus/options.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/val_std.hpp"
#include <catch2/catch_test_macros.hpp>
#include <list>

namespace nautilus::tracing {

namespace {

void noThrowTarget(int32_t) noexcept {
}

void mayThrowTarget(int32_t) {
}

struct TrackedValue {
	TrackedValue() noexcept : value(42) {
	}

	~TrackedValue() noexcept {
	}

	int32_t value;
};

std::unique_ptr<TraceModule> trace(std::function<void()> function, common::Arena& arena) {
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", std::move(function));
	return ExceptionBasedTraceContext::Trace(functions, engine::Options {}, arena);
}

std::vector<const TraceOperation*> calls(const ExecutionTrace& trace) {
	std::vector<const TraceOperation*> result;
	for (const auto* block : trace.blocks) {
		for (const auto* operation : block->operations) {
			if (operation->op == Op::CALL) {
				result.push_back(operation);
			}
		}
	}
	return result;
}

} // namespace

TEST_CASE("Tracing infers noUnwind from exact noexcept function pointers") {
	common::Arena arena;
	auto module = trace(
	    [] {
		    invoke(noThrowTarget, val<int32_t> {1});
		    invoke(mayThrowTarget, val<int32_t> {2});
	    },
	    arena);
	const auto tracedCalls = calls(*module->getFunction("execute"));

	REQUIRE(tracedCalls.size() == 2);
	const auto* noThrow = std::get<FunctionCall*>(tracedCalls[0]->input.front());
	const auto* mayThrow = std::get<FunctionCall*>(tracedCalls[1]->input.front());
	REQUIRE(noThrow->fnAttrs.noUnwind);
	REQUIRE_FALSE(mayThrow->fnAttrs.noUnwind);
}

TEST_CASE("Tracing records construction activation and destruction deactivation") {
	common::Arena arena;
	auto module = trace(
	    [] {
		    val<TrackedValue> value;
		    (void) value;
	    },
	    arena);
	const auto* executionTrace = module->getFunction("execute");
	const auto tracedCalls = calls(*executionTrace);

	REQUIRE(executionTrace->allocaSpecs.size() == 1);
	REQUIRE(executionTrace->allocaSpecs.front().destructor.has_value());
	REQUIRE(executionTrace->allocaSpecs.front().destructor->attributes.noUnwind);
	REQUIRE(tracedCalls.size() == 2);
	REQUIRE(tracedCalls[0]->cleanupEffect == CleanupEffect {CleanupEffectKind::ActivateAfterSuccess, AllocaIndex {0}});
	REQUIRE(tracedCalls[1]->cleanupEffect == CleanupEffect {CleanupEffectKind::DeactivateBeforeCall, AllocaIndex {0}});
}

TEST_CASE("Trace-to-IR conversion preserves exception cleanup metadata") {
	common::Arena arena;
	auto tracedModule = trace(
	    [] {
		    val<TrackedValue> value;
		    (void) value;
	    },
	    arena);
	auto module = std::shared_ptr<TraceModule>(std::move(tracedModule));
	auto ir = TraceToIRConversionPhase().apply(module, "exception-cleanup-conversion");
	const auto* function = ir->getFunctionOperation("execute");

	REQUIRE(function != nullptr);
	REQUIRE(function->getAllocaSpecs().size() == 1);
	REQUIRE(function->getAllocaSpecs().front().destructor.has_value());

	const compiler::ir::AllocaOperation* alloca = nullptr;
	std::vector<const compiler::ir::ProxyCallOperation*> irCalls;
	for (const auto* block : function->getBasicBlocks()) {
		for (const auto* operation : block->getOperations()) {
			if (operation->getOperationType() == compiler::ir::Operation::OperationType::AllocaOp) {
				alloca = static_cast<const compiler::ir::AllocaOperation*>(operation);
			} else if (operation->getOperationType() == compiler::ir::Operation::OperationType::ProxyCallOp) {
				irCalls.push_back(static_cast<const compiler::ir::ProxyCallOperation*>(operation));
			}
		}
	}

	REQUIRE(alloca != nullptr);
	REQUIRE_FALSE(alloca->getCleanupEffect().has_value());
	REQUIRE(irCalls.size() == 2);
	REQUIRE(irCalls[0]->getCleanupEffect() == CleanupEffect {CleanupEffectKind::ActivateAfterSuccess, AllocaIndex {0}});
	REQUIRE(irCalls[1]->getCleanupEffect() == CleanupEffect {CleanupEffectKind::DeactivateBeforeCall, AllocaIndex {0}});
}

} // namespace nautilus::tracing
