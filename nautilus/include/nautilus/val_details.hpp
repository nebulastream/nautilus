#pragma once

#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/val_concepts.hpp"
#include <type_traits>

namespace nautilus {

// Forward declaration of val template
template <typename T>
class val;

namespace details {

/// Helper struct to extract raw values from val<T> wrappers.
///
/// This allows access to the underlying value stored in a val<T> instance.
/// Specialized for each val type to handle its value extraction.
template <typename LHS>
struct RawValueResolver {
	/// Extracts the raw value from a val<LHS> wrapper
	static LHS inline getRawValue(const val<LHS>& val) {
		return val.value;
	}
};

/// Helper macros for type deduction in binary operations
#define COMMON_RETURN_TYPE                                                                                             \
	typename std::common_type_t<typename std::remove_cvref_t<LHS>::basic_type,                                         \
	                            typename std::remove_cvref_t<RHS>::basic_type>

#define DEDUCT_RETURN_TYPE(OP)                                                                                         \
	decltype(RawValueResolver<typename std::remove_cvref_t<decltype(left)>::raw_type>::getRawValue(left)               \
	             OP RawValueResolver<typename std::remove_cvref_t<decltype(right)>::raw_type>::getRawValue(right))

/// Helper struct to extract tracing state from val<T> wrappers.
///
/// When tracing is enabled, val<T> instances carry state information in the
/// TypedValueRefHolder. This resolver provides uniform access to that state.
template <typename T>
struct StateResolver {
	/// Extracts the tracing state from a val<T> wrapper
	template <typename U = T>
	static tracing::TypedValueRef getState(U&& value) {
		return value.state;
	}
};

#ifdef ENABLE_TRACING
/// Helper struct to check if a val<T> holds a trace-time constant.
///
/// During tracing, val<T> instances are registered as constants on construction
/// from literals and unregistered when assigned to or destroyed. This allows
/// constant folding during tracing — when both operands of a binary operation
/// are constants, the result can be computed immediately and emitted as a single
/// CONST operation. The tracking is managed externally in the trace context
/// (similar to static_val), so constants that become non-const at runtime
/// (e.g. via assignment in a loop) are correctly handled.
template <typename T>
struct ConstResolver {
	template <typename U = T>
	static bool isConst(U&& value) {
		return tracing::isConstVal(&value);
	}
};
#endif

} // namespace details

/// Converts fundamental types to val<T> wrappers, or returns val<T> as-is.
///
/// This utility function provides transparent conversion to val<T> types,
/// allowing operators to work seamlessly with mixed val<T> and raw types.
template <is_fundamental_val Type>
decltype(auto) inline make_value(Type&& value) {
	return std::forward<Type>(value);
}

/// Converts a raw function pointer to a val<FuncPtr> wrapper.
template <typename Type>
    requires is_function_ptr<Type>
decltype(auto) inline make_value(Type value) {
	return val<Type>(value);
}

/// Converts convertible types to val<T> wrappers.
///
/// Handles conversion of fundamental types, pointers, and enums to appropriate
/// val<T> specializations for use in operations.
template <convertible_to_fundamental Type>
decltype(auto) inline make_value(const Type& value) {
	if constexpr (std::is_fundamental_v<Type>) {
		return val<Type>(value);
	} else if constexpr (is_ptr<Type>) {
		return val<Type>(value);
	} else if constexpr (is_fundamental_ptr<Type>) {
		return val<Type>(value);
	} else if constexpr (std::is_enum_v<Type>) {
		return val<Type>(value);
	} else if constexpr (std::is_convertible_v<Type, int>) {
		return val<int>(value);
	}
}

/// Casts val<LeftType> to a compatible val<RightType> if needed.
///
/// Used internally by operators to ensure consistent types for binary operations.
/// If types match, returns the value as-is. Otherwise, casts to common type.
template <typename LeftType, is_fundamental RightType>
decltype(auto) inline cast_value(LeftType&& value) {
	using basic_type = typename std::remove_reference_t<LeftType>::basic_type;
	using commonType = std::common_type_t<basic_type, RightType>;

	if constexpr (tracing::same_nautilus_type<basic_type, RightType>::value) {
		return std::forward<LeftType>(value);
	} else {
		return static_cast<val<commonType>>(value);
	}
}

/// Specialization of cast_value for bool types (no conversion needed).
template <typename LeftType, is_bool RightType>
auto&& cast_value(LeftType&& value) {
	return std::forward<LeftType>(value);
}

/// Specialization of cast_value for enum types (no conversion needed).
template <typename LeftType, is_enum RightType>
auto&& cast_value(LeftType&& value) {
	return std::forward<LeftType>(value);
}

} // namespace nautilus
