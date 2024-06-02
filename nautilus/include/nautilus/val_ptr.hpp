
#pragma once

#include "nautilus/function.hpp"
#include "nautilus/val.hpp"

namespace nautilus {

template <is_fundamental_ref ValueType>
class val<ValueType> {
public:
	using baseType = std::remove_cvref_t<ValueType>;
	using ref_less_type = std::remove_reference_t<ValueType>;
	using ptrType = ref_less_type*;

#ifdef ENABLE_TRACING
	tracing::value_ref state;
	val(ValueType ref) : state(tracing::value_ref()), ptr(&ref), alignment(0) {};

	val(ValueType ref, tracing::value_ref value_ref) : state(value_ref), ptr(&ref), alignment(0) {};

	val(val<ptrType> ptr, tracing::value_ref ref, int8_t alignment) : state(ref), ptr(ptr), alignment(alignment) {};
#else

	val(ValueType ref) : ptr(&ref), alignment(0) {};

	val(val<ptrType> ptr, int8_t alignment) : ptr(ptr), alignment(alignment) {};
#endif

	template <class T>
	    requires std::is_convertible_v<T, baseType>
	void operator=(T other) noexcept {
		auto value = make_value<baseType>(other);

		// store value
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceStore(state, value.state, tracing::to_type<ValueType>());
			return;
		}
#endif
		auto rawPtr = details::getRawValue(ptr);
		*rawPtr = details::getRawValue(value);
	}

	template <class T>
	    requires std::is_convertible_v<T, baseType>
	void operator=(val<T> other) noexcept {
		// store value
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceStore(ptr.state, other.state, tracing::to_type<ValueType>());
			return;
		}
#endif
		auto rawPtr = details::getRawValue(ptr);
		*rawPtr = details::getRawValue(other);
	}

	operator val<baseType>() {
		// load
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto ref = tracing::traceLoad(ptr.state, tracing::to_type<ValueType>());
			return val<baseType>(ref);
		}
#endif
		auto rawPtr = details::getRawValue(ptr);
		return val<baseType>(*rawPtr);
	}

private:
	val<ptrType> ptr;
	const int8_t alignment;
	friend val<ptrType>;
};

template <is_fundamental_ptr ValuePtrType>
class val<ValuePtrType> {
public:
	using ValType = std::remove_pointer_t<ValuePtrType>;
	using raw_type = ValuePtrType;
	using basic_type = std::remove_pointer_t<ValuePtrType>;
	using pointer_type = ValuePtrType;

#ifdef ENABLE_TRACING
	tracing::value_ref state;
	val() : value(), alignment() {};
	val(ValuePtrType ptr, int8_t alignment = 1) : value(ptr), alignment(alignment) {};

	inline val(ValuePtrType ptr, tracing::value_ref tc, int8_t alignment) : state(tc),  value(ptr), alignment(alignment) {};

	inline val(const val<ValuePtrType>& otherValue) : state(otherValue.state), value(otherValue.value) {
	}

	val(tracing::value_ref ref) : state(ref), value(nullptr) {
	}

#else

	val(ValuePtrType ptr, int8_t alignment = 1) :  value(ptr), alignment(alignment) {};
	val(const val<ValuePtrType>& otherValue) :  value(otherValue.value) {
	}
#endif

	val<ValuePtrType>& operator=(const val<ValuePtrType>& other) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceAssignment(state, other.state, tracing::to_type<ValuePtrType>());
		}
#endif
		this->value = other.value;
		return *this;
	};

	val<ValType&> operator*() {
#ifdef ENABLE_TRACING
		return val<ValType&>(*this, state, alignment);
#else
		return val<ValType&>(*this, alignment);
#endif
	};

	val<ValType&> operator[](val<int32_t> indexOffset) {
		auto valuePtr = (*this) + indexOffset;
#ifdef ENABLE_TRACING
		return val<ValType&>(valuePtr, state, alignment);
#else
		return val<ValType&>(valuePtr, alignment);
#endif
	}

	template <typename OtherType>
	    requires std::is_pointer_v<OtherType>
	operator val<OtherType>() const {
		// ptr cast
#ifdef ENABLE_TRACING
		return val<OtherType>((OtherType) value, state, alignment);
#else
		return val<OtherType>((OtherType) value, alignment);
#endif
	}

	const val<ValuePtrType>& operator++() {
		// increment
		++value;
		// assign
		return *this;
	}

	ValuePtrType value;
	int8_t alignment;
};

template <class T>
concept is_void_ptr = ((std::is_void_v<std::remove_pointer_t<T>> && std::is_pointer_v<T>) );

template <is_void_ptr T>
class val<T>  {
public:
	using ValType = std::remove_pointer_t<T>;
	using raw_type = T;
	using basic_type = std::remove_pointer_t<T>;
	using pointer_type = T;

#ifdef ENABLE_TRACING
	tracing::value_ref state;
	val(T ptr, int8_t alignment = 1) : state(tracing::traceConstant(ptr)), value(ptr), alignment(alignment) {};

	inline val(T, tracing::value_ref tc, int8_t alignment) : state(tc), alignment(alignment) {};

	inline val(val<T>& otherValue) : state(otherValue.state), value(otherValue.value) {
	}

	val(tracing::value_ref var) : state(var), value(nullptr) {
	}
#else
	val(T ptr, int8_t alignment = 1) : value(ptr), alignment(alignment) {};

	val(tracing::value_ref) :  value(nullptr) {
	}
#endif

	T value;
	int8_t alignment;
};

template <typename T, typename M>
M get_member_type(M T::*);

template <typename T, typename M>
T get_class_type(M T::*);

template <typename T, typename R, R T::*M>
std::size_t offset_of() {
	return reinterpret_cast<std::size_t>(&(((T*) 0)->*M));
}

template <typename ValueType, typename IndexType>
    requires std::is_pointer_v<ValueType>
val<ValueType> inline operator+(val<ValueType> left, IndexType offset) {
	auto offsetValue = make_value(offset);
	auto size = ((typename IndexType::raw_type)(sizeof(typename IndexType::raw_type)));
	auto offsetBytes = offsetValue * size;
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto tc = tracing::traceBinaryOp<tracing::ADD, ValueType>(left.state, offsetBytes.state);
		return val<ValueType>(nullptr, tc, 0);
	}
#endif
	auto newPtr = (ValueType) (((uint8_t*) left.value) + details::getRawValue(offsetBytes));
	return val<ValueType>(newPtr, left.alignment);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator-(val<ValueType> left, std::remove_pointer_t<ValueType> right) {
	return val<ValueType>(left.ptr, left.index - right, left.alignment);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator==(val<ValueType> left, val<ValueType> right) {
	return val<bool>(left.value == right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator<=(val<ValueType> left, val<ValueType> right) {
	return val<bool>(left.value <= right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator<(val<ValueType> left, val<ValueType> right) {
	return val<bool>(left.value < right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator>(val<ValueType> left, val<ValueType> right) {
	return val<bool>(left.value > right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator>=(val<ValueType> left, val<ValueType> right) {
	return val<bool>(left.value >= right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator!=(val<ValueType> left, val<ValueType> right) {
	return val<bool>(left.value != right.value);
}

template <int8_t alignment, typename ValuePtrType>
    requires std::is_pointer_v<ValuePtrType>
val<ValuePtrType> assume_aligned(val<ValuePtrType> ptr) {
	return val<ValuePtrType>(ptr.value, alignment);
}

} // namespace nautilus
