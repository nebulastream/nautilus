#pragma once

#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/val.hpp"
#include <concepts>
#include <type_traits>

#ifdef ENABLE_TRACING
// Include TraceContext for the template implementation
// This is a relative path from include/nautilus/ to src/nautilus/tracing/
#include "../../src/nautilus/tracing/TraceContext.hpp"
#endif

namespace nautilus {

/**
 * @brief If-then-else operation with lambda branches and direct block creation.
 *
 * This operation creates proper control flow blocks (CMP, true block, false block,
 * and merge block) directly during tracing, without relying on symbolic execution.
 *
 * Key differences from select():
 * - Creates proper control flow blocks instead of a single SELECT operation
 * - Both branches are traced (for complete IR generation)
 * - In non-tracing mode, only the appropriate branch is executed (short-circuit)
 *
 * Key differences from native if-else:
 * - Returns a value directly (expression-based)
 * - Creates blocks directly without symbolic execution iteration
 * - Both branches are traced in a single pass
 *
 * @tparam ThenFn Callable type for the true branch (must take no arguments)
 * @tparam ElseFn Callable type for the false branch (must take no arguments)
 * @param condition Boolean condition to test
 * @param thenBranch Lambda to execute/trace for the true case
 * @param elseBranch Lambda to execute/trace for the false case
 * @return The result from the appropriate branch (as a val<T>)
 *
 * Example usage:
 * @code
 * val<int32_t> result = ifThenElse(x > y,
 *     [&]{ return x; },      // true branch
 *     [&]{ return y; }       // false branch
 * );
 * @endcode
 *
 * Generated trace structure:
 * @code
 * B0(...)
 *     CMP $cond B1() B2()
 * B1()
 *     ... true branch operations ...
 *     JMP B3($trueResult)
 * B2()
 *     ... false branch operations ...
 *     JMP B3($falseResult)
 * B3($result) ControlFlowMerge
 *     ... continue with $result ...
 * @endcode
 */
template <typename ThenFn, typename ElseFn>
    requires std::invocable<ThenFn> && std::invocable<ElseFn>
auto ifThenElse(val<bool> condition, ThenFn&& thenBranch, ElseFn&& elseBranch) -> std::invoke_result_t<ThenFn> {
	using ThenResult = std::invoke_result_t<ThenFn>;
	using ElseResult = std::invoke_result_t<ElseFn>;

	static_assert(std::is_same_v<ThenResult, ElseResult>, "ifThenElse: both branches must return the same type");

	if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
		// Get condition state WITHOUT converting to bool (avoids triggering symbolic execution)
		auto conditionState = details::StateResolver<const val<bool>&>::getState(condition);
		double probability = condition.getIsTrueProbability();

		// Get the trace context and call the direct block creation method
		auto* ctx = tracing::getTracerIfActive();
		return ctx->template traceIfThenElse<ThenResult>(conditionState, probability, std::forward<ThenFn>(thenBranch),
		                                                 std::forward<ElseFn>(elseBranch));
#endif
	}

	// Non-tracing path: normal if-else with short-circuit evaluation
	if (details::RawValueResolver<bool>::getRawValue(condition)) {
		return std::forward<ThenFn>(thenBranch)();
	} else {
		return std::forward<ElseFn>(elseBranch)();
	}
}

/**
 * @brief If-then-else operation for void-returning branches.
 *
 * Similar to the value-returning version, but for branches that perform
 * side effects without returning a value.
 *
 * @tparam ThenFn Callable type for the true branch (must return void)
 * @tparam ElseFn Callable type for the false branch (must return void)
 * @param condition Boolean condition to test
 * @param thenBranch Lambda to execute/trace for the true case
 * @param elseBranch Lambda to execute/trace for the false case
 *
 * Example usage:
 * @code
 * ifThenElse(shouldLog,
 *     [&]{ logMessage("enabled"); },
 *     [&]{ }  // empty else branch
 * );
 * @endcode
 */
template <typename ThenFn, typename ElseFn>
    requires std::invocable<ThenFn> && std::invocable<ElseFn> && std::is_void_v<std::invoke_result_t<ThenFn>>
void ifThenElse(val<bool> condition, ThenFn&& thenBranch, ElseFn&& elseBranch) {
	static_assert(std::is_void_v<std::invoke_result_t<ElseFn>>, "ifThenElse: both branches must have void return type");

	if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
		// Get condition state WITHOUT converting to bool
		auto conditionState = details::StateResolver<const val<bool>&>::getState(condition);
		double probability = condition.getIsTrueProbability();

		// Get the trace context and call the direct block creation method
		auto* ctx = tracing::getTracerIfActive();
		ctx->traceIfThenElseVoid(conditionState, probability, std::forward<ThenFn>(thenBranch),
		                         std::forward<ElseFn>(elseBranch));
		return;
#endif
	}

	// Non-tracing path: normal if-else
	if (details::RawValueResolver<bool>::getRawValue(condition)) {
		std::forward<ThenFn>(thenBranch)();
	} else {
		std::forward<ElseFn>(elseBranch)();
	}
}

} // namespace nautilus
