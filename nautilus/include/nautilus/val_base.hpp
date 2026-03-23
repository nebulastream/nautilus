#pragma once

#include "nautilus/tracing/Types.hpp"
#include <cassert>
#include <type_traits>

#ifdef ENABLE_TRACING
#include "nautilus/tracing/TypedValueRef.hpp"
#endif

namespace nautilus {

// Forward declaration
template <typename T>
class val;

/// Base interface for all val<T> specializations.
///
/// Provides a common polymorphic type that can be used to hold
/// pointers or references to any val<T> instance, regardless of
/// the underlying type T. This enables type-erased collections
/// and generic algorithms over heterogeneous val types.
///
/// # Type checking (member helpers)
/// ```cpp
/// val_base* v = &someVal;
/// if (v->isBool()) { ... }
/// if (v->isInteger()) { ... }
/// ```
///
/// # LLVM-style casting (free functions)
/// ```cpp
/// val<int32_t> intVal = 42;
/// val_base* v = &intVal;
///
/// if (isa<val<int32_t>>(v)) { ... }
/// auto* p = dyn_cast<val<int32_t>>(v);  // returns nullptr if wrong type
/// auto& r = cast<val<int32_t>>(*v);     // asserts correct type
/// ```
class val_base {
public:
	val_base() = default;
	val_base(const val_base&) = default;
	val_base(val_base&&) = default;
	val_base& operator=(const val_base&) = default;
	val_base& operator=(val_base&&) = default;
	virtual ~val_base() = default;

	/// Returns the runtime nautilus::Type of this value.
	[[nodiscard]] virtual Type getType() const = 0;

	/// Checks if this value has the given runtime type.
	[[nodiscard]] bool isType(Type t) const {
		return getType() == t;
	}

	/// Returns true if this value is a boolean.
	[[nodiscard]] bool isBool() const {
		return getType() == Type::b;
	}

	/// Returns true if this value is a signed or unsigned integer.
	[[nodiscard]] bool isInteger() const {
		return nautilus::isInteger(getType());
	}

	/// Returns true if this value is a floating-point type (f32 or f64).
	[[nodiscard]] bool isFloat() const {
		return nautilus::isFloat(getType());
	}

	/// Returns true if this value is a pointer type.
	[[nodiscard]] bool isPointer() const {
		return getType() == Type::ptr;
	}

	/// Returns the bit width of this value's type (-1 for void).
	[[nodiscard]] int8_t getBitWidth() const {
		return nautilus::getBitWith(getType());
	}

#ifdef ENABLE_TRACING
	/// Returns the tracing state reference for this value.
	[[nodiscard]] virtual tracing::TypedValueRef getState() const = 0;
#endif
};

// ============================================================================
// Compile-time val<T> → Type mapping
// ============================================================================

namespace detail {

/// Maps the inner type T of a val<T> to its runtime Type enum value.
/// This is used by isa/cast/dyn_cast to determine the expected type tag.
template <typename T>
constexpr Type resolve_val_type() {
	if constexpr (std::is_same_v<T, bool>) {
		return Type::b;
	} else if constexpr (std::is_arithmetic_v<T>) {
		return tracing::TypeResolver<T>::to_type();
	} else if constexpr (std::is_pointer_v<T>) {
		return Type::ptr;
	} else if constexpr (std::is_enum_v<T>) {
		return tracing::TypeResolver<std::underlying_type_t<T>>::to_type();
	} else if constexpr (std::is_reference_v<T>) {
		return resolve_val_type<std::remove_reference_t<T>>();
	} else {
		// Class types, function pointers — all stored as ptr
		return Type::ptr;
	}
}

/// Extracts the inner type T from val<T> and resolves its Type enum.
template <typename ValT>
struct val_type_tag;

template <typename T>
struct val_type_tag<val<T>> {
	static constexpr Type value = resolve_val_type<T>();
};

} // namespace detail

// ============================================================================
// LLVM-style isa / cast / dyn_cast
// ============================================================================

/// Checks whether a val_base pointer holds a specific val<T> type.
///
/// Compares the runtime Type tag against the compile-time expected type.
/// Note: types that share the same Type tag (e.g. all pointer types
/// share Type::ptr) are indistinguishable at this level.
///
/// ```cpp
/// val<int32_t> x = 42;
/// val_base* v = &x;
/// assert(isa<val<int32_t>>(v));   // true
/// assert(!isa<val<bool>>(v));     // false
/// ```
template <typename ValT>
[[nodiscard]] bool isa(const val_base* v) {
	assert(v && "isa called on null pointer");
	return v->getType() == detail::val_type_tag<ValT>::value;
}

/// Reference overload of isa.
template <typename ValT>
[[nodiscard]] bool isa(const val_base& v) {
	return v.getType() == detail::val_type_tag<ValT>::value;
}

/// Unchecked downcast — asserts the type matches in debug builds.
///
/// Like LLVM's cast<>, this assumes the caller has already verified the
/// type (e.g. via isa<>). Undefined behavior if the type doesn't match
/// in release builds.
///
/// ```cpp
/// val_base* v = &intVal;
/// val<int32_t>* p = cast<val<int32_t>>(v);  // asserts type match
/// ```
template <typename ValT>
[[nodiscard]] ValT* cast(val_base* v) {
	assert(v && "cast called on null pointer");
	assert(isa<ValT>(v) && "cast<> type mismatch");
	return static_cast<ValT*>(v);
}

template <typename ValT>
[[nodiscard]] const ValT* cast(const val_base* v) {
	assert(v && "cast called on null pointer");
	assert(isa<ValT>(v) && "cast<> type mismatch");
	return static_cast<const ValT*>(v);
}

/// Reference overload of cast.
template <typename ValT>
[[nodiscard]] ValT& cast(val_base& v) {
	assert(isa<ValT>(v) && "cast<> type mismatch");
	return static_cast<ValT&>(v);
}

template <typename ValT>
[[nodiscard]] const ValT& cast(const val_base& v) {
	assert(isa<ValT>(v) && "cast<> type mismatch");
	return static_cast<const ValT&>(v);
}

/// Checked downcast — returns nullptr if the type doesn't match.
///
/// Like LLVM's dyn_cast<>, combines an isa<> check with a static_cast.
/// Returns nullptr on type mismatch instead of asserting.
///
/// ```cpp
/// val_base* v = &someVal;
/// if (auto* p = dyn_cast<val<int32_t>>(v)) {
///     // use p as val<int32_t>*
/// }
/// ```
template <typename ValT>
[[nodiscard]] ValT* dyn_cast(val_base* v) {
	if (v && isa<ValT>(v)) {
		return static_cast<ValT*>(v);
	}
	return nullptr;
}

template <typename ValT>
[[nodiscard]] const ValT* dyn_cast(const val_base* v) {
	if (v && isa<ValT>(v)) {
		return static_cast<const ValT*>(v);
	}
	return nullptr;
}

} // namespace nautilus
