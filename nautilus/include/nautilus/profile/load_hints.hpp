
#pragma once

#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"

namespace nautilus {

/**
 * @brief Loads a value from a pointer, hinting to the optimizer that the loaded value is invariant
 * (i.e., it does not change). Maps to LLVM !invariant.load metadata, enabling optimizations
 * like loop-invariant code motion (LICM).
 */
template <typename T>
    requires is_arithmetic<T>
val<T> load_invariant(val<T*> ptr) {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto& ref = tracing::traceUnaryOp(tracing::LOAD_INVARIANT, tracing::TypeResolver<T>::to_type(), ptr.state);
		return val<T>(ref);
	}
#endif
	return *ptr;
}

/**
 * @brief Loads a pointer value, hinting to the optimizer that the result is non-null.
 * Maps to LLVM !nonnull metadata on the load instruction.
 */
template <typename T>
    requires is_ptr<T>
val<T> load_nonnull(val<T*> ptr) {
#ifdef ENABLE_TRACING
	if (tracing::inTracer()) {
		auto& ref = tracing::traceUnaryOp(tracing::LOAD_NONNULL, tracing::TypeResolver<T>::to_type(), ptr.state);
		return val<T>(ref);
	}
#endif
	return *ptr;
}

} // namespace nautilus
