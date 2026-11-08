#pragma once

#include "nautilus/static.hpp"
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

/// The type C++ converts a literal operand from: a `static_val<T>` is a value C++ knows, exactly like a literal of
/// type `T` (it converts through `operator T()`), and any other operand is its own type.
template <typename T>
struct literal_raw {
	using type = T;
};
template <typename T>
struct literal_raw<static_val<T>> {
	using type = T;
};
template <typename T>
using literal_raw_t = typename literal_raw<std::remove_cvref_t<T>>::type;

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
	} else if constexpr (!std::is_same_v<details::literal_raw_t<Type>, Type>) {
		// a static_val<T> is a val<T>, not a val<int>: converting it to int would truncate a wider type
		using Raw = details::literal_raw_t<Type>;
		return val<Raw>(static_cast<Raw>(value));
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

/// Converts val<LeftType> to exactly val<To> if it is not one yet, narrowing included. cast_value only widens (it
/// converts to the common type of both), which is not what a shift needs for its amount: the amount takes the type of
/// the shifted value, whatever its own width.
template <typename LeftType, is_fundamental To>
decltype(auto) inline convert_value(LeftType&& value) {
	using basic_type = typename std::remove_reference_t<LeftType>::basic_type;
	if constexpr (tracing::same_nautilus_type<basic_type, To>::value) {
		return std::forward<LeftType>(value);
	} else {
		return static_cast<val<To>>(value);
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
