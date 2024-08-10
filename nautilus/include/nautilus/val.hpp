#pragma once

#include "nautilus/static.hpp"
#include "nautilus/val_concepts.hpp"
#include <iostream>
#include <memory>

#ifdef ENABLE_TRACING

#include "common/traceing.hpp"

#endif

namespace nautilus {

template <class T>
constexpr auto getType();

namespace details {

template <typename LHS>
LHS getRawValue(val<LHS>& val);

#define COMMON_RETURN_TYPE val<typename std::common_type<typename LHS::basic_type, typename RHS::basic_type>::type>

#define INFERED_RETURN_TYPE(OP) val<decltype(getRawValue(left) OP getRawValue(right))>

template <typename LHS, typename RHS>
COMMON_RETURN_TYPE add(LHS& left, val<RHS>& right);

template <is_fundamental LHS, is_fundamental RHS>
COMMON_RETURN_TYPE sub(val<LHS>& left, val<RHS>& right);

template <is_fundamental LHS, is_fundamental RHS>
COMMON_RETURN_TYPE mul(val<LHS>& left, val<RHS>& right);

template <is_fundamental LHS, is_fundamental RHS>
COMMON_RETURN_TYPE div(val<LHS>& left, val<RHS>& right);

template <is_fundamental LHS, is_fundamental RHS>
COMMON_RETURN_TYPE mod(val<LHS>& left, val<RHS>& right);

template <is_fundamental LHS, is_fundamental RHS>
val<bool> eq(val<LHS>& left, val<RHS>& right);

template <is_fundamental LHS, is_fundamental RHS>
val<bool> lt(val<LHS>& left, val<RHS>& right);

template <is_fundamental LHS, is_fundamental RHS>
val<bool> gt(val<LHS>& left, val<RHS>& right);

template <is_fundamental LHS, is_fundamental RHS>
val<bool> lte(val<LHS>& left, val<RHS>& right);

template <is_fundamental LHS, is_fundamental RHS>
val<bool> gte(val<LHS>& left, val<RHS>& right);

val<bool> lAnd(val<bool>& left, val<bool>& right);

val<bool> lOr(val<bool>& left, val<bool>& right);

val<bool> lNot(val<bool>& val);

template <is_fundamental LHS, is_fundamental RHS>
COMMON_RETURN_TYPE bAnd(val<LHS>& left, val<RHS>& right);

template <is_fundamental LHS, is_fundamental RHS>
COMMON_RETURN_TYPE bOr(val<LHS>& left, val<RHS>& right);

template <is_fundamental LHS, is_fundamental RHS>
COMMON_RETURN_TYPE bXOr(val<LHS>& left, val<RHS>& right);

template <is_integral LHS, is_integral RHS>
auto shl(val<LHS>& left, val<RHS>& right);

template <is_integral LHS, is_integral RHS>
auto shr(val<LHS>& left, val<RHS>& right);

template <is_fundamental LHS>
val<LHS> neg(val<LHS>& val);

template <typename T>
tracing::value_ref getState(T&& value) {
	return value.state;
}

} // namespace details

// val substitution for all arithmetic value type, integer, float, bool
template <is_arithmetic ValueType>
class val<ValueType> {
public:
	using raw_type = ValueType;
	using basic_type = ValueType;

#ifdef ENABLE_TRACING
	const tracing::TypedValueRefHolder state;
	val() : state(tracing::traceConstant(0)) {};
	val(ValueType value) : state(tracing::traceConstant(value)), value(value) {};
	// copy constructor
	val(const val<ValueType>& other) : state(tracing::traceCopy(other.state)), value(other.value) {};
	// move constructor
	val(const val<ValueType>&& other) noexcept : state((other.state)), value(other.value) {};
	val(tracing::value_ref& tc) : state(tc), value() {};
#else
	val() {};
	val(ValueType value) : value(value) {};
	// copy constructor
	val(const val<ValueType>& other) : value(other.value) {};
	// move constructor
	val(const val<ValueType>&& other) : value(other.value) {};
#endif

	val<ValueType>& operator=(const val<ValueType>& other) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceAssignment(state, other.state, tracing::to_type<ValueType>());
		}
#endif
		this->value = other.value;
		return *this;
	};

	template <typename OtherType>
	    requires std::is_convertible_v<ValueType, OtherType>
	operator val<OtherType>() const {
		// cast
		if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
			auto resultRef = tracing::traceCast(state, tracing::to_type<OtherType>());
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

	static ValueType toNativeValue(val<ValueType>& value) {
		return value.value;
	}
	static val<ValueType> fromNativeType(ValueType& value) {
		return val<ValueType>(value);
	}

private:
	friend ValueType details::getRawValue<ValueType>(val<ValueType>& left);
	ValueType value;

	template <is_arithmetic LHS, is_arithmetic RHS>
	friend COMMON_RETURN_TYPE mul(val<LHS>& left, val<RHS>& right);

	template <is_arithmetic LHS, is_arithmetic RHS>
	friend COMMON_RETURN_TYPE details::add(val<LHS>& left, val<RHS>& right);

	template <is_arithmetic LHS, is_arithmetic RHS>
	friend COMMON_RETURN_TYPE details::sub(val<LHS>& left, val<RHS>& right);

	template <is_arithmetic LHS, is_arithmetic RHS>
	friend COMMON_RETURN_TYPE details::div(val<LHS>& left, val<RHS>& right);

	template <is_arithmetic LHS, is_arithmetic RHS>
	friend COMMON_RETURN_TYPE details::mod(val<LHS>& left, val<RHS>& right);

	template <is_fundamental LHS, is_fundamental RHS>
	friend val<bool> details::eq(val<LHS>& left, val<RHS>& right);

	template <is_fundamental LHS, is_fundamental RHS>
	friend val<bool> details::lt(val<LHS>& left, val<RHS>& right);

	template <is_fundamental LHS, is_fundamental RHS>
	friend val<bool> details::gt(val<LHS>& left, val<RHS>& right);

	template <is_fundamental LHS, is_fundamental RHS>
	friend val<bool> details::lte(val<LHS>& left, val<RHS>& right);

	template <is_fundamental LHS, is_fundamental RHS>
	friend val<bool> details::gte(val<LHS>& left, val<RHS>& right);

	friend val<bool> details::lAnd(val<bool>& left, val<bool>& right);

	friend val<bool> details::lOr(val<bool>& left, val<bool>& right);

	friend val<bool> details::lNot(val<bool>& val);

	template <is_fundamental LHS, is_fundamental RHS>
	friend COMMON_RETURN_TYPE details::bAnd(val<LHS>& left, val<RHS>& right);

	template <is_fundamental LHS, is_fundamental RHS>
	friend COMMON_RETURN_TYPE details::bOr(val<LHS>& left, val<RHS>& right);

	template <is_fundamental LHS, is_fundamental RHS>
	friend COMMON_RETURN_TYPE details::bXOr(val<LHS>& left, val<RHS>& right);

	template <is_integral LHS, is_integral RHS>
	friend auto details::shl(val<LHS>& left, val<RHS>& right);

	template <is_integral LHS, is_integral RHS>
	friend auto details::shr(val<LHS>& left, val<RHS>& right);

	template <is_fundamental LHS>
	friend auto details::neg(val<LHS>& val);
};

template <>
class val<bool> {
public:
	using raw_type = bool;
	using basic_type = bool;

#ifdef ENABLE_TRACING

	tracing::value_ref state;
	val() : state(tracing::traceConstant(0)), value(false) {};
	val(bool value) : state(tracing::traceConstant(value)), value(value) {};
	// copy constructor
	val(const val<bool>& other) : state(tracing::traceCopy(other.state)), value(other.value) {};
	// move constructor
	val(const val<bool>&& other) noexcept : state(other.state), value(other.value) {};
	val(tracing::value_ref& tc) : state(tc) {};

#else
	val() {};
	val(bool value) : value(value) {};
	// copy constructor
	val(const val<bool>& other) : value(other.value) {};
	// move constructor
	val(const val<bool>&& other) : value(other.value) {};
#endif

	~val() {
		if SHOULD_TRACE () {

			// tracing::getVarRefMap()[state.ref]--;
			// if (tracing::getVarRefMap()[state.ref] == 0) {
			//  tracing::traceValueDestruction(state);
			// std::cout << "destructor " << state << " - " << tag << std::endl;
			//  }
		}
	}

	val<bool>& operator=(const val<bool>& other) {

		if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
			tracing::traceAssignment(state, other.state, tracing::to_type<bool>());
#endif
		}

		this->value = other.value;
		return *this;
	};

	operator bool() const {
		if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
			auto ref = state;
			return tracing::traceBool(ref);
#endif
		}
		return value;
	}

	bool value;
};

template <class T>
concept is_fundamental_value = requires(val<T> value) { std::is_fundamental_v<typename std::remove_reference_t<T>::basic_type>; };

template <class T>
concept is_arithmetic_value = std::is_arithmetic_v<typename std::remove_reference_t<T>::basic_type>;

template <is_fundamental_value Type>
auto inline&& make_value(Type&& value) {
	return std::forward<Type>(value);
};

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
	typedef typename LeftType::basic_type basic_type;
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
};

template <typename LeftType, is_enum RightType>
auto&& cast_value(LeftType&& value) {
	return std::forward<LeftType>(value);
};

namespace details {

#ifdef ENABLE_TRACING
#define TRAC_BINARY_OP(OP)                                                                                                                                                                                                                     \
	if (tracing::inTracer()) {                                                                                                                                                                                                                 \
		auto tc = tracing::traceBinaryOp<tracing::OP, commonType>(lValue.state, rValue.state);                                                                                                                                                 \
		return val<commonType>(tc);                                                                                                                                                                                                            \
	}

#define TRAC_BINARY_OP_DIRECT(OP)                                                                                                                                                                                                              \
	if (tracing::inTracer()) {                                                                                                                                                                                                                 \
		auto tc = tracing::traceBinaryOp<tracing::OP, commonType>(left.state, right.state);                                                                                                                                                    \
		return val<commonType>(tc);                                                                                                                                                                                                            \
	}
#else
#define TRAC_OP(OP)
#endif

#ifdef ENABLE_TRACING
#define TRAC_LOGICAL_BINARY_OP(OP)                                                                                                                                                                                                             \
	if (tracing::inTracer()) {                                                                                                                                                                                                                 \
		auto tc = tracing::traceBinaryOp<tracing::OP, bool>(lValue.state, rValue.state);                                                                                                                                                       \
		return val<bool>(tc);                                                                                                                                                                                                                  \
	}
#else
#define TRAC_LOGICAL_BINARY_OP(OP)
#endif

#define DEFINE_BINARY_OPERATOR_HELPER(OP, OP_NAME, OP_TRACE, RES_TYPE)                                                                                                                                                                         \
	template <typename LHS, typename RHS>                                                                                                                                                                                                      \
	auto inline OP_NAME(LHS&& left, RHS&& right) {                                                                                                                                                                                             \
		typedef typename std::common_type<typename LHS::basic_type, typename RHS::basic_type>::type commonType;                                                                                                                                \
		auto&& lValue = cast_value<LHS, commonType>(std::forward<LHS>(left));                                                                                                                                                                  \
		auto&& rValue = cast_value<RHS, commonType>(std::forward<RHS>(right));                                                                                                                                                                 \
		using resultType = decltype(getRawValue(lValue) OP getRawValue(rValue));                                                                                                                                                                  \
		if SHOULD_TRACE () {                                                                                                                                                                                                                   \
			auto tc = tracing::traceBinaryOp<tracing::OP_TRACE, resultType>(details::getState(lValue), details::getState(rValue));                                                                                                             \
			return RES_TYPE(tc);                                                                                                                                                                                                               \
		}                                                                                                                                                                                                                                      \
		return RES_TYPE(getRawValue(lValue) OP getRawValue(rValue));                                                                                                                                                                           \
	}


DEFINE_BINARY_OPERATOR_HELPER(+, add, ADD, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(-, sub, SUB, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(*, mul, MUL, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(/, div, DIV, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(%, mod, MOD, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(==, eq, EQ, val<bool>)

DEFINE_BINARY_OPERATOR_HELPER(!=, neq, NEQ, val<bool>)

DEFINE_BINARY_OPERATOR_HELPER(<, lt, LT, val<bool>)

DEFINE_BINARY_OPERATOR_HELPER(<=, lte, LTE, val<bool>)

DEFINE_BINARY_OPERATOR_HELPER(>, gt, GT, val<bool>)

DEFINE_BINARY_OPERATOR_HELPER(>=, gte, GTE, val<bool>)

DEFINE_BINARY_OPERATOR_HELPER(>>, shr, RSH, INFERED_RETURN_TYPE(>>))

DEFINE_BINARY_OPERATOR_HELPER(<<, shl, LSH, INFERED_RETURN_TYPE(<<))

DEFINE_BINARY_OPERATOR_HELPER(&, bAnd, BAND, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(|, bOr, BOR, COMMON_RETURN_TYPE)

DEFINE_BINARY_OPERATOR_HELPER(^, bXOr, BXOR, COMMON_RETURN_TYPE)

template <is_fundamental LHS>
val<LHS> neg(val<LHS>& val) {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto tc = traceUnaryOp<tracing::NEGATE, LHS>(val.state);
		return tc;
	}
#endif
	return ~getRawValue(val);
}

template <typename LHS>
LHS inline getRawValue(val<LHS>& val) {
	return val.value;
}

} // namespace details

#define DEFINE_BINARY_OPERATOR(OP, FUNC)                                                                                                                                                                                                       \
	template <typename LHS, typename RHS>                                                                                                                                                                                                      \
	    requires(is_fundamental_value<LHS> && (is_fundamental_value<RHS> || convertible_to_fundamental<RHS>) ) || ((is_fundamental_value<LHS> || convertible_to_fundamental<LHS>) && is_fundamental_value<RHS>)                                \
	auto inline operator OP(LHS&& left, RHS&& right) {                                                                                                                                                                                         \
		auto&& lhsV = make_value(std::forward<LHS>(left));                                                                                                                                                                                     \
		auto&& rhsV = make_value(std::forward<RHS>(right));                                                                                                                                                                                    \
		return details::FUNC(std::move(lhsV), std::move(rhsV));                                                                                                                                                                                \
	}
#define DEFINE_ARITHMETICAL_BINARY_OPERATOR(OP, FUNC)                                                                                                                                                                                          \
	template <typename LHS, typename RHS>                                                                                                                                                                                                      \
	    requires(is_arithmetic_value<LHS> && (is_arithmetic_value<RHS> || convertible_to_fundamental<RHS>) ) || ((is_arithmetic_value<LHS> || convertible_to_fundamental<LHS>) && is_arithmetic_value<RHS>)                                    \
	auto inline operator OP(LHS&& left, RHS&& right) {                                                                                                                                                                                         \
		auto&& lhsV = make_value(std::forward<LHS>(left));                                                                                                                                                                                     \
		auto&& rhsV = make_value(std::forward<RHS>(right));                                                                                                                                                                                    \
		return details::FUNC(std::move(lhsV), std::move(rhsV));                                                                                                                                                                                \
	}

DEFINE_ARITHMETICAL_BINARY_OPERATOR(+, add)

DEFINE_BINARY_OPERATOR(-, sub)

DEFINE_BINARY_OPERATOR(*, mul)

DEFINE_BINARY_OPERATOR(/, div)

DEFINE_BINARY_OPERATOR(%, mod)

DEFINE_BINARY_OPERATOR(==, eq)

DEFINE_BINARY_OPERATOR(!=, neq)

DEFINE_BINARY_OPERATOR(>, gt)

DEFINE_BINARY_OPERATOR(>=, gte)

DEFINE_BINARY_OPERATOR(<, lt)

DEFINE_BINARY_OPERATOR(<=, lte)

DEFINE_BINARY_OPERATOR(>>, shr)

DEFINE_BINARY_OPERATOR(<<, shl)

DEFINE_BINARY_OPERATOR(|, bOr)

DEFINE_BINARY_OPERATOR(&, bAnd)

DEFINE_BINARY_OPERATOR(^, bXOr)

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
auto& operator|=(val<LHS>& left, RHS right) {
	left = left | right;
	return left;
}

template <typename LHS, typename RHS>
auto& operator^=(val<LHS>& left, RHS right) {
	left = left ^ right;
	return left;
}

template <typename LHS, typename RHS>
auto& operator&=(val<LHS>& left, RHS right) {
	left = left & right;
	return left;
}

template <typename LHS, typename RHS>
auto& operator<<=(val<LHS>& left, RHS right) {
	left = left << right;
	return left;
}

template <typename LHS, typename RHS>
auto& operator>>=(val<LHS>& left, RHS right) {
	left = left >> right;
	return left;
}

namespace details {
val<bool> inline lOr(val<bool>& left, val<bool>& right) {
#ifdef ENABLE_TRACING
	if SHOULD_TRACE () {
		auto tc = tracing::traceBinaryOp<tracing::OR, bool>(left.state, right.state);
		return val<bool> {tc};
	}
#endif
	return left.value || right.value;
}

val<bool> inline lAnd(val<bool>& left, val<bool>& right) {
#ifdef ENABLE_TRACING
	if SHOULD_TRACE () {
		auto tc = tracing::traceBinaryOp<tracing::AND, bool>(left.state, right.state);
		return val<bool> {tc};
	}
#endif
	return left.value && right.value;
}

val<bool> inline lNot(val<bool>& arg) {
#ifdef ENABLE_TRACING
	if SHOULD_TRACE () {
		auto tc = tracing::traceUnaryOp<tracing::NOT, bool>(arg.state);
		return val<bool> {tc};
	}
#endif
	return !arg.value;
}
} // namespace details

template <typename LHS, typename RHS>
auto inline operator||(LHS left, RHS right) {
	if constexpr (std::is_same_v<LHS, bool>) {
		auto leftV = make_value(left);
		return details::lOr(leftV, right);
	} else if constexpr (std::is_same_v<RHS, bool>) {
		auto rightV = make_value(right);
		return details::lOr(left, rightV);
	} else {
		return details::lOr(left, right);
	}
}

template <typename LHS, typename RHS>
auto inline operator&&(LHS left, RHS right) {
	if constexpr (std::is_same_v<LHS, bool>) {
		auto leftV = make_value(left);
		return details::lAnd(leftV, right);
	} else if constexpr (std::is_same_v<RHS, bool>) {
		auto rightV = make_value(right);
		return details::lAnd(left, rightV);
	} else {
		return details::lAnd(left, right);
	}
}

template <typename LHS>
auto inline operator!(val<LHS> left) {
	return details::lNot(left);
}

} // namespace nautilus
