#pragma once

#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/val.hpp"

namespace nautilus {

/**
 * @brief Select operation - chooses one value based on a boolean condition.
 *
 * This operation behaves like LLVM's select or MLIR's arith.select:
 * - If condition is true, returns true_value
 * - If condition is false, returns false_value
 *
 * This is implemented as a proper IR operation that can be more efficient than branching
 * for simple value selection, as it can be compiled to conditional move instructions
 * rather than control flow branches.
 *
 * Works with any val<T> type (integers, floats, booleans, pointers, etc.)
 *
 * @param condition Boolean condition to test
 * @param true_value Value to return if condition is true
 * @param false_value Value to return if condition is false
 * @return The selected value (true_value if condition is true, false_value otherwise)
 */
template <typename T>
    requires is_arithmetic<T>
val<T> select(val<bool> condition, val<T> true_value, val<T> false_value) {
	if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
		auto resultRef = tracing::traceTernaryOp(tracing::SELECT, tracing::TypeResolver<T>::to_type(),
		                                         details::StateResolver<const val<bool>&>::getState(condition),
		                                         details::StateResolver<const val<T>&>::getState(true_value),
		                                         details::StateResolver<const val<T>&>::getState(false_value));
		return val<T>(resultRef);
#endif
	}
	// Non-tracing path: just use ternary operator
	return val<T>(details::RawValueResolver<bool>::getRawValue(condition)
	                  ? details::RawValueResolver<T>::getRawValue(true_value)
	                  : details::RawValueResolver<T>::getRawValue(false_value));
}

/**
 * @brief Select operation for pointer types.
 *
 * Selects between two pointer values based on a boolean condition.
 * Works with any pointer type val<T*>.
 *
 * @param condition Boolean condition to test
 * @param true_value Pointer value to return if condition is true
 * @param false_value Pointer value to return if condition is false
 * @return The selected pointer value
 */
template <typename T>
    requires std::is_pointer_v<T>
val<T> select(val<bool> condition, val<T> true_value, val<T> false_value) {
	if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
		auto resultRef = tracing::traceTernaryOp(tracing::SELECT, tracing::TypeResolver<T>::to_type(),
		                                         details::StateResolver<const val<bool>&>::getState(condition),
		                                         details::StateResolver<const val<T>&>::getState(true_value),
		                                         details::StateResolver<const val<T>&>::getState(false_value));
		return val<T>(resultRef);
#endif
	}
	// Non-tracing path: just use ternary operator
	return val<T>(details::RawValueResolver<bool>::getRawValue(condition)
	                  ? details::RawValueResolver<T>::getRawValue(true_value)
	                  : details::RawValueResolver<T>::getRawValue(false_value));
}

} // namespace nautilus
