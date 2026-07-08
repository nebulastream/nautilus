#pragma once
#include "nautilus/config.hpp"
#include "nautilus/val_base.hpp"
#include "nautilus/val_concepts.hpp"
#include <iostream>
#include <memory>
#include <utility>

#ifdef ENABLE_TRACING

#include "nautilus/tracing/TracingUtil.hpp"

#endif

namespace nautilus {

template <typename T>
    requires std::is_enum_v<T>
class val<T> : public val_base {
public:
	using underlying_type_t = std::underlying_type_t<T>;
	using raw_type = underlying_type_t;
	using basic_type = T;

	[[nodiscard]] Type getType() const override {
		return tracing::TypeResolver<underlying_type_t>::to_type();
	}

	[[nodiscard]] TypeId getTypeId() const override {
		return typeIdOf<val<T>>();
	}

#ifdef ENABLE_TRACING
	template <T>
	    requires std::is_enum_v<T> && (!std::is_convertible_v<T, std::underlying_type_t<T>>)
	val(T val)
	    : state(tracing::traceConstant(static_cast<std::underlying_type_t<T>>(val))),
	      value(static_cast<std::underlying_type_t<T>>(val)) {
	}

	template <T>
	    requires std::is_enum_v<T> && (!std::is_convertible_v<T, std::underlying_type_t<T>>)
	val(val<T>& val)
	    : state(tracing::traceConstant(static_cast<std::underlying_type_t<T>>(val))),
	      value(static_cast<std::underlying_type_t<T>>(val)) {
	}
	val(val<underlying_type_t> t)
	    : state(t.state), value((T) details::RawValueResolver<underlying_type_t>::getRawValue(t)) {
	}
	val(val<T>& t) : state(tracing::traceCopy(t.state)), value(t.value) {
	}
	val(val<T>&& t) : state(t.state), value(t.value) {
	}
	val(T val) : state(tracing::traceConstant(static_cast<std::underlying_type_t<T>>(val))), value(val) {
	}
#else
	template <T>
	    requires std::is_enum_v<T> && (!std::is_convertible_v<T, std::underlying_type_t<T>>)
	val(T val) : value(static_cast<std::underlying_type_t<T>>(val)) {
	}

	template <T>
	    requires std::is_enum_v<T> && (!std::is_convertible_v<T, std::underlying_type_t<T>>)
	val(val<T>& val) : value(static_cast<std::underlying_type_t<T>>(val)) {
	}

	val(val<underlying_type_t> t) : value((T) details::RawValueResolver<underlying_type_t>::getRawValue(t)) {
	}
	val(val<T>& t) : value(t.value) {
	}
	val(T val) : value(val) {
	}
#endif

	val<bool> operator==(val<T>& other) const {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto tc = tracing::traceBinaryOp(tracing::EQ, Type::b, state, other.state);
			return val<bool>(tc);
		}
#endif
		return value == other.value;
	}

	val<bool> operator==(const T& other) const {
		auto res = val<T>(other);
		return *this == res;
	}

	val<bool> operator!=(val<T>& other) const {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto tc = tracing::traceBinaryOp(tracing::NEQ, Type::b, state, other.state);
			return val<bool>(tc);
		}
#endif
		return value != other.value;
	}

	val<bool> operator!=(const T& other) const {
		return *this == val<T>(other);
	}

	operator val<underlying_type_t>() const {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			// Mirrors val_arith.hpp's cross-type cast operator: `state` is a
			// const TypedValueRefHolder, but val<underlying_type_t>'s
			// trace-state constructor takes a non-const TypedValueRef&, so a
			// real CAST op (rather than reusing `state` directly) is needed to
			// get a bindable, non-const result reference.
			auto resultRef =
			    tracing::traceUnaryOp(tracing::CAST, tracing::TypeResolver<underlying_type_t>::to_type(), state);
			return val<underlying_type_t>(resultRef);
		}
#endif
		return val<underlying_type_t>(static_cast<std::underlying_type_t<T>>(value));
	}

	val<T>& operator=(const val<T>& other) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceAssignment(state, other.state, tracing::TypeResolver<underlying_type_t>::to_type());
		}
#endif
		this->value = other.value;
		return *this;
	}

#ifdef ENABLE_TRACING
	[[nodiscard]] tracing::TypedValueRef getState() const override {
		return state;
	}

	tracing::TypedValueRefHolder state;
#endif

private:
	friend details::RawValueResolver<T>;
	// Not const: operator=() above assigns through this member, which would
	// be ill-formed (and was, until this fix -- never previously exercised)
	// if it stayed const like the constructors' member-init-list style might
	// suggest.
	T value;
};

template <typename Type>
    requires std::is_enum_v<Type>
auto inline make_value(const Type& value) {
	return val<Type>(value);
}

} // namespace nautilus
