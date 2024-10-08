#pragma once

#include "nautilus/static.hpp"
#include "nautilus/val_concepts.hpp"
#include <iostream>
#include <memory>

#ifdef ENABLE_TRACING

#include "nautilus/tracing/TracingUtil.hpp"

#endif

namespace nautilus {

namespace details {
template <typename LHS>
LHS getRawValue(const val<LHS>& val);

#define COMMON_RETURN_TYPE                                                                                             \
	typename std::common_type_t<typename std::remove_cvref_t<LHS>::basic_type,                                         \
	                            typename std::remove_cvref_t<RHS>::basic_type>

#define DEDUCT_RETURN_TYPE(OP) decltype(getRawValue(left) OP getRawValue(right))

template <typename T>
tracing::TypedValueRef getState(T&& value) {
	return value.state;
}

} // namespace details

// val substitution for all arithmetic value type, integer, float, bool
template <is_arithmetic ValueType>
class val<ValueType> {
public:
	// raw type is the val type
	using raw_type = ValueType;
	// basic type is the underling val type
	using basic_type = ValueType;

#ifdef ENABLE_TRACING
	val() : state(tracing::traceConstant(0)) {
	}
	val(ValueType value) : state(tracing::traceConstant(value)), value(value) {
	}
	// copy constructor
	val(const val<ValueType>& other) : state(tracing::traceCopy(other.state)), value(other.value) {
	}
	// move constructor
	val(const val<ValueType>&& other) noexcept : state(std::move(other.state)), value(other.value) {
	}
	val(tracing::TypedValueRef& tc) : state(tc), value() {
	}
#else
	val() {
	}
	val(ValueType value) : value(value) {
	}
	// copy constructor
	val(const val<ValueType>& other) : value(other.value) {
	}
	// move constructor
	val(const val<ValueType>&& other) : value(other.value) {
	}
#endif

	val<ValueType>& operator=(const val<ValueType>& other) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceAssignment(state, other.state, tracing::to_type<ValueType>());
		}
#endif
		this->value = other.value;
		return *this;
	}

	template <typename OtherType>
	    requires std::is_convertible_v<ValueType, OtherType>
	operator val<OtherType>() const {
		// cast
		if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
			auto resultRef = tracing::traceUnaryOp(tracing::CAST, tracing::to_type<OtherType>(), state);
			return val<OtherType>(resultRef);
#endif
		}
		return val<OtherType>(value);
	}

	const val<ValueType>& operator++() {
		// increment
		*this = *this + (ValueType) 1;
		// assign
		return *this;
	}

	val<ValueType> operator++(int) {
		// returned value should be a copy of the object before increment
		auto temp = *this;
		// this = details::add(*this, val<ValueType>(1));
		*this = *this + (ValueType) 1;
		// ++value;
		return temp;
	}

	const val<ValueType>& operator--() {
		// decrement
		*this = *this - (ValueType) 1;
		// assign
		return *this;
	}

	val<ValueType> operator--(int) {
		// returned value should be a copy of the object before decrement
		auto temp = *this;
		*this = *this - (ValueType) 1;
		return temp;
	}
#ifdef ENABLE_TRACING
	const tracing::TypedValueRefHolder state;
#endif
private:
	friend ValueType details::getRawValue<ValueType>(const val<ValueType>& left);
	ValueType value;
};

template <>
class val<bool> {
public:
	using raw_type = bool;
	using basic_type = bool;

#ifdef ENABLE_TRACING
	const tracing::TypedValueRefHolder state;
#endif

#ifdef ENABLE_TRACING

	val() : state(tracing::traceConstant(0)), value(false) {
	}
	val(bool value) : state(tracing::traceConstant(value)), value(value) {
	}
	// copy constructor
	val(const val<bool>& other) : state(tracing::traceCopy(other.state)), value(other.value) {
	}
	// move constructor
	val(const val<bool>&& other) : state(other.state), value(other.value) {
	}
	val(tracing::TypedValueRef& tc) : state(tc) {
	}

#else
	val() {
	}
	val(bool value) : value(value) {
	}
	// copy constructor
	val(const val<bool>& other) : value(other.value) {
	}
	// move constructor
	val(const val<bool>&& other) : value(other.value) {
	}
#endif

	val<bool>& operator=(const val<bool>& other) {
		if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
			tracing::traceAssignment(state, other.state, tracing::to_type<bool>());
#endif
		}

		this->value = other.value;
		return *this;
	}

	operator bool() const {
		if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
			auto ref = state;
			return tracing::traceBool(ref);
#endif
		}
		return value;
	}

private:
	friend bool details::getRawValue<bool>(const val<bool>& left);
	bool value;
};

template <is_fundamental_val Type>
auto inline&& make_value(Type&& value) {
	return std::forward<Type>(value);
}

template <convertible_to_fundamental Type>
auto inline make_value(const Type& value) {
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

template <typename LeftType, is_fundamental RightType>
auto inline cast_value(LeftType&& value) {
	typedef typename std::remove_reference_t<LeftType>::basic_type basic_type;
	typedef typename std::common_type<basic_type, RightType>::type commonType;
	if constexpr (std::is_same_v<basic_type, RightType>) {
		return std::forward<LeftType>(value);
	} else {
		return static_cast<val<commonType>>(value);
	}
}

template <typename LeftType, is_bool RightType>
auto&& cast_value(LeftType&& value) {
	return std::forward<LeftType>(value);
}

template <typename LeftType, is_enum RightType>
auto&& cast_value(LeftType&& value) {
	return std::forward<LeftType>(value);
}

namespace details {

#ifdef ENABLE_TRACING
#define TRAC_LOGICAL_BINARY_OP(OP)                                                                                     \
	if (tracing::inTracer()) {                                                                                         \
		auto tc = tracing::traceBinaryOp<tracing::OP, bool>(lValue.state, rValue.state);                               \
		return val<bool>(tc);                                                                                          \
	}
#else
#define TRAC_LOGICAL_BINARY_OP(OP)
#endif

#define DEFINE_BINARY_OPERATOR_HELPER(OP, OP_NAME, OP_TRACE, RES_TYPE)                                                 \
	template <typename LHS, typename RHS>                                                                              \
	auto inline OP_NAME(LHS&& left, RHS&& right) {                                                                     \
		typedef std::common_type_t<typename std::remove_reference_t<LHS>::basic_type,                                  \
		                           typename std::remove_reference_t<RHS>::basic_type>                                  \
		    commonType;                                                                                                \
		auto&& lValue = cast_value<LHS, commonType>(std::forward<LHS>(left));                                          \
		auto&& rValue = cast_value<RHS, commonType>(std::forward<RHS>(right));                                         \
		if SHOULD_TRACE () {                                                                                           \
			auto tc = tracing::traceBinaryOp(tracing::OP_TRACE, tracing::to_type<RES_TYPE>(),                          \
			                                 details::getState(lValue), details::getState(rValue));                    \
			return val<RES_TYPE>(tc);                                                                                  \
		}                                                                                                              \
		return val<RES_TYPE>(getRawValue(lValue) OP getRawValue(rValue));                                              \
	}

DEFINE_BINARY_OPERATOR_HELPER(+, add, ADD, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(-, sub, SUB, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(*, mul, MUL, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(/, div, DIV, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(%, mod, MOD, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(==, eq, EQ, bool)

DEFINE_BINARY_OPERATOR_HELPER(!=, neq, NEQ, bool)

DEFINE_BINARY_OPERATOR_HELPER(<, lt, LT, bool)

DEFINE_BINARY_OPERATOR_HELPER(<=, lte, LTE, bool)

DEFINE_BINARY_OPERATOR_HELPER(>, gt, GT, bool)

DEFINE_BINARY_OPERATOR_HELPER(>=, gte, GTE, bool)

DEFINE_BINARY_OPERATOR_HELPER(>>, shr, RSH, DEDUCT_RETURN_TYPE(>>))

DEFINE_BINARY_OPERATOR_HELPER(<<, shl, LSH, DEDUCT_RETURN_TYPE(<<))

DEFINE_BINARY_OPERATOR_HELPER(&, bAnd, BAND, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(|, bOr, BOR, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(^, bXOr, BXOR, COMMON_RETURN_TYPE)

template <is_integral LHS>
val<LHS> neg(val<LHS>& val) {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto tc = tracing::traceUnaryOp(tracing::NEGATE, tracing::to_type<LHS>(), val.state);
		return tc;
	}
#endif
	return ~getRawValue(val);
}

template <typename LHS>
LHS inline getRawValue(const val<LHS>& val) {
	return val.value;
}

} // namespace details

#define DEFINE_BINARY_OPERATOR(OP, FUNC, CON_VAL, CON_VALUE)                                                           \
	template <typename LHS, typename RHS>                                                                              \
	    requires(CON_VAL<LHS> && CON_VAL<RHS>)                                                                         \
	auto inline operator OP(LHS&& left, RHS&& right) {                                                                 \
		return details::FUNC(std::move(left), std::move(right));                                                       \
	}                                                                                                                  \
                                                                                                                       \
	template <typename LHS, typename RHS>                                                                              \
	    requires(CON_VAL<LHS> && CON_VALUE<RHS>)                                                                       \
	auto inline operator OP(LHS&& left, RHS&& right) {                                                                 \
		auto&& rhsV = make_value(std::forward<RHS>(right));                                                            \
		return details::FUNC(std::move(left), std::move(rhsV));                                                        \
	}                                                                                                                  \
                                                                                                                       \
	template <typename LHS, typename RHS>                                                                              \
	    requires(CON_VALUE<LHS> && CON_VAL<RHS>)                                                                       \
	auto inline operator OP(LHS&& left, RHS&& right) {                                                                 \
		auto&& lhsV = make_value(std::forward<LHS>(left));                                                             \
		return details::FUNC(std::move(lhsV), std::move(right));                                                       \
	}

DEFINE_BINARY_OPERATOR(+, add, is_fundamental_val, convertible_to_fundamental)

DEFINE_BINARY_OPERATOR(-, sub, is_fundamental_val, convertible_to_fundamental)

DEFINE_BINARY_OPERATOR(*, mul, is_fundamental_val, convertible_to_fundamental)

DEFINE_BINARY_OPERATOR(/, div, is_fundamental_val, convertible_to_fundamental)

DEFINE_BINARY_OPERATOR(%, mod, is_fundamental_val, convertible_to_fundamental)

DEFINE_BINARY_OPERATOR(==, eq, is_fundamental_val, convertible_to_fundamental)

DEFINE_BINARY_OPERATOR(!=, neq, is_fundamental_val, convertible_to_fundamental)

DEFINE_BINARY_OPERATOR(>, gt, is_fundamental_val, convertible_to_fundamental)

DEFINE_BINARY_OPERATOR(>=, gte, is_fundamental_val, convertible_to_fundamental)

DEFINE_BINARY_OPERATOR(<, lt, is_fundamental_val, convertible_to_fundamental)

DEFINE_BINARY_OPERATOR(<=, lte, is_fundamental_val, convertible_to_fundamental)

DEFINE_BINARY_OPERATOR(>>, shr, is_integral_val, is_integral)

DEFINE_BINARY_OPERATOR(<<, shl, is_integral_val, is_integral)

DEFINE_BINARY_OPERATOR(|, bOr, is_integral_val, is_integral)

DEFINE_BINARY_OPERATOR(&, bAnd, is_integral_val, is_integral)

DEFINE_BINARY_OPERATOR(^, bXOr, is_integral_val, is_integral)

template <typename LHS>
auto inline operator~(LHS left) {
	return details::neg(left);
}

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

namespace details {
val<bool> inline lOr(val<bool>& left, val<bool>& right) {
#ifdef ENABLE_TRACING
	if SHOULD_TRACE () {
		auto tc = tracing::traceBinaryOp(tracing::OR, tracing::to_type<bool>(), left.state, right.state);
		return val<bool> {tc};
	}
#endif
	return getRawValue(left) || getRawValue(right);
}

val<bool> inline lAnd(val<bool>& left, val<bool>& right) {
#ifdef ENABLE_TRACING
	if SHOULD_TRACE () {
		auto tc = tracing::traceBinaryOp(tracing::AND, tracing::to_type<bool>(), left.state, right.state);
		return val<bool> {tc};
	}
#endif
	return getRawValue(left) && getRawValue(right);
}

val<bool> inline lNot(val<bool>& arg) {
#ifdef ENABLE_TRACING
	if SHOULD_TRACE () {
		auto tc = tracing::traceUnaryOp(tracing::NOT, Type::b, arg.state);
		return val<bool> {tc};
	}
#endif
	return !getRawValue(arg);
}
} // namespace details

auto inline operator||(bool left, val<bool> right) {
	auto leftVal = make_value(left);
	return details::lOr(leftVal, right);
}
auto inline operator||(val<bool> left, bool right) {
	auto rightVal = make_value(right);
	return details::lOr(left, rightVal);
}
auto inline operator||(val<bool> left, val<bool> right) {
	return details::lOr(left, right);
}

auto inline operator&&(bool left, val<bool> right) {
	auto leftVal = make_value(left);
	return details::lAnd(leftVal, right);
}
auto inline operator&&(val<bool> left, bool right) {
	auto rightVal = make_value(right);
	return details::lAnd(left, rightVal);
}
auto inline operator&&(val<bool> left, val<bool> right) {
	return details::lAnd(left, right);
}

auto inline operator!(val<bool> left) {
	return details::lNot(left);
}

} // namespace nautilus
