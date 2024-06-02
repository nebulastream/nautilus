#pragma once

#include "nautilus/config.hpp"
#include "nautilus/val_concepts.hpp"
#include <iostream>
#include <memory>

#ifdef ENABLE_TRACING

#include "common/traceing.hpp"

#endif

namespace nautilus {

template <typename T>
    requires std::is_enum_v<T>
class val<T> {
public:
	using underlying_type_t = std::underlying_type_t<T>;
	using raw_type = underlying_type_t;
	using basic_type = T;

	val() : value() {};

#ifdef ENABLE_TRACING
	val(val<underlying_type_t> t) : state(t.state), value((T) t.value) {};
	val(val<T>& t) : state(tracing::traceCopy(t.state)), value(t.value) {};
	val(T val) : state(tracing::traceConstant((underlying_type_t) val)), value(val) {};
	const tracing::value_ref state;
#else
	val(val<underlying_type_t> t) : value((T) t.value) {};
	val(val<T>& t) : value(t.value) {};
	val(T val) : value(val) {};
#endif

	operator val<underlying_type_t>() {
		return value;
	}

	val<T>& operator=(const val<T>& other) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceAssignment(state, other.state, tracing::to_type<underlying_type_t>());
		}
#endif
		this->value = other.value;
		return *this;
	};

	T value;
};

} // namespace nautilus
