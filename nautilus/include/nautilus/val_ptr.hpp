
#pragma once

#include "nautilus/static.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/TypedValueRef.hpp"
#include "nautilus/tracing/Types.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_base.hpp"
#include "nautilus/val_concepts.hpp"
#include <cstdint>
#include <type_traits>
#include <utility>

namespace nautilus {

template <is_nautilus_ref ValueType>
class val<ValueType> : public val_base {
public:
	using baseType = std::remove_cvref_t<ValueType>;
	using ref_less_type = std::remove_reference_t<ValueType>;
	using ptrType = ref_less_type*;

	[[nodiscard]] Type getType() const override {
		return tracing::TypeResolver<baseType>::to_type();
	}

	[[nodiscard]] TypeId getTypeId() const override {
		return typeIdOf<val<ValueType>>();
	}

#ifdef ENABLE_TRACING
	[[nodiscard]] tracing::TypedValueRef getState() const override {
		return state;
	}

	const tracing::TypedValueRefHolder state;
#endif
#ifdef ENABLE_TRACING
	val(ValueType ref) : state(tracing::TypedValueRef()), ptr(&ref) {
	}
	val(ValueType ref, tracing::TypedValueRef TypedValueRef) : state(TypedValueRef), ptr(&ref) {
	}
	val(val<ptrType> ptr, tracing::TypedValueRef ref) : state(ref), ptr(ptr) {
	}
#else
	val(ValueType ref) : ptr(&ref) {
	}
	val(val<ptrType> ptr) : ptr(ptr) {
	}
#endif
	operator val<baseType>() {
		// load
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto& ref = tracing::traceUnaryOp(tracing::LOAD, tracing::TypeResolver<ValueType>::to_type(), ptr.state);
			return val<baseType>(ref);
		}
#endif
		auto rawPtr =
		    details::RawValueResolver<typename std::remove_cvref_t<decltype((ptr))>::raw_type>::getRawValue(ptr);
		return val<baseType>(*rawPtr);
	}

	template <class T>
	    requires std::is_convertible_v<T, baseType>
	void operator=(val<T> other) {
		val<baseType> value {other};

		// store value
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceBinaryOp(tracing::STORE, Type::v, ptr.state, value.state);
			return;
		}
#endif
		auto rawPtr =
		    details::RawValueResolver<typename std::remove_cvref_t<decltype((ptr))>::raw_type>::getRawValue(ptr);
		*rawPtr =
		    details::RawValueResolver<typename std::remove_cvref_t<decltype((value))>::raw_type>::getRawValue(value);
	}

	template <class T>
	    requires std::is_convertible_v<T, baseType>
	void operator=(T other) {
		val<baseType> value {val<T> {other}};
		*this = value;
	}

#define BINARY_AND_ASSIGN_OPERATOR(OP)                                                                                 \
	template <class T>                                                                                                 \
	    requires std::is_convertible_v<T, baseType>                                                                    \
	void operator OP##=(T other) noexcept {                                                                            \
		val<baseType> value {other};                                                                                   \
		*this OP## = value;                                                                                            \
	}                                                                                                                  \
	template <class T>                                                                                                 \
	    requires std::is_convertible_v<T, baseType>                                                                    \
	void operator OP##=(val<T> other) noexcept {                                                                       \
		val<baseType> value {other};                                                                                   \
		*this = *this OP value;                                                                                        \
	}                                                                                                                  \
                                                                                                                       \
	template <class T>                                                                                                 \
	    requires std::is_convertible_v<T, baseType>                                                                    \
	auto operator OP(T other) {                                                                                        \
		val<baseType> value {other};                                                                                   \
		return *this OP value;                                                                                         \
	}                                                                                                                  \
	template <class T>                                                                                                 \
	    requires std::is_convertible_v<T, baseType>                                                                    \
	auto operator OP(val<T> other) {                                                                                   \
		val<baseType> ourVal {*this};                                                                                  \
		return ourVal OP other;                                                                                        \
	}

	BINARY_AND_ASSIGN_OPERATOR(+)
	BINARY_AND_ASSIGN_OPERATOR(-)
	BINARY_AND_ASSIGN_OPERATOR(*)
	BINARY_AND_ASSIGN_OPERATOR(/)
	BINARY_AND_ASSIGN_OPERATOR(%)
	BINARY_AND_ASSIGN_OPERATOR(|)
	BINARY_AND_ASSIGN_OPERATOR(&)
	BINARY_AND_ASSIGN_OPERATOR(^)

#define BINARY_COMPARISON_OPERATOR(OP)                                                                                 \
	template <class T>                                                                                                 \
	    requires std::is_convertible_v<T, baseType>                                                                    \
	bool operator OP(T other) {                                                                                        \
		val<baseType> value {other};                                                                                   \
		return *this OP value;                                                                                         \
	}                                                                                                                  \
                                                                                                                       \
	template <class T>                                                                                                 \
	    requires std::is_convertible_v<T, baseType>                                                                    \
	bool operator OP(val<T> other) {                                                                                   \
		val<baseType> ourVal {*this};                                                                                  \
		return ourVal OP other;                                                                                        \
	}

	BINARY_COMPARISON_OPERATOR(<)
	BINARY_COMPARISON_OPERATOR(>)
	BINARY_COMPARISON_OPERATOR(<=)
	BINARY_COMPARISON_OPERATOR(>=)
	BINARY_COMPARISON_OPERATOR(==)
	BINARY_COMPARISON_OPERATOR(!=)

private:
	val<ptrType> ptr;
	friend val<ptrType>;
};

template <is_ptr ValuePtrType>
class base_ptr_val : public val_base {
public:
	using ValType = std::remove_pointer_t<ValuePtrType>;
	using raw_no_qualifiers = std::remove_cv_t<ValType>;
	using raw_type = ValuePtrType;
	using basic_type = std::remove_pointer_t<ValuePtrType>;
	using pointer_type = ValuePtrType;

#ifdef ENABLE_TRACING
	base_ptr_val() : state(tracing::traceConstant<void*>(nullptr)), value() {
	}
	base_ptr_val(ValuePtrType ptr) : state(tracing::traceConstant((void*) ptr)), value(ptr) {
	}
	base_ptr_val(ValuePtrType ptr, tracing::TypedValueRef tc) : state(tc), value(ptr) {
	}
	base_ptr_val(ValuePtrType ptr, tracing::TypedValueRefHolder tc) : state(std::move(tc)), value(ptr) {
	}

	base_ptr_val(tracing::TypedValueRef ref) : state(ref), value(nullptr) {
	}
#else
	base_ptr_val(ValuePtrType ptr) : value(ptr) {
	}
	base_ptr_val() : value() {
	}
#endif

	operator bool() const {
		return *this != static_cast<val<ValuePtrType>>(nullptr);
	}

	/// Conversion from pointer to arithmetic type (ptr→int, ptr→float).
	/// Treats the pointer value as a uintptr_t and casts to the target type.
	template <typename OtherType>
	    requires(is_arithmetic<OtherType> && !std::is_same_v<OtherType, bool>)
	operator val<OtherType>() const {
		if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
			auto resultRef = tracing::traceUnaryOp(tracing::CAST, tracing::TypeResolver<OtherType>::to_type(), state);
			return val<OtherType>(resultRef);
#endif
		}
		return val<OtherType>(static_cast<OtherType>(reinterpret_cast<uintptr_t>(value)));
	}

	[[nodiscard]] Type getType() const override {
		return Type::ptr;
	}

	[[nodiscard]] TypeId getTypeId() const override {
		return typeIdOf<val<ValuePtrType>>();
	}

#ifdef ENABLE_TRACING
	[[nodiscard]] tracing::TypedValueRef getState() const override {
		return state;
	}

	const tracing::TypedValueRefHolder state;
#endif

protected:
	template <typename ValueType>
	    requires std::is_pointer_v<ValueType>
	friend val<bool> inline operator==(val<ValueType> left, val<ValueType> right);
	template <typename ValueType>
	    requires std::is_pointer_v<ValueType>
	friend val<bool> inline operator==(val<ValueType> left, std::nullptr_t);
	template <typename ValueType>
	    requires std::is_pointer_v<ValueType>
	friend val<bool> inline operator==(std::nullptr_t, val<ValueType> right);
	template <typename ValueType>
	    requires std::is_pointer_v<ValueType>
	friend val<bool> inline operator<=(val<ValueType> left, val<ValueType> right);
	template <typename ValueType>
	    requires std::is_pointer_v<ValueType>
	friend val<bool> inline operator<(val<ValueType> left, val<ValueType> right);
	template <typename ValueType>
	    requires std::is_pointer_v<ValueType>
	friend val<bool> inline operator>(val<ValueType> left, val<ValueType> right);
	template <typename ValueType>
	    requires std::is_pointer_v<ValueType>
	friend val<bool> inline operator>=(val<ValueType> left, val<ValueType> right);
	template <typename ValueType>
	    requires std::is_pointer_v<ValueType>
	friend val<bool> inline operator!=(val<ValueType> left, val<ValueType> right);
	template <is_ptr ValueType, is_fundamental_val IndexType>
	friend val<ValueType> inline operator+(val<ValueType> left, IndexType offset);

	friend details::RawValueResolver<ValuePtrType>;
	friend val<ValType>;
	ValuePtrType value;
};

template <typename T, typename F>
std::size_t field_offset(F T::*pm) {
	alignas(T) std::byte storage[sizeof(T)] {};
	T* obj = std::launder(reinterpret_cast<T*>(storage));                       // ← reinterpret_cast: not constexpr
	return reinterpret_cast<char*>(&(obj->*pm)) - reinterpret_cast<char*>(obj); // ← same
}

template <is_ptr ValuePtrType>
class val<ValuePtrType> : public base_ptr_val<ValuePtrType> {

public:
	using base_ptr_val<ValuePtrType>::base_ptr_val;
	using ValType = typename base_ptr_val<ValuePtrType>::ValType;

	template <typename F, typename T = ValType>
	    requires std::is_class_v<T>
	auto get(F T::*pm) {
		auto offset = field_offset(pm);
		val<uint8_t*> bytePtr = static_cast<val<uint8_t*>>(*this);
		val<uint8_t*> fieldBytePtr = bytePtr + offset;
		val<F*> fieldPtr = static_cast<val<F*>>(fieldBytePtr);
#ifdef ENABLE_TRACING
		return val<F&>(fieldPtr, fieldPtr.state);
#else
		return val<F&>(fieldPtr);
#endif
	}

	template <typename F, typename T = ValType>
	    requires std::is_class_v<T>
	void set(F T::*pm, val<F> value) {
		val<F&> valueRef = get(pm);
		valueRef = value;
	}

	template <typename F, typename T = ValType>
	    requires std::is_class_v<T>
	void set(F T::*pm, F value) {
		val<F&> valueRef = get(pm);
		valueRef = value;
	}

#ifdef ENABLE_TRACING
	val(const val<ValuePtrType>& otherValue)
	    : base_ptr_val<ValuePtrType>(otherValue.value, tracing::traceCopy(otherValue.state)) {
	}
#else
	val(const val<ValuePtrType>& otherValue) : base_ptr_val<ValuePtrType>(otherValue.value) {
	}
#endif

	val<ValuePtrType>& operator=(const val<ValuePtrType>& other) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceAssignment(this->state, other.state, tracing::TypeResolver<ValuePtrType>::to_type());
		}
#endif
		this->value = other.value;
		return *this;
	}

	val<ValType&> operator*()
	    requires is_arithmetic<ValType> || is_ptr<ValType>
	{
#ifdef ENABLE_TRACING
		return val<ValType&>(*this, this->state);
#else
		return val<ValType&>(*this);
#endif
	}

	template <class T>
	val<ValType&> operator[](T&& io)
	    requires is_arithmetic<ValType> || is_ptr<ValType>
	{
		auto indexOffset = static_cast<val<int32_t>>(io);
		auto valuePtr = (*this) + indexOffset;
#ifdef ENABLE_TRACING
		return val<ValType&>(valuePtr, this->state);
#else
		return val<ValType&>(valuePtr);
#endif
	}

	template <typename OtherType>
	    requires std::is_pointer_v<OtherType>
	operator val<OtherType>() const {
		// ptr cast
#ifdef ENABLE_TRACING
		return val<OtherType>((OtherType) this->value, this->state);
#else
		return val<OtherType>((OtherType) this->value);
#endif
	}

	template <typename OtherType>
	    requires std::is_pointer_v<OtherType>
	operator val<const OtherType>() const {
		// ptr cast
#ifdef ENABLE_TRACING
		return val<OtherType>((OtherType) this->value, this->state);
#else
		return val<OtherType>((OtherType) this->value);
#endif
	}

	/// Conversion from pointer to arithmetic type (ptr→int, ptr→float).
	template <typename OtherType>
	    requires(is_arithmetic<OtherType> && !std::is_same_v<OtherType, bool>)
	operator val<OtherType>() const {
		if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
			auto resultRef =
			    tracing::traceUnaryOp(tracing::CAST, tracing::TypeResolver<OtherType>::to_type(), this->state);
			return val<OtherType>(resultRef);
#endif
		}
		return val<OtherType>(static_cast<OtherType>(reinterpret_cast<uintptr_t>(this->value)));
	}

	operator bool() const {
		return *this != static_cast<val<ValuePtrType>>(nullptr);
	}

	template <typename IndexType>
	    requires is_integral<IndexType> || is_fundamental_val<IndexType>
	val<ValuePtrType>& operator+=(IndexType offset) {
		*this = *this + offset;
		return *this;
	}

	template <typename IndexType>
	    requires is_integral<IndexType> || is_fundamental_val<IndexType>
	val<ValuePtrType>& operator-=(IndexType offset) {
		*this = *this - offset;
		return *this;
	}

	val<ValuePtrType>& operator++() {
		*this += static_cast<int32_t>(1);
		return *this;
	}
};

template <is_void_ptr ValuePtrType>
class val<ValuePtrType> : public base_ptr_val<ValuePtrType> {
public:
	using base_ptr_val<ValuePtrType>::base_ptr_val;

#ifdef ENABLE_TRACING
	val(const val<ValuePtrType>& otherValue)
	    : base_ptr_val<ValuePtrType>(otherValue.value, tracing::traceCopy(otherValue.state)) {
	}
#else
	val(const val<ValuePtrType>& otherValue) : base_ptr_val<ValuePtrType>(otherValue.value) {
	}
#endif

	val<ValuePtrType>& operator=(const val<ValuePtrType>& other) {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceAssignment(this->state, other.state, tracing::TypeResolver<ValuePtrType>::to_type());
		}
#endif
		this->value = other.value;
		return *this;
	}

	template <typename OtherType>
	    requires std::is_pointer_v<OtherType>
	operator val<OtherType>() const {
		// ptr cast
#ifdef ENABLE_TRACING
		return val<OtherType>((OtherType) this->value, this->state);
#else
		return val<OtherType>((OtherType) this->value);
#endif
	}

	/// Conversion from pointer to arithmetic type (ptr→int, ptr→float).
	template <typename OtherType>
	    requires(is_arithmetic<OtherType> && !std::is_same_v<OtherType, bool>)
	operator val<OtherType>() const {
		if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
			auto resultRef =
			    tracing::traceUnaryOp(tracing::CAST, tracing::TypeResolver<OtherType>::to_type(), this->state);
			return val<OtherType>(resultRef);
#endif
		}
		return val<OtherType>(static_cast<OtherType>(reinterpret_cast<uintptr_t>(this->value)));
	}

	operator bool() const {
		return *this != static_cast<val<ValuePtrType>>(nullptr);
	}
};

template <is_ptr ValueType, is_fundamental_val IndexType>
val<ValueType> inline operator+(val<ValueType> left, IndexType offset) {
	auto offsetValue = make_value(offset);
	auto size = ((size_t) (sizeof(typename std::remove_pointer_t<ValueType>)));
	auto offsetBytes = offsetValue * size;
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto tc = tracing::traceBinaryOp(tracing::ADD, tracing::TypeResolver<ValueType>::to_type(), left.state,
		                                 offsetBytes.state);
		return val<ValueType>(tc);
	}
#endif
	auto newPtr =
	    (ValueType) (((uint8_t*) left.value) +
	                 details::RawValueResolver<
	                     typename std::remove_cvref_t<decltype((offsetBytes))>::raw_type>::getRawValue(offsetBytes));
	return val<ValueType>(newPtr);
}

template <is_ptr ValueType, is_integral IndexType>
val<ValueType> inline operator+(val<ValueType>& left, IndexType offset) {
	return left + val<size_t>(offset);
}

template <is_ptr ValueType, is_integral IndexType>
val<ValueType> inline operator+(val<ValueType>& left, static_val<IndexType> offset) {
	return left + static_cast<IndexType>(offset);
}

template <is_ptr ValueType, typename IndexType>
    requires is_integral<IndexType> || is_fundamental_val<IndexType>
val<ValueType> inline operator-(val<ValueType>& left, IndexType&& offset) {
	return left + (0 - offset);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
val<bool> inline operator==(val<ValueType> left, val<ValueType> right) {

#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto tc = tracing::traceBinaryOp(tracing::EQ, Type::b, left.state, right.state);
		return val<bool>(tc);
	}
#endif
	return val<bool>(left.value == right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
val<bool> inline operator==(val<ValueType> left, std::nullptr_t) {
	auto nullVal = val<ValueType>(NULL);
	return left == nullVal;
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
val<bool> inline operator==(std::nullptr_t, val<ValueType> right) {
	auto nullVal = val<ValueType>(NULL);
	return nullVal == right;
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
val<bool> inline operator<=(val<ValueType> left, val<ValueType> right) {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto tc = tracing::traceBinaryOp(tracing::LTE, Type::b, left.state, right.state);
		return val<bool>(tc);
	}
#endif
	return val<bool>(left.value <= right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
val<bool> inline operator<(val<ValueType> left, val<ValueType> right) {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto tc = tracing::traceBinaryOp(tracing::LT, Type::b, left.state, right.state);
		return val<bool>(tc);
	}
#endif
	return val<bool>(left.value < right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
val<bool> inline operator>(val<ValueType> left, val<ValueType> right) {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto tc = tracing::traceBinaryOp(tracing::GT, Type::b, left.state, right.state);
		return val<bool>(tc);
	}
#endif
	return val<bool>(left.value > right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
val<bool> inline operator>=(val<ValueType> left, val<ValueType> right) {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto tc = tracing::traceBinaryOp(tracing::GTE, Type::b, left.state, right.state);
		return val<bool>(tc);
	}
#endif
	return val<bool>(left.value >= right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
val<bool> inline operator!=(val<ValueType> left, val<ValueType> right) {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto tc = tracing::traceBinaryOp(tracing::NEQ, Type::b, left.state, right.state);
		return val<bool>(tc);
	}
#endif
	return val<bool>(left.value != right.value);
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator!=(val<ValueType> left, std::nullptr_t) {
	auto nullVal = val<ValueType>(NULL);
	return left != nullVal;
}

template <typename ValueType>
    requires std::is_pointer_v<ValueType>
auto inline operator!=(std::nullptr_t, val<ValueType> right) {
	auto nullVal = val<ValueType>(NULL);
	return nullVal != right;
}

template <>
class val<bool&> : public val_base {
public:
	using baseType = std::remove_cvref_t<bool&>;
	using ref_less_type = std::remove_reference_t<bool&>;
	using ptrType = ref_less_type*;

	[[nodiscard]] Type getType() const override {
		return Type::b;
	}

	[[nodiscard]] TypeId getTypeId() const override {
		return typeIdOf<val<bool&>>();
	}

#ifdef ENABLE_TRACING
	[[nodiscard]] tracing::TypedValueRef getState() const override {
		return state;
	}

	tracing::TypedValueRefHolder state;
	val(bool ref) : state(tracing::TypedValueRef()), ptr(&ref) {
	}
	val(bool& ref, tracing::TypedValueRef TypedValueRef) : state(TypedValueRef), ptr(&ref) {
	}
	val(val<ptrType> ptr, tracing::TypedValueRef ref) : state(ref), ptr(ptr) {
	}
#else
	val(bool ref) : ptr(&ref) {
	}
	val(val<ptrType> ptr) : ptr(ptr) {
	}
#endif

	template <class T>
	    requires std::is_convertible_v<T, baseType>
	void operator=(T other) {
		val<baseType> value {other};
		*this = value;
	}

	template <class T>
	    requires std::is_convertible_v<T, baseType>
	void operator=(val<T> other) {
		val<baseType> value {other};

		// store value
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			tracing::traceBinaryOp(tracing::STORE, Type::v, ptr.state, value.state);
			return;
		}
#endif
		auto rawPtr = details::RawValueResolver<bool*>::getRawValue(ptr);
		*rawPtr =
		    details::RawValueResolver<typename std::remove_cvref_t<decltype((value))>::raw_type>::getRawValue(value);
	}

	operator val<baseType>() {
		// load
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto& ref = tracing::traceUnaryOp(tracing::LOAD, tracing::TypeResolver<baseType>::to_type(), ptr.state);
			return val<baseType>(ref);
		}
#endif
		auto rawPtr = details::RawValueResolver<bool*>::getRawValue(ptr);
		return val<baseType>(*rawPtr);
	}

	operator bool() {
		val<bool> boolValue = *this;
		return boolValue;
	}

private:
	val<bool*> ptr;
	friend val<ptrType>;
};

} // namespace nautilus
