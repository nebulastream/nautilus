#include "nautilus/CompilableFunction.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/options.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/val_func.hpp"
#include "nautilus/val_std.hpp"
#include <catch2/catch_test_macros.hpp>
#include <list>

namespace nautilus::tracing {

namespace {

struct TrackedCleanupValue {
	TrackedCleanupValue() noexcept = default;
	~TrackedCleanupValue() noexcept {
	}
};

void noThrowTarget(int32_t) noexcept {
}

void mayThrowTarget(int32_t) {
}

val<int32_t> nestedNoThrowTarget(val<int32_t> value) noexcept {
	return value;
}

val<int32_t> nestedMayThrowTarget(val<int32_t> value) {
	return value;
}

auto nestedNoThrow = NautilusFunction {"nestedNoThrowCleanupTarget", nestedNoThrowTarget};
auto nestedMayThrow = NautilusFunction {"nestedMayThrowCleanupTarget", nestedMayThrowTarget};

template <typename TraceContext>
std::unique_ptr<TraceModule> traceWith(std::function<void()> function, common::Arena& arena) {
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", std::move(function));
	return TraceContext::Trace(functions, engine::Options {}, arena);
}

template <typename OperationType>
std::vector<const OperationType*> findOperations(const compiler::ir::FunctionOperation& function) {
	auto result = std::vector<const OperationType*> {};
	for (const auto* block : function.getBasicBlocks()) {
		for (const auto* operation : block->getOperations()) {
			if (OperationType::classof(operation)) {
				result.push_back(static_cast<const OperationType*>(operation));
			}
		}
	}
	return result;
}

} // namespace

TEST_CASE("Tracing preserves direct exception cleanup metadata through IR") {
	auto run = []<typename TraceContext>() {
		common::Arena arena;
		auto tracedModule = traceWith<TraceContext>(
		    [] {
			    {
				    val<TrackedCleanupValue> first;
				    val<TrackedCleanupValue> second;
				    invoke(mayThrowTarget, val<int32_t> {7});
				    invoke(noThrowTarget, val<int32_t> {8});
			    }
			    tracing::traceReturnOperation(Type::v, {});
		    },
		    arena);
		auto* trace = tracedModule->getFunction("execute");
		const TraceOperation* throwingOperation = nullptr;
		const TraceOperation* noThrowOperation = nullptr;
		for (const auto* block : trace->getBlocks()) {
			for (const auto* operation : block->operations) {
				if (operation->op != Op::CALL && operation->op != Op::CALL_WITH_EXCEPTION_HANDLING) {
					continue;
				}
				const auto* call = std::get<FunctionCall*>(operation->input.front());
				if (call->ptr == reinterpret_cast<void*>(mayThrowTarget)) {
					throwingOperation = operation;
				} else if (call->ptr == reinterpret_cast<void*>(noThrowTarget)) {
					noThrowOperation = operation;
				}
			}
		}

		REQUIRE(throwingOperation != nullptr);
		REQUIRE(noThrowOperation != nullptr);
		const auto* throwingCall = std::get<FunctionCall*>(throwingOperation->input.front());
		const auto* noThrowCall = std::get<FunctionCall*>(noThrowOperation->input.front());
		REQUIRE(throwingOperation->op == Op::CALL_WITH_EXCEPTION_HANDLING);
		REQUIRE(throwingCall->destructors.size() == 2);
		REQUIRE(throwingCall->destructors[0].address != throwingCall->destructors[1].address);
		REQUIRE(noThrowOperation->op == Op::CALL);
		REQUIRE(noThrowCall->fnAttrs.noUnwind);
		REQUIRE(noThrowCall->destructors.empty());

		auto afterSsa = SSACreationPhase().apply(std::shared_ptr<TraceModule>(std::move(tracedModule)));
		auto ir = TraceToIRConversionPhase().apply(afterSsa, "direct-exception-cleanup");
		auto* function = ir->getFunctionOperation("execute");
		REQUIRE(function != nullptr);
		const auto calls = findOperations<compiler::ir::ProxyCallOperation>(*function);
		const compiler::ir::ProxyCallOperation* throwingIrCall = nullptr;
		const compiler::ir::ProxyCallOperation* noThrowIrCall = nullptr;
		for (const auto* call : calls) {
			if (call->getFunctionPtr() == reinterpret_cast<void*>(mayThrowTarget)) {
				throwingIrCall = call;
			} else if (call->getFunctionPtr() == reinterpret_cast<void*>(noThrowTarget)) {
				noThrowIrCall = call;
			}
		}
		REQUIRE(throwingIrCall != nullptr);
		REQUIRE(throwingIrCall->requiresExceptionHandling());
		REQUIRE(throwingIrCall->getDestructors().size() == 2);
		REQUIRE(noThrowIrCall != nullptr);
		REQUIRE_FALSE(noThrowIrCall->requiresExceptionHandling());
		REQUIRE(noThrowIrCall->getDestructors().empty());
	};

	SECTION("exception-based tracer") {
		run.template operator()<ExceptionBasedTraceContext>();
	}
	SECTION("lazy tracer") {
		run.template operator()<LazyTraceContext>();
	}
}

TEST_CASE("Tracing preserves indirect exception cleanup metadata through IR") {
	auto run = []<typename TraceContext>() {
		common::Arena arena;
		auto tracedModule = traceWith<TraceContext>(
		    [] {
			    {
				    val<TrackedCleanupValue> value;
				    auto target = val<void (*)(int32_t)> {mayThrowTarget};
				    target(val<int32_t> {7});
			    }
			    tracing::traceReturnOperation(Type::v, {});
		    },
		    arena);
		auto* trace = tracedModule->getFunction("execute");
		const IndirectFunctionCall* tracedCall = nullptr;
		for (const auto* block : trace->getBlocks()) {
			for (const auto* operation : block->operations) {
				if (operation->op == Op::INDIRECT_CALL) {
					tracedCall = std::get<IndirectFunctionCall*>(operation->input.front());
				}
			}
		}
		REQUIRE(tracedCall != nullptr);
		REQUIRE(tracedCall->exceptionHandling);
		REQUIRE(tracedCall->destructors.size() == 1);

		auto afterSsa = SSACreationPhase().apply(std::shared_ptr<TraceModule>(std::move(tracedModule)));
		auto ir = TraceToIRConversionPhase().apply(afterSsa, "indirect-exception-cleanup");
		auto* function = ir->getFunctionOperation("execute");
		REQUIRE(function != nullptr);
		const auto calls = findOperations<compiler::ir::IndirectCallOperation>(*function);
		REQUIRE(calls.size() == 1);
		REQUIRE(calls.front()->requiresExceptionHandling());
		REQUIRE(calls.front()->getDestructors().size() == 1);
	};

	SECTION("exception-based tracer") {
		run.template operator()<ExceptionBasedTraceContext>();
	}
	SECTION("lazy tracer") {
		run.template operator()<LazyTraceContext>();
	}
}

TEST_CASE("Exact noexcept indirect calls retain the direct path") {
	auto run = []<typename TraceContext>() {
		common::Arena arena;
		auto tracedModule = traceWith<TraceContext>(
		    [] {
			    {
				    val<TrackedCleanupValue> value;
				    auto target = val<decltype(&noThrowTarget)> {noThrowTarget};
				    target(val<int32_t> {7});
			    }
			    tracing::traceReturnOperation(Type::v, {});
		    },
		    arena);
		auto* trace = tracedModule->getFunction("execute");
		const IndirectFunctionCall* tracedCall = nullptr;
		for (const auto* block : trace->getBlocks()) {
			for (const auto* operation : block->operations) {
				if (operation->op == Op::INDIRECT_CALL) {
					tracedCall = std::get<IndirectFunctionCall*>(operation->input.front());
				}
			}
		}
		REQUIRE(tracedCall != nullptr);
		REQUIRE(tracedCall->fnAttrs.noUnwind);
		REQUIRE_FALSE(tracedCall->exceptionHandling);
		REQUIRE(tracedCall->destructors.empty());

		auto afterSsa = SSACreationPhase().apply(std::shared_ptr<TraceModule>(std::move(tracedModule)));
		auto ir = TraceToIRConversionPhase().apply(afterSsa, "indirect-noexcept-cleanup");
		auto* function = ir->getFunctionOperation("execute");
		REQUIRE(function != nullptr);
		const auto calls = findOperations<compiler::ir::IndirectCallOperation>(*function);
		REQUIRE(calls.size() == 1);
		REQUIRE(calls.front()->getFunctionAttributes().noUnwind);
		REQUIRE_FALSE(calls.front()->requiresExceptionHandling());
		REQUIRE(calls.front()->getDestructors().empty());
	};

	SECTION("exception-based tracer") {
		run.template operator()<ExceptionBasedTraceContext>();
	}
	SECTION("lazy tracer") {
		run.template operator()<LazyTraceContext>();
	}
}

TEST_CASE("Nested Nautilus calls classify cleanup from exact noexcept type") {
	auto run = []<typename TraceContext>() {
		common::Arena arena;
		auto tracedModule = traceWith<TraceContext>(
		    [] {
			    {
				    val<TrackedCleanupValue> value;
				    [[maybe_unused]] auto noThrowResult = nestedNoThrow(val<int32_t> {1});
				    [[maybe_unused]] auto mayThrowResult = nestedMayThrow(val<int32_t> {2});
			    }
			    tracing::traceReturnOperation(Type::v, {});
		    },
		    arena);
		auto* trace = tracedModule->getFunction("execute");
		const TraceOperation* noThrowOperation = nullptr;
		const TraceOperation* mayThrowOperation = nullptr;
		for (const auto* block : trace->getBlocks()) {
			for (const auto* operation : block->operations) {
				if (operation->op != Op::CALL && operation->op != Op::CALL_WITH_EXCEPTION_HANDLING) {
					continue;
				}
				const auto* call = std::get<FunctionCall*>(operation->input.front());
				if (call->functionName == "nestedNoThrowCleanupTarget") {
					noThrowOperation = operation;
				} else if (call->functionName == "nestedMayThrowCleanupTarget") {
					mayThrowOperation = operation;
				}
			}
		}
		REQUIRE(noThrowOperation != nullptr);
		REQUIRE(mayThrowOperation != nullptr);
		REQUIRE(noThrowOperation->op == Op::CALL);
		REQUIRE(std::get<FunctionCall*>(noThrowOperation->input.front())->destructors.empty());
		REQUIRE(mayThrowOperation->op == Op::CALL_WITH_EXCEPTION_HANDLING);
		REQUIRE(std::get<FunctionCall*>(mayThrowOperation->input.front())->destructors.size() == 1);
	};

	SECTION("exception-based tracer") {
		run.template operator()<ExceptionBasedTraceContext>();
	}
	SECTION("lazy tracer") {
		run.template operator()<LazyTraceContext>();
	}
}

} // namespace nautilus::tracing
