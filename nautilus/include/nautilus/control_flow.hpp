#pragma once

#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/val.hpp"
#include <functional>
#include <type_traits>

/**
 * @file control_flow.hpp
 * @brief Explicit, closure-style control-flow constructs for Nautilus.
 *
 * Nautilus normally traces native C++ control flow (`if`, `for`, `while` over a
 * `val<bool>`) via exception-based symbolic execution: the traced function is
 * re-run once per path, which is O(2^N) in the number of branches. The
 * constructs in this header instead let you state control flow explicitly:
 *
 * @code
 *   If(x == 42, [&]{ r = r + 1; }, [&]{ r = r + 2; });
 *   val<int32_t> m = If(a > b, [&]{ return a; }, [&]{ return b; }); // value-yielding
 *   For(val<int32_t>(0), n, [&](val<int32_t> i){ sum = sum + i; });
 *   While([&]{ return a != b; }, [&]{ If(a > b, [&]{ Break(); }); ... });
 * @endcode
 *
 * Each construct emits the corresponding CMP/JMP blocks directly, in a single
 * trace pass, and never enqueues a symbolic path. A function whose control flow
 * is expressed entirely with these constructs therefore traces in exactly one
 * iteration regardless of how many (or how deeply nested) the branches are.
 *
 * `Break()` / `Continue()` provide early loop exit; `If` additionally has a
 * value-yielding overload (when the arms return a `val<T>`) built on a merge
 * block argument, so only the taken arm's value is materialised at runtime
 * (unlike `select(cond, a, b)`, which evaluates both).
 *
 * @warning Do not mix these explicit constructs with implicit native control
 * flow (a `val<bool>` used in a native `if`/`for`/`while`) in the same traced
 * function — it is detected and rejected with a clear error. Express *all* of a
 * function's control flow with explicit constructs; they compose freely and
 * arbitrarily nest among themselves.
 *
 * @warning When generating constructs from a host (plain C++) loop, make the
 * loop counter a `static_val<T>` (as `static_iterable` loops do). The explicit
 * constructs themselves are collision-free, but the `val<T>` operations inside
 * their *bodies* are traced through the normal snapshot machinery; if those body
 * operations sit on the same source line across host-loop iterations their
 * snapshots alias and corrupt the trace. A `static_val` counter gives each
 * unrolled iteration a distinct snapshot:
 * @code
 *   for (static_val<int32_t> k = 0; k < n; ++k) {        // NOT a plain int k
 *       If(x > val<int32_t>((int32_t) k), [&]{ r = r + 1; });
 *   }
 * @endcode
 */

namespace nautilus {

namespace cf_detail {
/// Thrown by Break()/Continue() in the non-tracing (interpreter) path so the
/// enclosing While/For can unwind out of the body lambda — mirroring the CFG
/// jump emitted in the tracing path, so both paths agree on what runs after a
/// break/continue.
struct BreakSignal {};
struct ContinueSignal {};

template <typename CondFn>
inline tracing::TypedValueRef condState(CondFn&& condFn, double& probabilityOut) {
#ifdef ENABLE_TRACING
	val<bool> condition = condFn();
	probabilityOut = condition.getIsTrueProbability();
	return details::StateResolver<const val<bool>&>::getState(condition);
#else
	(void) condFn;
	(void) probabilityOut;
	return {};
#endif
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
void If(val<bool> condition, ThenFn&& thenBody, ElseFn&& elseBody) {
	if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
		auto blocks = tracing::emitExplicitCmp(details::StateResolver<const val<bool>&>::getState(condition),
		                                       condition.getIsTrueProbability());
		tracing::switchToBlock(blocks.trueBlock);
		thenBody();
		auto thenExit = tracing::currentBlock();
		tracing::switchToBlock(blocks.falseBlock);
		elseBody();
		auto elseExit = tracing::currentBlock();
		auto merge = tracing::openMergeBlock();
		tracing::jumpTo(thenExit, merge);
		tracing::jumpTo(elseExit, merge);
		tracing::switchToBlock(merge);
		return;
#endif
	}
	if (details::RawValueResolver<bool>::getRawValue(condition)) {
		thenBody();
	} else {
		elseBody();
	}
}

/**
 * @brief Explicit if-then (no else): a degenerate if-then-else with an empty
 * else arm.
 */
template <typename ThenFn>
void If(val<bool> condition, ThenFn&& thenBody) {
	If(condition, std::forward<ThenFn>(thenBody), []() {});
}

/**
 * @brief Value-yielding if-then-else: returns the taken arm's `val<T>`.
 *
 * Selected when the arms return a (non-void) `val<T>`. Only the taken arm is
 * evaluated at runtime; the result is reconciled at the merge as a block
 * argument. Prefer `select(cond, a, b)` when both values are cheap and you want
 * a branch-free conditional-move.
 */
template <typename ThenFn, typename ElseFn>
    requires(!std::is_void_v<std::invoke_result_t<ThenFn>>)
auto If(val<bool> condition, ThenFn&& thenBody, ElseFn&& elseBody) -> std::invoke_result_t<ThenFn> {
	using Result = std::invoke_result_t<ThenFn>;
	Result result {};
	If(condition, [&]() { result = thenBody(); }, [&]() { result = elseBody(); });
	return result;
}

/**
 * @brief Break out of the innermost explicit For/While loop.
 *
 * In the tracing path this emits a jump to the loop's exit block; in the
 * interpreter it unwinds out of the body lambda. Code sequentially after a
 * Break() on the same path does not run (consistent across both paths).
 */
inline void Break() {
	if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
		tracing::breakLoop();
		return;
#endif
	}
	throw cf_detail::BreakSignal {};
}

/**
 * @brief Skip to the next iteration of the innermost explicit For/While loop.
 *
 * For `While` this re-evaluates the condition; for `For` it runs the step first
 * (matching C `for`-loop `continue` semantics).
 */
inline void Continue() {
	if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
		tracing::continueLoop();
		return;
#endif
	}
	throw cf_detail::ContinueSignal {};
}

/**
 * @brief Explicit while loop.
 *
 * Emits a real cyclic CFG (a loop header with the preheader and the loop-body
 * back-edge as predecessors), so loop-carried values mutated in @p bodyFn are
 * reconciled by the SSA phase via the back-edge — just like an implicit loop.
 *
 * @param condFn Callable returning a fresh `val<bool>` for the loop condition.
 * @param bodyFn Callable emitting the loop body.
 */
template <typename CondFn, typename BodyFn>
void While(CondFn&& condFn, BodyFn&& bodyFn) {
	if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
		auto preheader = tracing::currentBlock();
		auto header = tracing::openMergeBlock();
		tracing::jumpTo(preheader, header);
		tracing::switchToBlock(header);
		double probability = 0.5;
		auto conditionState = cf_detail::condState(condFn, probability);
		auto blocks = tracing::emitExplicitCmp(conditionState, probability);
		// Continue() re-evaluates the condition -> the continue target is the header.
		tracing::pushLoopFrame(header, blocks.falseBlock);
		tracing::switchToBlock(blocks.trueBlock);
		bodyFn();
		auto bodyExit = tracing::currentBlock();
		tracing::jumpTo(bodyExit, header);
		tracing::popLoopFrame();
		tracing::switchToBlock(blocks.falseBlock);
		return;
#endif
	}
	while (details::RawValueResolver<bool>::getRawValue(condFn())) {
		try {
			bodyFn();
		} catch (const cf_detail::BreakSignal&) {
			break;
		} catch (const cf_detail::ContinueSignal&) {
			continue;
		}
	}
}

namespace cf_detail {
/// Shared tracing-path emission for For loops: a header, body, dedicated step
/// block (the Continue() target, so `continue` runs the step), and exit.
template <typename CondFn, typename StepFn, typename BodyFn>
inline void emitForLoopTraced(CondFn&& condFn, StepFn&& stepFn, BodyFn&& bodyFn) {
#ifdef ENABLE_TRACING
	auto preheader = tracing::currentBlock();
	auto header = tracing::openMergeBlock();
	tracing::jumpTo(preheader, header);
	tracing::switchToBlock(header);
	double probability = 0.5;
	auto conditionState = condState(condFn, probability);
	auto blocks = tracing::emitExplicitCmp(conditionState, probability);
	auto stepBlock = tracing::openMergeBlock();
	tracing::pushLoopFrame(stepBlock, blocks.falseBlock);
	tracing::switchToBlock(blocks.trueBlock);
	bodyFn();
	auto bodyExit = tracing::currentBlock();
	tracing::jumpTo(bodyExit, stepBlock);
	tracing::popLoopFrame();
	tracing::switchToBlock(stepBlock);
	stepFn();
	tracing::jumpTo(stepBlock, header);
	tracing::switchToBlock(blocks.falseBlock);
#else
	(void) condFn;
	(void) stepFn;
	(void) bodyFn;
#endif
}
} // namespace cf_detail

/**
 * @brief Explicit counted for loop: iterates @p i over [begin, end) with the
 * given @p step (ascending). For descending or custom conditions, use the
 * three-clause `For` overload.
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
	if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
		cf_detail::emitForLoopTraced(condFn, stepFn, body);
		return;
#endif
	}
	for (; details::RawValueResolver<bool>::getRawValue(condFn()); stepFn()) {
		try {
			body();
		} catch (const cf_detail::BreakSignal&) {
			break;
		} catch (const cf_detail::ContinueSignal&) {
			continue;
		}
	}
}

/**
 * @brief Explicit counted for loop with unit stride: iterates @p i over [begin, end).
 */
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
	if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
		cf_detail::emitForLoopTraced(cond, step, body);
		return;
#endif
	}
	for (; details::RawValueResolver<bool>::getRawValue(cond()); step()) {
		try {
			body();
		} catch (const cf_detail::BreakSignal&) {
			break;
		} catch (const cf_detail::ContinueSignal&) {
			continue;
		}
	}
}

} // namespace nautilus
