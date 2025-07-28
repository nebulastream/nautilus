#pragma once

#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/val_concepts.hpp"
#include <type_traits>
#include <utility>

#ifdef ENABLE_TRACING

#include "nautilus/tracing/TracingUtil.hpp"

#endif

namespace nautilus {

namespace details {
template <typename LHS>
struct RawValueResolver {
	static LHS inline getRawValue(const val<LHS>& val) {
		return val.value;
	}
};

#define COMMON_RETURN_TYPE                                                                                             \
	typename std::common_type_t<typename std::remove_cvref_t<LHS>::basic_type,                                         \
	                            typename std::remove_cvref_t<RHS>::basic_type>

#define DEDUCT_RETURN_TYPE(OP)                                                                                         \
	decltype(RawValueResolver<typename std::remove_cvref_t<decltype(left)>::raw_type>::getRawValue(left)               \
	             OP RawValueResolver<typename std::remove_cvref_t<decltype(right)>::raw_type>::getRawValue(right))

template <typename T>
struct StateResolver {
	template <typename U = T>
	static tracing::TypedValueRef getState(U&& value) {
		return value.state;
	}
};
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
	val() : state(tracing::traceConstant<raw_type>(0)), value(0) {
	}
	val(ValueType value) : state(tracing::traceConstant(value)), value(value) {
	}
	val(const val<ValueType>& other) : state(tracing::traceCopy(other.state)), value(other.value) {
	}
	val(val<ValueType>&& other) noexcept : state(other.state), value(std::move(other.value)) {
	}
	val(tracing::TypedValueRef& tc) : state(tc), value() {
	}
#else
	val() : value(0) {
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
			tracing::traceAssignment(state, other.state, tracing::TypeResolver<ValueType>::to_type());
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
			auto resultRef = tracing::traceUnaryOp(tracing::CAST, tracing::TypeResolver<OtherType>::to_type(), state);
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
	friend details::RawValueResolver<ValueType>;
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
	val(tracing::TypedValueRef& tc) : state(tc), value() {
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
			tracing::traceAssignment(state, other.state, Type::b);
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
	friend details::RawValueResolver<bool>;
	bool value;
};

template <is_fundamental_val Type>
decltype(auto) inline make_value(Type&& value) {
	return std::forward<Type>(value);
}

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

template <typename LeftType, is_fundamental RightType>
decltype(auto) inline cast_value(LeftType&& value) {
	using basic_type = typename std::remove_reference_t<LeftType>::basic_type;
	using commonType = std::common_type_t<basic_type, RightType>;

	if constexpr (std::is_same_v<basic_type, RightType>) {
		// Perfectly forward — no copy, no move
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
			                                 details::StateResolver<decltype(lValue)>::getState(lValue),               \
			                                 details::StateResolver<decltype(rValue)>::getState(rValue));              \
			return val<RES_TYPE>(tc);                                                                                  \
		}                                                                                                              \
                                                                                                                       \
		return val<RES_TYPE>(RawValueResolver<commonType>::getRawValue(std::forward<decltype(lValue)>(                 \
		    lValue)) OP RawValueResolver<commonType>::getRawValue(std::forward<decltype(rValue)>(rValue)));            \
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
		auto tc = tracing::traceUnaryOp(tracing::NEGATE, tracing::TypeResolver<LHS>::to_type(), val.state);
		return tc;
	}
#endif
	return ~RawValueResolver<LHS>::getRawValue(val);
}
} // namespace details

#define DEFINE_BINARY_OPERATOR(OP, FUNC, CATEGORY)                                                                     \
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
		return left OP rV;                                                                                             \
	}

DEFINE_BINARY_OPERATOR(+, add, fundamental)

DEFINE_BINARY_OPERATOR(-, sub, fundamental)

DEFINE_BINARY_OPERATOR(*, mul, fundamental)

DEFINE_BINARY_OPERATOR(/, div, fundamental)

DEFINE_BINARY_OPERATOR(%, mod, integral)

DEFINE_BINARY_OPERATOR(==, eq, fundamental)

DEFINE_BINARY_OPERATOR(!=, neq, fundamental)

DEFINE_BINARY_OPERATOR(>, gt, fundamental)

DEFINE_BINARY_OPERATOR(>=, gte, fundamental)

DEFINE_BINARY_OPERATOR(<, lt, fundamental)

DEFINE_BINARY_OPERATOR(<=, lte, fundamental)

DEFINE_BINARY_OPERATOR(>>, shr, integral)

DEFINE_BINARY_OPERATOR(<<, shl, integral)

DEFINE_BINARY_OPERATOR(|, bOr, integral)

DEFINE_BINARY_OPERATOR(&, bAnd, integral)

DEFINE_BINARY_OPERATOR(^, bXOr, integral)

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
		auto tc = tracing::traceBinaryOp(tracing::OR, Type::b, left.state, right.state);
		return val<bool> {tc};
	}
#endif
	return RawValueResolver<bool>::getRawValue(left) || RawValueResolver<bool>::getRawValue(right);
}

val<bool> inline lAnd(val<bool>& left, val<bool>& right) {
#ifdef ENABLE_TRACING
	if SHOULD_TRACE () {
		auto tc = tracing::traceBinaryOp(tracing::AND, Type::b, left.state, right.state);
		return val<bool> {tc};
	}
#endif
	return RawValueResolver<bool>::getRawValue(left) && RawValueResolver<bool>::getRawValue(right);
}

val<bool> inline lNot(val<bool>& arg) {
#ifdef ENABLE_TRACING
	if SHOULD_TRACE () {
		auto tc = tracing::traceUnaryOp(tracing::NOT, Type::b, arg.state);
		return val<bool> {tc};
	}
#endif
	return !RawValueResolver<bool>::getRawValue(arg);
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
