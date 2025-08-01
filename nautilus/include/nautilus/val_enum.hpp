#pragma once
#include "nautilus/config.hpp"
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
class val<T> {
public:
	using underlying_type_t = std::underlying_type_t<T>;
	using raw_type = underlying_type_t;
	using basic_type = T;

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
			return val<underlying_type_t>(state);
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
	tracing::TypedValueRefHolder state;
#endif

private:
	friend details::RawValueResolver<T>;
	const T value;
};

template <typename Type>
    requires std::is_enum_v<Type>
auto inline make_value(const Type& value) {
	return val<Type>(value);
}

} // namespace nautilus
