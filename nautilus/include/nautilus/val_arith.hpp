#pragma once

#include "nautilus/val_concepts.hpp"
#include "nautilus/val_details.hpp"
#include <concepts>
#include <type_traits>
#include <utility>

namespace nautilus {

/// Generic val<T> template specialization for arithmetic types (integer, float, but NOT bool).
///
/// This template wraps arithmetic values to track their operations during execution,
/// enabling JIT compilation with optimization. It provides:
///
/// 1. **Transparent Arithmetic Operations**: All standard operators (+, -, *, /, %, <<, >>)
///    with automatic type promotion and tracing.
///
/// 2. **Comparison Operations**: Equality and relational comparisons (==, !=, <, >, <=, >=)
///    with proper bool result type.
///
/// 3. **Bitwise Operations**: Bitwise AND, OR, XOR for integral types with automatic tracing.
///
/// 4. **Type Conversion**: Implicit conversion to underlying type and explicit casts between
///    compatible arithmetic types.
///
/// 5. **Prefix and Postfix Operators**: Support for ++ and -- operators with proper semantics.
///
/// # Template Parameters
/// - `ValueType`: Must satisfy `is_arithmetic` concept (integral or floating-point, NOT bool).
///
/// # Tracing Behavior
/// When `ENABLE_TRACING` is enabled, all operations are recorded in the execution trace,
/// allowing the JIT compiler to analyze and optimize the computation.
///
/// # Example
/// ```cpp
/// Engine engine;
/// auto result = engine.Compile<int64_t(val<int64_t>)>(
///     [](val<int64_t> x) {
///         val<int64_t> y = 10;
///         return x + y;  // Operation is traced
///     }
/// );
/// ```
///
/// @note Boolean values use a specialized `val<bool>` template (see val_bool.hpp)
///       with additional probability tracking for branch prediction.
///
/// @see val_bool.hpp for the bool specialization
/// @see val_details.hpp for implementation utilities

// Partial specialization for arithmetic types only
template <typename ValueType>
    requires is_arithmetic<ValueType>
class val<ValueType> {
public:
	/// The raw type stored by this value wrapper
	using raw_type = ValueType;

	/// The basic underlying type
	using basic_type = ValueType;

#ifdef ENABLE_TRACING
	/// Default constructor. Initializes with constant 0 and records in trace.
	val() : state(tracing::traceConstant<raw_type>(0)), value(0) {
	}

	/// Value constructor. Initializes with provided value and records in trace.
	val(ValueType value) : state(tracing::traceConstant(value)), value(value) {
	}

	/// Copy constructor. Copies value and tracing state.
	val(const val<ValueType>& other) : state(tracing::traceCopy(other.state)), value(other.value) {
	}

	/// Move constructor. Moves value and tracing state.
	val(val<ValueType>&& other) noexcept : state(std::move(other.state)), value(std::move(other.value)) {
	}

	/// Tracing constructor. Creates from TypedValueRef for internal use.
	val(tracing::TypedValueRef& tc) : state(tc), value() {
	}
#else
	/// Default constructor. Initializes with 0.
	val() : value(0) {
	}

	/// Value constructor. Initializes with provided value.
	val(ValueType value) : value(value) {
	}

	/// Copy constructor. Copies the value.
	val(const val<ValueType>& other) : value(other.value) {
	}

	/// Move constructor. Moves the value.
	val(val<ValueType>&& other) noexcept : value(std::move(other.value)) {
	}
#endif

	/// Assignment operator. Assigns value and traces if enabled.
	val<ValueType>& operator=(const val<ValueType>& other) {
#ifdef ENABLE_TRACING
		tracing::traceAssignment(state, other.state, tracing::TypeResolver<ValueType>::to_type());
#endif
		this->value = other.value;
		return *this;
	}

	/// Type conversion operator. Casts to compatible arithmetic types with tracing.
	template <typename OtherType>
	    requires std::is_convertible_v<ValueType, OtherType>
	operator val<OtherType>() const {
		if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
			auto resultRef = tracing::traceUnaryOp(tracing::CAST, tracing::TypeResolver<OtherType>::to_type(), state);
			return val<OtherType>(resultRef);
#endif
		}
		return val<OtherType>(value);
	}

	/// Prefix increment operator.
	const val<ValueType>& operator++() {
		*this = *this + (ValueType) 1;
		return *this;
	}

	/// Postfix increment operator.
	val<ValueType> operator++(int) {
		auto temp = *this;
		*this = *this + (ValueType) 1;
		return temp;
	}

	/// Unary plus operator. Performs integral promotion if needed.
	auto operator+() {
		if constexpr (std::integral<ValueType> && sizeof(ValueType) < sizeof(int)) {
			return static_cast<val<int>>(*this);
		}
		return *this;
	}

	/// Unary negation operator.
	val<ValueType> operator-() {
		return (ValueType) 0 - *this;
	}

	/// Prefix decrement operator.
	const val<ValueType>& operator--() {
		*this = *this - (ValueType) 1;
		return *this;
	}

	/// Postfix decrement operator.
	val<ValueType> operator--(int) {
		auto temp = *this;
		*this = *this - (ValueType) 1;
		return temp;
	}

#ifdef ENABLE_TRACING
	/// Holds the tracing state for this value when tracing is enabled
	const tracing::TypedValueRefHolder state;
#endif

private:
	template <typename>
	friend struct details::RawValueResolver;

	/// The underlying arithmetic value
	ValueType value;
};

// ============================================================================
// Arithmetic Operators for val<arithmetic types>
// ============================================================================

namespace details {

/// Bitwise negation (NOT) operation for integral types
template <is_integral LHS>
val<LHS> neg(const val<LHS>& val) {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto tc = tracing::traceUnaryOp(tracing::NEGATE, tracing::TypeResolver<LHS>::to_type(), val.state);
		return tc;
	}
#endif
	return ~RawValueResolver<LHS>::getRawValue(val);
}

// Type helper for integer promotion rules
template <typename LHS, typename RHS>
using ArithmeticResultType = std::common_type_t<decltype(+std::declval<LHS>()), decltype(+std::declval<RHS>())>;

/// Binary operator helper for arithmetic operations with integer promotion
#define DEFINE_BINARY_OPERATOR_HELPER_WITH_PROMOTION(OP, OP_NAME, OP_TRACE)                                            \
	template <typename LHS, typename RHS>                                                                              \
	auto inline OP_NAME(LHS&& left, RHS&& right) {                                                                     \
		using LHSVal = std::remove_cvref_t<LHS>;                                                                       \
		using RHSVal = std::remove_cvref_t<RHS>;                                                                       \
		using LBase = typename LHSVal::basic_type;                                                                     \
		using RBase = typename RHSVal::basic_type;                                                                     \
		using commonType = ArithmeticResultType<LBase, RBase>;                                                         \
		using resultType = decltype(std::declval<LBase>() OP std::declval<RBase>());                                   \
                                                                                                                       \
		auto&& lValue = cast_value<LHS, commonType>(std::forward<LHS>(left));                                          \
		auto&& rValue = cast_value<RHS, commonType>(std::forward<RHS>(right));                                         \
                                                                                                                       \
		if SHOULD_TRACE () {                                                                                           \
			auto tc = tracing::traceBinaryOp(tracing::OP_TRACE, tracing::TypeResolver<resultType>::to_type(),          \
			                                 StateResolver<decltype(lValue)>::getState(lValue),                        \
			                                 StateResolver<decltype(rValue)>::getState(rValue));                       \
			return val<resultType>(tc);                                                                                \
		}                                                                                                              \
                                                                                                                       \
		return val<resultType>(RawValueResolver<resultType>::getRawValue(std::forward<decltype(lValue)>(               \
		    lValue)) OP RawValueResolver<resultType>::getRawValue(std::forward<decltype(rValue)>(rValue)));            \
	}

DEFINE_BINARY_OPERATOR_HELPER_WITH_PROMOTION(+, add, ADD)

DEFINE_BINARY_OPERATOR_HELPER_WITH_PROMOTION(-, sub, SUB)

DEFINE_BINARY_OPERATOR_HELPER_WITH_PROMOTION(*, mul, MUL)

DEFINE_BINARY_OPERATOR_HELPER_WITH_PROMOTION(/, div, DIV)

DEFINE_BINARY_OPERATOR_HELPER_WITH_PROMOTION(%, mod, MOD)

DEFINE_BINARY_OPERATOR_HELPER_WITH_PROMOTION(>>, shr, RSH)

DEFINE_BINARY_OPERATOR_HELPER_WITH_PROMOTION(<<, shl, LSH)

/// Binary operator helper for comparison operations (no promotion)
#define DEFINE_BINARY_OPERATOR_HELPER(OP, OP_NAME, OP_TRACE, RES_TYPE)                                                 \
	template <typename LHS, typename RHS>                                                                              \
	auto inline OP_NAME(LHS&& left, RHS&& right) {                                                                     \
		using LHSVal = std::remove_cvref_t<LHS>;                                                                       \
		using RHSVal = std::remove_cvref_t<RHS>;                                                                       \
		using LBase = typename LHSVal::basic_type;                                                                     \
		using RBase = typename RHSVal::basic_type;                                                                     \
		using commonType = std::common_type_t<LBase, RBase>;                                                           \
                                                                                                                       \
		auto&& lValue = cast_value<LHS, commonType>(std::forward<LHS>(left));                                          \
		auto&& rValue = cast_value<RHS, commonType>(std::forward<RHS>(right));                                         \
                                                                                                                       \
		if SHOULD_TRACE () {                                                                                           \
			auto tc = tracing::traceBinaryOp(tracing::OP_TRACE, tracing::TypeResolver<RES_TYPE>::to_type(),            \
			                                 StateResolver<decltype(lValue)>::getState(lValue),                        \
			                                 StateResolver<decltype(rValue)>::getState(rValue));                       \
			return val<RES_TYPE>(tc);                                                                                  \
		}                                                                                                              \
                                                                                                                       \
		return val<RES_TYPE>(RawValueResolver<commonType>::getRawValue(std::forward<decltype(lValue)>(                 \
		    lValue)) OP RawValueResolver<commonType>::getRawValue(std::forward<decltype(rValue)>(rValue)));            \
	}

DEFINE_BINARY_OPERATOR_HELPER(==, eq, EQ, bool)

DEFINE_BINARY_OPERATOR_HELPER(!=, neq, NEQ, bool)

DEFINE_BINARY_OPERATOR_HELPER(<, lt, LT, bool)

DEFINE_BINARY_OPERATOR_HELPER(<=, lte, LTE, bool)

DEFINE_BINARY_OPERATOR_HELPER(>, gt, GT, bool)

DEFINE_BINARY_OPERATOR_HELPER(>=, gte, GTE, bool)

DEFINE_BINARY_OPERATOR_HELPER(&, bAnd, BAND, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(|, bOr, BOR, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(^, bXOr, BXOR, COMMON_RETURN_TYPE)

} // namespace details

// Operator overloads for arithmetic types
#define DEFINE_ARITHMETIC_OPERATOR(OP, FUNC, CATEGORY)                                                                 \
	template <typename LHS, typename RHS>                                                                              \
	    requires(is_##CATEGORY##_val<LHS> && is_##CATEGORY##_val<RHS>)                                                 \
	auto inline operator OP(LHS&& left, RHS&& right) {                                                                 \
		return details::FUNC(std::forward<LHS>(left), std::forward<RHS>(right));                                       \
	}                                                                                                                  \
                                                                                                                       \
	template <typename LHS, typename RHS>                                                                              \
	    requires(is_##CATEGORY##_val<LHS> && convertible_to_##CATEGORY<RHS>)                                           \
	auto inline operator OP(LHS&& left, RHS&& right) {                                                                 \
		auto&& rhsV = make_value(std::forward<RHS>(right));                                                            \
		return details::FUNC(std::forward<LHS>(left), std::forward<decltype(rhsV)>(rhsV));                             \
	}                                                                                                                  \
                                                                                                                       \
	template <typename LHS, typename RHS>                                                                              \
	    requires(convertible_to_##CATEGORY<LHS> && is_##CATEGORY##_val<RHS>)                                           \
	auto inline operator OP(LHS&& left, RHS&& right) {                                                                 \
		auto&& lhsV = make_value(std::forward<LHS>(left));                                                             \
		return details::FUNC(std::forward<decltype(lhsV)>(lhsV), std::forward<RHS>(right));                            \
	}                                                                                                                  \
                                                                                                                       \
	template <typename LHS, typename RHS>                                                                              \
	    requires(is_##CATEGORY##_val<LHS> && !is_##CATEGORY##_val<RHS> &&                                              \
	             !std::is_##CATEGORY##_v<std::remove_cvref_t<RHS>> && convertible_to_##CATEGORY##_val<RHS, LHS>)       \
	auto inline operator OP(LHS&& left, RHS&& right) {                                                                 \
		auto rV = static_cast<std::remove_cvref_t<LHS>>(right);                                                        \
		return details::FUNC(std::forward<LHS>(left), std::forward<decltype(rV)>(rV));                                 \
	}

DEFINE_ARITHMETIC_OPERATOR(+, add, fundamental)
DEFINE_ARITHMETIC_OPERATOR(-, sub, fundamental)
DEFINE_ARITHMETIC_OPERATOR(*, mul, fundamental)
DEFINE_ARITHMETIC_OPERATOR(/, div, fundamental)
DEFINE_ARITHMETIC_OPERATOR(%, mod, integral)
DEFINE_ARITHMETIC_OPERATOR(==, eq, fundamental)
DEFINE_ARITHMETIC_OPERATOR(!=, neq, fundamental)
DEFINE_ARITHMETIC_OPERATOR(>, gt, fundamental)
DEFINE_ARITHMETIC_OPERATOR(>=, gte, fundamental)
DEFINE_ARITHMETIC_OPERATOR(<, lt, fundamental)
DEFINE_ARITHMETIC_OPERATOR(<=, lte, fundamental)
DEFINE_ARITHMETIC_OPERATOR(>>, shr, integral)
DEFINE_ARITHMETIC_OPERATOR(<<, shl, integral)
DEFINE_ARITHMETIC_OPERATOR(|, bOr, integral)
DEFINE_ARITHMETIC_OPERATOR(&, bAnd, integral)
DEFINE_ARITHMETIC_OPERATOR(^, bXOr, integral)

// Compound assignment operators for arithmetic types
template <typename LHS, typename RHS>
auto& operator+=(val<LHS>& left, RHS right) {
	left = left + right;
	return left;
}

template <typename LHS, typename RHS>
auto& operator-=(val<LHS>& left, RHS right) {
	left = left - right;
	return left;
}

template <typename LHS, typename RHS>
auto& operator*=(val<LHS>& left, RHS right) {
	left = left * right;
	return left;
}

template <typename LHS, typename RHS>
auto& operator/=(val<LHS>& left, RHS right) {
	left = left / right;
	return left;
}

template <typename LHS, typename RHS>
auto& operator%=(val<LHS>& left, RHS right) {
	left = left % right;
	return left;
}

template <typename LHS, typename RHS>
    requires(is_integral<LHS> && (is_integral_val<RHS> || convertible_to_integral<RHS>) )
auto& operator|=(val<LHS>& left, RHS right) {
	left = left | right;
	return left;
}

template <typename LHS, typename RHS>
    requires(is_integral<LHS> && (is_integral_val<RHS> || convertible_to_integral<RHS>) )
auto& operator^=(val<LHS>& left, RHS right) {
	left = left ^ right;
	return left;
}

template <typename LHS, typename RHS>
    requires(is_integral<LHS> && (is_integral_val<RHS> || convertible_to_integral<RHS>) )
auto& operator&=(val<LHS>& left, RHS right) {
	left = left & right;
	return left;
}

template <typename LHS, typename RHS>
    requires(is_integral<LHS> && (is_integral_val<RHS> || convertible_to_integral<RHS>) )
auto& operator<<=(val<LHS>& left, RHS right) {
	left = left << right;
	return left;
}

template <typename LHS, typename RHS>
    requires(is_integral<LHS> && (is_integral_val<RHS> || convertible_to_integral<RHS>) )
auto& operator>>=(val<LHS>& left, RHS right) {
	left = left >> right;
	return left;
}

// Bitwise NOT for integral types
template <typename LHS>
auto inline operator~(LHS left) {
	return details::neg(left);
}

} // namespace nautilus
