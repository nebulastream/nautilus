
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
	void operator OP## = (T other) noexcept {                                                                          \
		val<baseType> value {other};                                                                                   \
		*this OP## = value;                                                                                            \
	}                                                                                                                  \
	template <class T>                                                                                                 \
	    requires std::is_convertible_v<T, baseType>                                                                    \
	void operator OP## = (val<T> other) noexcept {                                                                     \
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

template <typename T>
struct is_static_val_type : std::false_type {};

template <typename T>
struct is_static_val_type<static_val<T>> : std::true_type {};

template <typename T>
concept is_static_val = is_static_val_type<std::remove_cvref_t<T>>::value;

template <typename T>
struct is_integral_ref_val_type : std::false_type {};

template <typename T>
struct is_integral_ref_val_type<val<T&>> : std::bool_constant<std::is_integral_v<std::remove_cv_t<T>>> {};

template <typename T>
concept is_integral_ref_val = is_integral_ref_val_type<std::remove_cvref_t<T>>::value;

/// Types a pointer can be offset by: like the built-in operators, only integral values.
template <typename T>
concept ptr_offset =
    is_integral<T> || is_integral_val<std::remove_cvref_t<T>> || is_static_val<T> || is_integral_ref_val<T>;

namespace details {
template <is_ptr ValueType, is_integral_val IndexType>
val<ValueType> ptrAdd(val<ValueType> left, IndexType offset);
} // namespace details

template <is_ptr ValuePtrType>
class base_ptr_val : public val_base {
public:
	using ValType = std::remove_pointer_t<ValuePtrType>;
	using raw_no_qualifiers = std::remove_cv_t<ValType>;
	using raw_type = ValuePtrType;
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

	// Intentionally no implicit `operator bool()`. Converting a pointer to a native
	// bool materializes the null test into a control-flow branch (traceBool) that can
	// be split from the dereference it guards -- in particular across loop back-edges
	// where the pointer is reassigned -- leaving an unchecked load that LLVM assumes
	// non-null and miscompiles at -O3. Use explicit `== nullptr` / `!= nullptr`, which
	// stay symbolic `val<bool>` data values feeding a single condition branch.

	/// Conversion from pointer to arithmetic type (ptr→int, ptr→float).
	/// Treats the pointer value as a uintptr_t and casts to the target type.
	/// Explicit, like the reinterpret_cast it models: an implicit conversion would let pointers silently
	/// take part in integer arithmetic and comparisons.
	template <typename OtherType>
	    requires(is_arithmetic<OtherType> && !std::is_same_v<OtherType, bool>)
	explicit operator val<OtherType>() const {
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
	template <is_ptr ValueType, is_integral_val IndexType>
	friend val<ValueType> inline details::ptrAdd(val<ValueType> left, IndexType offset);

	friend details::RawValueResolver<ValuePtrType>;
	friend val<ValType>;
	ValuePtrType value;
};

template <typename T, typename F>
std::size_t field_offset(F T::* pm) {
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
	auto get(F T::* pm) {
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
	void set(F T::* pm, val<F> value) {
		val<F&> valueRef = get(pm);
		valueRef = value;
	}

	template <typename F, typename T = ValType>
	    requires std::is_class_v<T>
	void set(F T::* pm, F value) {
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

	val<ValType&> operator*() const
	    requires is_arithmetic<ValType> || is_ptr<ValType>
	{
#ifdef ENABLE_TRACING
		return val<ValType&>(*this, this->state);
#else
		return val<ValType&>(*this);
#endif
	}

	template <ptr_offset IndexType>
	val<ValType&> operator[](IndexType&& index) const
	    requires is_arithmetic<ValType> || is_ptr<ValType>
	{
		auto valuePtr = (*this) + std::forward<IndexType>(index);
#ifdef ENABLE_TRACING
		return val<ValType&>(valuePtr, this->state);
#else
		return val<ValType&>(valuePtr);
#endif
	}

	template <typename OtherType>
	    requires std::is_pointer_v<OtherType>
	explicit(!std::is_convertible_v<ValuePtrType, OtherType>) operator val<OtherType>() const {
		// ptr cast
#ifdef ENABLE_TRACING
		return val<OtherType>((OtherType) this->value, this->state);
#else
		return val<OtherType>((OtherType) this->value);
#endif
	}

	template <typename OtherType>
	    requires std::is_pointer_v<OtherType>
	explicit(!std::is_convertible_v<ValuePtrType, OtherType>) operator val<const OtherType>() const {
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
	explicit operator val<OtherType>() const {
		if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
			auto resultRef =
			    tracing::traceUnaryOp(tracing::CAST, tracing::TypeResolver<OtherType>::to_type(), this->state);
			return val<OtherType>(resultRef);
#endif
		}
		return val<OtherType>(static_cast<OtherType>(reinterpret_cast<uintptr_t>(this->value)));
	}

	// Intentionally no implicit `operator bool()`. Converting a pointer to a native
	// bool materializes the null test into a control-flow branch (traceBool) that can
	// be split from the dereference it guards -- in particular across loop back-edges
	// where the pointer is reassigned -- leaving an unchecked load that LLVM assumes
	// non-null and miscompiles at -O3. Use explicit `== nullptr` / `!= nullptr`, which
	// stay symbolic `val<bool>` data values feeding a single condition branch.

	template <ptr_offset IndexType>
	val<ValuePtrType>& operator+=(IndexType&& offset) {
		*this = *this + std::forward<IndexType>(offset);
		return *this;
	}

	template <ptr_offset IndexType>
	val<ValuePtrType>& operator-=(IndexType&& offset) {
		*this = *this - std::forward<IndexType>(offset);
		return *this;
	}

	val<ValuePtrType>& operator++() {
		*this += static_cast<int32_t>(1);
		return *this;
	}

	val<ValuePtrType>& operator--() {
		*this -= static_cast<int32_t>(1);
		return *this;
	}

	val<ValuePtrType> operator++(int) {
		val<ValuePtrType> old = *this;
		++*this;
		return old;
	}

	val<ValuePtrType> operator--(int) {
		val<ValuePtrType> old = *this;
		--*this;
		return old;
	}

	val<ValuePtrType> operator+() const {
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

	/// Pointer casts are implicit only where the raw pointer conversion is (T* -> const T*, T* -> void*,
	/// Derived* -> Base*); every other cast has to be spelled out, e.g. static_cast<val<U*>>(ptr).
	template <typename OtherType>
	    requires std::is_pointer_v<OtherType>
	explicit(!std::is_convertible_v<ValuePtrType, OtherType>) operator val<OtherType>() const {
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
	explicit operator val<OtherType>() const {
		if SHOULD_TRACE () {
#ifdef ENABLE_TRACING
			auto resultRef =
			    tracing::traceUnaryOp(tracing::CAST, tracing::TypeResolver<OtherType>::to_type(), this->state);
			return val<OtherType>(resultRef);
#endif
		}
		return val<OtherType>(static_cast<OtherType>(reinterpret_cast<uintptr_t>(this->value)));
	}

	// Intentionally no implicit `operator bool()`. Converting a pointer to a native
	// bool materializes the null test into a control-flow branch (traceBool) that can
	// be split from the dereference it guards -- in particular across loop back-edges
	// where the pointer is reassigned -- leaving an unchecked load that LLVM assumes
	// non-null and miscompiles at -O3. Use explicit `== nullptr` / `!= nullptr`, which
	// stay symbolic `val<bool>` data values feeding a single condition branch.

	val<ValuePtrType> operator+() const {
		return *this;
	}
};

namespace details {
/// Core of all pointer arithmetic: advances `left` by `offset` elements.
template <is_ptr ValueType, is_integral_val IndexType>
val<ValueType> inline ptrAdd(val<ValueType> left, IndexType offset) {
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

/// Integral offset as a traced value; mirrors the conversions the built-in pointer operators apply.
template <typename IndexType>
auto inline ptrOffsetValue(IndexType&& offset) {
	using Index = std::remove_cvref_t<IndexType>;
	if constexpr (is_static_val<Index>) {
		return val<size_t>(static_cast<typename Index::raw_type>(offset));
	} else if constexpr (is_integral_ref_val<Index>) {
		return val<typename Index::baseType>(offset);
	} else {
		return val<size_t>(offset);
	}
}
} // namespace details

// Pointer arithmetic follows the built-in rules: ptr + n, n + ptr, ptr - n and ptr - ptr, with n integral
// (including bool, integral vals and static_vals); arithmetic on void pointers is ill-formed.
template <is_ptr ValueType, ptr_offset IndexType>
    requires(!is_void_ptr<ValueType>)
val<ValueType> inline operator+(const val<ValueType>& left, IndexType&& offset) {
	if constexpr (is_integral_val<std::remove_cvref_t<IndexType>>) {
		return details::ptrAdd(left, offset);
	} else {
		return details::ptrAdd(left, details::ptrOffsetValue(std::forward<IndexType>(offset)));
	}
}

template <is_ptr ValueType, ptr_offset IndexType>
    requires(!is_void_ptr<ValueType>)
val<ValueType> inline operator+(IndexType&& offset, const val<ValueType>& right) {
	return right + std::forward<IndexType>(offset);
}

template <is_ptr ValueType, ptr_offset IndexType>
    requires(!is_void_ptr<ValueType>)
val<ValueType> inline operator-(const val<ValueType>& left, IndexType&& offset) {
	using Index = std::remove_cvref_t<IndexType>;
	if constexpr (is_integral_val<Index>) {
		return details::ptrAdd(left, 0 - offset);
	} else if constexpr (is_static_val<Index>) {
		return details::ptrAdd(left, details::ptrOffsetValue(0 - static_cast<typename Index::raw_type>(offset)));
	} else if constexpr (is_integral_ref_val<Index>) {
		return details::ptrAdd(left, 0 - val<typename Index::baseType>(offset));
	} else {
		return details::ptrAdd(left, details::ptrOffsetValue(0 - offset));
	}
}

/// Pointer difference in elements; both operands must point to the same type (ignoring cv-qualifiers).
template <is_ptr LeftType, is_ptr RightType>
    requires(!is_void_ptr<LeftType> && std::is_same_v<std::remove_cv_t<std::remove_pointer_t<LeftType>>,
                                                      std::remove_cv_t<std::remove_pointer_t<RightType>>>)
val<std::ptrdiff_t> inline operator-(const val<LeftType>& left, const val<RightType>& right) {
	auto byteDiff = static_cast<val<std::ptrdiff_t>>(left) - static_cast<val<std::ptrdiff_t>>(right);
	return byteDiff / static_cast<std::ptrdiff_t>(sizeof(std::remove_pointer_t<LeftType>));
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

namespace details {
/// Composite pointer type of two distinct pointer types that the built-in operators may compare
/// (e.g. T* and const T*, T* and void*, Derived* and Base*).
template <typename LeftType, typename RightType>
using composite_ptr_t = decltype(true ? std::declval<LeftType>() : std::declval<RightType>());

template <typename LeftType, typename RightType>
concept comparable_ptrs = is_ptr<LeftType> && is_ptr<RightType> && !std::is_same_v<LeftType, RightType> &&
                          requires { typename composite_ptr_t<LeftType, RightType>; };
} // namespace details

#define DEFINE_MIXED_PTR_COMPARISON(OP)                                                                                \
	template <typename LeftType, typename RightType>                                                                   \
	    requires details::comparable_ptrs<LeftType, RightType>                                                         \
	val<bool> inline operator OP(const val<LeftType>& left, const val<RightType>& right) {                             \
		using Composite = details::composite_ptr_t<LeftType, RightType>;                                               \
		return static_cast<val<Composite>>(left) OP static_cast<val<Composite>>(right);                                \
	}

DEFINE_MIXED_PTR_COMPARISON(==)
DEFINE_MIXED_PTR_COMPARISON(!=)
DEFINE_MIXED_PTR_COMPARISON(<)
DEFINE_MIXED_PTR_COMPARISON(<=)
DEFINE_MIXED_PTR_COMPARISON(>)
DEFINE_MIXED_PTR_COMPARISON(>=)

#undef DEFINE_MIXED_PTR_COMPARISON

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
