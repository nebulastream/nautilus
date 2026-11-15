#pragma once

#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/val.hpp"
#include <functional>
#include <stdexcept>
#include <type_traits>

/**
 * @file control_flow.hpp
 * @brief Explicit, closure-style control-flow constructs for Nautilus.
 *
 * Nautilus normally traces native C++ control flow (`if`, `for`, `while` over a `val<bool>`): the traced function is
 * re-run once per explored path, which costs more the more branches a function has. The constructs in this header state
 * control flow explicitly:
 *
 * @code
 *   If(x == 42, [&] { r = r + 1; }, [&] { r = r + 2; });
 *   val<int32_t> m = If(a > b, [&] { return a; }, [&] { return b; });          // value form
 *   For(val<int32_t>(0), n, [&](val<int32_t> i) { sum = sum + i; });
 *   While([&] { return a != b; }, [&] { If(a > b, [&] { Break(); }); ... });
 * @endcode
 *
 * Each construct emits its blocks directly. The arms, the loop header, the body and the step are scopes of their own,
 * as a `region()` body is, so they can be mixed with native control flow: a native `if` inside an arm replays only that
 * arm, and a native `if` outside a construct skips it when the enclosing function runs again. A function written
 * entirely with these constructs traces in one pass.
 *
 * `Break()` and `Continue()` leave the innermost explicit loop, and `Return(v)` leaves the function. They are plain
 * function calls, so the code after them in the same lambda still runs in C++, but it is not recorded. The owned values
 * (`registerDestructor`) between the exit and its target are destroyed on the exit path. An exit does not cross a
 * `region()`.
 *
 * Values flow out of a construct through `val`s declared outside it and assigned inside, as with `region()`; a value
 * created inside an arm or body must not outlive it.
 *
 * Without tracing the lambdas run natively. With the 0.3 tracer (`tracing.nirTracer=false`) the constructs trace as
 * the native control flow they stand for, except `Return`, which needs the NIR tracer.
 *
 * @warning When generating constructs from a host (plain C++) loop, make the loop counter a `static_val<T>` (as
 * `static_iterable` loops do), so that the operations inside the bodies of different iterations get distinct snapshots.
 */

namespace nautilus {

namespace cf_detail {
/// Thrown by Break()/Continue() when no tracer records them (direct mode, 0.3 tracer) so the enclosing While/For can
/// leave the body lambda, mirroring the jump the NIR tracer records.
struct BreakSignal {};
struct ContinueSignal {};

/// Whether the active tracer records the constructs as blocks.
inline bool explicitTracing() {
#ifdef ENABLE_TRACING
	return tracing::inTracer() && tracing::getActiveTracer()->tracesExplicitControlFlow();
#else
	return false;
#endif
}

#ifdef ENABLE_TRACING
inline void reportCondition(const val<bool>& condition) {
	tracing::getActiveTracer()->traceLoopCondition(details::StateResolver<const val<bool>&>::getState(condition),
	                                               condition.getIsTrueProbability());
}
#endif

/// A loop `for (; cond(); step()) body()` of the three callables.
template <typename CondFn, typename StepFn, typename BodyFn>
void loop(CondFn& condFn, StepFn* stepFn, BodyFn& bodyFn) {
#ifdef ENABLE_TRACING
	if (explicitTracing()) {
		std::function<void()> header = [&]() {
			val<bool> condition = condFn();
			reportCondition(condition);
		};
		std::function<void()> body = [&]() {
			bodyFn();
		};
		if (stepFn != nullptr) {
			std::function<void()> step = [&]() {
				(*stepFn)();
			};
			tracing::getActiveTracer()->traceLoop(header, body, &step);
		} else {
			tracing::getActiveTracer()->traceLoop(header, body, nullptr);
		}
		return;
	}
#endif
	for (; condFn(); stepFn != nullptr ? (*stepFn)() : void()) {
		try {
			bodyFn();
		} catch (const BreakSignal&) {
			break;
		} catch (const ContinueSignal&) {
			continue;
		}
	}
}
} // namespace cf_detail

/**
 * @brief Explicit if-then-else.
 * @param condition Branch condition.
 * @param thenBody Callable emitting the true branch.
 * @param elseBody Callable emitting the false branch.
 */
template <typename ThenFn, typename ElseFn>
    requires std::is_void_v<std::invoke_result_t<ThenFn>>
void If(const val<bool>& condition, ThenFn&& thenBody, ElseFn&& elseBody) {
#ifdef ENABLE_TRACING
	if (cf_detail::explicitTracing()) {
		std::function<void()> thenArm = [&]() {
			thenBody();
		};
		std::function<void()> elseArm = [&]() {
			elseBody();
		};
		tracing::getActiveTracer()->traceIf(details::StateResolver<const val<bool>&>::getState(condition),
		                                    condition.getIsTrueProbability(), thenArm, elseArm);
		return;
	}
#endif
	if (condition) {
		thenBody();
	} else {
		elseBody();
	}
}

/// Explicit if-then (no else): a degenerate if-then-else with an empty else arm.
template <typename ThenFn>
void If(const val<bool>& condition, ThenFn&& thenBody) {
	If(condition, std::forward<ThenFn>(thenBody), []() {});
}

/**
 * @brief Value-yielding if-then-else: returns the taken arm's `val<T>`.
 *
 * Selected when the arms return a (non-void) `val<T>`. Only the taken arm is evaluated at runtime; the result is
 * reconciled through a variable that both arms assign. Prefer `select(cond, a, b)` when both values are cheap and you
 * want a branch-free conditional move.
 */
template <typename ThenFn, typename ElseFn>
    requires(!std::is_void_v<std::invoke_result_t<ThenFn>>)
auto If(const val<bool>& condition, ThenFn&& thenBody, ElseFn&& elseBody) -> std::invoke_result_t<ThenFn> {
	using Result = std::invoke_result_t<ThenFn>;
	Result result {};
	If(condition, [&]() { result = thenBody(); }, [&]() { result = elseBody(); });
	return result;
}

/**
 * @brief Leave the innermost explicit For/While loop.
 *
 * The rest of the calling lambda still runs in C++ but is not recorded. Without a tracer that records the construct, it
 * unwinds out of the body lambda.
 */
inline void Break() {
#ifdef ENABLE_TRACING
	if (cf_detail::explicitTracing()) {
		tracing::getActiveTracer()->traceBreak();
		return;
	}
#endif
	throw cf_detail::BreakSignal {};
}

/**
 * @brief Skip to the next iteration of the innermost explicit For/While loop.
 *
 * For `While` this re-evaluates the condition; for `For` it runs the step first (as C's `continue` does).
 */
inline void Continue() {
#ifdef ENABLE_TRACING
	if (cf_detail::explicitTracing()) {
		tracing::getActiveTracer()->traceContinue();
		return;
	}
#endif
	throw cf_detail::ContinueSignal {};
}

/**
 * @brief Return @p value from the traced function out of an explicit construct.
 *
 * The caller still needs its own `return` statement after the construct, for C++; it is not recorded after a
 * `Return`. Only the NIR tracer records it; without it the call throws.
 */
template <typename T>
void Return(const val<T>& value) {
#ifdef ENABLE_TRACING
	if (cf_detail::explicitTracing()) {
		tracing::getActiveTracer()->traceExplicitReturn(tracing::TypeResolver<T>::to_type(),
		                                                details::StateResolver<const val<T>&>::getState(value));
		return;
	}
#endif
	(void) value;
	throw std::logic_error("Return() is only supported while tracing with the NIR tracer");
}

/// Return from a traced function without a value.
inline void Return() {
#ifdef ENABLE_TRACING
	if (cf_detail::explicitTracing()) {
		tracing::getActiveTracer()->traceExplicitReturn(Type::v, tracing::TypedValueRef());
		return;
	}
#endif
	throw std::logic_error("Return() is only supported while tracing with the NIR tracer");
}

/**
 * @brief Explicit while loop.
 *
 * Emits a header, a body and a back edge; loop-carried values that @p bodyFn assigns become block arguments in the
 * conversion, as in an implicit loop.
 *
 * @param condFn Callable returning a fresh `val<bool>` for the loop condition.
 * @param bodyFn Callable emitting the loop body.
 */
template <typename CondFn, typename BodyFn>
void While(CondFn&& condFn, BodyFn&& bodyFn) {
	using NoStep = void (*)();
	NoStep* none = nullptr;
	cf_detail::loop(condFn, none, bodyFn);
}

/**
 * @brief Explicit counted for loop: iterates @p i over [begin, end) with the given @p step (ascending). For descending
 * or custom conditions, use the three-clause `For` overload.
 */
template <typename T, typename BodyFn>
void For(val<T> begin, val<T> end, val<T> step, BodyFn&& bodyFn) {
	val<T> i = begin;
	auto condFn = [&]() {
		return i < end;
	};
	auto stepFn = [&]() {
		i = i + step;
	};
	auto body = [&]() {
		bodyFn(i);
	};
	cf_detail::loop(condFn, &stepFn, body);
}

/// Explicit counted for loop with unit stride: iterates @p i over [begin, end).
template <typename T, typename BodyFn>
void For(val<T> begin, val<T> end, BodyFn&& bodyFn) {
	For(begin, end, val<T>(1), std::forward<BodyFn>(bodyFn));
}

/**
 * @brief Explicit three-clause for loop: `for (init; cond; step) body`.
 * @param init Callable run once before the loop.
 * @param cond Callable returning a fresh `val<bool>` for the loop condition.
 * @param step Callable run after each iteration (and on Continue()).
 * @param body Callable emitting the loop body.
 */
template <typename InitFn, typename CondFn, typename StepFn, typename BodyFn>
void For(InitFn&& init, CondFn&& cond, StepFn&& step, BodyFn&& body) {
	init();
	cf_detail::loop(cond, &step, body);
}

} // namespace nautilus
