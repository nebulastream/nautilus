#pragma once

#include "nautilus/tracing/Types.hpp"

#ifdef ENABLE_TRACING
#include "nautilus/tracing/TypedValueRef.hpp"
#endif

namespace nautilus {

/// Base interface for all val<T> specializations.
///
/// Provides a common polymorphic type that can be used to hold
/// pointers or references to any val<T> instance, regardless of
/// the underlying type T. This enables type-erased collections
/// and generic algorithms over heterogeneous val types.
///
/// # Example
/// ```cpp
/// val<int32_t> intVal = 42;
/// val<bool> boolVal = true;
///
/// val_base* values[] = {&intVal, &boolVal};
/// for (auto* v : values) {
///     std::cout << toString(v->getType()) << std::endl;
/// }
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

#ifdef ENABLE_TRACING
	/// Returns the tracing state reference for this value.
	[[nodiscard]] virtual tracing::TypedValueRef getState() const = 0;
#endif
};

} // namespace nautilus
