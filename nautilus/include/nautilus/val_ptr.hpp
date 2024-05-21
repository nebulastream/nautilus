
#pragma once

#include "nautilus/val.hpp"

namespace nautilus {

template <is_fundamental_ref ValueType>
class val<ValueType> : public base_value {
public:
	using baseType = std::remove_cvref_t<ValueType>;
	using ref_less_type = std::remove_reference_t<ValueType>;
	using ptrType = ref_less_type*;

#ifdef ENABLE_TRACING

	val(ValueType ref) : base_value(tracing::value_ref()), ptr(&ref), alignment(0) {};

	val(ValueType ref, tracing::value_ref value_ref) : base_value(value_ref), ptr(&ref), alignment(0) {};

	val(val<ptrType> ptr, tracing::value_ref ref, int8_t alignment)
	    : base_value(ref), ptr(ptr), alignment(alignment) {};
#else

	val(ValueType ref) : base_value(), ptr(&ref), alignment(0) {};

	val(val<ptrType> ptr, int8_t alignment) : base_value(), ptr(ptr), alignment(alignment) {};
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
class val<ValuePtrType> : public base_value {
public:
	using ValType = std::remove_pointer_t<ValuePtrType>;
	using raw_type = ValuePtrType;
	using basic_type = std::remove_pointer_t<ValuePtrType>;
	using pointer_type = ValuePtrType;

#ifdef ENABLE_TRACING

	val(ValuePtrType ptr, int8_t alignment = 1)
	    : base_value(tracing::traceConstant(ptr)), value(ptr), alignment(alignment) {};

	inline val(ValuePtrType, tracing::value_ref tc, int8_t alignment) : base_value(tc), alignment(alignment) {};

	inline val(const val<ValuePtrType>& otherValue) : base_value(otherValue.state), value(otherValue.value) {
	}

#else

	val(ValuePtrType ptr, int8_t alignment = 1) : base_value(), value(ptr), alignment(alignment) {};
	val(const val<ValuePtrType>& otherValue) : base_value(), value(otherValue.value) {
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
		return val<OtherType>((OtherType) value, alignment);
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

template <typename T, typename M>
M get_member_type(M T::*);

template <typename T, typename M>
T get_class_type(M T::*);

template <typename T, typename R, R T::*M>
std::size_t offset_of() {
	return reinterpret_cast<std::size_t>(&(((T*) 0)->*M));
}

#define OFFSET_OF(m) offset_of<decltype(get_class_type(m)), decltype(get_member_type(m)), m>()

template <is_member_ptr2 pointer_type>
class val<pointer_type> : public base_value {
public:
#ifdef ENABLE_TRACING

	val(pointer_type ptr) : base_value(tracing::value_ref()), value(ptr) {
	}

	val(pointer_type ptr, tracing::value_ref ref) : base_value(ref), value(ptr) {
	}

#else

	val(pointer_type ptr) : base_value(), value(ptr) {
	}

	val(pointer_type ptr, tracing::value_ref) : base_value(), value(ptr) {
	}

#endif

	val<pointer_type>& operator=(const val<pointer_type>& other) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceAssignment(state, other.state, tracing::to_type<pointer_type>());
		}
#endif
		this->value = other.value;
		return *this;
	};

protected:
	pointer_type value;

	friend pointer_type details::getRawValue<pointer_type>(val<pointer_type>& left);
};

/*
template<is_member_ptr ValuePtrType>
class val<ValuePtrType> : public base_value {
public:
    using ValType = std::remove_pointer_t<ValuePtrType>;


    val(ValuePtrType ptr, val<size_t> index = val<size_t>((size_t) 0), int8_t alignment = 1) : ptr(ptr),
                                                                                               index(index),
                                                                                               alignment(
                                                                                                       alignment) {};

    inline val<ValuePtrType>(tracing::value_ref *tc, val<size_t> index,
                             int8_t alignment = 1) : state(tc),
                                                     index(index),
                                                     alignment(alignment) {};

    template<typename R>
    val<R &> get(R ValType::*ptiptr) {
        [[maybe_unused]] auto offset = reinterpret_cast<std::size_t>(&(((ValType *) 0)->*ptiptr));
        auto memPtr = ((int8_t *) ptr) + offset;
        return val<R &>((R *) memPtr, index, alignment);
    }

    template<typename TP, typename... ValueArguments>
    auto
    invoke(TP ValType::* __pm, ValueArguments... args) {
        auto member = MemberFunction(__pm);
        return member(ptr, args...);
    }

    ValuePtrType ptr;
    val<size_t> index;
    int8_t alignment;

#ifdef ENABLE_TRACING
    tracing::value_ref *state;
#endif
};
 */

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
