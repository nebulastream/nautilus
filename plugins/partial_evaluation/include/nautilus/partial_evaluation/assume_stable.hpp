#pragma once

#include "nautilus/config.hpp"

#ifndef ENABLE_CONSTANT_TRACER
#error "nautilus/partial_evaluation/assume_stable.hpp included without ENABLE_CONSTANT_TRACER."
#endif

#include "nautilus/val_concepts.hpp"
// Forward-declares val<ValuePtrType> via val.hpp transitively. Users
// normally reach this header through <nautilus/val.hpp>, which pulls in
// val_ptr.hpp (where val<T*> is defined) and then — guarded by
// ENABLE_CONSTANT_TRACER — this header.
#include "nautilus/val.hpp"

namespace nautilus {

/// `assume_stable(ptr)` — mark `ptr` as safe for trace-time folding of
/// pointer arithmetic and LOAD ops. The caller is asserting two things:
///
///   1. The pointer is valid at trace time and still valid at execution.
///   2. The memory it points to is stable across trace→execution — no
///      writer modifies those bytes between when tracing ends and the
///      first time the compiled code runs.
///
/// After this call, the returned pointer (and any pointer derived from
/// it via `ptr + offset` etc.) carries the "assumed stable" flag. When
/// the flag is set *and* the pointer is a compile-time Constant (i.e.
/// constructed from a literal, not a function argument), subsequent
/// pointer arithmetic and LOAD ops fold at trace time: the new pointer
/// / the loaded bytes get baked into the trace as Constants, skipping
/// the ADD / LOAD ops entirely.
///
/// No fold happens when:
///   - the caller didn't call assume_stable, OR
///   - the pointer is Materialized (e.g. a function argument), OR
///   - built without ENABLE_CONSTANT_TRACER.
///
/// `assume_stable` is a no-op in every other case, so wrapping a
/// pointer in it is always semantically equivalent to using the raw
/// pointer — the difference is only whether the tracer is allowed to
/// fold uses of it.
///
/// If the caller's promise is violated (memory actually changed
/// between trace and execution), the compiled code sees the trace-
/// time value — stale from the caller's perspective. That's the
/// explicit trade-off of the primitive; for mutable memory, use plain
/// `*ptr` and `ptr + x` instead.
template <is_ptr ValuePtrType>
val<ValuePtrType> assume_stable(val<ValuePtrType> ptr) {
	ptr.assumed_stable_ = true;
	return ptr;
}

} // namespace nautilus
