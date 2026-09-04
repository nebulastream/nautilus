#include "nautilus/config.hpp"

#ifdef ENABLE_TRACING

#include "nautilus/CompilableFunction.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/NoThrowInferencePass.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <list>
#include <stdexcept>

// End-to-end coverage for NoThrowInferencePass over IR produced by the real
// tracing pipeline, rather than the hand-built graphs in
// ir-pass-tests/NoThrowInferenceTest.cpp. This is what pins the parts the
// synthetic tests cannot reach: that `FunctionCall::isNautilusCall` actually
// survives tracing into `CallOperation`, and that a call's recorded
// function name really does resolve to the callee's FunctionOperation in the
// same module.
namespace nautilus::engine {

namespace {

val<int32_t> pureAddHelper(val<int32_t> a, val<int32_t> b) {
	return a + b;
}
static auto pureAdd = NautilusFunction {"noThrowPureAdd", pureAddHelper};

val<int32_t> callsPureNautilusFunction(val<int32_t> a, val<int32_t> b) {
	return pureAdd(a, b);
}

void throwingRuntimeFn() {
	throw std::runtime_error("no-throw-inference integration");
}

val<int32_t> throwingHelper(val<int32_t> a) {
	invoke(throwingRuntimeFn);
	return a;
}
static auto throwingNautilusFn = NautilusFunction {"noThrowThrowingHelper", throwingHelper};

val<int32_t> callsThrowingNautilusFunction(val<int32_t> a) {
	return throwingNautilusFn(a);
}

/// Traces @p func through the real pipeline and returns the resulting IR,
/// stopping short of the pass manager so the test controls when (and
/// whether) NoThrowInferencePass runs.
std::shared_ptr<compiler::ir::IRGraph> traceToIR(const std::function<void()>& func, common::Arena& arena) {
	std::list<compiler::CompilableFunction> functionsToTrace;
	functionsToTrace.emplace_back("execute", func);
	auto trace = tracing::LazyTraceContext::Trace(functionsToTrace, engine::Options(), arena);
	auto afterSSA = tracing::SSACreationPhase().apply(std::shared_ptr<tracing::TraceModule>(std::move(trace)));
	return tracing::TraceToIRConversionPhase().apply(std::move(afterSSA));
}

void runPass(compiler::ir::IRGraph& ir) {
	engine::Options opts;
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::NoThrowInferencePass>());
	mgr.run(ir);
}

struct CallCounts {
	size_t nautilusCalls = 0;
	size_t throwingNautilusCalls = 0;
};

CallCounts countCalls(const compiler::ir::IRGraph& ir) {
	CallCounts counts;
	for (const auto* fn : ir.getFunctionOperations()) {
		if (fn == nullptr) {
			continue;
		}
		for (const auto* block : fn->getBasicBlocks()) {
			for (const auto* op : block->getOperations()) {
				const auto* proxy = compiler::ir::dyn_cast<compiler::ir::CallOperation>(op);
				if (proxy == nullptr || !proxy->isNautilusFunctionCall()) {
					continue;
				}
				++counts.nautilusCalls;
				if (proxy->requiresExceptionHandling() || !proxy->getFunctionAttributes().noUnwind) {
					++counts.throwingNautilusCalls;
				}
			}
		}
	}
	return counts;
}

} // namespace

TEST_CASE("NoThrowInference end-to-end: isNautilusCall survives tracing into the IR") {
	common::Arena arena;
	auto ir = traceToIR(details::createFunctionWrapper(callsPureNautilusFunction), arena);

	// The marker must reach the IR at all -- if this regresses, the pass
	// silently becomes a no-op rather than failing loudly.
	const auto before = countCalls(*ir);
	REQUIRE(before.nautilusCalls > 0);
}

TEST_CASE("NoThrowInference end-to-end: a pure Nautilus callee is proven noThrow") {
	common::Arena arena;
	auto ir = traceToIR(details::createFunctionWrapper(callsPureNautilusFunction), arena);

	// Precondition: the trace-time heuristic is pessimistic here, so the
	// call starts out needing exception handling. Without this the test
	// could pass vacuously.
	const auto before = countCalls(*ir);
	REQUIRE(before.nautilusCalls > 0);
	REQUIRE(before.throwingNautilusCalls > 0);

	runPass(*ir);

	const auto after = countCalls(*ir);
	REQUIRE(after.nautilusCalls == before.nautilusCalls);
	REQUIRE(after.throwingNautilusCalls == 0);
}

TEST_CASE("NoThrowInference end-to-end: a Nautilus callee that throws keeps exception handling") {
	common::Arena arena;
	auto ir = traceToIR(details::createFunctionWrapper(callsThrowingNautilusFunction), arena);

	const auto before = countCalls(*ir);
	REQUIRE(before.nautilusCalls > 0);
	REQUIRE(before.throwingNautilusCalls > 0);

	runPass(*ir);

	// The callee reaches a genuinely throwing invoke(), so the call into it
	// must survive the pass untouched.
	const auto after = countCalls(*ir);
	REQUIRE(after.throwingNautilusCalls == before.throwingNautilusCalls);
}

TEST_CASE("NoThrowInference end-to-end: the pass is idempotent") {
	common::Arena arena;
	auto ir = traceToIR(details::createFunctionWrapper(callsPureNautilusFunction), arena);

	runPass(*ir);
	const auto afterFirst = countCalls(*ir);
	runPass(*ir);
	const auto afterSecond = countCalls(*ir);

	REQUIRE(afterSecond.nautilusCalls == afterFirst.nautilusCalls);
	REQUIRE(afterSecond.throwingNautilusCalls == afterFirst.throwingNautilusCalls);
}

} // namespace nautilus::engine

#endif
