#pragma once

#include "nautilus/tracing/Types.hpp"
#include <cassert>

#ifdef ENABLE_TRACING
#include "nautilus/tracing/TypedValueRef.hpp"
#endif

namespace nautilus {

// Forward declaration
template <typename T>
class val;

// ============================================================================
// Unique per-type identity — address of a per-type inline variable
// ============================================================================

/// Opaque type identity token. Each distinct val<T> gets a unique TypeId.
using TypeId = const void*;

namespace detail {

/// Each instantiation of this template variable for a distinct T occupies a
/// unique address, which serves as a lightweight type identity token.
/// The inline keyword guarantees a single address across translation units.
template <typename T>
inline const char type_id_tag = 0;

} // namespace detail

/// Returns the unique TypeId for a given type T.
template <typename T>
constexpr TypeId typeIdOf() {
	return &detail::type_id_tag<T>;
}

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

	/// Returns the unique TypeId for the concrete val<T> type.
	/// Each distinct val<T> instantiation has a different TypeId.
	[[nodiscard]] virtual TypeId getTypeId() const = 0;

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
// LLVM-style isa / cast / dyn_cast
// ============================================================================

/// Checks whether a val_base pointer holds a specific val<T> type.
///
/// Uses a unique per-type identity so that every distinct val<T>
/// is distinguishable — e.g. val<int32_t*> vs val<double*>.
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
	return v->getTypeId() == typeIdOf<ValT>();
}

/// Reference overload of isa.
template <typename ValT>
[[nodiscard]] bool isa(const val_base& v) {
	return v.getTypeId() == typeIdOf<ValT>();
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
