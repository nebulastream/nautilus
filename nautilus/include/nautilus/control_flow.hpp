#pragma once

#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/val.hpp"

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
 *   For(val<int32_t>(0), n, [&](val<int32_t> i){ sum = sum + i; });
 *   While([&]{ return a != b; }, [&]{ ... });
 * @endcode
 *
 * Each construct emits the corresponding CMP/JMP blocks directly, in a single
 * trace pass, and never enqueues a symbolic path. A function whose control flow
 * is expressed entirely with these constructs therefore traces in exactly one
 * iteration regardless of how many (or how deeply nested) the branches are.
 *
 * Branches are side-effecting: bodies mutate captured `val<T>` variables (whose
 * assignments are reconciled across blocks by the SSA phase, exactly as for the
 * implicit path). To *select a value* based on a condition, prefer
 * `select(cond, a, b)` from `nautilus/select.hpp`.
 *
 * @warning Do not mix these explicit constructs with implicit native control
 * flow (a `val<bool>` used in a native `if`/`for`/`while`) in the same traced
 * function. Implicit branches trigger symbolic re-execution, which is detected
 * and rejected with a clear error. Express *all* of a function's control flow
 * with explicit constructs — they compose freely and arbitrarily nest among
 * themselves.
 */

namespace nautilus {

/**
 * @brief Explicit if-then (no else).
 * @param condition Branch condition.
 * @param thenBody Callable invoked to emit the taken branch.
 */
template <typename ThenFn>
void If(val<bool> condition, ThenFn&& thenBody) {
	if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
		auto blocks = tracing::emitExplicitCmp(details::StateResolver<const val<bool>&>::getState(condition),
		                                       condition.getIsTrueProbability());
		tracing::switchToBlock(blocks.trueBlock);
		thenBody();
		auto thenExit = tracing::currentBlock();
		auto merge = tracing::openMergeBlock();
		tracing::jumpTo(thenExit, merge);
		tracing::jumpTo(blocks.falseBlock, merge);
		tracing::switchToBlock(merge);
		return;
#endif
	}
	if (details::RawValueResolver<bool>::getRawValue(condition)) {
		thenBody();
	}
}

/**
 * @brief Explicit if-then-else.
 * @param condition Branch condition.
 * @param thenBody Callable invoked to emit the true branch.
 * @param elseBody Callable invoked to emit the false branch.
 */
template <typename ThenFn, typename ElseFn>
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
 * @brief Explicit while loop.
 *
 * Emits a real cyclic CFG (a loop header with the preheader and the loop-body
 * back-edge as predecessors), so the loop-carried values mutated in @p bodyFn are
 * reconciled by the SSA phase via the back-edge — just like an implicit loop.
 *
 * @param condFn Callable returning a fresh `val<bool>` for the loop condition.
 * @param bodyFn Callable invoked to emit the loop body.
 */
template <typename CondFn, typename BodyFn>
void While(CondFn&& condFn, BodyFn&& bodyFn) {
	if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
		auto preheader = tracing::currentBlock();
		auto header = tracing::openMergeBlock();
		tracing::jumpTo(preheader, header);
		tracing::switchToBlock(header);
		val<bool> condition = condFn();
		auto blocks = tracing::emitExplicitCmp(details::StateResolver<const val<bool>&>::getState(condition),
		                                       condition.getIsTrueProbability());
		tracing::switchToBlock(blocks.trueBlock);
		bodyFn();
		auto bodyExit = tracing::currentBlock();
		tracing::jumpTo(bodyExit, header);
		tracing::switchToBlock(blocks.falseBlock);
		return;
#endif
	}
	while (details::RawValueResolver<bool>::getRawValue(condFn())) {
		bodyFn();
	}
}

/**
 * @brief Explicit counted for loop: iterates @p i over [begin, end).
 * @param begin Inclusive lower bound.
 * @param end Exclusive upper bound.
 * @param bodyFn Callable invoked with the current induction value `val<T>`.
 */
template <typename T, typename BodyFn>
void For(val<T> begin, val<T> end, BodyFn&& bodyFn) {
	val<T> i = begin;
	While([&]() { return i < end; },
	      [&]() {
		      bodyFn(i);
		      i = i + val<T>(1);
	      });
}

/**
 * @brief Explicit three-clause for loop: `for (init; cond; step) body`.
 * @param init Callable run once before the loop.
 * @param cond Callable returning a fresh `val<bool>` for the loop condition.
 * @param step Callable run after each iteration of @p body.
 * @param body Callable invoked to emit the loop body.
 */
template <typename InitFn, typename CondFn, typename StepFn, typename BodyFn>
void For(InitFn&& init, CondFn&& cond, StepFn&& step, BodyFn&& body) {
	init();
	While(std::forward<CondFn>(cond), [&]() {
		body();
		step();
	});
}

} // namespace nautilus
