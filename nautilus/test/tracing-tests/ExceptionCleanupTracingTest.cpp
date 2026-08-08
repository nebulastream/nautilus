#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/ExceptionCleanup.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/passes/ExceptionCleanupPreparationPass.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/options.hpp"
#include "nautilus/static.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/val_std.hpp"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <list>
#include <optional>

namespace nautilus::tracing {

namespace {

void noThrowTarget(int32_t) noexcept {
}

void mayThrowTarget(int32_t) {
}

void destroyTrackedValue(void*) noexcept {
}

val<int32_t> nestedNoThrowTarget(val<int32_t> value) noexcept {
	return value + 1;
}

val<int32_t> nestedMayThrowTarget(val<int32_t> value) {
	return value + 2;
}

auto nestedNoThrow = NautilusFunction {"nestedNoThrow", nestedNoThrowTarget};
auto nestedMayThrow = NautilusFunction {"nestedMayThrow", nestedMayThrowTarget};
auto nestedErasedNoThrow =
    NautilusFunction {"nestedErasedNoThrow", std::function<val<int32_t>(val<int32_t>)> {nestedNoThrowTarget}};

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

template <typename TraceContext>
std::unique_ptr<TraceModule> traceWith(std::function<void()> function, common::Arena& arena) {
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", std::move(function));
	return TraceContext::Trace(functions, engine::Options {}, arena);
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

std::vector<const TraceOperation*> callsTo(const ExecutionTrace& trace, void* target) {
	auto result = std::vector<const TraceOperation*> {};
	for (const auto* operation : calls(trace)) {
		if (std::get<FunctionCall*>(operation->input.front())->ptr == target) {
			result.push_back(operation);
		}
	}
	return result;
}

std::vector<const TraceOperation*> callsNamed(const ExecutionTrace& trace, const std::string& name) {
	auto result = std::vector<const TraceOperation*> {};
	for (const auto* operation : calls(trace)) {
		if (std::get<FunctionCall*>(operation->input.front())->functionName == name) {
			result.push_back(operation);
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

TEST_CASE("Nested Nautilus calls infer noUnwind from exact callable types") {
	auto run = []<typename TraceContext>() {
		common::Arena arena;
		auto tracedModule = traceWith<TraceContext>(
		    [] {
			    {
				    val<TrackedValue> value;
				    [[maybe_unused]] auto noThrowResult = nestedNoThrow(val<int32_t> {7});
				    [[maybe_unused]] auto mayThrowResult = nestedMayThrow(val<int32_t> {7});
				    [[maybe_unused]] auto erasedResult = nestedErasedNoThrow(val<int32_t> {7});
			    }
			    tracing::traceReturnOperation(Type::v, {});
		    },
		    arena);

		const auto* trace = tracedModule->getFunction("execute");
		const auto noThrowCalls = callsNamed(*trace, "nestedNoThrow");
		const auto mayThrowCalls = callsNamed(*trace, "nestedMayThrow");
		const auto erasedCalls = callsNamed(*trace, "nestedErasedNoThrow");
		REQUIRE(noThrowCalls.size() == 1);
		REQUIRE(mayThrowCalls.size() == 1);
		REQUIRE(erasedCalls.size() == 1);
		REQUIRE(std::get<FunctionCall*>(noThrowCalls.front()->input.front())->fnAttrs.noUnwind);
		REQUIRE_FALSE(std::get<FunctionCall*>(mayThrowCalls.front()->input.front())->fnAttrs.noUnwind);
		REQUIRE_FALSE(std::get<FunctionCall*>(erasedCalls.front()->input.front())->fnAttrs.noUnwind);
		REQUIRE(noThrowCalls.front()->tag.getCleanupStateId() == mayThrowCalls.front()->tag.getCleanupStateId());
		REQUIRE(noThrowCalls.front()->tag.getCleanupStateId() != EMPTY_CLEANUP_STATE);

		auto module = std::shared_ptr<TraceModule>(std::move(tracedModule));
		auto afterSsa = SSACreationPhase().apply(std::move(module));
		auto ir = TraceToIRConversionPhase().apply(afterSsa, "nested-exception-cleanup");
		const auto* function = ir->getFunctionOperation("execute");
		REQUIRE(function != nullptr);

		const compiler::ir::ProxyCallOperation* noThrowCall = nullptr;
		const compiler::ir::ProxyCallOperation* mayThrowCall = nullptr;
		const compiler::ir::ProxyCallOperation* erasedCall = nullptr;
		for (const auto* block : function->getBasicBlocks()) {
			for (const auto* operation : block->getOperations()) {
				if (operation->getOperationType() != compiler::ir::Operation::OperationType::ProxyCallOp) {
					continue;
				}
				const auto* call = static_cast<const compiler::ir::ProxyCallOperation*>(operation);
				if (call->getFunctionName() == "nestedNoThrow") {
					noThrowCall = call;
				} else if (call->getFunctionName() == "nestedMayThrow") {
					mayThrowCall = call;
				} else if (call->getFunctionName() == "nestedErasedNoThrow") {
					erasedCall = call;
				}
			}
		}

		REQUIRE(noThrowCall != nullptr);
		REQUIRE(mayThrowCall != nullptr);
		REQUIRE(erasedCall != nullptr);
		REQUIRE(noThrowCall->getFunctionAttributes().noUnwind);
		REQUIRE_FALSE(noThrowCall->getCleanupState().has_value());
		REQUIRE_FALSE(mayThrowCall->getFunctionAttributes().noUnwind);
		REQUIRE(mayThrowCall->getCleanupState().has_value());
		REQUIRE_FALSE(erasedCall->getFunctionAttributes().noUnwind);
		REQUIRE(erasedCall->getCleanupState().has_value());

		compiler::ir::ExceptionCleanupPreparationPass().apply(*ir);
		REQUIRE(function->hasExceptionRegion());
		const auto& exceptionRegion = function->getExceptionRegion();
		REQUIRE(exceptionRegion.pads.size() == 1);
		REQUIRE(exceptionRegion.callSites.size() == 2);
		auto isExceptionalCall = [&](const auto* call) {
			return std::ranges::any_of(exceptionRegion.callSites,
			                           [&](const auto& callSite) { return callSite.call == call; });
		};
		REQUIRE_FALSE(isExceptionalCall(noThrowCall));
		REQUIRE(isExceptionalCall(mayThrowCall));
		REQUIRE(isExceptionalCall(erasedCall));
	};

	SECTION("exception-based tracer") {
		run.template operator()<ExceptionBasedTraceContext>();
	}
	SECTION("lazy tracer") {
		run.template operator()<LazyTraceContext>();
	}
}

TEST_CASE("execution traces intern ordered cleanup states") {
	common::Arena arena;
	ExecutionTrace trace(arena);

	REQUIRE(trace.getCleanupState(EMPTY_CLEANUP_STATE).active.empty());
	const auto first = trace.internCleanupState({0, 1});
	const auto repeated = trace.internCleanupState({0, 1});
	const auto reordered = trace.internCleanupState({1, 0});

	REQUIRE(first == repeated);
	REQUIRE(first != reordered);
	REQUIRE(trace.getCleanupState(first).active == std::vector<AllocaIndex> {0, 1});
}

TEST_CASE("snapshot identity includes exact cleanup state") {
	Snapshot empty(nullptr, 7, CleanupStateId {0});
	Snapshot active(nullptr, 7, CleanupStateId {1});

	REQUIRE(empty != active);
	REQUIRE(std::hash<Snapshot> {}(empty) != std::hash<Snapshot> {}(active));
}

TEST_CASE("branch paths retain distinct cleanup states at a shared throwing call") {
	auto run = []<typename TraceContext>() {
		common::Arena arena;
		auto module = traceWith<TraceContext>(
		    [] {
			    auto alloca = std::optional<AllocaIndex> {};
			    if (val<bool> {true}) {
				    static_val<int32_t> branchIdentity {0};
				    tracing::traceAlloca(sizeof(TrackedValue), alignof(TrackedValue), alloca,
				                         reinterpret_cast<void*>(&destroyTrackedValue), {.noUnwind = true});
			    } else {
				    static_val<int32_t> branchIdentity {1};
				    tracing::traceAlloca(sizeof(TrackedValue), alignof(TrackedValue), alloca,
				                         reinterpret_cast<void*>(&destroyTrackedValue), {.noUnwind = true});
			    }
			    tracing::activateCleanup(*alloca);
			    invoke(mayThrowTarget, val<int32_t> {7});
			    tracing::deactivateCleanup(*alloca);
		    },
		    arena);
		const auto* executionTrace = module->getFunction("execute");
		const auto throwingCalls = callsTo(*executionTrace, reinterpret_cast<void*>(&mayThrowTarget));

		REQUIRE(executionTrace->allocaSpecs.size() == 2);
		REQUIRE(throwingCalls.size() == 2);
		const auto leftState = throwingCalls[0]->tag.getCleanupStateId();
		const auto rightState = throwingCalls[1]->tag.getCleanupStateId();
		REQUIRE(leftState != rightState);
		REQUIRE(executionTrace->getCleanupState(leftState).active == std::vector<AllocaIndex> {0});
		REQUIRE(executionTrace->getCleanupState(rightState).active == std::vector<AllocaIndex> {1});
	};

	SECTION("exception-based tracer") {
		run.template operator()<ExceptionBasedTraceContext>();
	}
	SECTION("lazy tracer") {
		run.template operator()<LazyTraceContext>();
	}
}

TEST_CASE("FOLLOW mode rejects cleanup-state drift") {
	auto run = []<typename TraceContext>() {
		common::Arena arena;
		auto iteration = uint32_t {0};
		auto function = std::function<void()> {[&] {
			auto alloca = std::optional<AllocaIndex> {};
			tracing::traceAlloca(sizeof(TrackedValue), alignof(TrackedValue), alloca,
			                     reinterpret_cast<void*>(&destroyTrackedValue), {.noUnwind = true});
			const auto activate = iteration++ == 0;
			if (activate) {
				tracing::activateCleanup(*alloca);
			}
			if (val<bool> {true}) {
				invoke(noThrowTarget, val<int32_t> {1});
			}
			if (activate) {
				tracing::deactivateCleanup(*alloca);
			}
		}};

		REQUIRE_THROWS_AS(traceWith<TraceContext>(std::move(function), arena), RuntimeException);
	};

	SECTION("exception-based tracer") {
		run.template operator()<ExceptionBasedTraceContext>();
	}
	SECTION("lazy tracer") {
		run.template operator()<LazyTraceContext>();
	}
}

TEST_CASE("tracing rejects a normal return with an active cleanup") {
	auto run = []<typename TraceContext>() {
		common::Arena arena;
		auto function = std::function<void()> {[] {
			auto alloca = std::optional<AllocaIndex> {};
			tracing::traceAlloca(sizeof(TrackedValue), alignof(TrackedValue), alloca,
			                     reinterpret_cast<void*>(&destroyTrackedValue), {.noUnwind = true});
			tracing::activateCleanup(*alloca);
		}};

		REQUIRE_THROWS_AS(traceWith<TraceContext>(std::move(function), arena), RuntimeException);
	};

	SECTION("exception-based tracer") {
		run.template operator()<ExceptionBasedTraceContext>();
	}
	SECTION("lazy tracer") {
		run.template operator()<LazyTraceContext>();
	}
}

TEST_CASE("Tracing records construction activation and destruction deactivation") {
	common::Arena arena;
	auto module = trace(
	    [] {
		    val<TrackedValue> value;
		    invoke(mayThrowTarget, val<int32_t> {7});
	    },
	    arena);
	const auto* executionTrace = module->getFunction("execute");
	const auto tracedCalls = calls(*executionTrace);

	REQUIRE(executionTrace->allocaSpecs.size() == 1);
	REQUIRE(executionTrace->allocaSpecs.front().destructor.has_value());
	REQUIRE(executionTrace->allocaSpecs.front().destructor->attributes.noUnwind);
	REQUIRE(tracedCalls.size() == 3);
	REQUIRE(tracedCalls[0]->tag.getCleanupStateId() == EMPTY_CLEANUP_STATE);
	const auto active = tracedCalls[1]->tag.getCleanupStateId();
	REQUIRE(executionTrace->getCleanupState(active).active == std::vector<AllocaIndex> {0});
	REQUIRE(tracedCalls[2]->tag.getCleanupStateId() == EMPTY_CLEANUP_STATE);
}

TEST_CASE("Trace-to-IR conversion preserves exception cleanup metadata") {
	common::Arena arena;
	auto tracedModule = trace(
	    [] {
		    {
			    val<TrackedValue> value;
			    invoke(mayThrowTarget, val<int32_t> {7});
		    }
		    tracing::traceReturnOperation(Type::v, {});
	    },
	    arena);
	auto module = std::shared_ptr<TraceModule>(std::move(tracedModule));
	auto afterSsa = SSACreationPhase().apply(std::move(module));
	auto ir = TraceToIRConversionPhase().apply(afterSsa, "exception-cleanup-conversion");
	const auto* function = ir->getFunctionOperation("execute");

	REQUIRE(function != nullptr);
	REQUIRE(function->getAllocaSpecs().size() == 1);
	REQUIRE(function->getAllocaSpecs().front().destructor.has_value());
	REQUIRE(function->getCleanupStates().size() == 2);

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
	REQUIRE(irCalls.size() == 3);
	REQUIRE_FALSE(irCalls[0]->getCleanupState().has_value());
	REQUIRE(irCalls[1]->getCleanupState().has_value());
	REQUIRE(function->getCleanupStates().at(*irCalls[1]->getCleanupState()).active == std::vector<AllocaIndex> {0});
	REQUIRE_FALSE(irCalls[2]->getCleanupState().has_value());
}

} // namespace nautilus::tracing
